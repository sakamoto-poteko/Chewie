/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#include "hnavigator.h"
#include "ui_hnavigator.h"
#include "analyzerdlg.h"
#include "Utilities.h"

#include "MessageTools.h"

#include <set>

#include <TKey.h>
#include <TFile.h>
#include <TIterator.h>
#include <TDirectory.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TF1.h>
#include <TH2F.h>
#include <TIterator.h>
#include <TClass.h>

#include <QFileDialog>
#include <QList>
//#include <QVBoxLayout>

//===========================================================================
HNavigator::HNavigator(QWidget * parent) :
    QTabWidget          (parent)
  , theMainWindow_      ((MainWindow*)parent)
  , timer_              (new QTimer(this))
  , ui_                 (new Ui::HNavigator)
  , theHTreeBrowser_    (0)
  , emptyFileLabel_     ("No files are currently open")
  , displayAllLabel_    ("Display all")
  , displayMemoryLabel_ ("Display memory resident objects")
  , counter_            (0)
  , shiftHSValue_       (0)
{
    ui_->setupUi(this);

    this->addItem(emptyFileLabel_.toStdString()) ;

    this->collectExistingWidgets();

    shiftHSValue_ = ui_->shiftHS->value();

    connect(ui_->hNavigatorInputFileCB,
            SIGNAL(currentIndexChanged(QString)),
            this,
            SLOT(  updateTree(         QString))) ;


    connect(timer_,
            SIGNAL(timeout                () ),
            this,
            SLOT  (checkNewObjectsInMemory()));

    for(int i=0; i<10; ++i)
    {
        ss_.str(""); ss_ << i ;
        ui_->hCanvasCB->addItem(QString(ss_.str().c_str()));
    }

    ui_->canvasSizeCB->addItem(QString("Small" ));
    ui_->canvasSizeCB->addItem(QString("Medium"));
    ui_->canvasSizeCB->addItem(QString("Large" ));
    ui_->canvasSizeCB->addItem(QString("Huge"  ));
    ui_->canvasSizeCB->setCurrentIndex(2);

    gStyle->SetPalette(1,0) ;

    timer_->start(1000) ;

    theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    theParamManager_[0]->setParName("Choose a function");

    ui_->parametersTW->insertTab(0, theParamManager_[0], "Par 0");
    ui_->fitMethodLE->setText("L");
    ui_->fitLimitCB->setChecked(false);
    emit (on_fitLimitCB_clicked(false));
    numberOfFitting_ = 0;

    ui_->fitFuncLW->insertItem(0, "Eta");
    ui_->fitFuncLW->insertItem(1, "Eta (No Depletion)");
    ui_->fitFuncLW->insertItem(2, "Eta Derivative");
    ui_->fitFuncLW->insertItem(3, "Eta with Cut");
    ui_->fitFuncLW->insertItem(4, "Eta (No Depletion) with Cut");
    ui_->fitFuncLW->insertItem(5, "Eta Inverse");
    ui_->fitFuncLW->insertItem(6, "Langaus");
    ui_->fitFuncLW->insertItem(7, "Gauss");
    ui_->fitFuncLW->insertItem(8, "Gauss Convoluted with Constant");
    ui_->fitFuncLW->insertItem(9, "Eta Convoluted with Gauss");
    ui_->fitFuncLW->insertItem(10,"Gauss-constant plus Constant");
    ui_->fitFuncLW->insertItem(11,"Eta Reproduction");
    ui_->fitFuncLW->insertItem(11,"Eta Distribution");
}

//===========================================================================
HNavigator::~HNavigator()
{
    timer_->stop();
    delete timer_ ;
    if(theHTreeBrowser_)
    {
        delete theHTreeBrowser_;
        theHTreeBrowser_ = 0;
    }
    delete ui_;
}

//===========================================================================
void HNavigator::on_openFilePB_clicked()
{
    QString localPath = getenv("CHEWIEOUTPUTDIR");
    QStringList fileNames = QFileDialog::getOpenFileNames(this,"Event root tree files",localPath,"Root Tree Files(*.root)");
    if(fileNames.isEmpty())
        return;
    for(int f=0; f<fileNames.size(); f++)
    {
        TFile* file=TFile::Open(fileNames[f].toAscii(),"READ");
        this->addFile(fileNames[f].toStdString(),file);
    }
}

//===========================================================================
void HNavigator::checkNewObjectsInMemory(void)
{
    //    STDLINE("Checking new objects",ACRed);
    if(theHTreeBrowser_)
        theHTreeBrowser_->populate(gROOT->GetRootFolder()) ;
    this->fillWidget();
}

//===========================================================================
void HNavigator::updateTree(QString currentFile)
{
    //STDLINE(currentFile.toStdString(),ACRed);
    theHTreeBrowser_->clear();
    if( currentFile != displayAllLabel_ )
    {
        this->fillWidgetTree(currentFile.toStdString());
    }
    else
    {
        for(int item=0; item < ui_->hNavigatorInputFileCB->count(); ++item)
        {
            this->fillWidgetTree(ui_->hNavigatorInputFileCB->itemText(item).toStdString());
        }
    }
}

//===========================================================================
void HNavigator::collectExistingWidgets()
{
    //    STDLINE("collectExistingWidgets",ACRed);

    if( !theHTreeBrowser_ )
    {
        // Create a tree-like folder-navigation tool
        //    this->setGeometry(this->x(),this->y(),this->width(),this->height()+120) ;
        this->show() ;
        this->setCurrentIndex(0);
        QWidget * input  = this->widget(0) ;
        theHTreeBrowser_ = new hTreeBrowser(input, theMainWindow_) ;
        theHTreeBrowser_->setGeometry(ui_->hNavigatorTreeFrame->geometry()) ;
        theHTreeBrowser_->show() ;
    }

    // Recover pointers to essential objects
    //  theHManager_  = theMainWindow_->getHManager()  ;
    //  theTabWidget_ = theMainWindow_->getTabWidget() ;

    // Populate the folders navigation tool with the existing open files
    // and their content and point to the current one
    //  if( !theTabWidget_ ) // No mainTabs widget has been opened yet.
    //  {
    //    return ;
    //  }
    //  else
    //  {
    //    theFileEater_ = theTabWidget_->getFileEater() ;
    //  }

    this->fillWidget()     ; // Fill the combo-box with the list of open files
    //    this->fillWidgetTree() ; // Populate the tree widget with file structure content
}

//===========================================================================
QMdiArea* HNavigator::getMdiArea(void)
{
    return theMainWindow_->getMdiArea();
}

//===========================================================================
MainWindow* HNavigator::getMainWindow(void)
{
    return theMainWindow_;
}

//===========================================================================
// Fill the combo-box with the list of currently open root files
void HNavigator::fillWidget()
{
    //STDLINE("fillWidget",ACRed);
    AnalyzerDlg* tmpAnalyzer = theMainWindow_->getAnalyzer();
    if(tmpAnalyzer != 0)
    {
        const std::map<std::string,TFile*>& fileList = tmpAnalyzer->getOpenFiles() ;

        for(std::map<std::string,TFile*>::const_iterator it=fileList.begin(); it!=fileList.end(); ++it)
        {
            this->addFile(it->first,it->second);
        }
        this->fillWidgetTree(ui_->hNavigatorInputFileCB->currentText().toStdString());
    }
}

//===========================================================================
void HNavigator::addFile(std::string fileName, TFile* file)
{
    if( openFiles_.find(fileName) == openFiles_.end() )
    {
        if(file->IsOpen())
        {
            openFiles_[fileName] = file;
            this->addItem(fileName) ;
        }
    }
    else if(!file->IsOpen())
    {
        openFiles_.erase(fileName);
        this->delItem(fileName);
    }

}

//===========================================================================
// Fill the tree
void HNavigator::fillWidgetTree(std::string currentFile)
{
    //STDLINE(std::string("fillWidgetTree. File: ") + currentFile,ACRed);
    //STDLINE(openFiles_.size(),ACPurple);
    if(openFiles_.size() == 0)
        return;
    TFile* file  = openFiles_[currentFile] ;
    theHTreeBrowser_->setCurrentFile(file);
    theHTreeBrowser_->populate((TDirectory *)file) ;
}

//===========================================================================
void HNavigator::addNewFile(const QString &)
{
    //    STDLINE("addNewFile",ACRed);
    this->refresh() ;
    this->collectExistingWidgets();
}

//===========================================================================
void HNavigator::refresh()
{
    STDLINE("Refreshing the content of the file tree-structure navigator",ACCyan) ;

    // Populate the folders navigation tool with the existing open files
    // and their content and point to the current one
    //  if( !theTabWidget_ )
    //  {
    //    return ;
    //  }
    //  else
    //  {
    //    theFileEater_ = theTabWidget_->getFileEater() ;
    //  }
}

//===========================================================================
void HNavigator::addItem(std::string item)
{
    //    STDLINE("addItem",ACRed);
    bool alreadyIncluded = false ;

    for(int i=0; i<ui_->hNavigatorInputFileCB->count(); i++)
    {
        if( ui_->hNavigatorInputFileCB->itemText(i).toStdString() == emptyFileLabel_.toStdString()          ) ui_->hNavigatorInputFileCB->setItemText(i,displayAllLabel_) ;
        if( ui_->hNavigatorInputFileCB->itemText(i).toStdString() == displayAllLabel_.toStdString() && i > 0) ui_->hNavigatorInputFileCB->removeItem(i) ;
        if( ui_->hNavigatorInputFileCB->itemText(i).toStdString() == item                                   ) alreadyIncluded = true ;
    }

    if( !alreadyIncluded )
    {
        ui_->hNavigatorInputFileCB->addItem(QString::fromStdString(item)) ;
    }

    ui_->hNavigatorInputFileCB->setCurrentIndex(ui_->hNavigatorInputFileCB->count()-1);
}

//===========================================================================
void HNavigator::delItem(std::string item)
{
    //    STDLINE("delItem",ACRed);
    for(int i=0; i<ui_->hNavigatorInputFileCB->count(); i++)
    {
        if( ui_->hNavigatorInputFileCB->itemText(i).toStdString() == item )
        {
            ui_->hNavigatorInputFileCB->removeItem(i);
            break;
        }
    }

    //    ui_->hNavigatorInputFileCB->setCurrentIndex(ui_->hNavigatorInputFileCB->count()-1);
}

//===========================================================================
void HNavigator::resizeEvent(QResizeEvent * )
{
    //STDLINE("resizeEvent",ACRed);
    /* +-O-------------------------------+
       |                                 |
       | A-----------------------------a |
       | |            file             | |
       | B-----------------------------b |
       |                                 |
       | A-----------------------------a |
       | |            comb             | |
       | B-----------------------------b |
       |                                 |
       | C-----------------------------c |
       | |                             | |
       | |            fram             | |
       | |                             | |
       | |                             | |
       | D-----------------------------d |
       |                                 |
       | E-----------------------------e |
       | |            grbx             | |
       | F-----------------------------f |
       |                                 |
       +---------------------------------+

       A: topComb
       B: botComb
       C: topFram
       D: botFram
       E: topGrbx
       F: botGrbx

       AB: combRect.height()
       CD: framRect.height()
       EF: grbxRect.height()

       OA: topComb
       BC: topFram - botComb
       DE: topGrbx - botFram (dYGrbx)

   */
    QRect thisRect = this->geometry() ;

    QRect fileRect = ui_->openFilePB           ->geometry() ;
    QRect combRect = ui_->hNavigatorInputFileCB->geometry() ;
    QRect framRect = ui_->hNavigatorTreeFrame  ->geometry() ;
    QRect grbxRect = ui_->hNavigatorCanvasGB   ->geometry() ;

    int topComb = combRect.y() ; int botComb = topComb + combRect.height() ;
    int topFram = framRect.y() ; int botFram = topFram + framRect.height() ;
    int topGrbx = grbxRect.y() ; int botGrbx = topGrbx + grbxRect.height() ;

    int dw = fileRect.height() + combRect.height() + grbxRect.height() + 30 ; // 40 is the total amount of vertical space between components

    // Adjust for width stretching first
    ui_->hNavigatorInputFileCB->setGeometry(combRect.x(), combRect.y(), thisRect.width()-5, combRect.height()) ;
    ui_->hNavigatorTreeFrame  ->setGeometry(framRect.x(), framRect.y(), thisRect.width()-5, framRect.height()) ;

    // Adjust for height stretching the only strechable part (the frame)
    framRect = ui_->hNavigatorTreeFrame  ->geometry() ;
    ui_->hNavigatorTreeFrame  ->setGeometry(framRect.x(), framRect.y(), framRect.width()   , thisRect.height()-dw) ; // Consider room for components without vertical stretch

    // Recompute current corners for vertical stretch
    combRect = ui_->hNavigatorInputFileCB->geometry() ;
    framRect = ui_->hNavigatorTreeFrame  ->geometry() ;

    topComb = combRect.y()    ; botComb = topComb + combRect.height() ;
    topFram = framRect.y()    ; botFram = topFram + framRect.height() ;
    topGrbx = botFram         ; botGrbx = topGrbx + grbxRect.height() ;

    // Adjust for vertical stretching
    //  if( theHTreeBrowser_ ) // At first call this component is not yet existing
    //    theHTreeBrowser_       ->setGeometry(framRect.x(), framRect.y(), thisRect.width()-5, framRect.height() ) ;

    ui_->hNavigatorCanvasGB   ->setGeometry(grbxRect.x(), topGrbx     , thisRect.width()-5, grbxRect.height() ) ;

    int currentIndex = this->currentIndex() ;
    if( currentIndex == 0 )
    {
        this->setCurrentIndex(1);
        this->setCurrentIndex(0);
    }
    else
    {
        this->setCurrentIndex(0);
        this->setCurrentIndex(currentIndex);
    }

}

//===========================================================================
std::string HNavigator::twoDOption(void)
{
    if( ui_->surfaceRB->isChecked() ) return "SURF4";
    if( ui_->contourRB->isChecked() ) return "CONT" ;
    if( ui_->legoRB   ->isChecked() ) return "LEGO4";
    if( ui_->lego2RB  ->isChecked() ) return "LEGO2";
    if( ui_->colzRB   ->isChecked() ) return "COLZ" ;
    return "" ;
}

//===========================================================================
void HNavigator::on_saveComponentsPB_clicked()
{
//    if ((*theHTreeBrowser_->getSelectedItems().begin()->second.begin())->GetName())
//    {
//        STDLINE("No histogram to save", ACRed);
        //return;
//    }

     //Ask for output file name
    std::stringstream ss;
    ss << "$CHEWIEFITOUTPUTDIR/" << theHTreeBrowser_->getFileName() << "|" << (*theHTreeBrowser_->getSelectedItems().begin()->second.begin())->GetName() << "_fitted.root";
      QString fileName = QFileDialog::getSaveFileName(this,
                                                      tr("Save File"),
                                                      (QString)ss.str().c_str(),
                                                      tr("Root files (*.root)"));
      if(fileName.isEmpty()) return ;

      TFile * tmpFile = new TFile(fileName.toStdString().c_str(),"recreate") ;

      // Get list of selected items to dump into new file
      hTreeBrowser::selectedObjectsDef selectedObjects = theHTreeBrowser_->getSelectedItems()  ;

      // Build the folder structure (if requested, otherwise dump a flat list)
      if( !ui_->flattenHierarchyCB->isChecked())
      {
        for(hTreeBrowser::selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
        {
          tmpFile->cd() ; // Restart always from top directory
          std::string slashTerminatedFullPath = it->first + std::string("/") ;
          ss_.str("") ;
          ss_ << "makeDir(" << slashTerminatedFullPath << ")" ;
          STDLINE(ss_.str(),ACWhite) ;
          this->makeDir(slashTerminatedFullPath) ;
        }
      }

      // Save each selected item (if requested, into the appropriate folder)
      for(hTreeBrowser::selectedObjectsDef::iterator it=selectedObjects.begin(); it!=selectedObjects.end(); ++it)
      {

        for(hTreeBrowser::tFileVDef::iterator jt=it->second.begin(); jt!=it->second.end(); ++jt)
        {
          tmpFile->cd() ; // Restart always from top directory
          if( !ui_->flattenHierarchyCB->isChecked())
          {
            tmpFile->cd(it->first.c_str()) ;
          }

          if( (*jt)->IsA() == TFolder::Class() ) continue ;

                (*jt)->Write() ;
        }
      }

      tmpFile->Close() ;
      delete tmpFile ;
}

//===========================================================================
void HNavigator::makeDir(std::string dirName)
{
    // Recursive method: builds a directory structure into the currently open file
    //                   by scanning the elements names in dirName
    boost::cmatch what;
    static const boost::regex exp("(.*?)/(.*)", boost::regex::perl);

    std::string match = "" ;
    std::string rest  = "" ;

    // Split directory name according to the '/' character
    if( boost::regex_match(dirName.c_str(), what, exp, boost::match_extra) )
    {
        match = what[1] ;
        rest  = what[2] ;
        TKey * key = gDirectory->FindKey(match.c_str()) ;
        if( key )
        {
            gDirectory->cd(match.c_str()) ;
        }
        else
        {
            gDirectory->mkdir(match.c_str())->cd() ;
        }
        if( !rest.empty()) this->makeDir(rest) ; // If there are still additional components in the directory name, continue splitting
    }
}

//===========================================================================
void HNavigator::on_hCanvasCB_activated(int index)
{
    //    STDLINE("on_hCanvasCB_activated",ACRed);
    theHTreeBrowser_->setCurrentCanvas(index) ;
}

//===========================================================================
void HNavigator::on_canvasSizeCB_activated(QString size)
{
    theHTreeBrowser_->setCanvasSize(size.toStdString()) ;
}

//===========================================================================
bool HNavigator::plotStatBox(void)
{
    return ui_->statCB->isChecked();
}

//===========================================================================
bool HNavigator::plotFitBox(void)
{
    return ui_->fitCB->isChecked();
}

//===========================================================================
bool HNavigator::logX(void)
{
    return ui_->logxCB->isChecked();
}

//===========================================================================
bool HNavigator::logY(void)
{
    return ui_->logyCB->isChecked();
}

//===========================================================================
bool HNavigator::logZ(void)
{
    return ui_->logzCB->isChecked();
}

//===========================================================================
std::string HNavigator::canvasSplitType(void)
{
    if     (ui_->splitCanvasXRB->isChecked())
        return "X";
    else if(ui_->splitCanvasYRB->isChecked())
        return "Y";
    else
        return "XY";
}

//===========================================================================
void HNavigator::checkBoxesHandler(bool)
{
    //STDLINE("Bool",ACRed);
    theHTreeBrowser_->showContextMenu(QPoint());
}

//===========================================================================
void HNavigator::checkBoxesHandler(int)
{
    //STDLINE("Int",ACGreen);
    theHTreeBrowser_->showContextMenu(QPoint());
}

//===========================================================================
void HNavigator::on_unZoomPB_clicked()
{
    theHTreeBrowser_->unZoom() ;
}

//===========================================================================
void HNavigator::on_setRangePB_clicked()
{
    theHTreeBrowser_->setRange( ui_->xMinLE->text().toFloat(),ui_->xMaxLE->text().toFloat()
                              , ui_->yMinLE->text().toFloat(),ui_->yMaxLE->text().toFloat()
                              , ui_->zMinLE->text().toFloat(),ui_->zMaxLE->text().toFloat());
}

//===========================================================================
void HNavigator::on_shiftPlusPB_clicked()
{
    float shiftMin = ui_->xMinLE->text().toFloat()+ui_->shiftLE->text().toFloat();
    float shiftMax = ui_->xMaxLE->text().toFloat()+ui_->shiftLE->text().toFloat();
    ui_->xMinLE->setText(QString("%1").arg(shiftMin));
    ui_->xMaxLE->setText(QString("%1").arg(shiftMax));
    emit on_setRangePB_clicked();
}

//===========================================================================
void HNavigator::on_shiftMinusPB_clicked()
{
    float shiftMin = ui_->xMinLE->text().toFloat()-ui_->shiftLE->text().toFloat();
    if(shiftMin < 0)
        shiftMin = 0;
    float shiftMax = ui_->xMaxLE->text().toFloat()-ui_->shiftLE->text().toFloat();
    if(shiftMax < 0)
        shiftMax = 0;
    ui_->xMinLE->setText(QString("%1").arg(shiftMin));
    ui_->xMaxLE->setText(QString("%1").arg(shiftMax));
    emit on_setRangePB_clicked();
}

//===========================================================================
void HNavigator::on_shiftHS_valueChanged(int value)
{
//    STDLINE(shiftHSValue_,ACRed);
//    STDLINE(value,ACPurple);
    int delta = value - shiftHSValue_;

    float shiftMin = ui_->xMinLE->text().toFloat()+delta*ui_->shiftLE->text().toFloat();
    //if(shiftMin < 0)
    //    shiftMin = 0;
    float shiftMax = ui_->xMaxLE->text().toFloat()+delta*ui_->shiftLE->text().toFloat();
    //if(shiftMax < 0)
    //    shiftMax = 0;
    ui_->xMinLE->setText(QString("%1").arg(shiftMin));
    ui_->xMaxLE->setText(QString("%1").arg(shiftMax));
//    STDLINE(shiftMin,ACRed);
//    STDLINE(shiftMax,ACGreen);

    shiftHSValue_ = value;
    emit on_setRangePB_clicked();
}

void HNavigator::on_fitFuncLW_itemDoubleClicked(QListWidgetItem *item)
{
    if (ui_->customFunctionCB->isChecked()) return;

    std::stringstream ss;

    for (unsigned int j = 0; j < theParamManager_.size(); ++j)
    {
        delete theParamManager_[j];
        ui_->parametersTW->removeTab(j);
    }
    theParamManager_.clear();

    if (item->text() == "Eta")
    {
        for (int i = 0; i < 6; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Thickness                        ");
                theParamManager_[i]->setParValue (500);
                theParamManager_[i]->setParLimInf(500);
                theParamManager_[i]->setParLimSup(500);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            else if (i == 1)
            {
                theParamManager_[i]->setParName  ("2*mu*DepletionVoltage            ");
                theParamManager_[i]->setParValue (1.);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(9.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 2)
            {
                theParamManager_[i]->setParName  ("DepletionVoltage                 ");
                theParamManager_[i]->setParValue (25.);
                theParamManager_[i]->setParLimInf(10.);
                theParamManager_[i]->setParLimSup(100.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1.);
            }
            else if (i == 3)
            {
                theParamManager_[i]->setParName  ("AppliedVoltage + DepletionVoltage");
                theParamManager_[i]->setParValue (525.);
                theParamManager_[i]->setParLimInf(510.);
                theParamManager_[i]->setParLimSup(600.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1.);
            }
            else if (i == 4)
            {
                theParamManager_[i]->setParPrecision(5);
                theParamManager_[i]->setParStep(0.00001);
                theParamManager_[i]->setParName  ("4*DiffusionConstant              ");
                theParamManager_[i]->setParValue (0.0007);
                theParamManager_[i]->setParLimInf(0.00001);
                theParamManager_[i]->setParLimSup(0.3);
                theParamManager_[i]->setParFixed (false);
            }
            else if (i == 5)
            {
                theParamManager_[i]->setParName  ("ScaleFactor                      ");
                theParamManager_[i]->setParValue (1.);
                theParamManager_[i]->setParLimInf(1.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );

        }
    }

    else if (item->text() == "Eta (No Depletion)")
    {
        for (int i = 0; i < 4; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Thickness                     ");
                theParamManager_[i]->setParValue (500);
                theParamManager_[i]->setParLimInf(500);
                theParamManager_[i]->setParLimSup(500);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            else if (i == 1)
            {
                theParamManager_[i]->setParName  ("Thickness/(mu*AppliedVoltage)");
                theParamManager_[i]->setParValue (6.5);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(30.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 2)
            {
                theParamManager_[i]->setParPrecision(5);
                theParamManager_[i]->setParStep(0.00001);
                theParamManager_[i]->setParName  ("4*DiffusionConstant           ");
                theParamManager_[i]->setParValue (0.0007);
                theParamManager_[i]->setParLimInf(0.00001);
                theParamManager_[i]->setParLimSup(0.3);
                theParamManager_[i]->setParFixed (false);
            }
            else if (i == 3)
            {
                theParamManager_[i]->setParName  ("ScaleFactor                   ");
                theParamManager_[i]->setParValue (1.);
                theParamManager_[i]->setParLimInf(1.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );
        }
    }

    else if (item->text() == "Eta Derivative")
    {
        for (int i = 0; i < 5; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Thickness                     ");
                theParamManager_[i]->setParValue (500);
                theParamManager_[i]->setParLimInf(500);
                theParamManager_[i]->setParLimSup(500);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            else if (i == 1)
            {
                theParamManager_[i]->setParName  ("Thickness/(mu*AppliedVoltage)");
                theParamManager_[i]->setParValue (6.5);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(30.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 2)
            {
                theParamManager_[i]->setParPrecision(5);
                theParamManager_[i]->setParStep(0.00001);
                theParamManager_[i]->setParName  ("4*DiffusionConstant           ");
                theParamManager_[i]->setParValue (0.0007);
                theParamManager_[i]->setParLimInf(0.00001);
                theParamManager_[i]->setParLimSup(0.3);
                theParamManager_[i]->setParFixed (false);
            }
            else if (i == 3)
            {
                theParamManager_[i]->setParName  ("ScaleFactor                   ");
                theParamManager_[i]->setParValue (1.);
                theParamManager_[i]->setParLimInf(1.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 4)
            {
                theParamManager_[i]->setParName  ("Tan(angle)                    ");
                theParamManager_[i]->setParValue (0.);
                theParamManager_[i]->setParLimits(-100, 100);
                theParamManager_[i]->setParLimInf(-1.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(4);
                theParamManager_[i]->setParStep(0.0001);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );

        }
    }

    else if (item->text() == "Eta with Cut")
    {
        for (int i = 0; i < 7; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Thickness                        ");
                theParamManager_[i]->setParValue (500);
                theParamManager_[i]->setParLimInf(500);
                theParamManager_[i]->setParLimSup(500);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            else if (i == 1)
            {
                theParamManager_[i]->setParName  ("2*mu*DepletionVoltage            ");
                theParamManager_[i]->setParValue (1.);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(9.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 2)
            {
                theParamManager_[i]->setParName  ("DepletionVoltage                 ");
                theParamManager_[i]->setParValue (25.);
                theParamManager_[i]->setParLimInf(10.);
                theParamManager_[i]->setParLimSup(100.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1.);
            }
            else if (i == 3)
            {
                theParamManager_[i]->setParName  ("AppliedVoltage + DepletionVoltage");
                theParamManager_[i]->setParValue (525.);
                theParamManager_[i]->setParLimInf(510.);
                theParamManager_[i]->setParLimSup(600.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1.);
            }
            else if (i == 4)
            {
                theParamManager_[i]->setParPrecision(5);
                theParamManager_[i]->setParStep(0.00001);
                theParamManager_[i]->setParName  ("4*DiffusionConstant              ");
                theParamManager_[i]->setParValue (0.0007);
                theParamManager_[i]->setParLimInf(0.00001);
                theParamManager_[i]->setParLimSup(0.3);
                theParamManager_[i]->setParFixed (false);
            }
            else if (i == 5)
            {
                theParamManager_[i]->setParName  ("ScaleFactor                      ");
                theParamManager_[i]->setParValue (1.);
                theParamManager_[i]->setParLimInf(1.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 6)
            {
                theParamManager_[i]->setParName  ("CutThreashold                    ");
                theParamManager_[i]->setParValue (0.0005);
                theParamManager_[i]->setParLimInf(0.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(4);
                theParamManager_[i]->setParStep(0.0001);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );

        }
    }

    else if (item->text() == "Eta (No Depletion) with Cut")
    {
        for (int i = 0; i < 5; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Thickness                     ");
                theParamManager_[i]->setParValue (500);
                theParamManager_[i]->setParLimInf(500);
                theParamManager_[i]->setParLimSup(500);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            else if (i == 1)
            {
                theParamManager_[i]->setParName  ("Thickness/(mu*AppliedVoltage)");
                theParamManager_[i]->setParValue (6.5);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(30.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 2)
            {
                theParamManager_[i]->setParPrecision(5);
                theParamManager_[i]->setParStep(0.00001);
                theParamManager_[i]->setParName  ("4*DiffusionConstant           ");
                theParamManager_[i]->setParValue (0.0007);
                theParamManager_[i]->setParLimInf(0.00001);
                theParamManager_[i]->setParLimSup(0.3);
                theParamManager_[i]->setParFixed (false);
            }
            else if (i == 3)
            {
                theParamManager_[i]->setParName  ("ScaleFactor                   ");
                theParamManager_[i]->setParValue (1.);
                theParamManager_[i]->setParLimInf(1.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 4)
            {
                theParamManager_[i]->setParName  ("CutThreashold                 ");
                theParamManager_[i]->setParValue (0.0005);
                theParamManager_[i]->setParLimInf(0.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(4);
                theParamManager_[i]->setParStep(0.0001);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );
        }
    }

    else if (item->text() == "Eta Inverse")
    {
        for (int i = 0; i < 5; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Thickness                     ");
                theParamManager_[i]->setParValue (500);
                theParamManager_[i]->setParLimInf(500);
                theParamManager_[i]->setParLimSup(500);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            else if (i == 1)
            {
                theParamManager_[i]->setParName  ("Thickness/(mu*AppliedVoltage)");
                theParamManager_[i]->setParValue (6.5);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(30.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 2)
            {
                theParamManager_[i]->setParPrecision(5);
                theParamManager_[i]->setParStep(0.00001);
                theParamManager_[i]->setParName  ("4*DiffusionConstant           ");
                theParamManager_[i]->setParValue (0.0007);
                theParamManager_[i]->setParLimInf(0.00001);
                theParamManager_[i]->setParLimSup(0.3);
                theParamManager_[i]->setParFixed (false);
            }
            else if (i == 3)
            {
                theParamManager_[i]->setParName  ("ScaleFactor                   ");
                theParamManager_[i]->setParValue (1.);
                theParamManager_[i]->setParLimInf(1.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 4)
            {
                theParamManager_[i]->setParName  ("Tan(theta)                    ");
                theParamManager_[i]->setParLimits(-100, 100);
                theParamManager_[i]->setParValue (0);
                theParamManager_[i]->setParLimInf(-1);
                theParamManager_[i]->setParLimSup(1);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(4);
                theParamManager_[i]->setParStep(0.0001);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );

        }
    }

    else if (item->text() == "Langaus")
    {
        for (int i = 0; i < 4; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Width");
                theParamManager_[i]->setParLimits(10, 10000);
                theParamManager_[i]->setParValue (3000);
                theParamManager_[i]->setParLimInf(1000);
                theParamManager_[i]->setParLimSup(10000);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(10);
            }
            else if (i == 1)
            {
                theParamManager_[i]->setParName  ("MPV");
                theParamManager_[i]->setParLimits(10, 100000);
                theParamManager_[i]->setParValue (20000);
                theParamManager_[i]->setParLimInf(1000);
                theParamManager_[i]->setParLimSup(30000);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(10);
            }
            else if (i == 2)
            {
                theParamManager_[i]->setParName  ("Area");
                theParamManager_[i]->setParLimits(10000, 1000000000);
                theParamManager_[i]->setParValue (10000000);
                theParamManager_[i]->setParLimInf(100000);
                theParamManager_[i]->setParLimSup(1000000000);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1000);
            }
            else if (i == 3)
            {
                theParamManager_[i]->setParName  ("GSigma");
                theParamManager_[i]->setParLimits(10, 100000);
                theParamManager_[i]->setParValue (3000);
                theParamManager_[i]->setParLimInf(500);
                theParamManager_[i]->setParLimSup(10000);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(10);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );

        }
    }

    else if (item->text() == "Gauss")
    {
        for (int i = 0; i < 3; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Constant");
                theParamManager_[i]->setParValue (1);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(1000);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(1);
                theParamManager_[i]->setParStep(0.1);
            }
            else if (i == 1)
            {
                theParamManager_[i]->setParName  ("Mean");
                theParamManager_[i]->setParLimits(-100000, 100000);
                theParamManager_[i]->setParValue (1);
                theParamManager_[i]->setParLimInf(-100);
                theParamManager_[i]->setParLimSup(100);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(1);
                theParamManager_[i]->setParStep(0.1);
            }
            else if (i == 2)
            {
                theParamManager_[i]->setParName  ("Sigma");
                theParamManager_[i]->setParValue (1);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(100.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );

        }
    }

    else if (item->text() == "Gauss Convoluted with Constant")
    {
        for (int i = 0; i < 4; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Pixel Limit");
                theParamManager_[i]->setParLimits(-100, 100);
                theParamManager_[i]->setParValue (50);
                theParamManager_[i]->setParLimInf(50);
                theParamManager_[i]->setParLimSup(75);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            if (i == 1)
            {
                theParamManager_[i]->setParName  ("Cluster 1 extension");
                theParamManager_[i]->setParValue (40);
                theParamManager_[i]->setParLimInf(0);
                theParamManager_[i]->setParLimSup(50);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            if (i == 2)
            {
                theParamManager_[i]->setParName  ("Sigma Gauss");
                theParamManager_[i]->setParValue (5);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(75);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            if (i == 3)
            {
                theParamManager_[i]->setParName  ("Scale Factor");
                theParamManager_[i]->setParLimits(0, 100000);
                theParamManager_[i]->setParValue (100);
                theParamManager_[i]->setParLimInf(10);
                theParamManager_[i]->setParLimSup(10000);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );
        }
    }

        else if (item->text() == "Eta Convoluted with Gauss")
        {
            for (int i = 0; i < 9; ++i)
            {
                theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
        //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

                if (i == 0)
                {
                    theParamManager_[i]->setParName  ("Thickness                     ");
                    theParamManager_[i]->setParValue (500);
                    theParamManager_[i]->setParLimInf(500);
                    theParamManager_[i]->setParLimSup(500);
                    theParamManager_[i]->setParFixed (true);
                    theParamManager_[i]->setParPrecision(0);
                    theParamManager_[i]->setParStep(1);
                }
                else if (i == 1)
                {
                    theParamManager_[i]->setParName  ("Thickness/(mu*AppliedVoltage) ");
                    theParamManager_[i]->setParValue (6.5);
                    theParamManager_[i]->setParLimInf(0.1);
                    theParamManager_[i]->setParLimSup(30.);
                    theParamManager_[i]->setParFixed (false);
                    theParamManager_[i]->setParPrecision(2);
                    theParamManager_[i]->setParStep(0.01);
                }
                else if (i == 2)
                {
                    theParamManager_[i]->setParPrecision(5);
                    theParamManager_[i]->setParStep(0.00001);
                    theParamManager_[i]->setParName  ("4*DiffusionConstant           ");
                    theParamManager_[i]->setParValue (0.0700);
                    theParamManager_[i]->setParLimInf(0.00001);
                    theParamManager_[i]->setParLimSup(0.3);
                    theParamManager_[i]->setParFixed (false);
                }
                else if (i == 3)
                {
                    theParamManager_[i]->setParName  ("ScaleFactor                   ");
                    theParamManager_[i]->setParValue (1.);
                    theParamManager_[i]->setParLimInf(1.);
                    theParamManager_[i]->setParLimSup(1.);
                    theParamManager_[i]->setParFixed (true);
                    theParamManager_[i]->setParPrecision(2);
                    theParamManager_[i]->setParStep(0.01);
                }
                else if (i == 4)
                {
                    theParamManager_[i]->setParName  ("Extension of constant function");
                    theParamManager_[i]->setParLimits(0, 1000);
                    theParamManager_[i]->setParValue (4.);
                    theParamManager_[i]->setParLimInf(0.);
                    theParamManager_[i]->setParLimSup(50);
                    theParamManager_[i]->setParFixed (false);
                    theParamManager_[i]->setParPrecision(3);
                    theParamManager_[i]->setParStep(0.001);
                }
                else if (i == 5)
                {
                    theParamManager_[i]->setParName  ("Sigma                         ");
                    theParamManager_[i]->setParLimits(0,100);
                    theParamManager_[i]->setParValue (10);
                    theParamManager_[i]->setParLimInf(1);
                    theParamManager_[i]->setParLimSup(100);
                    theParamManager_[i]->setParFixed (false);
                    theParamManager_[i]->setParPrecision(1);
                    theParamManager_[i]->setParStep(0.1);
                }
                else if (i == 6)
                {
                    theParamManager_[i]->setParName  ("Scale Factor                  ");
                    theParamManager_[i]->setParLimits(0, 10000);
                    theParamManager_[i]->setParValue (400);
                    theParamManager_[i]->setParLimInf(100);
                    theParamManager_[i]->setParLimSup(10000);
                    theParamManager_[i]->setParFixed (false);
                    theParamManager_[i]->setParPrecision(0);
                    theParamManager_[i]->setParStep(1);
                }
                else if (i == 7)
                {
                    theParamManager_[i]->setParName  ("Center of distribution        ");
                    theParamManager_[i]->setParLimits(-100, 100);
                    theParamManager_[i]->setParValue (0);
                    theParamManager_[i]->setParLimInf(-100);
                    theParamManager_[i]->setParLimSup(100);
                    theParamManager_[i]->setParFixed (false);
                    theParamManager_[i]->setParPrecision(1);
                    theParamManager_[i]->setParStep(0.1);
                }
                else if (i == 8)
                {
                    theParamManager_[i]->setParName  ("Vertical translation Constant ");
                    theParamManager_[i]->setParValue (100);
                    theParamManager_[i]->setParLimInf(0);
                    theParamManager_[i]->setParLimSup(1000);
                    theParamManager_[i]->setParFixed (false);
                    theParamManager_[i]->setParPrecision(0);
                    theParamManager_[i]->setParStep(1);
                }

                connect(theParamManager_[i]->getParFixed(),
                        SIGNAL(clicked() ),
                        theParamManager_[i],
                        SLOT(on_fixPar_clicked())
                        );

        }
    }

    else if (item->text() == "Gauss-constant plus Constant")
    {
        for (int i = 0; i < 5; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Extension of constant function");
                theParamManager_[i]->setParLimits(0, 100000);
                theParamManager_[i]->setParValue (4.);
                theParamManager_[i]->setParLimInf(0.);
                theParamManager_[i]->setParLimSup(50);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(3);
                theParamManager_[i]->setParStep(0.001);
            }
            else if (i == 1)
            {
                theParamManager_[i]->setParName  ("Sigma                         ");
                theParamManager_[i]->setParLimits(0,100);
                theParamManager_[i]->setParValue (10);
                theParamManager_[i]->setParLimInf(1);
                theParamManager_[i]->setParLimSup(100);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(1);
                theParamManager_[i]->setParStep(0.1);
            }
            else if (i == 2)
            {
                theParamManager_[i]->setParName  ("Scale Factor                  ");
                theParamManager_[i]->setParLimits(0, 10000);
                theParamManager_[i]->setParValue (400);
                theParamManager_[i]->setParLimInf(100);
                theParamManager_[i]->setParLimSup(10000);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            else if (i == 3)
            {
                theParamManager_[i]->setParName  ("Center of distribution        ");
                theParamManager_[i]->setParLimits(-100, 100);
                theParamManager_[i]->setParValue (0);
                theParamManager_[i]->setParLimInf(-100);
                theParamManager_[i]->setParLimSup(100);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(1);
                theParamManager_[i]->setParStep(0.1);
            }
            else if (i == 4)
            {
                theParamManager_[i]->setParName  ("Vertical translation Constant ");
                theParamManager_[i]->setParValue (100);
                theParamManager_[i]->setParLimInf(0);
                theParamManager_[i]->setParLimSup(1000);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );

        }
    }

    else if (item->text() == "Eta Reproduction")
    {
        for (int i = 0; i < 11; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Thickness                     ");
                theParamManager_[i]->setParValue (500);
                theParamManager_[i]->setParLimInf(500);
                theParamManager_[i]->setParLimSup(500);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            else if (i == 1)
            {
                theParamManager_[i]->setParName  ("Thickness/(mu*AppliedVoltage)");
                theParamManager_[i]->setParValue (6.5);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(30.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 2)
            {
                theParamManager_[i]->setParPrecision(5);
                theParamManager_[i]->setParStep(0.00001);
                theParamManager_[i]->setParName  ("4*DiffusionConstant           ");
                theParamManager_[i]->setParValue (0.0007);
                theParamManager_[i]->setParLimInf(0.00001);
                theParamManager_[i]->setParLimSup(0.3);
                theParamManager_[i]->setParFixed (false);
            }
            else if (i == 3)
            {
                theParamManager_[i]->setParName  ("ScaleFactor                   ");
                theParamManager_[i]->setParValue (1.);
                theParamManager_[i]->setParLimInf(1.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            if (i == 4)
            {
                theParamManager_[i]->setParName  ("Cluster 1 extension           ");
                theParamManager_[i]->setParValue (40);
                theParamManager_[i]->setParLimInf(0);
                theParamManager_[i]->setParLimSup(50);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            if (i == 5)
            {
                theParamManager_[i]->setParName  ("Sigma Gauss (side)            ");
                theParamManager_[i]->setParLimits(-1000, 1000);
                theParamManager_[i]->setParValue (0);
                theParamManager_[i]->setParLimInf(-10);
                theParamManager_[i]->setParLimSup(10);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            if (i == 6)
            {
                theParamManager_[i]->setParName  ("Constant factor (side)        ");
                theParamManager_[i]->setParLimits(0, 10000);
                theParamManager_[i]->setParValue (1000);
                theParamManager_[i]->setParLimInf(100);
                theParamManager_[i]->setParLimSup(10000);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(10);
            }
            if (i == 7)
            {
                theParamManager_[i]->setParName  ("Sigma Gauss (center)          ");
                theParamManager_[i]->setParValue (10);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(75);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            if (i == 8)
            {
                theParamManager_[i]->setParName  ("Scale Gauss (center)          ");
                theParamManager_[i]->setParLimits(0, 10000);
                theParamManager_[i]->setParValue (500);
                theParamManager_[i]->setParLimInf(100);
                theParamManager_[i]->setParLimSup(1000);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            if (i == 9)
            {
                theParamManager_[i]->setParName  ("Horizontal Translation        ");
                theParamManager_[i]->setParLimits(-100, 100);
                theParamManager_[i]->setParValue (0);
                theParamManager_[i]->setParLimInf(-100);
                theParamManager_[i]->setParLimSup(100);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(1);
                theParamManager_[i]->setParStep(0.1);
            }
            if (i == 10)
            {
                theParamManager_[i]->setParName  ("Vertical Translation          ");
                theParamManager_[i]->setParValue (100);
                theParamManager_[i]->setParLimInf(0);
                theParamManager_[i]->setParLimSup(500);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );
        }
    }
    else if (item->text() == "Eta Distribution")
    {
        for (int i = 0; i < 6; ++i)
        {
            theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));
    //        if (i > 0) theParamManager_[i]->stackUnder(theParamManager_[i-1]);

            if (i == 0)
            {
                theParamManager_[i]->setParName  ("Thickness                     ");
                theParamManager_[i]->setParValue (500);
                theParamManager_[i]->setParLimInf(500);
                theParamManager_[i]->setParLimSup(500);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(0);
                theParamManager_[i]->setParStep(1);
            }
            else if (i == 1)
            {
                theParamManager_[i]->setParName  ("Thickness/(mu*AppliedVoltage)");
                theParamManager_[i]->setParValue (6.5);
                theParamManager_[i]->setParLimInf(0.1);
                theParamManager_[i]->setParLimSup(30.);
                theParamManager_[i]->setParFixed (false);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 2)
            {
                theParamManager_[i]->setParPrecision(5);
                theParamManager_[i]->setParStep(0.00001);
                theParamManager_[i]->setParName  ("4*DiffusionConstant           ");
                theParamManager_[i]->setParValue (0.0007);
                theParamManager_[i]->setParLimInf(0.00001);
                theParamManager_[i]->setParLimSup(0.3);
                theParamManager_[i]->setParFixed (false);
            }
            else if (i == 3)
            {
                theParamManager_[i]->setParName  ("ScaleFactor for Charge profile");
                theParamManager_[i]->setParValue (1.);
                theParamManager_[i]->setParLimInf(1.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }
            else if (i == 4)
            {
                theParamManager_[i]->setParName  ("Tan(theta)                    ");
                theParamManager_[i]->setParLimits(-100, 100);
                theParamManager_[i]->setParValue (0);
                theParamManager_[i]->setParLimInf(-1);
                theParamManager_[i]->setParLimSup(1);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(4);
                theParamManager_[i]->setParStep(0.0001);
            }
            else if (i == 5)
            {
                theParamManager_[i]->setParName  ("Global ScaleFactor            ");
                theParamManager_[i]->setParValue (1.);
                theParamManager_[i]->setParLimInf(1.);
                theParamManager_[i]->setParLimSup(1.);
                theParamManager_[i]->setParFixed (true);
                theParamManager_[i]->setParPrecision(2);
                theParamManager_[i]->setParStep(0.01);
            }

            connect(theParamManager_[i]->getParFixed(),
                    SIGNAL(clicked() ),
                    theParamManager_[i],
                    SLOT(on_fixPar_clicked())
                    );

        }
    }


    for (unsigned int j = 0; j < theParamManager_.size(); ++j)
    {
        ss.str("");
        ss << "Par " << j;
        ui_->parametersTW->insertTab(j, theParamManager_[j], (QString)ss.str().c_str());
    }
}

void HNavigator::on_fitFuncLE_editingFinished()
{
    if (!ui_->customFunctionCB->isChecked()) return;

    std::stringstream ss;

    for (unsigned int j = 0; j < theParamManager_.size(); ++j)
    {
        delete theParamManager_[j];
        ui_->parametersTW->removeTab(j);
    }
    theParamManager_.clear();

    QRegExp parFind ("\\[(\\w)\\]");
    std::set<int> params;

    int pos = 0;
    int nPars = 0;
    while ((pos = parFind.indexIn(ui_->fitFuncLE->text(), pos)) != -1) {
        params.insert(parFind.cap(1).toInt());
        pos += parFind.matchedLength();
    }

    for (std::set<int>::iterator it = params.begin(); it != params.end(); ++it)
    {
        theParamManager_.push_back(new FitParamManagerWidget(ui_->parametersTW));

        ss.str("");
        ss << "Par " << *it;
        theParamManager_[nPars]->setParName  (ss.str());
        theParamManager_[nPars]->setParLimits(-100000, 100000);
        theParamManager_[nPars]->setParValue (0);
        theParamManager_[nPars]->setParLimInf(0);
        theParamManager_[nPars]->setParLimSup(100);
        theParamManager_[nPars]->setParFixed (false);
        theParamManager_[nPars]->setParPrecision(1);
        theParamManager_[nPars]->setParStep(10);

        ++nPars;
    }

    for (unsigned int j = 0; j < theParamManager_.size(); ++j)
    {
        ss.str("");
        ss << "Par " << j;
        ui_->parametersTW->insertTab(j, theParamManager_[j], (QString)ss.str().c_str());
    }

}
//===========================================================================================================

void HNavigator::on_fitPB_clicked()
{
    ui_->fitPB->setEnabled(false);

    std::string fitMethod = ui_->fitMethodLE->text().toStdString();

    if (!theHTreeBrowser_->getCurrentObject())
    {
        STDLINE("Select an histogram to fit", ACYellow);
        ui_->fitPB->setEnabled(true);
        return;
    }

    TObject * toFit = theHTreeBrowser_->getCurrentObject();
    TH1F * toFit1D = 0;
    TH2F * toFit2D = 0;
    int hDimension = 0;
    if (this->getObjectType(toFit).find("TH1") != std::string::npos){
        toFit1D = (TH1F*)toFit;
        hDimension = 1;
    }
    if (this->getObjectType(toFit).find("TH2") != std::string::npos){
        toFit2D = (TH2F*)toFit;
        hDimension = 2;
    }

    std::stringstream ss;

    ss.str("");
    ss << toFit->GetName() << "_fitter_" << numberOfFitting_;
    std::string hName = ss.str();
    if (hDimension == 1) {
        double xMin = toFit1D->GetXaxis()->GetXmin();
        double xMax = toFit1D->GetXaxis()->GetXmax();  //check eventually for resize event
        ss.str("");
        ss << toFit1D->GetName();
        if (!ui_->customFunctionCB->isChecked())
        {
//------------------ ETA --------------------------------------------------------------------------------------
        if (ui_->fitFuncLW->currentItem()->text() == "Eta")
        {
            TF1 * fitter = new TF1(hName.c_str(), Utilities::etaFitFunc, xMin, xMax, 6);
            fitter->SetLineColor(kBlue);
            for (int i = 0; i < 6; ++i)
            {
                fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                else {
                    fitter->SetParameter(i, theParamManager_[i]->getParValue());
                    fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                }
            }
            STDLINE("Fitting " + ss.str() + " with Eta function", ACGreen);
            if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
            else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
        }
//------------------ ETA DERIVATIVE ------------------------------------------------------------------------------
        else if (ui_->fitFuncLW->currentItem()->text() == "Eta Derivative")
        {
            TF1 * fitter = new TF1(hName.c_str(), Utilities::integrandEtaFitFunc2Angle, xMin, xMax, 5);
            fitter->SetLineColor(kBlue);
            for (int i = 0; i < 5; ++i)
            {
                fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                else {
                    fitter->SetParameter(i, theParamManager_[i]->getParValue());
                    fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                }
            }
            STDLINE("Fitting " + ss.str() + " with Eta derivative function", ACGreen);
            if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
            else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
        }
//------------------ ETA INVERSE ---------------------------------------------------------------------------------
        else if (ui_->fitFuncLW->currentItem()->text() == "Eta Inverse")
        {
            TF1 * fitter = new TF1(hName.c_str(), Utilities::etaInverseFitFunc2Angle, xMin, xMax, 5);
            fitter->SetLineColor(kBlue);
            for (int i = 0; i < 5; ++i)
            {
                fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                else {
                    fitter->SetParameter(i, theParamManager_[i]->getParValue());
                    fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                }
            }
            STDLINE("Fitting " + ss.str() + " with Eta Inverse function", ACGreen);
            if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
            else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
        }
//------------------ GAUSS --------------------------------------------------------------------------------------
        else if (ui_->fitFuncLW->currentItem()->text() == "Gauss")
        {
            TF1 * fitter = new TF1(hName.c_str(), "gaus", xMin, xMax);
            fitter->SetLineColor(kBlue);
            for (int i = 0; i < 3; ++i)
            {
                fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                else {
                    fitter->SetParameter(i, theParamManager_[i]->getParValue());
                    fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                }
            }
            STDLINE("Fitting " + ss.str() + " with Gauss function", ACGreen);
            if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
            else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
        }
//------------------ GAUSS CONVOLUTED WITH CONSTANT (CENTER) PLUS CONSTANT---------------------------------------
        else if (ui_->fitFuncLW->currentItem()->text() == "Gauss-constant plus Constant")
        {
//            TF1 * fitter = new TF1(hName.c_str(), Utilities::uniformCenterSmearedPlusConstant , xMin, xMax, 5);
            TF1 * fitter = new TF1(hName.c_str(), "[0]*exp(-x*x/([1]*[1])) + [2]" , xMin, xMax);
            fitter->SetLineColor(kBlue);
            for (int i = 0; i < 5; ++i)
            {
                fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                else {
                    fitter->SetParameter(i, theParamManager_[i]->getParValue());
                    fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                }
            }
            STDLINE("Fitting " + ss.str() + " with Gauss plus constant function", ACGreen);
            if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
            else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
        }
//------------------ LANDAU-GAUSS CONVOLUTION -------------------------------------------------------------------------
        else if (ui_->fitFuncLW->currentItem()->text() == "Langaus")
        {
            TF1 * fitter = new TF1(hName.c_str(), Utilities::langaus, xMin, xMax, 4);
            fitter->SetLineColor(kBlue);
            for (int i = 0; i < 4; ++i)
            {
                fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                else {
                    fitter->SetParameter(i, theParamManager_[i]->getParValue());
                    fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                }
            }
            STDLINE("Fitting Langaus function", ACGreen);
            if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
            else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
        }
//---------------ETA FUNCTION WITH CUT ON MIN CHARGE--------------------------------------------------------------------
        else if (ui_->fitFuncLW->currentItem()->text() == "Eta with Cut")
        {
            TF1 * fitter = new TF1(hName.c_str(), Utilities::etaFitFunc_cut, xMin, xMax, 7);
            fitter->SetLineColor(kGreen);
            for (int i = 0; i < 7; ++i)
            {
                fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                else {
                    fitter->SetParameter(i, theParamManager_[i]->getParValue());
                    fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                }
            }
            STDLINE("Fitting " + ss.str() + " with cut Eta function", ACGreen);
            if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
            else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
        }
//---------------ETA FUNCTION WITH DEPLETION VOLTAGE TO ZERO---------------------------------------------------------------
                else if (ui_->fitFuncLW->currentItem()->text() == "Eta (No Depletion)")
                {
                    TF1 * fitter = new TF1(hName.c_str(), Utilities::etaFitFunc2, xMin, xMax, 4);
                    fitter->SetLineColor(kGreen);
                    for (int i = 0; i < 4; ++i)
                    {
                        fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                        if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                        else {
                            fitter->SetParameter(i, theParamManager_[i]->getParValue());
                            fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                        }
                    }
                    STDLINE("Fitting " + ss.str() + " with cut Eta (No depletion) function", ACGreen);
                    if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
                    else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
                }
//---------------ETA FUNCTION WITH DEPLETION VOLTAGE TO ZERO AND CUT ON MIN CHARGE-------------------------------------------
                        else if (ui_->fitFuncLW->currentItem()->text() == "Eta (No Depletion) with Cut")
                        {
                            TF1 * fitter = new TF1(hName.c_str(), Utilities::etaFitFunc_cut2, xMin, xMax, 5);
                            fitter->SetLineColor(kGreen);
                            for (int i = 0; i < 5; ++i)
                            {
                                fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                                if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                                else {
                                    fitter->SetParameter(i, theParamManager_[i]->getParValue());
                                    fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                                }
                            }
                            STDLINE("Fitting " + ss.str() + " with cut Eta (No depletion) with Cut function", ACGreen);
                            if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
                            else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
                        }
//---------------GAUSS CONVOLUTED WITH A CONSTANT FUNCTION----------------------------------------------------------------------
                                else if (ui_->fitFuncLW->currentItem()->text() == "Gauss Convoluted with Constant")
                                {
                                    TF1 * fitter = new TF1(hName.c_str(), Utilities::uniformSideSmeared, xMin, xMax, 4);
                                    fitter->SetLineColor(kGreen);
                                    for (int i = 0; i < 4; ++i)
                                    {
                                        fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                                        if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                                        else {
                                            fitter->SetParameter(i, theParamManager_[i]->getParValue());
                                            fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                                        }
                                    }
                                    STDLINE("Fitting " + ss.str() + " with cut Gauss convoluted with constant", ACGreen);
                                    if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
                                    else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
                                }
//---------------ETA FUNCTION WITH DEPLETION VOLTAGE TO ZERO CONVOLUTED WITH CONSTANT SMEARED PLUS CONSTANT-----------------------------
                                else if (ui_->fitFuncLW->currentItem()->text() == "Eta Convoluted with Gauss")
                                {
                                    TF1 * fitter = new TF1(hName.c_str(), Utilities::etaSmeared, xMin, xMax, 9);
                                    fitter->SetLineColor(kGreen);
                                    for (int i = 0; i < 9; ++i)
                                    {
                                        fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                                        if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                                        else {
                                            fitter->SetParameter(i, theParamManager_[i]->getParValue());
                                            fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                                        }
                                    }
                                    STDLINE("Fitting " + ss.str() + " with cut Eta (No depletion) with Cut function convoluted with Gaussian", ACGreen);
                                    if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
                                    else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
                                }
//-------------------TRIAL OF ETA REPRODUCTION-------------------------------------------------------------------------------------------------------
        else if (ui_->fitFuncLW->currentItem()->text() == "Eta Reproduction")
        {
            TF1 * fitter = new TF1(hName.c_str(), Utilities::etaReproduction, xMin, xMax, 11);
            fitter->SetLineColor(kGreen);
            for (int i = 0; i < 11; ++i)
            {
                fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                else {
                    fitter->SetParameter(i, theParamManager_[i]->getParValue());
                    fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                }
            }
            STDLINE("Fitting " + ss.str() + " with cut Eta reproduction", ACGreen);
            if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
            else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
        }
//-------------------ETA DISTRIBUTION-------------------------------------------------------------------------------------------------------------
        else if (ui_->fitFuncLW->currentItem()->text() == "Eta Distribution")
        {
            TF1 * fitter = new TF1(hName.c_str(), Utilities::etaDistributionAngle, xMin, xMax, 6);
            fitter->SetLineColor(kBlue);
            for (int i = 0; i < 6; ++i)
            {
                fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                else {
                    fitter->SetParameter(i, theParamManager_[i]->getParValue());
                    fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                }
            }
            STDLINE("Fitting " + ss.str() + " with Eta distribution function", ACGreen);
            if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
            else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
        }


        else STDLINE("Select a function!", ACYellow);
        }
        else
        {
            TF1 * fitter = new TF1(hName.c_str(), ui_->fitFuncLE->text().toStdString().c_str());
            fitter->SetLineColor(kGreen);
            for (unsigned int i = 0; i < theParamManager_.size(); ++i)
            {
                fitter->SetParName(i, theParamManager_[i]->getParName().c_str());
                if (theParamManager_[i]->isParFixed()) fitter->FixParameter(i, theParamManager_[i]->getParValue());
                else {
                    fitter->SetParameter(i, theParamManager_[i]->getParValue());
                    fitter->SetParLimits(i, theParamManager_[i]->getParLimInf(), theParamManager_[i]->getParLimSup());
                }
            }
            STDLINE("Fitting " + ss.str() + " with cut Custom function", ACGreen);
            if (!ui_->fitLimitCB->isChecked()) toFit1D->Fit(fitter, fitMethod.c_str());
            else toFit1D->Fit(fitter, fitMethod.c_str(), "", ui_->fitRangeMinSB->value(), ui_->fitRangeMaxSB->value());
        }
    }
    else STDLINE("Method for 2D fitting not implemented!", ACRed);
    ++numberOfFitting_;

    ui_->fitPB->setEnabled(true);
}

std::string HNavigator::getObjectType (TObject * obj)
{
    std::string objectType = "Unknown" ;
    TKey* keyH = 0 ;

    TIter bases(obj->IsA()->GetListOfBases());
    int count = 0 ;
    while((keyH = (TKey*)bases()))
    {
        if( count++ == 0 )
            objectType = keyH->GetName() ;
    }
    return objectType ;
}

void HNavigator::on_fitLimitCB_clicked(bool checked)
{
    if (checked == false)
    {
        ui_->fitRangeMinSB->setEnabled(false);
        ui_->fitRangeMaxSB->setEnabled(false);
    }
    else
    {
        ui_->fitRangeMinSB->setEnabled(true);
        ui_->fitRangeMaxSB->setEnabled(true);
        if(theHTreeBrowser_->getCurrentObject())
        {
            ui_->fitRangeMinSB->setRange(((TH1F*)(theHTreeBrowser_->getCurrentObject()))->GetXaxis()->GetXmin(), ((TH1F*)(theHTreeBrowser_->getCurrentObject()))->GetXaxis()->GetXmax());
            ui_->fitRangeMinSB->setValue(((TH1F*)(theHTreeBrowser_->getCurrentObject()))->GetXaxis()->GetXmin());
            ui_->fitRangeMaxSB->setRange(((TH1F*)(theHTreeBrowser_->getCurrentObject()))->GetXaxis()->GetXmin(), ((TH1F*)(theHTreeBrowser_->getCurrentObject()))->GetXaxis()->GetXmax());
            ui_->fitRangeMaxSB->setValue(((TH1F*)(theHTreeBrowser_->getCurrentObject()))->GetXaxis()->GetXmax());
        }
    }
}

void HNavigator::on_customFunctionCB_clicked()
{
        for (unsigned int j = 0; j < theParamManager_.size(); ++j)
        {
            delete theParamManager_[j];
            ui_->parametersTW->removeTab(j);
        }
        theParamManager_.clear();
}

