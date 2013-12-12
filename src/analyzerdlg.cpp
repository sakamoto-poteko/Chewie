#include "analyzerdlg.h"
#include "ui_analyzerdlg.h"

#include "MessageTools.h"
#include "AnalysisManager.h"
#include "EventManager.h"
#include "WindowsManager.h"
#include "XmlParser.h"
#include "XmlWindow.h"
#include "XmlPlane.h"
#include "XmlAnalysis.h"
#include "XmlGeneral.h"
#include "XmlConverter.h"
#include "XmlScan.h"
#include "PixelMatrix.h"

#include "TFile.h"

#include <cstdlib>
#include <iostream>

#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrentRun>
#include <QTimer>
#include <QRegExp>
#include <QStandardItem>

//============================================================================================================================================
AnalyzerDlg::AnalyzerDlg(QWidget *parent) :
    QDialog            (parent                  )
  , ui                 (new Ui::AnalyzerDlg     )
  , theTimer_          (new QTimer              )
  , theFuture_         (new QFuture<void*>      )
  , theWatcher_        (new QFutureWatcher<void>)
  , theXmlParser_      (new XmlParser           )
  , theEventManager_   (new EventManager        )
  , theAnalysisManager_(new AnalysisManager     )
{
    ui->setupUi(this);
    setWindowTitle(QApplication::translate("AnalyzerDlg", "Chewie Analyzer", 0, QApplication::UnicodeUTF8));

    setInnerGeometry();

    ui->studiesTW->setCurrentWidget(ui->runT);
    //Settings
    ui->progressBar->reset();
    ui->scanListTW     ->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui->dutsListLW  ->addItem("Dut0");
    ui->dutsListLW  ->addItem("Dut1");
    ui->dutsListLW_2->addItem("Dut0");
    ui->dutsListLW_2->addItem("Dut1");
    theTimer_->setInterval(1000);//milliseconds
    openFileList_.clear();

    //Connections
    connect(theWatcher_             , SIGNAL(finished())       , this                    , SLOT(displayFinishedBox()));
    connect(theTimer_               , SIGNAL(timeout())        , this                    , SLOT(advanceProgressBar()));
    connect(ui->convertMaxEventsCB  , SIGNAL(isToggled(bool))  , ui->convertMaxEventsSB  , SLOT(setEnabled(bool)));
    connect(ui->runMaxEventsCB      , SIGNAL(isToggled(bool))  , ui->runMaxEventsSB      , SLOT(setEnabled(bool)));
    //connect(ui->efficiencyMainCutsCB, SIGNAL(clicked  (bool)), ui->efficiencyMainCutsLE, SLOT(setEnabled(bool)));
    //connect(theTimer_  , SIGNAL(timeout()) , this, SLOT(updateFileName())    );
    //PixelMatrix* pixelMatrix = new PixelMatrix(this);
    ui->pixelMatrixW->init();
}

//============================================================================================================================================
AnalyzerDlg::~AnalyzerDlg(void)
{
    delete theAnalysisManager_;
    delete theEventManager_;
    delete theXmlParser_;
    delete theWatcher_;
    delete theFuture_;
    delete theTimer_;
    delete ui;
}

//============================================================================================================================================
void AnalyzerDlg::setInnerGeometry(void)
{
    ui->detectorsInfoTW     ->init();
    ui->efficiencyMainCutsLE->setInnerGeometry(ui->efficiencyMainCutsLE->geometry());
    ui->efficiencyCuts2LE   ->setInnerGeometry(ui->efficiencyCuts2LE   ->geometry());
    ui->efficiencyCuts3LE   ->setInnerGeometry(ui->efficiencyCuts3LE   ->geometry());
    ui->efficiencyCuts4LE   ->setInnerGeometry(ui->efficiencyCuts4LE   ->geometry());
    ui->chargeMainCutsLE    ->setInnerGeometry(ui->chargeMainCutsLE    ->geometry());
    ui->chargeCuts2LE       ->setInnerGeometry(ui->chargeCuts2LE       ->geometry());
    ui->chargeCuts3LE       ->setInnerGeometry(ui->chargeCuts3LE       ->geometry());
    ui->chargeCuts4LE       ->setInnerGeometry(ui->chargeCuts4LE       ->geometry());
    ui->chargeCuts5LE       ->setInnerGeometry(ui->chargeCuts5LE       ->geometry());
    ui->chargeCuts6LE       ->setInnerGeometry(ui->chargeCuts6LE       ->geometry());
    ui->par0minLE           ->setInnerGeometry(ui->par0minLE           ->geometry());
    ui->par0maxLE           ->setInnerGeometry(ui->par0maxLE           ->geometry());
    ui->par1minLE           ->setInnerGeometry(ui->par1minLE           ->geometry());
    ui->par1maxLE           ->setInnerGeometry(ui->par1maxLE           ->geometry());
    ui->par2minLE           ->setInnerGeometry(ui->par2minLE           ->geometry());
    ui->par2maxLE           ->setInnerGeometry(ui->par2maxLE           ->geometry());
    ui->par3minLE           ->setInnerGeometry(ui->par3minLE           ->geometry());
    ui->par3maxLE           ->setInnerGeometry(ui->par3maxLE           ->geometry());
    ui->resolutionMainCutsLE->setInnerGeometry(ui->resolutionMainCutsLE->geometry());
    ui->resolutionCuts2LE   ->setInnerGeometry(ui->resolutionCuts2LE   ->geometry());
    ui->resolutionCuts3LE   ->setInnerGeometry(ui->resolutionCuts3LE   ->geometry());
    ui->resolutionCuts4LE   ->setInnerGeometry(ui->resolutionCuts4LE   ->geometry());
    ui->convertMaxEventsSB  ->init();
    ui->convertMaxThreadsSB ->init();
    ui->runMaxEventsSB      ->init();
    ui->runMaxThreadsSB     ->init();
    ui->priorityWindowsSB   ->init();
    ui->priorityChargeSB    ->init();
    ui->priorityEfficiencySB->init();
    ui->priorityResolutionSB->init();
    ui->priorityTracksSB    ->init();
    ui->priorityTracksAfterSB->init();
    ui->lowerRowDUT0LE      ->setInnerGeometry(ui->lowerRowDUT0LE      ->geometry());
    ui->lowerColDUT0LE      ->setInnerGeometry(ui->lowerColDUT0LE      ->geometry());
    ui->higherRowDUT0LE     ->setInnerGeometry(ui->higherRowDUT0LE     ->geometry());
    ui->higherColDUT0LE     ->setInnerGeometry(ui->higherColDUT0LE     ->geometry());
    ui->lowerRowDUT1LE      ->setInnerGeometry(ui->lowerRowDUT1LE      ->geometry());
    ui->lowerColDUT1LE      ->setInnerGeometry(ui->lowerColDUT1LE      ->geometry());
    ui->higherRowDUT1LE     ->setInnerGeometry(ui->higherRowDUT1LE     ->geometry());
    ui->higherColDUT1LE     ->setInnerGeometry(ui->higherColDUT1LE     ->geometry());
    ui->xROCdut0LE          ->setInnerGeometry(ui->xROCdut0LE          ->geometry());
    ui->yROCdut0LE          ->setInnerGeometry(ui->yROCdut0LE          ->geometry());
    ui->xROCdut1LE          ->setInnerGeometry(ui->xROCdut1LE          ->geometry());
    ui->yROCdut1LE          ->setInnerGeometry(ui->yROCdut1LE          ->geometry());
    ui->ZposDUT0LE          ->setInnerGeometry(ui->ZposDUT0LE          ->geometry());
    ui->ZrotDUT0LE          ->setInnerGeometry(ui->ZrotDUT0LE          ->geometry());
    ui->ZposDUT1LE          ->setInnerGeometry(ui->ZposDUT1LE          ->geometry());
    ui->ZrotDUT1LE          ->setInnerGeometry(ui->ZrotDUT1LE          ->geometry());
}

//============================================================================================================================================
std::map<std::string,TFile*> AnalyzerDlg::getOpenFiles(void)
{
    const std::string& outFileName = theAnalysisManager_->getOutputFileName();

    if( outFileName != "" && !(openFileList_.find(outFileName) != openFileList_.end()) )
    {
        openFileList_[outFileName] = theAnalysisManager_->getOutputFile();
    }
    for(std::map<std::string,TFile*>::iterator it=openFileList_.begin(); it != openFileList_.end(); it++)
    {
        if(!it->second->IsOpen())
        {
            openFileList_.erase(it);
        }
    }
    return openFileList_;
}

//============================================================================================================================================
QString AnalyzerDlg::getEnvPath(QString environmentName)
{
    QString environmentValue = getenv(environmentName.toStdString().c_str()) ;

    if(environmentValue.isEmpty())
    {
        environmentValue = "./" ;
        std::stringstream ss;
        ss << ACRed << ACBold
           << "WARNING: "
           << ACPlain
           << "environment variable "
           << environmentName.toStdString()
           << " is undefined. Assuming "
           << environmentValue.toStdString()
           << " as its value." ;
        STDLINE(ss.str(),ACYellow) ;
    }
    if(environmentValue.at(environmentValue.size()-1) != '/') environmentValue += '/';
    return environmentValue ;
}

//============================================================================================================================================
void* AnalyzerDlg::threaderMethod(void* myClass, void* whatToRun, int )//Last variable are possible arguments to give to the method to be called
{
    AnalyzerDlg* theWindow = (AnalyzerDlg*) myClass;
    theWindow->whatToRun_ = QString((const char*)whatToRun);

    if(theWindow->whatToRun_ == "AnalysisManager::startSequence")
        theWindow->theAnalysisManager_->startSequence();
    else if(theWindow->whatToRun_ == "EventManager::startCalibrationManager")
        theWindow->theEventManager_->startCalibrationManager();
    else if(theWindow->whatToRun_ == "EventManager::startConverter")
        theWindow->theEventManager_->startConverter();
    else
    {
        FATAL((theWindow->whatToRun_ + " is not a recognized threaded operation!").toStdString(),ACRed);
        exit(EXIT_FAILURE);
    }
    return 0;
}

//============================================================================================================================================
void AnalyzerDlg::displayFinishedBox(void)
{
    theTimer_->stop();
    STDLINE("All events have been processed",ACPurple) ;
    ui->runningThreadsLE->setText("0");
    ui->progressBar->setValue(ui->progressBar->maximum());
    ui->activityLB->setText("Idle");
    if(whatToRun_ == "EventManager::startConverter")
    {
        ui->runConverterPB->setEnabled(true);
        ui->processedEventsLE->setText(QString("%1").arg(theEventManager_->getProcessedEvents()));
    }
    else if(whatToRun_ == "AnalysisManager::startSequence")
    {
        ui->runAnalysisPB->setEnabled(true);
        ui->processedEventsLE->setText(QString("%1").arg(theAnalysisManager_->getCurrentEntry()));
    }
    QMessageBox::information(this, tr("Chewie"), tr("Done"));
}

//============================================================================================================================================
void AnalyzerDlg::updateFileName(void)
{
/*
    QString     entries    ;
    QString     inFileName1;
    QString     inFileName2;
    QString     outFileName;
    QLineEdit*  inFileName1LE = 0;
    QLineEdit*  inFileName2LE = 0;
    QLineEdit*  outFileNameLE = 0;
    QLineEdit*  eventsLE      = 0;

    if(whatToRun_ == "AnalysisManager::startSequence")
    {
        //inFileName1 = QString(theAnalysisManager_->getInFileName().c_str());
        inFileName1LE = ui->loadedNtupleFileLE;
        eventsLE = ui->totalEventsLE;
        //QTextStream(&entries) << theAnalysisManager_->getEntries();
    }
    else if(whatToRun_ == "EventManager::startConverter")
    {
        inFileName1 = QString(theEventManager_->getInRootFileName().c_str());
        inFileName2 = QString(theEventManager_->getInGeoFileName().c_str());
        outFileName = QString(theEventManager_->getOutFileName().c_str());
        inFileName1LE = ui->loadedRootFileLE;
        inFileName2LE = ui->loadedGeoFileLE;
        outFileNameLE = ui->convertedFileLE;
        eventsLE = ui->eventsConverterLE;
        QTextStream(&entries) << theEventManager_->getEntries();
    }

    if(entries != eventsLE->displayText())
        eventsLE->setText(entries);

    if(inFileName1 != inFileName1LE->displayText())
        inFileName1LE->setText(inFileName1);

    if(inFileName2LE != 0 && inFileName2 != inFileName2LE->displayText())
        inFileName2LE->setText(inFileName2);

    if(outFileNameLE != 0 && outFileName != outFileNameLE->displayText())
        outFileNameLE->setText(outFileName);
*/
}

//============================================================================================================================================
void AnalyzerDlg::advanceProgressBar(void)
{
    int         completionStatus = 0     ;
    bool        isFinished       = false ;
    std::string currentOperation = "Idle";
    int         numberOfThreads  = 0;

    if(whatToRun_ == "AnalysisManager::startSequence")
    {
        completionStatus = theAnalysisManager_->getCompletionStatus();
        currentOperation = theAnalysisManager_->getCurrentOperation();
        isFinished       = theAnalysisManager_->isFinished();
        numberOfThreads  = theAnalysisManager_->getNumberOfThreads();
    }
    else if(whatToRun_ == "EventManager::startConverter")
    {
        completionStatus = theEventManager_->getCompletionStatus();
        currentOperation = theEventManager_->getCurrentOperation();
        isFinished       = theEventManager_->isFinished();
        numberOfThreads  = theEventManager_->getNumberOfThreads();
    }

    ui->activityLB->setText(QString::fromStdString(currentOperation));
    if(!isFinished)
    {
        ui->progressBar->setValue(completionStatus);
        ui->runningThreadsLE->setText(QString("%1").arg(numberOfThreads));
    }
    else
    {
        theTimer_->stop();
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_abortPB_clicked(void)
{
    if(whatToRun_ == "AnalysisManager::startSequence")
    {
        theAnalysisManager_->stopSequence();
        theFuture_->cancel();
    }
    else if(whatToRun_ == "EventManager::startConverter")
    {
        theEventManager_->stopConverter();
        theFuture_->cancel();
    }
    else
    {
        STDLINE(std::string("Don't know how to abort ") + whatToRun_.toStdString(), ACRed);
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_openRootFilePB_clicked(void)
{
    QString dataDir = this->getEnvPath("CHEWIEDATADIR");
    QStringList fileNames = QFileDialog::getOpenFileNames(this,"Root ntuple files",dataDir,"Root Ntuple Files(*.root)");
    if(fileNames.isEmpty())
        return;

    fileNames.sort();
    ui->runConverterPB->setEnabled(true);
    QStandardItemModel* model;
    if(!(model = (QStandardItemModel*)ui->convertFileListTV->model()))
    {
        //ui->runFileListTV->verticalHeader()->setDefaultSectionSize(20);
        model = new QStandardItemModel(fileNames.size(),4,this);
        ui->convertFileListTV->setModel(model);
    }
    else
        model->clear();

    std::vector<std::string> stdFileList;
    for(int f=0; f<fileNames.size(); f++)
    {
        QString fileName = fileNames.at(f);
        QStandardItem* directoryItem         = new QStandardItem(fileName.section('/',0,-2));
        fileName = fileName.section('/',-1);
        QStandardItem* fileNameItem          = new QStandardItem(fileName);
        QStandardItem* geoFileNameItem       = new QStandardItem(fileName.replace(fileName.lastIndexOf(".root"),5,".geo"));
        QStandardItem* convertedFileNameItem = new QStandardItem(fileName.replace(fileName.lastIndexOf(".geo"),4,"_Converted.root"));
        model->setItem(f,0,directoryItem);
        model->setItem(f,1,fileNameItem);
        model->setItem(f,2,geoFileNameItem);
        model->setItem(f,3,convertedFileNameItem);
        stdFileList.push_back(fileNames.at(f).toStdString());
    }
    model->setHeaderData(0, Qt::Horizontal, "Directory");
    model->setHeaderData(1, Qt::Horizontal, "Files");
    model->setHeaderData(2, Qt::Horizontal, "Geo");
    model->setHeaderData(3, Qt::Horizontal, "Output");
    ui->convertFileListTV->resizeColumnsToContents();

    theEventManager_->setInFilesList(stdFileList);
    ui->convertTotalEventsLE->setText(QString("%1").arg(theEventManager_->calculateTotalEvents()));
}
/*
//============================================================================================================================================
void AnalyzerDlg::on_openGeoFilePB_clicked(void)
{
    QString localPath = this->getEnvPath("CHEWIEDATADIR") ;
    QString fileName = QFileDialog::getOpenFileName(this,"Geometry files",localPath,"Geometry Files(*.geo)");
    if(fileName.isEmpty())
        return ;

    this->openGeoFile(fileName);
}
*/
//============================================================================================================================================
void AnalyzerDlg::on_runConverterPB_clicked(void)
{
    ui->runConverterPB->setEnabled(false);
    if(ui->convertMaxEventsCB->isChecked())
        theEventManager_->setMaxEvents(ui->convertMaxEventsSB->value());
    else
        theEventManager_->setMaxEvents(-1);

    ui->progressBar->reset();
    theTimer_->start();
    *theFuture_ = QtConcurrent::run(threaderMethod,this,(void*)"EventManager::startConverter",0);
    theWatcher_->setFuture(*theFuture_);
}

//============================================================================================================================================
void AnalyzerDlg::on_calibrationLoadGeoFilePB_clicked(void)
{
    QString localPath = this->getEnvPath("CHEWIEDATADIR") ;
    QString fileName = QFileDialog::getOpenFileName(this,"Geometry files",localPath,"Geometry Files(*.geo)");
    if(fileName.isEmpty())
        return ;

    theEventManager_->openGeoFile(fileName.toStdString());
    ui->loadedGeoFileLE_2->setText(fileName);
    ui->loadedGeoFileLE_2->setToolTip(fileName);

    QString outFileName = localPath + "/calibrationsPlots.root";
    theEventManager_->openCalibPlotsFile(outFileName.toStdString());
    ui->calibPlotsFileLE->setText(outFileName);
    ui->calibPlotsFileLE->setToolTip(outFileName);
}

//============================================================================================================================================
void AnalyzerDlg::on_openCalibrationsFilePB_clicked(void)
{
    QString localPath = this->getEnvPath("Monicelli_CalSample_Dir");
    QString fileName = QFileDialog::getOpenFileName(this,"Calibrations File",localPath,"Calibrations Files(*.txt)");
    if(fileName.isEmpty())
        return ;

    STDLINE("Calibration file: " + fileName.toStdString() + " succesfully loaded!", ACYellow);

    ui->loadedCalibFileLE->setText(fileName);
    ui->loadedCalibFileLE->setToolTip(fileName);

    theEventManager_->setCalibrationsFileName(fileName.toStdString());
}

//============================================================================================================================================
void AnalyzerDlg::on_calibParsCB_stateChanged(int)
{
    if(ui->calibParsCB->isChecked())
        theEventManager_->addCalibAnalysis("parameters plots");
    else
        theEventManager_->removeCalibAnalysis("parameters plots");
}

//============================================================================================================================================
void AnalyzerDlg::on_calibDispersionCB_stateChanged(int)
{
    if(ui->calibDispersionCB->isChecked())
        theEventManager_->addCalibAnalysis("dispersion plots");
    else
        theEventManager_->removeCalibAnalysis("dispersion plots");

}
//============================================================================================================================================
void AnalyzerDlg::on_makeCalibPlotsPB_clicked(void)
{
    if(!theXmlParser_->getDocument())
    {
        STDLINE("Load configuration file (.xml)!",ACRed);
        return;
    }

    if(ui->dutsListLW->selectedItems().size() == 0)
    {
        STDLINE("Select a dut from the list!",ACRed);
        return;
    }

    QString whichDut = "";

    if(ui->dutsListLW->findItems("Dut0",0)[0]->isSelected())
    {
        theEventManager_->whichDut("Dut0");
        whichDut = "Dut0";
    }
    else if(ui->dutsListLW->findItems("Dut1",0)[0]->isSelected())
    {
        theEventManager_->whichDut("Dut1");
        whichDut = "Dut1";
    }

    *theFuture_ = QtConcurrent::run(threaderMethod,this,(void*)"EventManager::startCalibrationManager",0);
    theWatcher_->setFuture(*theFuture_);

    ui->useCalibrationsCB->setEnabled(true);
    ui->dutsListLW_2->setEnabled(true);
    ui->dutsListLW_2->findItems(whichDut,0)[0]->setSelected(true);
}

//============================================================================================================================================
void AnalyzerDlg::on_openCalibrationPlotsPB_clicked(void)
{
    if(!theXmlParser_->getDocument())
    {
        STDLINE("Load configuration file (.xml)!",ACRed);
        return;
    }

    QString localPath = this->getEnvPath("CHEWIEDATADIR") ;
    QString fileName = QFileDialog::getOpenFileName(this,"Calibration plots",localPath,"Calibration Plots(*.root)");
    if(fileName.isEmpty())
        return ;

    ui->calibPlotsFileLE->setText(fileName);
    ui->calibPlotsFileLE->setToolTip(fileName);

    ui->useCalibrationsCB->setEnabled(true);

    for(int r=0; r<2; r++)
        ui->dutsListLW_2->item(r)->setSelected(false);
}

//============================================================================================================================================
void AnalyzerDlg::on_useCalibrationsCB_stateChanged(int)
{
    if(ui->useCalibrationsCB->isChecked())
        ui->dutsListLW_2->setEnabled(true);
    else
    {
        ui->dutsListLW_2->setEnabled(false);

        QListWidgetItem* item    ;
        QString          whichDut;

        for(int r=0; r<2; r++)
        {
            item = ui->dutsListLW_2->item(r);
            whichDut = item->text();
            std::cout << whichDut.toStdString() << " " << (int)(theXmlParser_->getPlanes())[whichDut.toStdString()]->useCalibrations() << std::endl;
            if((theXmlParser_->getPlanes())[whichDut.toStdString()]->useCalibrations())
                (theXmlParser_->getPlanes())[whichDut.toStdString()]->getCalibNode().toElement().setAttribute("use","no");
        }
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_dutsListLW_2_itemSelectionChanged(void)
{
    QListWidgetItem* item    ;
    QString          whichDut;

    for(int r=0; r<2; r++)
    {
        item = ui->dutsListLW_2->item(r);
        whichDut = item->text();
        if(item->isSelected())
        {
            QDomNode file = (theXmlParser_->getPlanes())[whichDut.toStdString()]->getCalibNode().toElement().elementsByTagName("file").at(0);
            (theXmlParser_->getPlanes())[whichDut.toStdString()]->getCalibNode().removeChild(file);
            QDomText text = theXmlParser_->getDocument()->createTextNode(ui->calibPlotsFileLE->text());
            QDomElement newEl = theXmlParser_->getDocument()->createElement("file");
            newEl.appendChild(text);
            (theXmlParser_->getPlanes())[whichDut.toStdString()]->getCalibNode().appendChild(newEl);
            (theXmlParser_->getPlanes())[whichDut.toStdString()]->getCalibNode().toElement().setAttribute("use","yes");
        }
        else
            (theXmlParser_->getPlanes())[whichDut.toStdString()]->getCalibNode().toElement().setAttribute("use","no");
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_openNtupleFilePB_clicked(void)
{
    QString dataDir = this->getEnvPath("CHEWIEINPUTDIR");
    QStringList fileNames = QFileDialog::getOpenFileNames(this,"Root ntuple files",dataDir,"Root Ntuple Files(*.root)");
    if(fileNames.isEmpty())
        return;

    fileNames.sort();
    ui->runAnalysisPB->setEnabled(true);
    QStandardItemModel* model;
    if(!(model = (QStandardItemModel*)ui->runFileListTV->model()))
    {
        //ui->runFileListTV->verticalHeader()->setDefaultSectionSize(20);
        model = new QStandardItemModel(fileNames.size(),1,this);
        ui->runFileListTV->setModel(model);
    }
    else
        model->clear();

    std::vector<std::string> stdFileList;
    for(int f=0; f<fileNames.size(); f++)
    {
        QStandardItem* fileNameItem = new QStandardItem(fileNames.at(f));
        model->setItem(f,0,fileNameItem);
        stdFileList.push_back(fileNames.at(f).toStdString());
    }
    model->setHeaderData(0, Qt::Horizontal, "Files");
    ui->runFileListTV->resizeColumnsToContents();

    theAnalysisManager_->setInFilesList(stdFileList);

    ui->runTotalEventsLE->setText(QString("%1").arg(theAnalysisManager_->calculateTotalEvents()));

    QString outputDir = this->getEnvPath("CHEWIEOUTPUTDIR");
    QString outFileName = "Chewie.root";
    int firstRun   = 0x7fffffff;
    int lastRun    = -1;
    int currentRun = 0;
    QRegExp runFile(".*/Run(\\d+).*.root") ;
    for(int f=0; f<fileNames.size(); f++)
    {
        if (runFile.exactMatch(fileNames.at(f)))
        {
            currentRun = runFile.cap(1).toInt();
            if(firstRun > currentRun) firstRun = currentRun;
            if(lastRun  < currentRun) lastRun  = currentRun;
        }
    }
    if(firstRun != 0x7fffffff && lastRun != -1)
    {
        if(firstRun == lastRun)
            outFileName = "Chewie_Run" + QString("%1").arg(firstRun) + ".root";
        else
            outFileName = "Chewie_Runs" + QString("%1_%2").arg(firstRun).arg(lastRun) + ".root";
    }
    ui->outputFileLE->setText(outputDir + outFileName);
    theAnalysisManager_->setOutputFileName(ui->outputFileLE->text().toStdString());
}

//============================================================================================================================================
void AnalyzerDlg::on_runAnalysisPB_clicked(void)
{
    ui->runAnalysisPB->setEnabled(false);
    if(ui->runMaxEventsCB->isChecked())
        theAnalysisManager_->setMaxEvents(ui->runMaxEventsSB->value());
    else
        theAnalysisManager_->setMaxEvents(-1);

    ui->progressBar->reset();
    theTimer_->start();
    *theFuture_ = QtConcurrent::run(threaderMethod,this,(void*)"AnalysisManager::startSequence",0);
    theWatcher_->setFuture(*theFuture_);
}

//============================================================================================================================================
void AnalyzerDlg::on_removePxlDUT0PB_clicked(void)
{
    if(!theXmlParser_->getDocument())
    {
        STDLINE("Load configuration file (.xml)!",ACRed);
        return;
    }

    QDomElement removedPxl = theXmlParser_->getDocument()->createElement("removedPixel");
    removedPxl.setAttribute("row",ui->removeRowDUT0LE->displayText());
    removedPxl.setAttribute("col",ui->removeColDUT0LE->displayText());
    theXmlParser_->getPlanes()["Dut0"]->getWindow()->getNode().appendChild(removedPxl);
}

//============================================================================================================================================
void AnalyzerDlg::on_removePxlDUT1PB_clicked(void)
{
    if(!theXmlParser_->getDocument())
    {
        STDLINE("Load configuration file (.xml)!",ACRed);
        return;
    }

    QDomElement removedPxl = theXmlParser_->getDocument()->createElement("removedPixel");
    removedPxl.setAttribute("row",ui->removeRowDUT1LE->displayText());
    removedPxl.setAttribute("col",ui->removeColDUT1LE->displayText());
    theXmlParser_->getPlanes()["Dut1"]->getWindow()->getNode().appendChild(removedPxl);
}

//============================================================================================================================================
void AnalyzerDlg::on_resetRemovedPxlsDUT0PB_clicked(void)
{
    if(!theXmlParser_->getDocument())
    {
        STDLINE("Load configuration file (.xml)!",ACRed);
        return;
    }

    QDomNodeList removedPxls = theXmlParser_->getPlanes()["Dut0"]->getWindow()->getNode().toElement().elementsByTagName("removedPixel");

    while(removedPxls.size()>0)
        theXmlParser_->getPlanes()["Dut0"]->getWindow()->getNode().removeChild(removedPxls.at(removedPxls.size()-1));
}

//============================================================================================================================================
void AnalyzerDlg::on_resetRemovedPxlsDUT1PB_clicked(void)
{
    if(!theXmlParser_->getDocument())
    {
        STDLINE("Load configuration file (.xml)!",ACRed);
        return;
    }

    QDomNodeList removedPxls = theXmlParser_->getPlanes()["Dut1"]->getWindow()->getNode().toElement().elementsByTagName("removedPixel");

    while(removedPxls.size()>0)
        theXmlParser_->getPlanes()["Dut1"]->getWindow()->getNode().removeChild(removedPxls.at(removedPxls.size()-1));
}
//============================================================================================================================================
void AnalyzerDlg::on_saveOutputFilePB_clicked(void)
{
//    QString localPath = this->getEnvPath("CHEWIEDATADIR");
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save File"),ui->outputFileLE->text(),tr("Root files (*.root)"));
    ui->outputFileLE->setText(fileName);

    if(fileName.isEmpty())
        return;

    theAnalysisManager_->setOutputFileName(ui->outputFileLE->text().toStdString());
}

//============================================================================================================================================
void AnalyzerDlg::on_openConfigurationFilePB_clicked(void)
{
    QString localPath = this->getEnvPath("CHEWIEXMLDIR");
    QString fileName = QFileDialog::getOpenFileName(this,"Xml configuration file",localPath,"Xml Files(*.xml)");
    if(fileName.isEmpty())
    {
        ui->studiesTW->setEnabled(false);
        ui->loadedConfigurationFileLE->setText("Load configuration file (.xml)");
        theXmlParser_->destroy();
        return ;
    }
    ui->studiesTW->setEnabled(true);
    ui->loadedConfigurationFileLE->setText(fileName);

    if(!theXmlParser_->parseDocument(fileName))
    {
        QMessageBox::information(this, tr("Chewie"), tr((QString("There was a problem parsing ") + fileName + ". Check the Standard output for error message!").toAscii()));
        return;
    }


    theEventManager_   ->setConfiguration(theXmlParser_);
    theAnalysisManager_->setConfiguration(theXmlParser_);

    ui->convertMaxThreadsSB->valueIsAttribute     (true                                                                                 );
    ui->convertMaxThreadsSB->setValue             ("Threads",theXmlParser_->getConverter()->getNumberOfThreads()                        );
    ui->convertMaxThreadsSB->assignXmlElement     (theXmlParser_->getConverter()->getNode()                                             );

    ui->convertMaxEventsCB->setChecked            ("IsToLimit",theXmlParser_->getConverter()->isToLimit()                               );
    ui->convertMaxEventsCB->assignXmlElement      (theXmlParser_->getConverter()->getNode()                                             );

    ui->convertMaxEventsSB->setEnabled(ui->convertMaxEventsCB->isChecked());
    ui->convertMaxEventsSB->valueIsAttribute      (true                                                                                 );
    ui->convertMaxEventsSB->setValue              ("MaxEvents",theXmlParser_->getConverter()->getMaxEvents()                            );
    ui->convertMaxEventsSB->assignXmlElement      (theXmlParser_->getConverter()->getNode()                                             );

    ui->runMaxThreadsSB->valueIsAttribute         (true                                                                                 );
    ui->runMaxThreadsSB->setValue                 ("Threads",theXmlParser_->getGeneral()->getNumberOfThreads()                          );
    ui->runMaxThreadsSB->assignXmlElement         (theXmlParser_->getGeneral()->getNode()                                               );

    ui->runMaxEventsCB->setChecked                ("IsToLimit",theXmlParser_->getGeneral()->isToLimit()                                 );
    ui->runMaxEventsCB->assignXmlElement          (theXmlParser_->getGeneral()->getNode()                                               );

    ui->runMaxEventsSB->setEnabled(ui->runMaxEventsCB->isChecked());
    ui->runMaxEventsSB->valueIsAttribute          (true                                                                                 );
    ui->runMaxEventsSB->setValue                  ("AnalysesMaxEvents",theXmlParser_->getGeneral()->getAnalysesMaxEvents()              );
    ui->runMaxEventsSB->assignXmlElement          (theXmlParser_->getGeneral()->getNode()                                               );

    ui->maxWindowEventsLE->textIsAttribute        (true                                                                                 );
    ui->maxWindowEventsLE->setText                ("PreAnalysisMaxEvents",theXmlParser_->getGeneral()->getPreAnalysisMaxEvents()        );
    ui->maxWindowEventsLE->assignXmlElement       (theXmlParser_->getGeneral()->getNode()                                               );

    ui->tracksCB->setChecked                      ("IsToRun",(theXmlParser_->getAnalysesFromString("Tracks"))->isToRun()                        );
    ui->tracksCB->assignXmlElement                ((theXmlParser_->getAnalysesFromString("Tracks"))->getNode()                                  );

    ui->tracksafterCB->setChecked                 ("IsToRun",(theXmlParser_->getAnalysesFromString("TracksAfter"))->isToRun()                   );
    ui->tracksafterCB->assignXmlElement           ((theXmlParser_->getAnalysesFromString("TracksAfter"))->getNode()                             );

    ui->efficiencyCB->setChecked                  ("IsToRun",(theXmlParser_->getAnalysesFromString("Efficiency"))->isToRun()                    );
    ui->efficiencyCB->assignXmlElement            ((theXmlParser_->getAnalysesFromString("Efficiency"))->getNode()                              );

    ui->efficiencyStandardCutsCB->setChecked      ("ApplyStandardCuts",(theXmlParser_->getAnalysesFromString("Efficiency"))->applyStandardCuts());
    ui->efficiencyStandardCutsCB->assignXmlElement((theXmlParser_->getAnalysesFromString("Efficiency"))->getNode());

    ui->chargeCB->setChecked                      ("IsToRun",(theXmlParser_->getAnalysesFromString("Charge"))->isToRun()                        );
    ui->chargeCB->assignXmlElement                ((theXmlParser_->getAnalysesFromString("Charge"))->getNode()                                  );

    ui->chargeStandardCutsCB->setChecked          ("ApplyStandardCuts",(theXmlParser_->getAnalysesFromString("Efficiency"))->applyStandardCuts());
    ui->chargeStandardCutsCB->assignXmlElement    ((theXmlParser_->getAnalysesFromString("Efficiency"))->getNode());

    ui->resolutionCB->setChecked                  ("IsToRun",(theXmlParser_->getAnalysesFromString("Resolution"))->isToRun()                    );
    ui->resolutionCB->assignXmlElement            ((theXmlParser_->getAnalysesFromString("Resolution"))->getNode()                              );

    ui->resolutionStandardCutsCB->setChecked      ("ApplyStandardCuts",(theXmlParser_->getAnalysesFromString("Efficiency"))->applyStandardCuts());
    ui->resolutionStandardCutsCB->assignXmlElement((theXmlParser_->getAnalysesFromString("Efficiency"))->getNode());

    ui->windowsCB->setChecked                     ("IsToRun",(theXmlParser_->getAnalysesFromString("Windows"))->isToRun()                       );
    ui->windowsCB->assignXmlElement               ((theXmlParser_->getAnalysesFromString("Windows"))->getNode()                                 );

    ui->priorityWindowsSB->valueIsAttribute       (true                                                                                         );
    ui->priorityWindowsSB->setValue               ("Priority", (theXmlParser_->getAnalysesFromString("Windows"))->getPriority()                 );
    ui->priorityWindowsSB->assignXmlElement       ((theXmlParser_->getAnalysesFromString("Windows"))->getNode()                                 );


    ui->priorityChargeSB->valueIsAttribute        (true                                                                                         );
    ui->priorityChargeSB->setValue                ("Priority", (theXmlParser_->getAnalysesFromString("Charge"))->getPriority()                  );
    ui->priorityChargeSB->assignXmlElement        ((theXmlParser_->getAnalysesFromString("Charge"))->getNode()                                  );


    ui->priorityEfficiencySB->valueIsAttribute    (true                                                                                         );
    ui->priorityEfficiencySB->setValue            ("Priority", (theXmlParser_->getAnalysesFromString("Efficiency"))->getPriority()              );
    ui->priorityEfficiencySB->assignXmlElement    ((theXmlParser_->getAnalysesFromString("Efficiency"))->getNode()                              );


    ui->priorityResolutionSB->valueIsAttribute    (true                                                                                         );
    ui->priorityResolutionSB->setValue            ("Priority", (theXmlParser_->getAnalysesFromString("Resolution"))->getPriority()              );
    ui->priorityResolutionSB->assignXmlElement    ((theXmlParser_->getAnalysesFromString("Resolution"))->getNode()                              );


    ui->priorityTracksSB->valueIsAttribute        (true                                                                                         );
    ui->priorityTracksSB->setValue                ("Priority", (theXmlParser_->getAnalysesFromString("Tracks"))->getPriority()                  );
    ui->priorityTracksSB->assignXmlElement        ((theXmlParser_->getAnalysesFromString("Tracks"))->getNode()                                  );


    ui->priorityTracksAfterSB->valueIsAttribute   (true                                                                                         );
    ui->priorityTracksAfterSB->setValue           ("Priority", (theXmlParser_->getAnalysesFromString("TracksAfter"))->getPriority()             );
    ui->priorityTracksAfterSB->assignXmlElement   ((theXmlParser_->getAnalysesFromString("TracksAfter"))->getNode()                             );


    ui->efficiencyMainCutsLE->textIsAttribute     (true                                                                                         );
    ui->efficiencyMainCutsLE->setText             ("CutString",(theXmlParser_->getAnalysesFromString("Efficiency"))->getCut("main cut")         );
    ui->efficiencyMainCutsLE->assignXmlElement    ((theXmlParser_->getAnalysesFromString("Efficiency"))->getCutNode("main cut")                 );

    ui->efficiencyCuts2LE->textIsAttribute        (true                                                                                         );
    ui->efficiencyCuts2LE->setText                ("CutString",(theXmlParser_->getAnalysesFromString("Efficiency"))->getCut("cell efficiency")  );
    ui->efficiencyCuts2LE->assignXmlElement       ((theXmlParser_->getAnalysesFromString("Efficiency"))->getCutNode("cell efficiency")          );

    ui->efficiencyCuts3LE->textIsAttribute        (true                                                                                         );
    ui->efficiencyCuts3LE->setText                ("CutString",(theXmlParser_->getAnalysesFromString("Efficiency"))->getCut("cell efficiency X"));
    ui->efficiencyCuts3LE->assignXmlElement       ((theXmlParser_->getAnalysesFromString("Efficiency"))->getCutNode("cell efficiency X")        );

    ui->efficiencyCuts4LE->textIsAttribute        (true                                                                                         );
    ui->efficiencyCuts4LE->setText                ("CutString",(theXmlParser_->getAnalysesFromString("Efficiency"))->getCut("cell efficiency Y"));
    ui->efficiencyCuts4LE->assignXmlElement       ((theXmlParser_->getAnalysesFromString("Efficiency"))->getCutNode("cell efficiency Y")        );

    ui->chargeMainCutsLE->textIsAttribute         (true                                                                                         );
    ui->chargeMainCutsLE->setText                 ("cutString",(theXmlParser_->getAnalysesFromString("Charge"))->getCut("main cut")             );
    ui->chargeMainCutsLE->assignXmlElement        ((theXmlParser_->getAnalysesFromString("Charge"))->getCutNode("main cut")                     );

    ui->chargeCuts2LE->textIsAttribute            (true                                                                                         );
    ui->chargeCuts2LE->setText                    ("cutString",(theXmlParser_->getAnalysesFromString("Charge"))->getCut("cell charge")          );
    ui->chargeCuts2LE->assignXmlElement           ((theXmlParser_->getAnalysesFromString("Charge"))->getCutNode("cell charge")                  );

    ui->chargeCuts3LE->textIsAttribute            (true                                                                                         );
    ui->chargeCuts3LE->setText                    ("CutString",(theXmlParser_->getAnalysesFromString("Charge"))->getCut("cell charge X")        );
    ui->chargeCuts3LE->assignXmlElement           ((theXmlParser_->getAnalysesFromString("Charge"))->getCutNode("cell charge X")                );

    ui->chargeCuts4LE->textIsAttribute            (true                                                                                         );
    ui->chargeCuts4LE->setText                    ("CutString",(theXmlParser_->getAnalysesFromString("Charge"))->getCut("cell charge Y")        );
    ui->chargeCuts4LE->assignXmlElement           ((theXmlParser_->getAnalysesFromString("Charge"))->getCutNode("cell charge Y")                );

    ui->chargeCuts5LE->textIsAttribute            (true                                                                                         );
    ui->chargeCuts5LE->setText                    ("CutString",(theXmlParser_->getAnalysesFromString("Charge"))->getCut("cluster Landau")       );
    ui->chargeCuts5LE->assignXmlElement           ((theXmlParser_->getAnalysesFromString("Charge"))->getCutNode("cluster Landau")               );

    ui->chargeCuts6LE->textIsAttribute            (true                                                                                         );
    ui->chargeCuts6LE->setText                    ("CutString",(theXmlParser_->getAnalysesFromString("Charge"))->getCut("cell Landau")          );
    ui->chargeCuts6LE->assignXmlElement           ((theXmlParser_->getAnalysesFromString("Charge"))->getCutNode("cell Landau")                  );

    ui->resolutionMainCutsLE->textIsAttribute     (true                                                                                         );
    ui->resolutionMainCutsLE->setText             ("CutString",(theXmlParser_->getAnalysesFromString("Resolution"))->getCut("main cut")         );
    ui->resolutionMainCutsLE->assignXmlElement    ((theXmlParser_->getAnalysesFromString("Resolution"))->getCutNode("main cut")                 );

    ui->resolutionCuts2LE->textIsAttribute        (true                                                                                         );
    ui->resolutionCuts2LE->setText                ("CutString",(theXmlParser_->getAnalysesFromString("Resolution"))->getCut("X resolution")     );
    ui->resolutionCuts2LE->assignXmlElement       ((theXmlParser_->getAnalysesFromString("Resolution"))->getCutNode("X resolution")             );

    ui->resolutionCuts3LE->textIsAttribute        (true                                                                                         );
    ui->resolutionCuts3LE->setText                ("CutString",(theXmlParser_->getAnalysesFromString("Resolution"))->getCut("Y resolution")     );
    ui->resolutionCuts3LE->assignXmlElement       ((theXmlParser_->getAnalysesFromString("Resolution"))->getCutNode("Y resolution")             );

    ui->resolutionCuts4LE->textIsAttribute        (true                                                                                         );
    ui->resolutionCuts4LE->setText                ("CutString",(theXmlParser_->getAnalysesFromString("Resolution"))->getCut("errors")           );
    ui->resolutionCuts4LE->assignXmlElement       ((theXmlParser_->getAnalysesFromString("Resolution"))->getCutNode("errors")                   );

    ui->lowerColDUT0LE->textIsAttribute           (false                                                                                        );
    ui->lowerColDUT0LE->setText                   ("LowerCol",(theXmlParser_->getPlanes())["Dut0"]->getWindow()->getLowerCol()                  );
    ui->lowerColDUT0LE->assignXmlElement          ((theXmlParser_->getPlanes())["Dut0"]->getWindow()->getNode()                                 );

    ui->higherColDUT0LE->textIsAttribute          (false                                                                                        );
    ui->higherColDUT0LE->setText                  ("HigherCol",(theXmlParser_->getPlanes())["Dut0"]->getWindow()->getHigherCol()                );
    ui->higherColDUT0LE->assignXmlElement         ((theXmlParser_->getPlanes())["Dut0"]->getWindow()->getNode()                                 );

    ui->lowerRowDUT0LE->textIsAttribute           (false                                                                                        );
    ui->lowerRowDUT0LE->setText                   ("LowerRow",(theXmlParser_->getPlanes())["Dut0"]->getWindow()->getLowerRow()                  );
    ui->lowerRowDUT0LE->assignXmlElement          ((theXmlParser_->getPlanes())["Dut0"]->getWindow()->getNode()                                 );

    ui->higherRowDUT0LE->textIsAttribute          (false                                                                                        );
    ui->higherRowDUT0LE->setText                  ("HigherRow",(theXmlParser_->getPlanes())["Dut0"]->getWindow()->getHigherRow()                );
    ui->higherRowDUT0LE->assignXmlElement         ((theXmlParser_->getPlanes())["Dut0"]->getWindow()->getNode()                                 );

    ui->lowerColDUT1LE->textIsAttribute           (false                                                                                        );
    ui->lowerColDUT1LE->setText                   ("LowerCol",(theXmlParser_->getPlanes())["Dut1"]->getWindow()->getLowerCol()                  );
    ui->lowerColDUT1LE->assignXmlElement          ((theXmlParser_->getPlanes())["Dut1"]->getWindow()->getNode()                                 );

    ui->higherColDUT1LE->textIsAttribute          (false                                                                                        );
    ui->higherColDUT1LE->setText                  ("HigherCol",(theXmlParser_->getPlanes())["Dut1"]->getWindow()->getHigherCol()                );
    ui->higherColDUT1LE->assignXmlElement         ((theXmlParser_->getPlanes())["Dut1"]->getWindow()->getNode()                                 );

    ui->lowerRowDUT1LE->textIsAttribute           (false                                                                                        );
    ui->lowerRowDUT1LE->setText                   ("LowerRow",(theXmlParser_->getPlanes())["Dut1"]->getWindow()->getLowerRow()                  );
    ui->lowerRowDUT1LE->assignXmlElement          ((theXmlParser_->getPlanes())["Dut1"]->getWindow()->getNode()                                 );

    ui->higherRowDUT1LE->textIsAttribute          (false                                                                                        );
    ui->higherRowDUT1LE->setText                  ("HigherRow",(theXmlParser_->getPlanes())["Dut1"]->getWindow()->getHigherRow()                );
    ui->higherRowDUT1LE->assignXmlElement         ((theXmlParser_->getPlanes())["Dut1"]->getWindow()->getNode()                                 );

    ui->xROCdut0LE->textIsAttribute               (true                                                                                         );
    ui->xROCdut0LE->setText                       ("XRocs",((theXmlParser_->getPlanes())["Dut0"]->getChipsNumber()).first                       );
    ui->xROCdut0LE->assignXmlElement              ((theXmlParser_->getPlanes())["Dut0"]->getNode()                                              );

    ui->yROCdut0LE->textIsAttribute               (true                                                                                         );
    ui->yROCdut0LE->setText                       ("YRocs",((theXmlParser_->getPlanes())["Dut0"]->getChipsNumber()).second                      );
    ui->yROCdut0LE->assignXmlElement              ((theXmlParser_->getPlanes())["Dut0"]->getNode()                                              );

    ui->xROCdut1LE->textIsAttribute               (true                                                                                         );
    ui->xROCdut1LE->setText                       ("XRocs",((theXmlParser_->getPlanes())["Dut1"]->getChipsNumber()).first                       );
    ui->xROCdut1LE->assignXmlElement              ((theXmlParser_->getPlanes())["Dut1"]->getNode()                                              );

    ui->yROCdut1LE->textIsAttribute               (true                                                                                         );
    ui->yROCdut1LE->setText                       ("YRocs",((theXmlParser_->getPlanes())["Dut1"]->getChipsNumber()).second                      );
    ui->yROCdut1LE->assignXmlElement              ((theXmlParser_->getPlanes())["Dut1"]->getNode()                                              );

    ui->ZposDUT0LE->textIsAttribute               (true                                                                                         );
    ui->ZposDUT0LE->setText                       ("ZPosition",(theXmlParser_->getPlanes())["Dut0"]->getZPosition()                             );
    ui->ZposDUT0LE->assignXmlElement              ((theXmlParser_->getPlanes())["Dut0"]->getNode()                                              );

    ui->ZrotDUT0LE->textIsAttribute               (true                                                                                         );
    ui->ZrotDUT0LE->setText                       ("ZRotation",(theXmlParser_->getPlanes())["Dut0"]->getZRotation()                             );
    ui->ZrotDUT0LE->assignXmlElement              ((theXmlParser_->getPlanes())["Dut0"]->getNode()                                              );

    ui->ZposDUT1LE->textIsAttribute               (true                                                                                         );
    ui->ZposDUT1LE->setText                       ("ZPosition",(theXmlParser_->getPlanes())["Dut1"]->getZPosition()                             );
    ui->ZposDUT1LE->assignXmlElement              ((theXmlParser_->getPlanes())["Dut1"]->getNode()                                              );

    ui->ZrotDUT1LE->textIsAttribute               (true                                                                                         );
    ui->ZrotDUT1LE->setText                       ("ZRotation",(theXmlParser_->getPlanes())["Dut1"]->getZRotation()                             );
    ui->ZrotDUT1LE->assignXmlElement              ((theXmlParser_->getPlanes())["Dut1"]->getNode()                                              );

    std::string whichDut = "";

    if((theXmlParser_->getPlanes())["Dut1"]->useCalibrations())
        whichDut = "Dut1";
    else if((theXmlParser_->getPlanes())["Dut0"]->useCalibrations())
        whichDut = "Dut0";

    if(whichDut!="")
    {
        ui->useCalibrationsCB->setEnabled(true);
        ui->useCalibrationsCB->setChecked(true);
        ui->calibPlotsFileLE->setText((theXmlParser_->getPlanes())[whichDut]->getCalibPlotsFile().c_str());
        ui->dutsListLW_2->findItems(whichDut.c_str(),0)[0]->setSelected(true);
    }

    QCheckBox*  parMinCB[4] = {ui->par0minCB,ui->par1minCB,ui->par2minCB,ui->par3minCB};
    QCheckBox*  parMaxCB[4] = {ui->par0maxCB,ui->par1maxCB,ui->par2maxCB,ui->par3maxCB};

    for(int p=0; p<4; p++)
    {
        if((theXmlParser_->getAnalysesFromString("Charge"))->isParToLimit(p).first)
            parMinCB[p]->setChecked(true);
        else
            parMinCB[p]->setChecked(false);
        if((theXmlParser_->getAnalysesFromString("Charge"))->isParToLimit(p).second)
            parMaxCB[p]->setChecked(true);
        else
            parMaxCB[p]->setChecked(false);
    }

    customLineEdit* parMinLE[4] = {ui->par0minLE,ui->par1minLE,ui->par2minLE,ui->par3minLE};
    customLineEdit* parMaxLE[4] = {ui->par0maxLE,ui->par1maxLE,ui->par2maxLE,ui->par3maxLE};

    for(int p=0; p<4; p++)
    {
        parMinLE[p]->textIsAttribute(false);
        parMinLE[p]->setText("Min",(theXmlParser_->getAnalysesFromString("Charge"))->getParLimits(p).first);
        parMinLE[p]->assignXmlElement((theXmlParser_->getAnalysesFromString("Charge"))->getNode());

        parMaxLE[p]->textIsAttribute(false);
        parMaxLE[p]->setText("Max",(theXmlParser_->getAnalysesFromString("Charge"))->getParLimits(p).second);
        parMaxLE[p]->assignXmlElement((theXmlParser_->getAnalysesFromString("Charge"))->getNode());
    }

    QDomNode threashold = (theXmlParser_->getAnalysesFromString("Charge"))->getNode().toElement().elementsByTagName("CellCharge").at(0).toElement().elementsByTagName("Threashold").at(0);
    ui->threasholdSB->setValue("threashold", threashold.toElement().text().toInt());
    ui->threasholdSB->assignXmlElement(threashold);
    QDomNode maxCharge = (theXmlParser_->getAnalysesFromString("Charge"))->getNode().toElement().elementsByTagName("CellCharge").at(0).toElement().elementsByTagName("MaxCharge").at(0);
    ui->maxChargeSB->setValue("maxCharge", maxCharge.toElement().text().toInt());
    ui->maxChargeSB->assignXmlElement(maxCharge);
    QDomNode minTotCharge = (theXmlParser_->getAnalysesFromString("Charge"))->getNode().toElement().elementsByTagName("TotalCharge").at(0).toElement().elementsByTagName("MinTotCharge").at(0);
    ui->minTotChargeSB->setValue("minTotCharge", minTotCharge.toElement().text().toInt());
    ui->minTotChargeSB->assignXmlElement(minTotCharge);
    QDomNode maxTotCharge = (theXmlParser_->getAnalysesFromString("Charge"))->getNode().toElement().elementsByTagName("TotalCharge").at(0).toElement().elementsByTagName("MaxTotCharge").at(0);
    ui->maxTotChargeSB->setValue("maxTotCharge", maxTotCharge.toElement().text().toInt());
    ui->maxTotChargeSB->assignXmlElement(maxTotCharge);

    std::string analysisName[3] = {"Charge"           ,"Efficiency"           ,"Resolution"           };
    QCheckBox*  analysisCB  [3] = {ui->hitsChargeCutCB,ui->hitsEfficiencyCutCB,ui->hitsResolutionCutCB};
    QSpinBox*   analysisSB  [3] = {ui->hitsChargeCutSB,ui->hitsEfficiencyCutSB,ui->hitsResolutionCutSB};
    QDomNode    hits;
    for(int a=0; a<3; a++)
    {
        hits = (theXmlParser_->getAnalysesFromString(analysisName[a]))->getNode().toElement().elementsByTagName("MinHits").at(0);
        if(!hits.isNull())
        {
            analysisCB[a]->setChecked(true);
            analysisSB[a]->setValue(hits.toElement().text().toInt());
        }
    }

    QDomNodeList filesList = theXmlParser_->getScan()->getNode().toElement().elementsByTagName("File");

    for(int f=0; f<filesList.size(); f++)
    {
        QString fileName = filesList.at(f).toElement().attribute("Name");
        QString value    = filesList.at(f).toElement().attribute("Value");
        ui->scanListTW->insertRow(f);
        ui->scanListTW->setItem(f,0,new QTableWidgetItem(fileName));
        ui->scanListTW->setItem(f,1,new QTableWidgetItem(value));
    }

    ui->detectorsInfoTW->assignXmlElement(theXmlParser_->getPlanesNode());

}

//============================================================================================================================================
void AnalyzerDlg::on_saveConfigurationFile_clicked(void)
{
    QString localPath = this->getEnvPath("CHEWIEXMLDIR");
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save File"),ui->loadedConfigurationFileLE->text(),tr("Xml files (*.xml)"));
    if(fileName.isEmpty())
        return;
    theXmlParser_->save(fileName);
}

//============================================================================================================================================
void AnalyzerDlg::on_loadFileToScanListPB_clicked(void)
{
    QString localPath = this->getEnvPath("CHEWIEDATADIR");
    QString fileName = QFileDialog::getOpenFileName(this,"Root ntuple files",localPath,"Root Ntuple Files(*.root)");

    if(fileName.isEmpty())
        return;

    int row = ui->scanListTW->rowCount();
    ui->scanListTW->insertRow(row);

    QTableWidgetItem *fileNameItem = new QTableWidgetItem();
    fileNameItem->setText(fileName);

    ui->scanListTW->setItem(row,0,fileNameItem);

    if(!theXmlParser_->getDocument())
    {
        STDLINE("Load configuration file (.xml)!",ACRed);
        return;
    }

    QDomNode file = theXmlParser_->getDocument()->createElement("file");
    file.toElement().setAttribute("name",fileName);
    theXmlParser_->getScan()->getNode().appendChild(file);
}

//============================================================================================================================================
void AnalyzerDlg::on_resetScanListPB_clicked(void)
{
    while(ui->scanListTW->rowCount()>0)
        ui->scanListTW->removeRow(ui->scanListTW->rowCount()-1);

    QDomNodeList filesList = theXmlParser_->getScan()->getNode().toElement().elementsByTagName("file");

    while(filesList.size()>0)
        theXmlParser_->getScan()->getNode().removeChild(filesList.at(filesList.size()-1));
}

//============================================================================================================================================
void AnalyzerDlg::on_scanListTW_cellChanged(int row, int column)
{
    QDomNode file = theXmlParser_->getScan()->getNode().toElement().elementsByTagName("file").at(row);
    if(column==1)
        file.toElement().setAttribute("value",ui->scanListTW->item(row,column)->text());
    else if (column==0)
        file.toElement().setAttribute("name",ui->scanListTW->item(row,column)->text());
}

//============================================================================================================================================
void AnalyzerDlg::on_runScanPB_clicked(void)
{
    if(ui->scanListTW->rowCount()==0)
        return;

    theXmlParser_->getScan()->addScanValues();

    QString fileName = ui->scanListTW->item(0,0)->text();

//    ui->loadedNtupleFileLE->setText(fileName);
//    ui->loadedNtupleFileLE->setToolTip(fileName);

    theAnalysisManager_->clearFilesList();
    for(int r=0; r<ui->scanListTW->rowCount(); ++r)
    {
        fileName = ui->scanListTW->item(r,0)->text();
        theAnalysisManager_->addInFile(fileName.toStdString());

    }
    std::stringstream numberOfEvents;
    numberOfEvents << theAnalysisManager_->calculateTotalEvents();
    ui->runTotalEventsLE->setText(QString::fromStdString(numberOfEvents.str()));

    QString localPath = this->getEnvPath("CHEWIEOUTPUTDIR");
    ui->outputFileLE->setText(localPath + QString("/Chewie.root"));
    theAnalysisManager_->setOutputFileName(ui->outputFileLE->text().toStdString());
    theAnalysisManager_->openOutFile();

    //int maximum = atoi(theXmlParser_->getGeneral()->getPreAnalysisMaxEvents().c_str());
    //ui->progressBar->setMaximum(maximum);
    ui->progressBar->reset();
    theTimer_->start();
    *theFuture_ = QtConcurrent::run(threaderMethod,this,(void*)"AnalysisManager::startSequence",0);
    theWatcher_->setFuture(*theFuture_);
}

//============================================================================================================================================

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------cuts----------------------------------------------------------------------------------*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//============================================================================================================================================
void AnalyzerDlg::on_xErrEfficiencyCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->efficiencyMainCutsLE->isEnabled())
        cutsLE = ui->efficiencyMainCutsLE;
    else if (ui->efficiencyCuts2LE->isEnabled())
        cutsLE = ui->efficiencyCuts2LE;
    else if (ui->efficiencyCuts3LE->isEnabled())
        cutsLE = ui->efficiencyCuts3LE;
    else if (ui->efficiencyCuts4LE->isEnabled())
        cutsLE = ui->efficiencyCuts4LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->xErrEfficiencyCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "xErrorPredictedLocal[9] <= " << ui->xErrEfficiencyCutSB->value();
        else if(text.indexOf("xErrorPredictedLocal[9]")==-1)
            QTextStream(&text) << " && xErrorPredictedLocal[9] <= " << ui->xErrEfficiencyCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("xErrorPredictedLocal[9]")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "xErrorPredictedLocal[9] <= " << ui->xErrEfficiencyCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "xErrorPredictedLocal[9] <= " << ui->xErrEfficiencyCutSB->value();
        }
        else
            QTextStream(&tmp) << " && xErrorPredictedLocal[9] <= " << ui->xErrEfficiencyCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_yErrEfficiencyCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->efficiencyMainCutsLE->isEnabled())
        cutsLE = ui->efficiencyMainCutsLE;
    else if (ui->efficiencyCuts2LE->isEnabled())
        cutsLE = ui->efficiencyCuts2LE;
    else if (ui->efficiencyCuts3LE->isEnabled())
        cutsLE = ui->efficiencyCuts3LE;
    else if (ui->efficiencyCuts4LE->isEnabled())
        cutsLE = ui->efficiencyCuts4LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->yErrEfficiencyCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "yErrorPredictedLocal[9] <= " << ui->yErrEfficiencyCutSB->value();
        else if(text.indexOf("yErrorPredictedLocal[9]")==-1)
            QTextStream(&text) << " && yErrorPredictedLocal[9] <= " << ui->yErrEfficiencyCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("yErrorPredictedLocal[9]")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "yErrorPredictedLocal[9] <= " << ui->yErrEfficiencyCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "yErrorPredictedLocal[9] <= " << ui->yErrEfficiencyCutSB->value();
        }
        else
            QTextStream(&tmp) << " && yErrorPredictedLocal[9] <= " << ui->yErrEfficiencyCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_chi2EfficiencyCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->efficiencyMainCutsLE->isEnabled())
        cutsLE = ui->efficiencyMainCutsLE;
    else if (ui->efficiencyCuts2LE->isEnabled())
        cutsLE = ui->efficiencyCuts2LE;
    else if (ui->efficiencyCuts3LE->isEnabled())
        cutsLE = ui->efficiencyCuts3LE;
    else if (ui->efficiencyCuts4LE->isEnabled())
        cutsLE = ui->efficiencyCuts4LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->chi2EfficiencyCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "chi2/ndof <= " << ui->chi2EfficiencyCutSB->value();
        else if(text.indexOf("chi2/ndof")==-1)
            QTextStream(&text) << " && chi2/ndof <= " << ui->chi2EfficiencyCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("chi2/ndof")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "chi2/ndof <= " << ui->chi2EfficiencyCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "chi2/ndof <= " << ui->chi2EfficiencyCutSB->value();
        }
        else
            QTextStream(&tmp) << " && chi2/ndof <= " << ui->chi2EfficiencyCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_nTrackEfficiencyCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->efficiencyMainCutsLE->isEnabled())
        cutsLE = ui->efficiencyMainCutsLE;
    else if (ui->efficiencyCuts2LE->isEnabled())
        cutsLE = ui->efficiencyCuts2LE;
    else if (ui->efficiencyCuts3LE->isEnabled())
        cutsLE = ui->efficiencyCuts3LE;
    else if (ui->efficiencyCuts4LE->isEnabled())
        cutsLE = ui->efficiencyCuts4LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->nTrackEfficiencyCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "numberOfTracks <= " << ui->nTrackEfficiencyCutSB->value();
        else if(text.indexOf("numberOfTracks")==-1)
            QTextStream(&text) << " && numberOfTracks <= " << ui->nTrackEfficiencyCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("numberOfTracks")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "numberOfTracks <= " << ui->nTrackEfficiencyCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "numberOfTracks <= " << ui->nTrackEfficiencyCutSB->value();
        }
        else
            QTextStream(&tmp) << " && numberOfTracks <= " << ui->nTrackEfficiencyCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_xEfficiencyCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->efficiencyMainCutsLE->isEnabled())
        cutsLE = ui->efficiencyMainCutsLE;
    else if (ui->efficiencyCuts2LE->isEnabled())
        cutsLE = ui->efficiencyCuts2LE;
    else if (ui->efficiencyCuts3LE->isEnabled())
        cutsLE = ui->efficiencyCuts3LE;
    else if (ui->efficiencyCuts4LE->isEnabled())
        cutsLE = ui->efficiencyCuts4LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->xEfficiencyCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "xPixelResidualLocal[9] <= " << ui->xEfficiencyCutSB->value();
        else if(text.indexOf("xPixelResidualLocal[9]")==-1)
            QTextStream(&text) << " && xPixelResidualLocal[9] <= " << ui->xEfficiencyCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("xPixelResidualLocal[9]")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "xPixelResidualLocal[9] <= " << ui->xEfficiencyCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "xPixelResidualLocal[9] <= " << ui->xEfficiencyCutSB->value();
        }
        else
            QTextStream(&tmp) << " && xPixelResidualLocal[9] <= " << ui->xEfficiencyCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_yEfficiencyCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->efficiencyMainCutsLE->isEnabled())
        cutsLE = ui->efficiencyMainCutsLE;
    else if (ui->efficiencyCuts2LE->isEnabled())
        cutsLE = ui->efficiencyCuts2LE;
    else if (ui->efficiencyCuts3LE->isEnabled())
        cutsLE = ui->efficiencyCuts3LE;
    else if (ui->efficiencyCuts4LE->isEnabled())
        cutsLE = ui->efficiencyCuts4LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->yChargeCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "yPixelResidualLocal[9] <= " << ui->yEfficiencyCutSB->value();
        else if(text.indexOf("yPixelResidualLocal[9]")==-1)
            QTextStream(&text) << " && yPixelResidualLocal[9] <= " << ui->yEfficiencyCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("yPixelResidualLocal[9]")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "yPixelResidualLocal[9] <= " << ui->yEfficiencyCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "yPixelResidualLocal[9] <= " << ui->yEfficiencyCutSB->value();
        }
        else
            QTextStream(&tmp) << " && yPixelResidualLocal[9] <= " << ui->yEfficiencyCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_hitsEfficiencyCutSB_valueChanged(const QString &arg1)
{
    QDomNode hitsCut = theXmlParser_->getAnalysesFromString("Efficiency")->getNode().toElement().elementsByTagName("minHits").at(0);
    theXmlParser_->getAnalysesFromString("Efficiency")->getNode().removeChild(hitsCut);
    QDomText value = theXmlParser_->getDocument()->createTextNode(arg1);
    QDomElement newEl = theXmlParser_->getDocument()->createElement("minHits");
    newEl.appendChild(value);
    theXmlParser_->getAnalysesFromString("Efficiency")->getNode().appendChild(newEl);
}

//============================================================================================================================================
void AnalyzerDlg::on_chi2ChargeCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->chargeMainCutsLE->isEnabled())
        cutsLE = ui->chargeMainCutsLE;
    else if (ui->chargeCuts2LE->isEnabled())
        cutsLE = ui->chargeCuts2LE;
    else if (ui->chargeCuts3LE->isEnabled())
        cutsLE = ui->chargeCuts3LE;
    else if (ui->chargeCuts4LE->isEnabled())
        cutsLE = ui->chargeCuts4LE;
    else if (ui->chargeCuts5LE->isEnabled())
        cutsLE = ui->chargeCuts5LE;
    else if (ui->chargeCuts6LE->isEnabled())
        cutsLE = ui->chargeCuts6LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->chi2ChargeCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "chi2/ndof <= " << ui->chi2ChargeCutSB->value();
        else if(text.indexOf("chi2/ndof")==-1)
            QTextStream(&text) << " && chi2/ndof <= " << ui->chi2ChargeCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("chi2/ndof")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "chi2/ndof <= " << ui->chi2ChargeCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "chi2/ndof <= " << ui->chi2ChargeCutSB->value();
        }
        else
            QTextStream(&tmp) << " && chi2/ndof <= " << ui->chi2ChargeCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_nTrackChargeCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->chargeMainCutsLE->isEnabled())
        cutsLE = ui->chargeMainCutsLE;
    else if (ui->chargeCuts2LE->isEnabled())
        cutsLE = ui->chargeCuts2LE;
    else if (ui->chargeCuts3LE->isEnabled())
        cutsLE = ui->chargeCuts3LE;
    else if (ui->chargeCuts4LE->isEnabled())
        cutsLE = ui->chargeCuts4LE;
    else if (ui->chargeCuts5LE->isEnabled())
        cutsLE = ui->chargeCuts5LE;
    else if (ui->chargeCuts6LE->isEnabled())
        cutsLE = ui->chargeCuts6LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->nTrackChargeCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "numberOfTracks <= " << ui->nTrackChargeCutSB->value();
        else if(text.indexOf("numberOfTracks")==-1)
            QTextStream(&text) << " && numberOfTracks <= " << ui->nTrackChargeCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("numberOfTracks")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "numberOfTracks <= " << ui->nTrackChargeCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "numberOfTracks <= " << ui->nTrackChargeCutSB->value();
        }
        else
            QTextStream(&tmp) << " && numberOfTracks <= " << ui->nTrackChargeCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_xErrChargeCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->chargeMainCutsLE->isEnabled())
        cutsLE = ui->chargeMainCutsLE;
    else if (ui->chargeCuts2LE->isEnabled())
        cutsLE = ui->chargeCuts2LE;
    else if (ui->chargeCuts3LE->isEnabled())
        cutsLE = ui->chargeCuts3LE;
    else if (ui->chargeCuts4LE->isEnabled())
        cutsLE = ui->chargeCuts4LE;
    else if (ui->chargeCuts5LE->isEnabled())
        cutsLE = ui->chargeCuts5LE;
    else if (ui->chargeCuts6LE->isEnabled())
        cutsLE = ui->chargeCuts6LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->xErrChargeCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "xErrorPredictedLocal[9] <= " << ui->xErrChargeCutSB->value();
        else if(text.indexOf("xErrorPredictedLocal[9]")==-1)
            QTextStream(&text) << " && xErrorPredictedLocal[9] <= " << ui->xErrChargeCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("xErrorPredictedLocal[9]")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "xErrorPredictedLocal[9] <= " << ui->xErrChargeCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "xErrorPredictedLocal[9] <= " << ui->xErrChargeCutSB->value();
        }
        else
            QTextStream(&tmp) << " && xErrorPredictedLocal[9] <= " << ui->xErrChargeCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_yErrChargeCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->chargeMainCutsLE->isEnabled())
        cutsLE = ui->chargeMainCutsLE;
    else if (ui->chargeCuts2LE->isEnabled())
        cutsLE = ui->chargeCuts2LE;
    else if (ui->chargeCuts3LE->isEnabled())
        cutsLE = ui->chargeCuts3LE;
    else if (ui->chargeCuts4LE->isEnabled())
        cutsLE = ui->chargeCuts4LE;
    else if (ui->chargeCuts5LE->isEnabled())
        cutsLE = ui->chargeCuts5LE;
    else if (ui->chargeCuts6LE->isEnabled())
        cutsLE = ui->chargeCuts6LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->yErrChargeCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "yErrorPredictedLocal[9] <= " << ui->yErrChargeCutSB->value();
        else if(text.indexOf("yErrorPredictedLocal[9]")==-1)
            QTextStream(&text) << " && yErrorPredictedLocal[9] <= " << ui->yErrChargeCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("yErrorPredictedLocal[9]")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "yErrorPredictedLocal[9] <= " << ui->yErrChargeCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "yErrorPredictedLocal[9] <= " << ui->yErrChargeCutSB->value();
        }
        else
            QTextStream(&tmp) << " && yErrorPredictedLocal[9] <= " << ui->yErrChargeCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_xChargeCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->chargeMainCutsLE->isEnabled())
        cutsLE = ui->chargeMainCutsLE;
    else if (ui->chargeCuts2LE->isEnabled())
        cutsLE = ui->chargeCuts2LE;
    else if (ui->chargeCuts3LE->isEnabled())
        cutsLE = ui->chargeCuts3LE;
    else if (ui->chargeCuts4LE->isEnabled())
        cutsLE = ui->chargeCuts4LE;
    else if (ui->chargeCuts5LE->isEnabled())
        cutsLE = ui->chargeCuts5LE;
    else if (ui->chargeCuts6LE->isEnabled())
        cutsLE = ui->chargeCuts6LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->xChargeCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "xPixelResidualLocal[9] <= " << ui->xChargeCutSB->value();
        else if(text.indexOf("xPixelResidualLocal[9]")==-1)
            QTextStream(&text) << " && xPixelResidualLocal[9] <= " << ui->xChargeCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("xPixelResidualLocal[9]")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "xPixelResidualLocal[9] <= " << ui->xChargeCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "xPixelResidualLocal[9] <= " << ui->xChargeCutSB->value();
        }
        else
            QTextStream(&tmp) << " && xPixelResidualLocal[9] <= " << ui->xChargeCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_yChargeCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->chargeMainCutsLE->isEnabled())
        cutsLE = ui->chargeMainCutsLE;
    else if (ui->chargeCuts2LE->isEnabled())
        cutsLE = ui->chargeCuts2LE;
    else if (ui->chargeCuts3LE->isEnabled())
        cutsLE = ui->chargeCuts3LE;
    else if (ui->chargeCuts4LE->isEnabled())
        cutsLE = ui->chargeCuts4LE;
    else if (ui->chargeCuts5LE->isEnabled())
        cutsLE = ui->chargeCuts5LE;
    else if (ui->chargeCuts6LE->isEnabled())
        cutsLE = ui->chargeCuts6LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->yChargeCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "yPixelResidualLocal[9] <= " << ui->yChargeCutSB->value();
        else if(text.indexOf("yPixelResidualLocal[9]")==-1)
            QTextStream(&text) << " && yPixelResidualLocal[9] <= " << ui->yChargeCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("yPixelResidualLocal[9]")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "yPixelResidualLocal[9] <= " << ui->yChargeCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "yPixelResidualLocal[9] <= " << ui->yChargeCutSB->value();
        }
        else
            QTextStream(&tmp) << " && yPixelResidualLocal[9] <= " << ui->yChargeCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_hitsChargeCutSB_valueChanged(const QString &arg1)
{
    QDomNode hitsCut = theXmlParser_->getAnalysesFromString("Charge")->getNode().toElement().elementsByTagName("minHits").at(0);
    theXmlParser_->getAnalysesFromString("Charge")->getNode().removeChild(hitsCut);
    QDomText value = theXmlParser_->getDocument()->createTextNode(arg1);
    QDomElement newEl = theXmlParser_->getDocument()->createElement("minHits");
    newEl.appendChild(value);
    theXmlParser_->getAnalysesFromString("Charge")->getNode().appendChild(newEl);
}

//============================================================================================================================================
//============================================================================================================================================
void AnalyzerDlg::on_chi2ResolutionCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->resolutionMainCutsLE->isEnabled())
        cutsLE = ui->resolutionMainCutsLE;
    else if (ui->resolutionCuts2LE->isEnabled())
        cutsLE = ui->resolutionCuts2LE;
    else if (ui->resolutionCuts3LE->isEnabled())
        cutsLE = ui->resolutionCuts3LE;
    else if (ui->resolutionCuts4LE->isEnabled())
        cutsLE = ui->resolutionCuts4LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->chi2ResolutionCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "chi2/ndof <= " << ui->chi2ResolutionCutSB->value();
        else if(text.indexOf("chi2/ndof")==-1)
            QTextStream(&text) << " && chi2/ndof <= " << ui->chi2ResolutionCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("chi2/ndof")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "chi2/ndof <= " << ui->chi2ResolutionCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "chi2/ndof <= " << ui->chi2ResolutionCutSB->value();
        }
        else
            QTextStream(&tmp) << " && chi2/ndof <= " << ui->chi2ResolutionCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_nTrackResolutionCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->resolutionMainCutsLE->isEnabled())
        cutsLE = ui->resolutionMainCutsLE;
    else if (ui->resolutionCuts2LE->isEnabled())
        cutsLE = ui->resolutionCuts2LE;
    else if (ui->resolutionCuts3LE->isEnabled())
        cutsLE = ui->resolutionCuts3LE;
    else if (ui->resolutionCuts4LE->isEnabled())
        cutsLE = ui->resolutionCuts4LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->nTrackResolutionCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "numberOfTracks <= " << ui->nTrackResolutionCutSB->value();
        else if(text.indexOf("numberOfTracks")==-1)
            QTextStream(&text) << " && numberOfTracks <= " << ui->nTrackResolutionCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("numberOfTracks")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "numberOfTracks <= " << ui->nTrackResolutionCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "numberOfTracks <= " << ui->nTrackResolutionCutSB->value();
        }
        else
            QTextStream(&tmp) << " && numberOfTracks <= " << ui->nTrackResolutionCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_xErrResolutionCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->resolutionMainCutsLE->isEnabled())
        cutsLE = ui->resolutionMainCutsLE;
    else if (ui->resolutionCuts2LE->isEnabled())
        cutsLE = ui->resolutionCuts2LE;
    else if (ui->resolutionCuts3LE->isEnabled())
        cutsLE = ui->resolutionCuts3LE;
    else if (ui->resolutionCuts4LE->isEnabled())
        cutsLE = ui->resolutionCuts4LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->xErrResolutionCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "xErrorPredictedLocal[9] <= " << ui->xErrResolutionCutSB->value();
        else if(text.indexOf("xErrorPredictedLocal[9]")==-1)
            QTextStream(&text) << " && xErrorPredictedLocal[9] <= " << ui->xErrResolutionCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("xErrorPredictedLocal[9]")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "xErrorPredictedLocal[9] <= " << ui->xErrResolutionCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "xErrorPredictedLocal[9] <= " << ui->xErrResolutionCutSB->value();
        }
        else
            QTextStream(&tmp) << " && xErrorPredictedLocal[9] <= " << ui->xErrResolutionCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_yErrResolutionCutCB_stateChanged(int)
{
    customLineEdit* cutsLE = 0;

    if(ui->resolutionMainCutsLE->isEnabled())
        cutsLE = ui->resolutionMainCutsLE;
    else if (ui->resolutionCuts2LE->isEnabled())
        cutsLE = ui->resolutionCuts2LE;
    else if (ui->resolutionCuts3LE->isEnabled())
        cutsLE = ui->resolutionCuts3LE;
    else if (ui->resolutionCuts4LE->isEnabled())
        cutsLE = ui->resolutionCuts4LE;

    if(!cutsLE)
        return;

    QString text = cutsLE->displayText();

    if(ui->yErrResolutionCutCB->isChecked())
    {
        if(text.size()==0)
            QTextStream(&text) << "yErrorPredictedLocal[9] <= " << ui->yErrResolutionCutSB->value();
        else if(text.indexOf("yErrorPredictedLocal[9]")==-1)
            QTextStream(&text) << " && yErrorPredictedLocal[9] <= " << ui->yErrResolutionCutSB->value();

        cutsLE->setText(text);
    }
    else
    {
        QString tmp;
        if(text.indexOf("yErrorPredictedLocal[9]")==0)
        {
            if(text.indexOf("&")!=-1)
                QTextStream(&tmp) << "yErrorPredictedLocal[9] <= " << ui->yErrResolutionCutSB->value() << " && ";
            else
                QTextStream(&tmp) << "yErrorPredictedLocal[9] <= " << ui->yErrResolutionCutSB->value();
        }
        else
            QTextStream(&tmp) << " && yErrorPredictedLocal[9] <= " << ui->yErrResolutionCutSB->value();

        cutsLE->setText(text.remove(tmp));
    }
}

//============================================================================================================================================
void AnalyzerDlg::on_hitsResolutionCutSB_valueChanged(const QString &arg1)
{
    QDomNode hitsCut = theXmlParser_->getAnalysesFromString("Resolution")->getNode().toElement().elementsByTagName("minHits").at(0);
    theXmlParser_->getAnalysesFromString("Resolution")->getNode().removeChild(hitsCut);
    QDomText value = theXmlParser_->getDocument()->createTextNode(arg1);
    QDomElement newEl = theXmlParser_->getDocument()->createElement("minHits");
    newEl.appendChild(value);
    theXmlParser_->getAnalysesFromString("Resolution")->getNode().appendChild(newEl);
}

//============================================================================================================================================
void AnalyzerDlg::on_resetListPB_clicked()
{
    ui->runAnalysisPB->setEnabled(false);
    QStandardItemModel* model;
    if((model = (QStandardItemModel*)ui->runFileListTV->model()))
        model->clear();
    ui->outputFileLE->setText("Output file with analysis plots (.root)");
}

//============================================================================================================================================
void AnalyzerDlg::on_convertResetListPB_clicked()
{
    ui->runConverterPB->setEnabled(false);
    QStandardItemModel* model;
    if((model = (QStandardItemModel*)ui->convertFileListTV->model()))
        model->clear();
}

////////////////////////////////////////////

void AnalyzerDlg::setPrioritiesFromUi ()
{
    std::string switches;

    switches = theAnalysisManager_->getPriorityFromUi("Windows",     ui->priorityWindowsSB->value()    );
    STDLINE(switches, ACGreen);
    switches = theAnalysisManager_->getPriorityFromUi("Charge",      ui->priorityChargeSB->value()     );
    STDLINE(switches, ACGreen);
    switches = theAnalysisManager_->getPriorityFromUi("Efficiency",  ui->priorityEfficiencySB->value() );
    STDLINE(switches, ACGreen);
    switches = theAnalysisManager_->getPriorityFromUi("Resolution",  ui->priorityResolutionSB->value() );
    STDLINE(switches, ACGreen);
    switches = theAnalysisManager_->getPriorityFromUi("Tracks",      ui->priorityTracksSB->value()     );
    STDLINE(switches, ACGreen);
    switches = theAnalysisManager_->getPriorityFromUi("TracksAfter", ui->priorityTracksAfterSB->value());
    STDLINE(switches, ACGreen);
}
