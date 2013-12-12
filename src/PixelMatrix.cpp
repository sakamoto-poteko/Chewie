//#include "renaissance.h"
#include "PixelMatrix.h"
//#include "PciExpander.h"
//#include "VDetector.h"
//#include "Singleton.h"
//#include "xmlsliderspinbox.h"
//#include "Formatter.h"
//#include "ANSIColors.h"
//#include "Constants.h"
#include "math.h"
#include <qpainter.h> 
#include <iostream>
#include <qlineedit.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qprogressbar.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qtabwidget.h>
//#include <qtable.h>
//#include <qlistbox.h>
#include <qcursor.h>
#include <qlabel.h>

#include <TStyle.h>
#include <TMath.h>

#include <unistd.h>
#include "MessageTools.h"
#include <QHBoxLayout>
#include <QGroupBox>


using namespace std;
//==================================================================================================
PixelMatrix::PixelMatrix ( QWidget* parent) :
      QScrollArea(parent)
{
    parent_       = parent ;
    setupColorSwatch() ;
/*
   //parentBgColor    = parent->paletteBackgroundColor() ;
   
   //theTimer         = new Chronos() ;
   
   setupColorSwatch() ;
   
   //hm               = NULL ;
   dcButtonGroup    = NULL ;
   rowButtonGroup   = NULL ;
   line             = NULL ;
   
   isControlPressed = false ;
   drawDc           = true  ;
   drawSwatch       = true  ;
   skipDrawing      = false ;
   dataFromFile     = false ;
   
   mouseStartRow    = 0 ;
   mouseStartCol    = 0 ;
   border_1         = 2 ;
   border_2         = 1 ;
   
   buttonSelection  = 0 ;
   bumpSelection    = 1 ;
   minRow           = 0 ;
   maxRow           = 1000 ;
   minCol           = 0 ;
   maxCol           = 1000 ;
   lastX            = 0;
   lastY            = 0;
   
   distributionTypes.push_back("Peak"  ) ;
   distributionTypes.push_back("Noise" ) ;
   distributionTypes.push_back("Prob"  ) ;
   distributionTypes.push_back("Chi2"  ) ;
   distributionTypes.push_back("fitSts") ;
   
   errorData      = new TArrayD();
//   FitFunSCurve   = new TF1("SCurve",    SCurve,0,NCHANS,2);
//   FitFunGaussian = new TF1("Gaussian",Gaussian,0,NCHANS,3);
   FitFun         = FitFunSCurve ;
   
   markerColorsMap["White"]   = 0 ;
   markerColorsMap["Black"]   = 1 ;
   markerColorsMap["Red"]     = 2 ;
   markerColorsMap["Green"]   = 3 ;
   markerColorsMap["Blue"]    = 4 ;
   markerColorsMap["Yellow"]  = 5 ;
   markerColorsMap["Magenta"] = 6 ;
   markerColorsMap["Cyan"]    = 7 ;
   
   //cout << theFormatter->getFormatted(mthn)
   //      << "Using "
   //      << FitFun->GetName()
   //      << " with "
   //      << FitFun->GetNpar()
   //      << " parameters"
   //      << endl ;
   //viewport()->setFocusPolicy(QWidget::StrongFocus);
   
   installEventFilter( this );
   */
    Ncols = 52;
    Nrows = 80;
    cellWidthX = 9;
    cellWidthY = 6;
    border_1   = 2 ;
    border_2   = 1 ;
    theMapName = "pixelMap";
    skipDrawing = false;
    drawSwatch  = true;
}
//==================================================================================================
PixelMatrix::~PixelMatrix() {
   string mthn = "[PixelMatrix::~PixelMatrix()]" ;
   //delete theTimer ;
}
//==================================================================================================
void PixelMatrix::destroy(void) {
   
   string mthn = "[PixelMatrix::destroy()]" ;
   //cout << theFormatter->getFormatted(mthn)
   //      << "Cleaning up mess"
   //      << endl ;
   
   if( getMapName() == "pixelMap" || getMapName() == "pixelData" ) 
   {
      for (map<int, QPushButton *>::iterator c=dcButton.begin(); c!=dcButton.end(); c++ )
      {
         //cout << theFormatter->getFormatted(mthn)
         //      << "deleting c:"
         //      << c->first
         //      << " for button "
         //      << c->second
         //      << endl ;
         //       delete c->second ;
      }
   }
   //   if( getMapName() == "pixelData" ) 
   //   {
   //      cout << theFormatter->getFormatted(mthn)
   //            << "Invoking HManager destructor: hmPrt is "
   //     << hm
   //            << endl ;
   //      hm->destroy() ;
   //   }
   //   dcButton.clear() ;
   //   if( dcButtonGroup  != NULL ) delete dcButtonGroup ;
   //   if( rowButtonGroup != NULL ) delete rowButtonGroup ;
}
//==================================================================================================
/*
void PixelMatrix::init(QMainWindow * mw, TQtWidget * c, TQtWidget * bigC)
{  
   string mthn = "[PixelMatrix::init(...)]" ;
   cout << theFormatter->getFormatted(mthn) 
         << "Initializing " 
         << ACYellow << ACBold
         << getMapName()
         << ACPlain
         << endl ;
   
   theRenaissance = (Renaissance*)mw ;
   HistoCanvas    = c ;
   bigHistoCanvas = bigC ;
   
   destroy() ;
   
   theMw = mw ;
   
   init();
}
*/
//==================================================================================================
void PixelMatrix::init(void)
{
    STDLINE("Initializing the pixel matrix",ACGreen);
   
   //theDaqManager = theRenaissance->getDaqManager() ;
   //hm = theRenaissance->getHManager() ;
   
   int chip ;
/*
   for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
   {
      chip = chipSelectionList[chipList] ;
//      clearAllInjections(chip) ;
      for( int r=0; r<Nrows;r++)
      {
         for(int c=0; c<Ncols; c++)
         {
            values[chip][c][r]      = 0;
            enableMask[chip][c][r]  = 0;
            injectMask[chip][c][r]  = 0;
            theColorMap[chip][r][c] = salmon ;
         }
      }
   }
*/
   
   //hm->setRows(Nrows)  ;
   //hm->setCols(Ncols)  ;
   //hm->setActiveChipList(chipSelectionList) ;

   if( true )
   {
      for (int chipList=0; chipList<1; chipList++)
      {
         //chip = chipSelectionList[chipList] ;
         dcButtonGroup = new QGroupBox(this);
         dcButtonGroup->setGeometry( QRect( 22, 492, 466, 10) );
         //dcButtonGroup->setPaletteBackgroundColor( QColor( 243,233,243)) ;
         dcButtonGroup->setFlat(true) ;
         //dcButtonGroup->show() ;
         //connect( dcButtonGroup, SIGNAL(released(int)), SLOT(dcButton_released(int)) );
//         QHBoxLayout* hbox = new QHBoxLayout;
         for (int c=0; c<Ncols; c+=2)
         {
            int x1 = c*(cellWidthX);
            int y1 = border_2 ;
//            dcButton[c] = new QPushButton( dcButtonGroup, QString("b") + QString::number(c) );
            QPalette* palette = new QPalette();
            palette->setColor(QPalette::Background,paleGreen);
            dcButton[c] = new QPushButton(dcButtonGroup);
            dcButton[c]->setGeometry( QRect( x1, y1, 14, 8 ) );
            //dcButton[c]->setPaletteBackgroundColor( paleGreen ) ;
            dcButton[c]->setPalette( *palette ) ;
            //dcButton[c]->show() ;
            dcState[chip][c] = false;
//            theDaqManager->enableDoubleColumn(theDaqManager->getCurrentCard(),
//                                              theDaqManager->getCurrentTbm(),
//                                              chip,
//                                              c,
//                                              false) ;
            //hbox->addWidget(dcButton[c]);
         }
//         dcButtonGroup->setLayout(hbox);

         QGroupBox* groupBox = new QGroupBox(this);
         groupBox->setGeometry( QRect( 14, 6, 8, 485) );
         rowButtonGroup = new QButtonGroup(groupBox);
//         rowButtonGroup->setPaletteBackgroundColor( QColor( 243,233,243)) ;
//         rowButtonGroup->setFlat(true) ;
//         rowButtonGroup->show() ;
         connect( rowButtonGroup, SIGNAL(buttonClicked(int)), SLOT(rowButton_released(int)) );
         for (int r=0; r<Nrows; r++) 
         {
            int border_y = border_2 ;
            int x1 = 1;
            int y1 = r*cellWidthY + border_y + 2 ;
//            rowButton[r] = new QPushButton( rowButtonGroup, QString("rb") + QString::number(r) );
            rowButton[r] = new QPushButton(groupBox);
            rowButton[r]->setGeometry( QRect( x1, y1, 6, 6 ) );
//            rowButton[r]->setPaletteBackgroundColor( paleGreen ) ;
//            rowButton[r]->show() ;
//            rowEnableState[chip][r] = false ;
//            rowInjectState[chip][r] = false ;
            rowButtonGroup->addButton(rowButton[r]);
         }
         colButtonGroup = new QGroupBox(this);
         colButtonGroup->setGeometry( QRect( 22, 504, 469, 8) );
//         colButtonGroup->setPaletteBackgroundColor( QColor( 243,233,243)) ;
         colButtonGroup->setFlat(true) ;
//         colButtonGroup->show() ;
//         connect( colButtonGroup, SIGNAL(released(int)), SLOT(colButton_released(int)) );
         for (int c=0; c<Ncols; c++) 
         {
            int border_x = border_2 ;
            int x1 = c*cellWidthX + border_x + 2 ;
            int y1 = 1;
//            colButton[c] = new QPushButton( colButtonGroup, QString("cb") + QString::number(c) );
            colButton[c] = new QPushButton(colButtonGroup);
            colButton[c]->setGeometry( QRect( x1, y1, 6, 6 ) );
            //colButton[c]->setPaletteBackgroundColor( paleGreen ) ;
            //colButton[c]->show() ;
            //colEnableState[chip][c] = false ;
            //colInjectState[chip][c] = false ;
         }
      }
   }
}
void PixelMatrix::paintEvent(QPaintEvent *e)
{
    QPainter painter(viewport());

    //a simple line
//    painter.drawLine(1,1,100,100);
    drawContents(&painter,10,10,100,100);
}
//==================================================================================================
void PixelMatrix::setupColorSwatch() {
   
   string mthn = "[PixelMatrix::setupColorSwatch()]" ;
   
   white       = QColor(255, 255, 255) ; 
   black       = QColor(  0,   0,   0) ; 
   red         = QColor(255,   0,   0) ;   
   green       = QColor(  0, 255,   0) ;   
   darkGreen   = QColor(  0, 150,   0) ;   
   blue        = QColor(  0,   0, 255) ;
   paleGreen   = QColor(193, 239, 173) ;   
   yellow      = QColor(255, 255,   0) ;
   acquaMarina = QColor(169, 237, 255) ;
   salmon      = QColor(255, 212, 174) ;
   grey        = QColor(227, 227, 227) ;
   darkGrey    = QColor( 75,  77,  75) ;
   
   disableCellButInjectWithCapacitor = darkGrey ;
   disableCellButInjectWithBumpBond  = acquaMarina ;       
   enableCellAndInjectWithCapacitor  = green ;
   enableCellAndInjectWithBumpBond   = red ;
   enableCellButInjectDisabled       = blue ;
   cellDisabled                      = grey ;
   
   brush_white         = new QBrush(white,       Qt::SolidPattern);
   brush_selected      = new QBrush(acquaMarina, Qt::SolidPattern);
   brush_label         = new QBrush(yellow,      Qt::SolidPattern);
   brush_dcol          = new QBrush(yellow,      Qt::SolidPattern);
   brush_dcolNo        = new QBrush(salmon,      Qt::SolidPattern);
   brush_filled        = new QBrush(paleGreen,   Qt::SolidPattern);
   brush_dense4Pattern = new QBrush(white,       Qt::Dense4Pattern);
   
   vector<QString> colorL ;
   colorL.push_back(QString("F")) ;
   colorL.push_back(QString("E")) ;
   colorL.push_back(QString("D")) ;
   colorL.push_back(QString("C")) ;
   colorL.push_back(QString("B")) ;
   colorL.push_back(QString("A")) ;
   colorL.push_back(QString("9")) ;
   colorL.push_back(QString("8")) ;
   colorL.push_back(QString("7")) ;
   colorL.push_back(QString("6")) ;
   colorL.push_back(QString("5")) ;
   colorL.push_back(QString("4")) ;
   colorL.push_back(QString("3")) ;
   colorL.push_back(QString("2")) ;
   colorL.push_back(QString("1")) ;
   colorL.push_back(QString("0")) ;
   
   bool ok ;
   int r = 0xFF ;
   int g = 0xFF ;
   int b = 0xFF ;
   for (int i=0; i<16; i+=2)
   {
      for (int j=0; j<16; j+=2)
      {
         QString tmp = colorL[i] + colorL[j] ;
         int b = tmp.toInt(&ok, 16) ;
         colorSwatch.push_back(QColor(r,g,b)) ;
      }
   }
   r = 0xFF ;
   g = 0xFF ;
   b = 0x00 ;
   for (int i=0; i<16; i+=2)
   {
      for (int j=0; j<16; j+=2)
      {
         QString tmp = colorL[i] + colorL[j] ;
         int g = tmp.toInt(&ok, 16) ;
         colorSwatch.push_back(QColor(r,g,b)) ;
      }
   }
}
//==================================================================================================
void PixelMatrix::changeColorSwatch(int whichButton, QColor color)
{
   if( whichButton == 0 ) 
      disableCellButInjectWithCapacitor = color ;
   if( whichButton == 1 ) 
      disableCellButInjectWithBumpBond  = color ;       
   if( whichButton == 2 ) 
      enableCellAndInjectWithCapacitor  = color ; 
   if( whichButton == 3 )       
      enableCellAndInjectWithBumpBond   = color ;
   if( whichButton == 4 ) 
      enableCellButInjectDisabled       = color ; 
   if( whichButton == 5 ) 
      cellDisabled                      = color ; 
   viewport()->update();
   viewport()->repaint();      
}
//==================================================================================================
void PixelMatrix::Calibrate(QString which, unsigned long trimOrLoop)
{  
/*
    string mthn = "[PixelMatrix::Calibrate()]" ;
   
   QApplication::eventLoop()->processEvents( QEventLoop::AllEvents, 10 );
   QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
   update() ;
   
   theRenaissance->sCurveLinearityToggleCB->setChecked(true) ; 
   
   cout << theFormatter->getFormatted(mthn) 
         << "Beginning Scurves for " 
         << which
         << endl ;
   
   theRenaissance->pixelTabWidget->setCurrentPage(2) ; 
   
   dataFromFile = false ;
   bool graphicsOn = theRenaissance->graphicsOnCB->isChecked() ;
   bool noFitsDueToAbortPress = false ; 
   
   map<int,vector<double> > numberOfHits ;
   for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
   {
      int chip = chipSelectionList[chipList];
      //FIRST ELEMENT IS NUMBER OF HITS
      numberOfHits[chip].push_back(0) ;
      //SECOND ELEMENT IS AVERAGE ADC
      numberOfHits[chip].push_back(0) ;
      if( which == "VCal") 
      {
         hm->destroySCurveTestHistograms(chip);
      }	
   }
   
   if(which != "MinThreshold")
   {
      hm->BookSCurves() ;
   }
   //_________________________________________________________________________________
   if( theRenaissance->fromChipRB->isChecked() )
   {
      double nHits               = 0  ;
      unsigned long card         = theRenaissance->cardCB->currentText().toULong() ;
      unsigned long tbm          = theRenaissance->TBMCB ->currentText().toULong() ;
      int           nBurster     = theRenaissance->xMLNInjectXCal  ->getValue();//100;
      int           trigFreq     = theRenaissance->xMLTrigFreqXCal ->getValue();// 55;
      int           thrDef       =  90 ;
      int           calDelDef    =  90 ;
      int           calDef       = 250 ;
      unsigned long nDivisions   = theRenaissance->xMLTrigFreqXCal->getXmlMaxValue();
      int trim                   = TRIMDEFAULT;
      int injectType             = 1;
      int zeroCells              = 0;
      int fullCells              = 0;
      int stepsMore              = 0;
      int stepsMoreHung          = 0;
      int currentCalStep         = 0;
      bool startCal              = false;
      unsigned long totalNumberOfCells = Nrows*Ncols*chipSelectionList.size();
     
      if(which == "MinThreshold")
      {
         nBurster     = 20;//10;
         trigFreq     = 57;// 55;
      }
      theDaqManager->getPciBus()->getCard(card)->getDeviceRegs()->setTriggerInject(nBurster);
      theDaqManager->getPciBus()->getCard(card)->getDeviceRegs()->setPulserFreq(trigFreq,nDivisions);
      
      for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
      {
         int chip = chipSelectionList[chipList];
         thrDef    = theRenaissance->getDefVal(chip,"Threshold") ;
         calDelDef = theRenaissance->getDefVal(chip,"VCalDelay") ;
         calDef    = theRenaissance->getDefVal(chip,"VCalValue") ;
         theDaqManager->setVThrVCalCalDel(chip,thrDef,calDef,calDelDef);
         cout << theFormatter->getFormatted(mthn) 
               << "Threshold: " << thrDef    << "\t"
               << "VCalDelay: " << calDelDef << "\t"
               << "VCalValue: " << calDef    << "\t"
               << endl ;

      }
      
      theRenaissance->vCalRangeCB_activated( false ); // Set back to Low Range
 
      theDaqManager->clearSystemBeforeLoop(card, tbm);
      
      double dynamicRange = 1 / ((double)colorSwatch.size() - 2) ;
      int step            = 0;
      int chip            = 0;
      int col             = 0;
      int fromCal         = 0;
      int toCal           = 0;
      int calSteps        = 0;
      
      if(which == "VCalTrim")
      {
         fromCal         = theRenaissance->fromTrimTestSB->value();
         toCal           = theRenaissance->toTrimTestSB  ->value();
         calSteps        = theRenaissance->calTrimStepsSB->value();
         trim            = trimOrLoop; 
      }
      else if(which == "BumpBondA") 
      {
         theRenaissance->vCalRangeCB_activated( true ); // Set back to high Range
         fromCal         = theRenaissance->fromBumpBondATestSB->value();
         toCal           = theRenaissance->toBumpBondATestSB  ->value();
         calSteps        = theRenaissance->calBumpBondAStepsSB->value();
         if(trimOrLoop == 0)
         {
           injectType      = 2;
         }
      }
      else if(which == "MinThreshold")
      {
         fromCal         = 40;
         toCal           = 40;
         calSteps        = 1;
      }
      else
      {
         fromCal         = theRenaissance->fromSCurvesTestSB->value();
         toCal           = theRenaissance->toSCurvesTestSB  ->value();
         calSteps        = theRenaissance->stepSCurvesTestSB->value();
      }
      theRenaissance->progressBarStep->setTotalSteps( toCal - fromCal ) ;
      theRenaissance->progressBar->show() ;
      
      theRenaissance->xMLControlSliderA  ->show() ;
      theRenaissance->xMLControlSliderATL->show() ;
      theRenaissance->xMLControlSliderATL->setText("VCal") ;
      theRenaissance->xMLControlSliderA->setXmlMinValue(0) ;
      theRenaissance->xMLControlSliderA->setXmlMaxValue(255) ;
      theRenaissance->xMLControlSliderB  ->hide() ;
      theRenaissance->xMLControlSliderBTL->hide() ;
      // Injection loop begins here
      stepsMore = 0;
      stepsMoreHung = 0;
      currentCalStep = 5*calSteps;
//      for(int cal=fromCal; cal<toCal; cal+=calSteps)
      int maxLoops = 1;
      if(which == "MinThreshold" )
      {
         maxLoops = 30;
      }
      map<int,int> zeroCellsMap;
      for(int loop = 0; loop < maxLoops; loop++ )
      {
         for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
         {
            zeroCellsMap[chipSelectionList[chipList]] = 0;
         }

         for(int cal=fromCal; cal<=toCal; cal+=currentCalStep)
         {         
            if( graphicsOn ) theRenaissance->progressBarStep->setProgress(cal-fromCal+1);
         
            for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
            {
               chip = chipSelectionList[chipList] ;
               if( which == "VCal" || which == "VCalTrim" || which == "BumpBondA" || which == "MinThreshold" ) 
               {
                  theRenaissance->xMLControlSliderA->setXmlValue(cal) ;
                  theDaqManager
                        ->getPciBus()
                        ->getCard(card)
                        ->getDevice(tbm)
                        ->getDetector(chip)
                        ->setVcal(cal);
//                     ->setVcThr(cal);
               }         
            }
            if( which == "VCal" || which == "VCalTrim" || which == "BumpBondA" || which == "MinThreshold" ) 
            {      
               if( graphicsOn ) theMw->statusBar()->message(mthn + 
                                                         QString("\tStep number ") + 
                                                         QString::number(step++) +
                                                         QString(" VCal: ") +
                                                         QString::number(cal)) ;
            }
            if( graphicsOn ) theRenaissance->progressBar->setTotalSteps( Ncols ) ;
         
            zeroCells = 0;
            fullCells = 0;
            for(int dcol=0; dcol<Ncols/2; dcol++)
//         for(int dcol=2; dcol<10; dcol++)  
            {
               for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
               {
                  theDaqManager->enableDoubleColumn(card,tbm,chipSelectionList[chipList],dcol,true);
               }
            
               for(int i=0;i<2;i++) //go through each column of a double column
               {
                  col = i + dcol*2;
                  if( graphicsOn ) theRenaissance->progressBar->setProgress(col);
                  for(int row=0; row<Nrows; row++)   
//          	     for(int row=8; row<16; row++)   
                  {
                     for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
                     {
                        if(which == "BumpBondA" && trimOrLoop == 1)
                        {
                           theDaqManager->enablePixel(card,tbm,chipSelectionList[chipList],row,col,true,trim);
                           theDaqManager->injectPixel(card,tbm,chipSelectionList[chipList],(row+theRenaissance->spacingBumpBondASB->value())%Nrows,col,injectType);
                        }
                        else if(which == "VCal" || which == "MinThreshold" || (which == "BumpBondA" && trimOrLoop == 0))
                        {
                           theDaqManager->injectPixelOn(card,tbm,chipSelectionList[chipList],row,col,true,injectType,trim);
                        }
                        else if(which == "VCalTrim")
                        {
                           theDaqManager->injectPixelOn(card,tbm,chipSelectionList[chipList],row,col,true,injectType,trim);
                        }
                     }
                     theDaqManager->injectOnce(card,tbm,numberOfHits);
                     for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
                     {
                        if(which == "BumpBondA" && trimOrLoop == 1)
                        {
                           theDaqManager->enablePixel(card,tbm,chipSelectionList[chipList],row,col,false,trim);
                           theDaqManager->clearCal();
                        }
                        else
                        {
                           theDaqManager->injectPixelOn(card,tbm,chipSelectionList[chipList],row,col,false,injectType,trim);
                        }
                     }
                     for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
                     {
                        nHits = numberOfHits[chipSelectionList[chipList]][0] ;
                        if(nHits == -1 && chipList==0)
                        {
                           theDaqManager->ROCReset();
                           usleep(100000);
                           theDaqManager->TBMReset();
                           nHits = nBurster+1;
                           cout << theFormatter->getFormatted(mthn) 
                                 << "ROC reset"
                                 << " r:c " 
                                 << row
                                 << ":"
                                 << col
                                 << endl ;
                        }
                     
                        double fraction = nHits/nBurster ;
                        if( graphicsOn ) 
                        {
                           int ind = (int)(fraction / dynamicRange);
                           if( ind < 0 ) ind = 0 ;
                           if( ind > (int)colorSwatch.size() - 1 ) ind = colorSwatch.size() - 1 ;
                           theColorMap[chipSelectionList[chipList]][row][col] = colorSwatch[ind] ;
                           drawSwatch = false ;
                        }
                        if(fraction == 0)
                        {
                           zeroCells++;   
                           if(which == "MinThreshold")
                           {
                              zeroCellsMap[chipSelectionList[chipList]]++;
                           }
                        }
                        else if(fraction == 1)
                        {
                           fullCells++;
                           if(which == "MinThreshold")
                           {
                              zeroCellsMap[chipSelectionList[chipList]]++;
                           }
                        }
                        if(which != "MinThreshold")
                        {

                           hm->fillThr(      chipSelectionList[chipList],row,col,cal,fraction) ;
                     //                     cout << theFormatter->getFormatted(mthn) 
                     //                           << "Chip: " 
                     //                           << chipSelectionList[chipList]
                     //                           << "\tr: " 
                     //                           << row 
                     //                           << "\tc: " 
                     //                           << col 
                     //                           << "\tf: " 
                     //                           << fraction
                     //                           << "\tadc: "
                     //                           << numberOfHits[chipSelectionList[chipList]][1]
                     //                           << endl;
                        }
                     } // end numberOfHits
                     if( !graphicsOn ) 
                     {
                        skipDrawing = true ;
                     } else {
                        QApplication::eventLoop()->processEvents( QEventLoop::AllEvents, 10);
                        if( theRenaissance->abortPB->isOn() ) {noFitsDueToAbortPress = true ; break ;}
                     }
                  } // end rows
                  if( theRenaissance->abortPB->isOn() ) {noFitsDueToAbortPress = true ; break ;}
                  if( graphicsOn ) repaintContents(false) ;
               } // end column whithin double columns
               if( theRenaissance->abortPB->isOn() ) {noFitsDueToAbortPress = true ; break ;}
               for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
               {
                  theDaqManager->enableDoubleColumn(card,tbm,chipSelectionList[chipList],dcol,false);
               }
            } // end double columns
            if( theRenaissance->abortPB->isOn() ) {noFitsDueToAbortPress = true ; break ;}
            if( !graphicsOn ) 
            {
               cout << theFormatter->getFormatted(mthn) 
                     << "Graphics off. Working on "
                     << which
                     << ": "
                     << cal
                     << endl ;
            }
            if(which != "MinThreshold")
            {
               if(!startCal)
               {
                  cout  << theFormatter->getFormatted(mthn) 
                        << "Sum: " << fullCells+zeroCells
                        << " == " << totalNumberOfCells << "; Total" << endl ;
                  cout  << theFormatter->getFormatted(mthn) 
                        <<  "Zero = " << zeroCells 
                        << " Full = " << fullCells
                        << endl;
                  if((fullCells+zeroCells) < (int)(totalNumberOfCells-chipSelectionList.size()))
                  {
                     cal -= 5*calSteps;
                     if(cal<0)
                     {
                        cal = 0;
                     }
                     currentCalStep = calSteps;
                     startCal = true;
                  }
                  cout  << theFormatter->getFormatted(mthn) 
                        << "CurrentCalStep: " << currentCalStep 
                        << " -- Cal: " << cal 
                        << endl;
               }
               if(fullCells>zeroCells)
               {
                  cout  << theFormatter->getFormatted(mthn) 
                        << "Sum: " << fullCells+zeroCells
                        << " == "  << totalNumberOfCells << " Total"
                        << endl;
                  if((fullCells+zeroCells) >= (int)(totalNumberOfCells-chipSelectionList.size()))
                  {
                     if(stepsMore >= 5)
                     {
                        break;
                     }
                     stepsMore++;
                  }
                  else if((fullCells+zeroCells) >= (int)(totalNumberOfCells*0.999))
                  {
                     if(stepsMoreHung >= 10)
                     {
                        break;
                     }
                     stepsMoreHung++;
                  }
                  else
                  {
                     stepsMoreHung = 0;
                  }
               }
            }
         } // end calibration step
         if( theRenaissance->abortPB->isOn() ) {noFitsDueToAbortPress = true ; break ;}
         
         bool breakTheLoop = true;
         if(which == "MinThreshold")
         {
            for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
            {
               int chip = chipSelectionList[chipList];
               int nChipCells = Nrows*Ncols;
               if(zeroCellsMap[chip] < nChipCells*0.999 )
               {
                  int thr = theRenaissance->getDefVal(chip,"Threshold") ;
                  thr--;
                  theRenaissance->setDefVal(chip,"Threshold",thr);
//        	       cout << mthn << "Setting def val:" << thr << " at loop: " << loop << endl;
                  theDaqManager
                        ->getPciBus()
                        ->getCard(card)
                        ->getDevice(tbm)
                        ->getDetector(chip)
                        ->setVcThr(thr);
                     
                  breakTheLoop  = false;
               }
            }
         }
         if(breakTheLoop)
         {
            break;
         }
      } //end loop
      if( graphicsOn ) theRenaissance->progressBarStep->setProgress(toCal-fromCal+1);

//      if( theRenaissance->abortPB->isOn() ) theRenaissance->abortPB->toggle() ;
      
      theRenaissance->statusBar()->message(mthn + QString("\tThreshold scan test ended")) ;
      theRenaissance->elapsedTE->setTime(theTimer->stop()) ;
      
      //GOING BACK TO ORIGINAL CONDITIONS
      theDaqManager
            ->getPciBus()
            ->getCard(card)
            ->getDeviceRegs()
            ->setTriggerInject(theDaqManager
                               ->getConfigurationManager()
                               ->getCard(card)
                               ->numberOfTriggers.value);
      theDaqManager
            ->getPciBus()
            ->getCard(card)
            ->getDeviceRegs()
            ->setPulserFreq(theDaqManager
                            ->getConfigurationManager()
                            ->getCard(card)
                            ->triggerFrequency.value,
                            nDivisions);
      
      theDaqManager->setDefaultVThrVCalCalDel();
      theDaqManager->clearSystemBeforeLoop(card, tbm);
      theRenaissance->vCalRangeCB_activated( false ); // Set back to Low Range
            
   } // endif calibration required
   
   //_________________________________________________________________________________
   
   if( theRenaissance->doAlsoFits->isChecked() && !noFitsDueToAbortPress &&  which != "MinThreshold" )
   {
      doFits() ;   
   }
   
//   if(which == "VCal")
//   {
//      reportCalibrationResults() ;
//   }
   
   if( hm->getSCurveHistogram(chipSelectionList[0],0,0) != NULL )
   {
      HistoCanvas->GetCanvas()->cd() ;
      hm->getSCurveHistogram(chipSelectionList[0],0,0)->Draw() ;
      HistoCanvas->GetCanvas()->Modified();
      HistoCanvas->GetCanvas()->Update();
   }
   
   theRenaissance->doFitPB                 ->setEnabled(true) ;
   theRenaissance->currentChipCB           ->setEnabled(true) ;
   theRenaissance->singleCellValuesTL      ->setEnabled(true) ;
   theRenaissance->getLimitsFromHistogramPB->setEnabled(true) ;
   theRenaissance->savePB                  ->setEnabled(true) ;
   theRenaissance->ModuleResults           ->setEnabled(true) ;
   
   theMw->repaint() ;
   theMw->update() ;
   
   theRenaissance->updateSummaryPlotsLV() ;
   QApplication::restoreOverrideCursor();
   update() ;
*/
}
//==================================================================================================
void PixelMatrix::CalibrateGain(double fromCal, double toCal, double calSteps, unsigned long trim)
{  
/*
    string mthn = "[PixelMatrix::CalibrateGain()]" ;
   
   theRenaissance->prepareTest();
   
   theRenaissance->sCurveLinearityToggleCB->setChecked(false) ; 
   
   cout << theFormatter->getFormatted(mthn) << "Beginning gain calibration" << endl ;
   
   theRenaissance->pixelTabWidget->setCurrentPage(2) ; 
   dataFromFile = false ;
   bool graphicsOn = theRenaissance->graphicsOnCB->isChecked() ;
   
   map<int,vector<double> > numberOfHits ;
   for (int chip=0; chip<(int)chipSelectionList.size(); chip++)
   {
      //FIRST ELEMENT IS NUMBER OF HITS
      numberOfHits[chipSelectionList[chip]].push_back(0) ;
      //SECOND ELEMENT IS AVERAGE ADC
      numberOfHits[chipSelectionList[chip]].push_back(0) ;
   }
 
  
   //_________________________________________________________________________________
   double nHits               = 0  ;
   unsigned long card         = theRenaissance->cardCB->currentText().toULong() ;
   unsigned long tbm          = theRenaissance->TBMCB ->currentText().toULong() ;
   int           nBurster     = theRenaissance->xMLNInjectXCal  ->getValue();
   int           trigFreq     = theRenaissance->xMLTrigFreqXCal ->getValue();
   int           thrDef       =  90 ;
   int           calDelDef    =  90 ;
   int           calDef       = 250 ;
   unsigned long nDivisions   = theRenaissance->xMLTrigFreqXCal->getXmlMaxValue();
   double        lowRangeCal  = 0;
   
   theDaqManager->getPciBus()->getCard(card)->getDeviceRegs()->setTriggerInject(nBurster);
   theDaqManager->getPciBus()->getCard(card)->getDeviceRegs()->setPulserFreq(trigFreq,nDivisions);
   
   for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
   {
      int chip = chipSelectionList[chipList];
      thrDef    = theRenaissance->getDefVal(chip,"Threshold") ;
      calDelDef = theRenaissance->getDefVal(chip,"VCalDelay") ;
      calDef    = theRenaissance->getDefVal(chip,"VCalValue") ;
      theDaqManager->setVThrVCalCalDel(chip,thrDef,calDef,calDelDef);
   }
   
   theDaqManager->clearSystemBeforeLoop(card, tbm);
   
   // Injection loop begins here
   double dynamicRange = 1 / ((double)colorSwatch.size() - 2) ;
   int step            = 0;
   int chip            = 0;
   int col             = 0;
   
   cout << theFormatter->getFormatted(mthn) 
         << "Scanning gain region from "
         << fromCal
         << " to "
         << toCal
         << " in steps of "
         << calSteps
         << endl ;
   
   int nOfSteps=0;
   if(fromCal<=255)
   {
      if(toCal<=255)
      {
	 nOfSteps=(int)((toCal-fromCal)/calSteps)+1;
      }
      else
      {
	 nOfSteps=(int)((255-fromCal)/calSteps)+1;	 
      }
   }
   if(toCal>255)
   {
      if(fromCal > 255)
      {
	 nOfSteps+=(int)((toCal-fromCal)/(calSteps*HIGHVCALCONVERSION))+1;
      }
      else
      {
	 nOfSteps+=(int)((toCal-255)/(calSteps*HIGHVCALCONVERSION))+1;
      }
   }
//   double goal = (double)(toCal - fromCal)/(double)calSteps * 2;
//   theRenaissance->progressBarStep->setTotalSteps( (int)goal ) ;
   theRenaissance->progressBarStep->setTotalSteps( nOfSteps ) ;
   
   theRenaissance->xMLControlSliderA  ->show() ;
   theRenaissance->xMLControlSliderATL->show() ;
   theRenaissance->xMLControlSliderATL->setText("vCal") ;
   theRenaissance->xMLControlSliderA->setXmlMinValue(0) ;
   theRenaissance->xMLControlSliderA->setXmlMaxValue(1800) ;
   
   theRenaissance->xMLControlSliderB  ->hide() ;
   theRenaissance->xMLControlSliderBTL->hide() ;
   
   double cal=fromCal;
   int startFromRange = 0;
   if(fromCal > 255)
   {
      startFromRange = 1;
   }
   int stopAtRange = 1;
   if(toCal<=255)
   {
      stopAtRange=0;
   }
   
   for(int rangeType=startFromRange; rangeType<=stopAtRange; rangeType++)
   {
      // level = 0 => Low Range level = 1 => High Range
      // void Renaissance::vCalRangeCB_activated( int level )
      theRenaissance->vCalRangeCB_activated( rangeType );
//      for(int cal=fromCal; cal<toCal; cal+=calSteps)
      if(rangeType==1)
      {
	 calSteps *= HIGHVCALCONVERSION;// 1800mV/260mV
	 double startingDac = 255;
	 if(fromCal > 255)
	 {
	    startingDac = fromCal;
	 }	
	 int calDac = (int)nearbyint(((cal-startingDac)/calSteps)+(startingDac/HIGHVCALCONVERSION));	
	 cal = calDac*HIGHVCALCONVERSION;
      }
      

      while(nearbyint(cal)<=toCal)
      {         
         if( graphicsOn ) theRenaissance->progressBarStep->setProgress(step++);
         
	 
	 int calDac = (int)nearbyint(cal);
         theRenaissance->xMLControlSliderA->setXmlValue(calDac) ;
	 if(rangeType == 1)
	 {
	    calDac=(int)nearbyint(cal/HIGHVCALCONVERSION);
	 }
         for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
         {
            chip = chipSelectionList[chipList] ;
            theDaqManager
                  ->getPciBus()
                  ->getCard(card)
                  ->getDevice(tbm)
                  ->getDetector(chip)
                  ->setVcal(calDac);
         }
         if( graphicsOn ) 
         {
            theMw->statusBar()->message(mthn + 
                                        QString("\tStep number ") + 
                                        QString::number(step) +
                                        QString(" VCal: ") +
                                        QString::number(nearbyint(cal))) ;
            theRenaissance->progressBar->setTotalSteps( Ncols -1) ;
            theRenaissance->progressBar->show() ;

         }
         if( theRenaissance->abortPB->isOn() ) {break ;}
         for(int dcol=0; dcol<Ncols/2; dcol++)  
         {
            for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
            {
               theDaqManager->enableDoubleColumn(card,tbm,chipSelectionList[chipList],dcol,true);
            }
            
            for(int i=0;i<2;i++) //go through each column of a double column
            {
               col = i + dcol*2;
               if( graphicsOn ) theRenaissance->progressBar->setProgress(col);
               for(int row=0; row<Nrows; row++)   
               {
                  for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
                  {
                     theDaqManager->injectPixelOn(card,tbm,chipSelectionList[chipList],row,col,true,1,trim);
                  }
                  theDaqManager->injectOnce(card,tbm,numberOfHits);
                  for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
                  {
                     theDaqManager->injectPixelOn(card,tbm,chipSelectionList[chipList],row,col,false,1,trim);
                  }
                  for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
                  {
                     nHits = numberOfHits[chipSelectionList[chipList]][0] ;
                     if(nHits == -1 && chipList==0)
                     {
                        theDaqManager->ROCReset();
                        usleep(100000);
                        theDaqManager->TBMReset();
                        nHits = nBurster+1;
                        cout << theFormatter->getFormatted(mthn) 
                              << "ROC reset for chip "
                              << chipSelectionList[chipList]
                              << " r:c " 
                              << row
                              << ":"
                              << col
                              << endl ;
                     }
                     
                     double fraction = nHits/nBurster ;
                     if( graphicsOn ) 
                     {
                        int ind = (int)(fraction / dynamicRange);
                        if( ind < 0 ) ind = 0 ;
                        if( ind > (int)colorSwatch.size() - 1 ) ind = colorSwatch.size() - 1 ;
                        theColorMap[chipSelectionList[chipList]][row][col] = colorSwatch[ind] ;
                        drawSwatch = false ;
                     }
                     lowRangeCal = nearbyint(cal);
//                     if(rangeType == 1)
//                     {
//                       lowRangeCal *= HIGHVCALCONVERSION; // 1800mV/260mV
//                     }
		     if(nHits > 0)
		     {
			hm->fillLinearity(chipSelectionList[chipList],
					  row,
					  col,
					  lowRangeCal,
					  numberOfHits[chipSelectionList[chipList]][1]) ;
		     }
                     //                     cout << theFormatter->getFormatted(mthn) 
                     //                           << "Chip: " 
                     //                           << chipSelectionList[chipList]
                     //                           << "\tr: " 
                     //                           << row 
                     //                           << "\tc: " 
                     //                           << col 
                     //                           << "\tf: " 
                     //                           << fraction
                     //                           << "\tadc: "
                     //                           << numberOfHits[chipSelectionList[chipList]][1]
                     //                           << endl;
                  } // end numberOfHits
                  if( !graphicsOn ) 
                  {
                     skipDrawing = true ;
                  } else {
                     QApplication::eventLoop()->processEvents( QEventLoop::AllEvents, 10);
                     if( theRenaissance->abortPB->isOn() ) {break ;}
                  }
                  if( theRenaissance->abortPB->isOn() ) {break ;}
               } // end rows
               if( graphicsOn ) repaintContents(false) ;
               if( theRenaissance->abortPB->isOn() ) {break ;}
            } // end column whithin double columns
            for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
            {
               theDaqManager->enableDoubleColumn(card,tbm,chipSelectionList[chipList],dcol,false);
            }
            if( theRenaissance->abortPB->isOn() ) {break ;}
         } // end double columns
         if( !graphicsOn ) 
         {
            cout << theFormatter->getFormatted(mthn) 
                  << "Graphics off. Working on: "
                  << cal
                  << endl ;
         }
	 if( theRenaissance->abortPB->isOn() ) {break ;}
	 cal += calSteps;
	 if(cal>255 && rangeType==0)
	 {
	    break;
	 }
      } // end calibration step
      if( theRenaissance->abortPB->isOn() ) {break ;}
   }
   if( graphicsOn ) theRenaissance->progressBarStep->setProgress(step++);
   
   //GOING BACK TO ORIGINAL CONDITIONS
   theRenaissance->vCalRangeCB_activated( false ); // Set back to Low Range
   theDaqManager->setDefaultVThrVCalCalDel();
   theDaqManager->clearSystemBeforeLoop(card, tbm);
   
   //_________________________________________________________________________________
   
   if( hm->getLinearityHistogram(chipSelectionList[0],0,0) != NULL )
   {
      HistoCanvas->GetCanvas()->cd() ;
      hm->getLinearityHistogram(chipSelectionList[0],0,0)->Draw() ;
      HistoCanvas->GetCanvas()->Modified();
      HistoCanvas->GetCanvas()->Update();
   }
   
   theRenaissance->currentChipCB           ->setEnabled(true) ;
   theRenaissance->singleCellValuesTL      ->setEnabled(true) ;
   theRenaissance->getLimitsFromHistogramPB->setEnabled(true) ;
   theRenaissance->savePB                  ->setEnabled(true) ;
   theRenaissance->ModuleResults           ->setEnabled(true) ;
   
   theMw->repaint() ;
   theMw->update() ;
   theRenaissance->closeOutTest();
*/
}   
//==================================================================================================
bool PixelMatrix::doLinearFits()
{
/*
    string mthn = "[PixelMatrix::doLinearFits()]" ;
   QApplication::eventLoop()->processEvents( QEventLoop::AllEvents, 10 );
   QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
   
   update() ;
   bool graphicsOn = theRenaissance->graphicsOnCB->isChecked() ;
   int fitFrom     =  theRenaissance->fitFromGainTestSB->value() ;
   int fitTo       =  theRenaissance->fitToGainTestSB  ->value() ;
   theRenaissance->progressBarStep->setTotalSteps( chipSelectionList.size() -1 ) ;
   theRenaissance->progressBar->show() ;
   theRenaissance->progressBar->setTotalSteps( Nrows -1) ;
   
   hm->BookGainGlobalCurves();   
   for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
   {
      hm->fillFirstBinPlot(chipSelectionList[chipList]);
      if( graphicsOn ) 
      {
         theRenaissance->statusBar()->message(mthn + QString("\tFitting chip ") + QString::number(chipSelectionList[chipList]) ) ;
         theRenaissance->progressBarStep->setProgress(chipList);
      }
      QApplication::eventLoop()->processEvents( QEventLoop::AllEvents, 10);
      if( theRenaissance->abortPB->isOn() ) {break ;}
      for (int r=0; r<Nrows; r++)
      {
         if( graphicsOn ) theRenaissance->progressBar->setProgress(r);
         for (int c=0; c<Ncols; c++)
         {
            hm->fitGainCurves(chipSelectionList[chipList],r,c,fitFrom,fitTo) ;
            QApplication::eventLoop()->processEvents( QEventLoop::AllEvents, 10);
            if( theRenaissance->abortPB->isOn() ) {break ;}
         }
         if( theRenaissance->abortPB->isOn() ) {break ;}
      }
      map<string, map<double, double> > fitResult = hm->fitGlobalGainCurves(chipSelectionList[chipList]) ;
      for(map<string, map<double, double> >::iterator fR=fitResult.begin(); fR!=fitResult.end(); fR++)
      {
         for(map<double, double>::iterator res=fR->second.begin(); res!=fR->second.end(); res++)
         {
            if( fR->first == "slope" )
            {
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 0,QString::number(res->first*1000, 'f',1)) ;
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 1,QString::number(res->second*1000,'f',2)) ;
            }
            if( fR->first == "intercept" )
            {
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 2,QString::number(res->first, 'f',2)) ;
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 3,QString::number(res->second,'f',2)) ;
            }
            if( fR->first == "saturation" )
            {
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 4,QString::number(res->first, 'f',0)) ;
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 5,QString::number(res->second,'f',1)) ;
            }
            if( fR->first == "outSlope" )
            {
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 6,QString::number(res->first, 'f',0)) ;
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 7,QString::number(res->second,'f',0)) ;
            }
            if( fR->first == "outIntercept" )
            {
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 8,QString::number(res->first, 'f',0)) ;
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 9,QString::number(res->second,'f',0)) ;
            }
            if( fR->first == "outSaturation" )
            {
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 10,QString::number(res->first, 'f',0)) ;
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 11,QString::number(res->second,'f',0)) ;
            }
            if( fR->first == "KS_Slope" )
            {
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 12,QString::number(res->first, 'f',2)) ;
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList], 13,QString::number(res->second,'f',2)) ;
            }
            if( fR->first == "KS_Intercept" )
            {
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList],14,QString::number(res->first, 'f',2)) ;
               theRenaissance->finalGainCurvesResultsT->setText(chipSelectionList[chipList],15,QString::number(res->second,'f',2)) ;
            }
         }
      }

      if( theRenaissance->abortPB->isOn() ) {break ;}
   }
      
   theRenaissance->statusBar()->message(mthn + QString("\tFits done")) ;
   QApplication::restoreOverrideCursor();
   update() ;
   
   if( theRenaissance->abortPB->isOn() ) 
   {
      theRenaissance->abortPB->toggle() ;
      return true ;
   } else {
      return false ;
   }
*/
 }
//==================================================================================================
void PixelMatrix::getHistogramsFromFile(QString )
{
   string mthn = "[PixelMatrix::getHistogramsFromFile()]" ;
   
   //   hm = Singleton<HManager>::GetInstance() ;
   //   
   //   TFile   * file   = new TFile(rootFile.ascii()) ;
   //   TTree   * tree   = (TTree*)file->Get("HManagerTree") ;
   //   TBranch * branch = tree->GetBranch("hm") ;
   //   
   //   unsigned long entry = 0;
   //   
   //   branch->SetAddress(&hm) ;
   //   int entries = tree->GetEntries() ;
   //   
   //   cout << theFormatter->getFormatted(mthn)
   //         << "Found " 
   //         << entries 
   //         << " entries" 
   //         << endl ;
   //   
   //   entry = tree->GetEntry(0);
   //   
   //   chipSelectionList = hm->getChipSelectionList() ;
   
}
//==================================================================================================
void PixelMatrix::doFits()
{
/*
    string mthn = "[PixelMatrix::doFits()]" ;
   
   cout << theFormatter->getFormatted(mthn) << "Fitting S curves" << endl;
   
   hm->BookSCurvesFitResults() ;
   
   theRenaissance->elapsedTE->setTime(theTimer->start()) ;
   if( !theRenaissance->abortPB->isOn() ) 
   {
      theRenaissance->progressBarStep->setTotalSteps( chipSelectionList.size() -1 ) ;
      theRenaissance->progressBar->show() ;
      theRenaissance->progressBar->setTotalSteps( Nrows -1) ;
      for (int chip=0; chip<(int)chipSelectionList.size(); chip++)
      {
         theRenaissance->progressBarStep->setProgress(chip);
         for( int r=0;r<Nrows;r++)
         {
            msg = mthn + QString("\tExploring threshold curves for row ") + QString::number(r) ;       
            theMw->statusBar()->message(msg) ;
            theRenaissance->progressBar->setProgress(r);
            for( int c=0;c<Ncols;c++)
            {  
               estimateSCurve(chipSelectionList[chip],r,c) ;
            }
            QApplication::eventLoop()->processEvents( QEventLoop::AllEvents, 10 );
            if( theRenaissance->abortPB->isOn() ) break ;
         }
         if( theRenaissance->abortPB->isOn() ) break ;
      }
   }
   
   // Fit of histograms here
   if( !theRenaissance->abortPB->isOn() ) 
   {
//      theRenaissance->progressBar->reset(); n = 0;
//      theRenaissance->progressBar->setTotalSteps( chipSelectionList.size() * Nrows - 1 ) ;
      for (int chip=0; chip<(int)chipSelectionList.size(); chip++)
      {
         theRenaissance->progressBarStep->setProgress(chip);
         for( int r=0;r<Nrows;r++)
         {
            msg = mthn + 
                  QString("\tFitting threshold curves for row ") + 
                  QString::number(r) + 
                  QString(" on chip ") +
                  QString::number(chipSelectionList[chip]);       
            theRenaissance->progressBar->setProgress(r);
            theMw->statusBar()->message(msg) ;
            for( int c=0;c<Ncols;c++)
            {  
               fitSCurve(chipSelectionList[chip],r,c) ;
               QApplication::eventLoop()->processEvents( QEventLoop::AllEvents, 10 );
               if( theRenaissance->abortPB->isOn() ) break ;
            } 
//            theRenaissance->progressBar->setProgress(n); n++ ;
            if( theRenaissance->abortPB->isOn() ) break ;
         } 
         fillGlobalPlot(chipSelectionList[chip],"Peak") ;    
         fillGlobalPlot(chipSelectionList[chip],"Noise") ;    
         fillGlobalPlot(chipSelectionList[chip],"Prob") ;    
         fillGlobalPlot(chipSelectionList[chip],"Chi2") ;    
         fillGlobalPlot(chipSelectionList[chip],"fitSts") ; 
         
         hm->fitGlobal1DPlot(chipSelectionList[chip], "Peak") ;
         hm->fitGlobal1DPlot(chipSelectionList[chip], "Noise") ;
         if( theRenaissance->abortPB->isOn() ) break ;
      }
//      theRenaissance->histoListLB->insertItem("Module_Peak_distribution") ;
//      theRenaissance->histoListLB->insertItem("Module_Noise_distribution") ;
//      theRenaissance->histoListLB->insertItem("Module_Prob_distribution") ;
//      theRenaissance->histoListLB->insertItem("Module_Chi2_distribution") ;
//      theRenaissance->histoListLB->insertItem("Module_fitSts_distribution") ;
   }
   
   theRenaissance->elapsedTE->setTime(theTimer->stop()) ;
   
   if( theRenaissance->abortPB->isOn() ) 
   {
      theRenaissance->abortPB->setOn(false) ;
      hm->getSCurveHistogram(chipSelectionList[0],0,0)->Draw() ;
      HistoCanvas->GetCanvas()->Modified();
      HistoCanvas->GetCanvas()->Update();
      theMw->repaint() ;
      theMw->update() ;
      return ;
   }
   
   hm->getSCurveHistogram(chipSelectionList[0],0,0)->Draw() ;
   
   display(QString("Peak")) ;
*/
}
//==================================================================================================
void PixelMatrix::fitSCurve(int chip, int row, int col)
{
/*
    string mthn = "[PixelMatrix::fitSCurve()]" ;
   
   double mean  = 0 ;
   double sigma = 0 ;
   
   TH1F * theHistogram = hm->getSCurveHistogram( chip, row, col ) ;
   if( theHistogram == NULL )
   {
      cout << theFormatter->getFormatted(mthn)
            << "Fatal: histogram for chip "
            << chip
            << " row "
            << row
            << " and col " 
            << col
            << " could no longer be found."
            << endl ;
      return ;
   }
   
   if( theRenaissance->fitFunctionCB->currentText() == "SCurve" )
   {
      FitFun = FitFunSCurve ;
      mean  = initPeak[chip][row][col]  ;
      sigma = initNoise[chip][row][col] ;
      FitFun->SetParameters(mean, sigma) ;
      
      double lowLim  = (double)lowSbin[chip][row][col] - 3. ;  
      double highLim = (double)higSbin[chip][row][col] + 3. ; 
      
      if( lowLim > highLim )
      {
         double temp = lowLim ;
         lowLim = highLim ;
         highLim = temp ;
      }
      FitFun->SetRange(lowLim, highLim) ;
      
      //for( int b=0; b<theHistogram->GetNbinsX(); b++)
      //{
      //   if( b % 2 == 0 ) 
      //   {
      //      theHistogram->SetBinContent(b+1,0.) ;
      //   }
      //}
      
      ErrorGenerator(theHistogram,errorData) ;
      // Set error bars
      theHistogram->SetError(errorData->GetArray());
      // Fit
      theHistogram->Fit(FitFun,"RQ");
   }
   
   if( theRenaissance->fitFunctionCB->currentText() == "Gaussian" )
   {
      int thisBin ;
      int nextBin ;
      tempHist = (TH1F*)theHistogram->Clone(); 
      for (int bin=1; bin<=theHistogram->GetNbinsX()-1; bin++)
      {
         thisBin = bin ;
         nextBin = bin+1 ;
         double diff = theHistogram->GetBinContent(nextBin) - 
                       theHistogram->GetBinContent(thisBin) ;
         if( diff == -1 ) diff = 0 ;
         diff = fabs(diff) ;
         tempHist->SetBinContent(thisBin,diff) ; 
      }
//      tempHist->SetTitle(theHistogram->GetTitle()) ;
      FitFun = FitFunGaussian ;
      mean  = tempHist->GetMean() ;
      sigma = tempHist->GetRMS() ;
      FitFun->SetParameters(.5, mean, sigma) ;
      FitFun->SetRange(mean - 3*sigma, mean + 3*sigma) ;
      tempHist->Fit(FitFun,"QR");
      theHistogram->GetListOfFunctions()->Add(FitFun);
//      hm->replaceSCurveHistogram(chip, row, col, theHistogram) ;
      tempHist->Delete() ;
   }
   
   fitter = TVirtualFitter::Fitter(theHistogram) ;
   
   double fmin, fedm, errdef ;
   int npari, nparx, istat ;
   istat = fitter->GetStats(fmin,fedm,errdef,npari,nparx) ;
   fitSts[chip][row][col] = istat ;
   
   chi2T[chip][row][col] = computeChisquare(chip,row,col) ;
   
//   theHistogram->Delete() ;
*/
}
/*
//====================================================================
void PixelMatrix::restoreChi2AndFitSts()
{   
   string mthn = "[PixelMatrix::restoreChi2AndFitSts()]" ;
   cout << theFormatter->getFormatted(mthn)
         << "Restoring Chi2 and fitStatus from saved data "
         << endl ;
   
   dataFromFile = true ;
   
   TH2F * h = NULL ;
   for (int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
   {
      int chip = chipSelectionList[chipList] ;
      cout << theFormatter->getFormatted(mthn) 
            << "Requesting Big2DPlot for chip " 
            << chip 
            << " and Chi2" 
            << " hm: "
            << hm
            << endl ;
//      h = hm->getGlobal2DSCurvesTestPlot(chip, "Chi2") ;
      h = hm->getGlobal2DPlot(chip, "Chi2") ;
      if( h == NULL ) continue ;
      for( int row=0; row<h->GetNbinsY(); row++)
      {
         for( int col=0; col<h->GetNbinsX(); col++ )
         {
            chi2T[chip][row][col] = h->GetBinContent(col+1,row+1) ;
            //            if( chip == 0 )
            //            {
            //               cout << theFormatter->getFormatted(mthn)
            //                     << "Chip: "
            //                     << chip
            //                     << "\tr:c " 
            //                     << row
            //                     << ":"
            //                     << col
            //                     << "\tchi2: "
            //                     << chi2T[chip][row][col]
            //                     << endl ;
            //            }
         }
      }
      
//      h = hm->getGlobal2DSCurvesTestPlot(chip, "fitSts") ;
      h = hm->getGlobal2DPlot(chip, "fitSts") ;
      if( h == NULL ) continue ;
      for( int row=0; row<h->GetNbinsY(); row++)
      {
         for( int col=0; col<h->GetNbinsX(); col++ )
         {
            fitSts[chip][row][col] = (int)h->GetBinContent(col+1,row+1) ;
         }
      }
   }
   cout << theFormatter->getFormatted(mthn) << "Done" << endl ;
}
//====================================================================
double PixelMatrix::computeChisquare(int chip, int row, int col)
{   
   string mthn = "[HManager::computeChisquare()]" ;
   
   TH1F * theHistogram = hm->getSCurveHistogram( chip, row, col ) ;
   if( theHistogram == NULL )
   {
      cout << theFormatter->getFormatted(mthn)
            << "Could not find histogram for chip "
            << chip
            << " row "
            << row
            << " col "
            << col
            << endl ;
      return 0;
   }
   TF1 * f = theHistogram->GetFunction(theRenaissance->fitFunctionCB->currentText().ascii()) ;   
   
   if( f == NULL )
   {
      //      cout << theFormatter->getFormatted(mthn)
      //            << "Could not find associated fit function "
      //            << theRenaissance->fitFunctionCB->currentText()
      //            << " for "
      //            << theHistogram->GetName()
      //            << endl ;
      return 0;
   }
   
   double xmin ;
   double xmax ;
   f->GetRange( xmin, xmax) ;
   
   double chi2 = f->GetChisquare() ; 
   int    NDOF = f->GetNDF() ;
   if( NDOF > 0 ) chi2/= (double)NDOF ;
   //   if( chip == 0 && row <2 && col <2 ) 
   //   {
   //      cout << "\t\t" << endl ;
   //      cout << theFormatter->getFormatted(mthn)
   //            << "Chip "
   //            << chip
   //            << " row "
   //            << row
   //            << " col "
   //            << col
   //            << " chi2: "
   //            << chi2
   //            << " NDOF: "
   //            << NDOF
   //            << " xmin: " 
   //            << xmin
   //            << " xmax: "
   //            << xmax
   //            << endl ;
   //      double myChi2 = 0 ;
   //      for( int b=0; b<theHistogram->GetNbinsX(); b++)
   //      {
   //         double c = theHistogram->GetBinContent(b+1) ;
   //         double x = theHistogram->GetBinCenter( b+1) ;
   //         if( x >= xmin && x <= xmax )
   //         {
   //            double e = theHistogram->GetBinError(  b+1) ;
   //            double p = f->Eval(x) ;
   //            if( e!=0 ) myChi2 += (c-p)/e ;
   //            cout << theFormatter->getFormatted(mthn)
   //                  << b
   //                  << "]\tx:" 
   //                  << x
   //                  << " c: "
   //                  << c
   //                  << " p: "
   //                  << p
   //                  << " myChi2: "
   //                  << myChi2
   //                  << endl ;
   //         }
   //      }
   //      
   //   }
   return chi2 ;
}
//==================================================================================================
void PixelMatrix::estimateSCurve(int chip, int row, int col)
{
   string mthn = "[PixelMatrix::estimateSCurve()]" ;
   
   TH1F * theHistogram = hm->getSCurveHistogram( chip, row, col ) ;
   
   if( theHistogram == NULL ) return ;
   
   int lowBin  = 0 ;
   int highBin = 0 ;
   int topBin  = 0 ;
   int effBin  = 0 ;
   
   int nBins = theHistogram->GetNbinsX() ;
   
   // Determine where the threshold curve reaches the top
   
   int    minBin   = theHistogram->GetMinimumBin() ;
   int    maxBin   = theHistogram->GetMaximumBin() ;
   double minValue = theHistogram->GetBinContent(minBin) ;
   double maxValue = theHistogram->GetBinContent(maxBin) ;
   if( minBin == 1 && maxBin == 1 )
   {
      //      cout << theFormatter->getFormatted(mthn) 
      //     << "Empty histogram for cell at row "
      //     << row
      //     << " col "
      //     << col
      //     << endl ;
      //        return ;
   }
   if( minValue != 0 || maxValue != 1. )
   {
      //      cout << theFormatter->getFormatted(mthn) 
      //     << "Out of dynamic range: min="
      //     << minValue
      //     << " (bin " 
      //     << minBin
      //     << ") max="
      //     << maxValue
      //     << " (bin "
      //     << maxBin
      //     << ") at row "
      //     << row
      //     << " col "
      //     << col
      //     << endl ;
      //        return ;
   }
   
   // Determine the starting point of the threshold curve
   
   lowBin = 1; 
   for (int bin=1; bin<=nBins; bin++)
   {
      if( theHistogram->GetBinContent(bin) > 0 )
      {
         lowBin = bin ;
         break ;
      } 
   }
   lowBin -= 1 ;
   
   // Find the saturation level begins, starting from the end point backwardsfillthr
   topBin = nBins ;
   for (int bin=nBins; bin>0; bin--)
   {
      if( theHistogram->GetBinContent(bin) > 0 )
      {
         topBin = bin ;
         break ;
      } 
   }
   topBin += 1;
   
   // Finalize the search
   highBin = NCHANS -1 ;
   for (int bin=topBin; bin>0; bin--)
   {
      if( theHistogram->GetBinContent(bin) == maxValue )
      {
         highBin = bin ;
         break ;
      } 
   }
   
   // Search first bin reaching saturation
   effBin = 0 ;
   for (int bin=highBin; bin>0; bin--)
   {
      if( theHistogram->GetBinContent(bin) < maxValue )
      {
         effBin = bin ;
         break ;
      } 
   }
   effBin += 1 ;
   
   effBin = maxBin + 3; // temporary patch (remove)
   
   if( lowBin > effBin ) 
   {
      int temp = lowBin ;
      lowBin = effBin ;
      effBin = temp ;
   }
   if( lowBin == effBin )
   {
      lowBin -= 1 ;
      effBin += 1 ;
   }
   
   // Define the starting values of the fit parameters
   double mean  = (double)(effBin + lowBin) / 2.  ;
   double sigma = (double)(effBin - lowBin) / 2.2 ;
   
   initPeak[chip][row][col]  = mean ;
   initNoise[chip][row][col] = sigma ;
   lowSbin[chip][row][col]   = lowBin ;
   higSbin[chip][row][col]   = effBin ;
}
//==================================================================================================
void PixelMatrix::displayModulePlot(int which)
{  
   string mthn = "[PixelMatrix::displayModulePlot()]" ;
   //   cout << theFormatter->getFormatted(mthn)
   //         << endl ;
   
   bigHistoCanvas->GetCanvas()->Clear() ;
   gStyle->SetPalette(1,0) ;   
   
   int nChips = 5 ;
   int Nrows = 80 ;
   int Ncols = 52 ;
   
   if( line != NULL ) delete line ;   
   line = new TLine() ;
   
   double low[2] ;
   double hig[2] ;
   
   low[0] = 0 ;
   low[1] = 0 ;
   hig[0] = 255 ;
   hig[1] = 255 ;
   map<int, TH2F *> resumePlot ;
   resumePlot.clear() ;
   if( which == 0 )
   {
      resumePlot[0] = hm->getModulePlot("Peak");
      resumePlot[1] = hm->getModulePlot("Noise");
      if( theRenaissance->useCutsCB->isChecked() )
      {
         low[0] = theRenaissance->thrMinCutLE  ->text().toDouble() ;
         low[1] = theRenaissance->noiseMinCutLE->text().toDouble() ;
         hig[0] = theRenaissance->thrMaxCutLE  ->text().toDouble() ;
         hig[1] = theRenaissance->noiseMaxCutLE->text().toDouble() ;
      }
   }
   if( which == 1 )
   {
      resumePlot[0] = hm->getModulePlot("Prob");
      resumePlot[1] = hm->getModulePlot("Chi2");
      if( theRenaissance->useCutsCB->isChecked() )
      {
         low[0] = theRenaissance->CLMinCutLE  ->text().toDouble() ;
         low[1] = theRenaissance->Chi2MinCutLE->text().toDouble() ;
         hig[0] = theRenaissance->CLMaxCutLE  ->text().toDouble() ;
         hig[1] = theRenaissance->Chi2MaxCutLE->text().toDouble() ;
      }
   }
   
   bigHistoCanvas->GetCanvas()->Divide(1,2) ;
   
   for(int plot=0; plot<2; plot++)
   {
      bigHistoCanvas->GetCanvas()->cd(plot+1) ;
      resumePlot[plot]->Draw(theRenaissance->hOptionsLargeCB->currentText().ascii()) ;
      resumePlot[plot]->SetMinimum(low[plot]) ;
      resumePlot[plot]->SetMaximum(hig[plot]) ;
      resumePlot[plot]->SetStats(false) ;
      for( int chip=0; chip<nChips; chip++)
      {
         line->DrawLine(Ncols*chip,0,Ncols*chip,Nrows*2) ;
      }
      line->DrawLine(0,Nrows,Ncols*nChips,Nrows) ;
      
      bigHistoCanvas->GetCanvas()->Modified() ;
      bigHistoCanvas->GetCanvas()->Update() ;
   }
   
}

//==================================================================================================
void PixelMatrix::displayPlot(int chip, QString what)
{
   string mthn = "[PixelMatrix::displayPlot()]" ;
   
//   global1DPlot = hm->getGlobal1DSCurvesTestPlot(chip, what) ;
   global1DPlot = hm->getGlobal1DPlot(chip, what) ;

   if( global1DPlot == NULL ) 
   {
      cout << theFormatter->getFormatted(mthn)
            << "Fatal: global plot for "
            << what 
            << " on chip "
            << chip
            << " is unavailable"
            << endl ;
      return;
   }
   
   HistoCanvas->GetCanvas()->cd() ;
   
   global1DPlot->Draw() ;
   
   double yMin = theRenaissance->yMinLE->text().toDouble() ;
   double yMax = theRenaissance->yMaxLE->text().toDouble() ;
   if( yMin < yMax )
   {
      global1DPlot->SetMinimum() ;
      global1DPlot->SetMaximum() ;
   }
   if(  theRenaissance->unZoomX->isChecked() )
   {
      global1DPlot->GetXaxis()->UnZoom();
   }
   if(  theRenaissance->unZoomY->isChecked() )
   {
      global1DPlot->GetYaxis()->UnZoom();
   }
   if(  theRenaissance->unZoomZ->isChecked() )
   {
      global1DPlot->GetZaxis()->UnZoom();
   }
   if(  theRenaissance->logScaleX->isChecked() )
   {
      gPad->SetLogx(1);
   } else {
      gPad->SetLogx(0);
   }
   if(  theRenaissance->logScaleY->isChecked() )
   {
      gPad->SetLogy(1);
   } else {
      gPad->SetLogy(0);
   }
   if(  theRenaissance->logScaleZ->isChecked() )
   {
      gPad->SetLogz(1);
   } else {
      gPad->SetLogz(0);
   }
   
   HistoCanvas->GetCanvas()->cd();
   
   global1DPlot->Draw() ;
   global1DPlot->SetLabelSize(0.06,"X") ;
   global1DPlot->SetLabelSize(0.06,"Y") ;
   global1DPlot->SetFillColor(theRenaissance->xMLhistogramBackgroundColor->getValue()) ;
   TF1 * f = global1DPlot->GetFunction("gaus") ;
   if ( f != NULL ) 
   {
      f->SetLineWidth(1) ;
      f->SetLineColor(2) ;
   }
   
   TDirectory * garbageDir = hm->makeDir(hm->getGarbageDirName());
   garbageDir->cd();
   if((tempHist=(TH1F*)garbageDir->FindObjectAny("TmpDrawingHistogram")) != NULL)
   {
       tempHist->Delete();
   }
   tempHist = (TH1F *)global1DPlot->Clone() ;
   tempHist->SetName("TmpDrawingHistogram");
   
   if( tempHist == NULL ) 
   {
      cout << theFormatter->getFormatted(mthn)
            << "Fatal: copy of global plot for "
            << what 
            << " on chip "
            << chip
            << " is unavailable"
            << endl ;
     return;
   }
   
   int    lBin ;    
   int    hBin ;
   double lValue = 0.;
   double hValue = NCHANS;
   
   if( theRenaissance->useCutsCB->isChecked())
   {
      if( what == "Peak" )
      { 
         lValue = theRenaissance->thrMinCutLE->text().toDouble() ;
         hValue = theRenaissance->thrMaxCutLE->text().toDouble() ;
      }
      if( what == "Noise" )
      { 
         lValue = theRenaissance->noiseMinCutLE->text().toDouble() ;
         hValue = theRenaissance->noiseMaxCutLE->text().toDouble() ;
      }
      if( what == "Prob" )
      { 
         lValue = theRenaissance->CLMinCutLE->text().toDouble() ;
         hValue = theRenaissance->CLMaxCutLE->text().toDouble() ;
      }
      if( what == "Chi2" )
      { 
         lValue = theRenaissance->Chi2MinCutLE->text().toDouble() ;
         hValue = theRenaissance->Chi2MaxCutLE->text().toDouble() ;
      }
      if( what == "fitSts" )
      { 
         lValue = theRenaissance->fitStsMinCutLE->text().toDouble() ;
         hValue = theRenaissance->fitStsMaxCutLE->text().toDouble() ;
      }
   }
   else
   {
//   if( what == "Peak" )
//   {
//      lValue = theRenaissance->xMLminThreshold->getValue() ;
//      hValue = theRenaissance->xMLmaxThreshold->getValue() ;
//   } 
   
//   if( what == "Noise" )
//   {
//      lValue = theRenaissance->xMLminNoise->getValue() ;
//      hValue = theRenaissance->xMLmaxNoise->getValue() ;
//   } 

   }
   
   lBin = tempHist->FindBin(lValue) ;
   hBin = tempHist->FindBin(hValue) ;
   tempHist->GetXaxis()->SetRange(lBin,hBin) ;
   
   tempHist->Draw("SAME") ;
   tempHist->SetFillColor(107) ;
   
   HistoCanvas->GetCanvas()->Modified();
   HistoCanvas->GetCanvas()->Update();
   
   computeFractions(chip, what) ;
   
   repaintContents(false) ;
   theMw->repaint() ;
   theMw->update() ;
}
//=================================================================================================
void PixelMatrix::computeFractions(int chip, QString what)
{
   string mthn = "[PixelMatrix::computeFractions()]" ;
   cout << theFormatter->getFormatted(mthn)
         << "Computing fraction of "
         << what
         << " for chip "
         << chip
         << endl ;
   
   if( what == "Peak" ) 
   {
//      theRenaissance->peakFractionLE->setText(QString::number(hm->getFraction(chip,what)*100.,'g',4)) ;
   }
   if( what == "Noise" ) 
   {
//      theRenaissance->noiseFractionLE->setText(QString::number(hm->getFraction(chip,what)*100.,'g',4)) ;
   }
}
//=================================================================================================
void PixelMatrix::fillGlobalPlot(int chip, QString what)
{
   string mthn = "[PixelMatrix::fillGlobalPlot()]" ;
   cout << theFormatter->getFormatted(mthn)
         << "Collecting "
         << what
         << " for chip "
         << chip
         << endl ;
   
   for (int row=0; row<Nrows; row++)
   { 
      for (int col=0; col<Ncols; col++)
      {
         getFitResults(chip,row,col) ;
         double value = fitResults[chip][row][col][what] ;
         hm->fillGlobal1DPlot(chip, what,        value) ;
         hm->fillGlobal2DPlot(chip, what,row,col,value) ;
      }
   }
   
   computeFractions(chip, what) ;
}
//==================================================================================================
void PixelMatrix::getFitResults(int chip, int row, int col)
{
   string mthn = "[PixelMatrix::getFitResults()]" ;
   
   double par[2] ;
   
   TH1F * theHistogram = hm->getSCurveHistogram( chip, row, col ) ;
   if( theHistogram == NULL ) 
   {
      return ;
   }
   TF1 * f = theHistogram->GetFunction(theRenaissance->fitFunctionCB->currentText().ascii()) ;
   if( f == NULL ) 
   {
      fitResults[chip][row][col][QString("Peak")]   = -1. ;
      fitResults[chip][row][col][QString("Noise")]  = -1. ;
      fitResults[chip][row][col][QString("Chi2")]   = -1. ;
      fitResults[chip][row][col][QString("Prob")]   = -1. ;
      fitResults[chip][row][col][QString("fitSts")] = -1. ;
      return ;
   }
   
   int NDOF        = f->GetNDF() ;
   double cl       = f->GetProb();
   double chi2     = f->GetChisquare() ;
   double chi2NDOF = 0 ;
   if( NDOF > 0 ) 
   {
      chi2NDOF = chi2 / (double)NDOF ;
   }
   
   f->GetParameters(par) ;
   if( theRenaissance->fitFunctionCB->currentText() == "SCurve" ) 
   {
      fitResults[chip][row][col][QString("Peak")]  = par[0] ;
      fitResults[chip][row][col][QString("Noise")] = par[1] ;
   }
   
   if( theRenaissance->fitFunctionCB->currentText() == "Gaussian" ) 
   {
      fitResults[chip][row][col][QString("Peak")]  = par[1] ;
      fitResults[chip][row][col][QString("Noise")] = par[2] ;
   }
   
   fitResults[chip][row][col][QString("Chi2")]   = chi2NDOF ;
   fitResults[chip][row][col][QString("Prob")]   = cl ;
   fitResults[chip][row][col][QString("fitSts")] = fitSts[chip][row][col] ;
   
   return ;
}
//==================================================================================================
void PixelMatrix::display(QString what)
{
   string mthn = "[PixelMatrix::display()]" ;
   
   if( theRenaissance == NULL ) {return;} // Protection in case the init method has not yet been called
   
   currentGlobalPlot = what ;
 
   int currentChip = 0 ;
   
   if( !theRenaissance->currentChipCB->currentText().isNull() && !theRenaissance->currentChipCB->currentText().isEmpty() ) 
   {
      currentChip = theRenaissance->currentChipCB->currentText().toInt() ;
   }
   cout << theFormatter->getFormatted(mthn) 
         << "Requested display of "
         << what 
         << " for chip "
         << currentChip
         << endl ;
   
   theRenaissance->valueTypeLE ->setText(what) ;
   
   computeCurrentSwatch(currentChip, what) ;
   drawHistogram(       currentChip, 0, 0) ;
   HistoCanvas->GetCanvas()->Modified();
   HistoCanvas->GetCanvas()->Update();
   
   repaintContents(false) ;
   theMw->repaint() ;
   theMw->update() ;
}
//==================================================================================================
void PixelMatrix::computeCurrentSwatch(int chip, QString what) {
   
   string mthn = "[PixelMatrix::computeCurrentSwatch()]" ;
   
   double highestValue = -1.E+30;    
   double lowestValue  =  1.E+30; 
   int hVr = 0 ;
   int hVc = 0 ;
   int lVr = 0 ;
   int lVc = 0 ;
   
   if( what == "Peak" || what == "Noise" )
   {
      pair<double, double> peakSigma = hm->getPeakSigmaGlobalFit(chip,what) ;
      lowestValue  = peakSigma.first - peakSigma.second * 3. ; // go  back   3 sigmas
      highestValue = peakSigma.first + peakSigma.second * 3. ; // go forward 3 sigmas
      cout << theFormatter->getFormatted(mthn)
            << "Mean of "
            << what 
            << " for chip "
            << chip
            << ": "
            << peakSigma.first
            << " sigma: "
            << peakSigma.second
            << endl ;
   }
   
   if( lowestValue > highestValue ) 
   {
      double temp  = lowestValue ;
      lowestValue  = highestValue ;
      highestValue = temp ;
   }
   
   
   if( theRenaissance->useCutsCB->isChecked())
   {
      if( what == "Peak" )
      { 
         lowestValue  = theRenaissance->thrMinCutLE->text().toDouble() ;
         highestValue = theRenaissance->thrMaxCutLE->text().toDouble() ;
      }
      if( what == "Noise" )
      { 
         lowestValue  = theRenaissance->noiseMinCutLE->text().toDouble() ;
         highestValue = theRenaissance->noiseMaxCutLE->text().toDouble() ;
      }
      if( what == "Prob" )
      { 
         lowestValue  = theRenaissance->CLMinCutLE->text().toDouble() ;
         highestValue = theRenaissance->CLMaxCutLE->text().toDouble() ;
      }
      if( what == "Chi2" )
      { 
         lowestValue  = theRenaissance->Chi2MinCutLE->text().toDouble() ;
         highestValue = theRenaissance->Chi2MaxCutLE->text().toDouble() ;
      }
      if( what == "fitSts" )
      { 
         lowestValue  = theRenaissance->fitStsMinCutLE->text().toDouble() ;
         highestValue = theRenaissance->fitStsMaxCutLE->text().toDouble() ;
      }
   } 
   
   double dynamicRange = ( highestValue - lowestValue ) / (double)(colorSwatch.size() - 2) ;
   
   theRenaissance->lowestValueLE ->setText(QString::number(lowestValue ,'f',2)) ;
   theRenaissance->highestValueLE->setText(QString::number(highestValue,'f',2)) ;
   theRenaissance->lVrLE         ->setText(QString::number(lVr               )) ;
   theRenaissance->lVcLE         ->setText(QString::number(lVc               )) ;
   theRenaissance->hVrLE         ->setText(QString::number(hVr               )) ;
   theRenaissance->hVcLE         ->setText(QString::number(hVc               )) ;
   theRenaissance->highestValueLE->setText(QString::number(highestValue,'f',2)) ;
   theRenaissance->dynamicRangeLE->setText(QString::number(dynamicRange,'f',2)) ;
   
   for (int r=0; r<Nrows; r++)
   {
      for (int c=0; c<Ncols; c++)
      {
         getFitResults(chip, r,c) ;
         double value = fitResults[chip][r][c][what.ascii()] ;
         int ind = 0 ;
         if( dynamicRange != 0 )
         {
            ind = (int)(( value - lowestValue ) / dynamicRange );
         }
         
         if(ind<0) 
         {
            theColorMap[chip][r][c] = QColor(255,0,225) ;
         } else if(ind>(int)colorSwatch.size()) {
            theColorMap[chip][r][c] = QColor(0,0,0) ;
         } else {
            theColorMap[chip][r][c] = colorSwatch[ind] ;
         }
      }
   }
}
//==================================================================================================
void PixelMatrix::polish( void ) {
   
   setResizePolicy(QScrollView::Manual) ;
   
   int x = Ncols * ( cellWidthX ) ;
   int y = Nrows * ( cellWidthY ) ;
   resizeContents(x,y); // doesn't work!
}
*/
//==================================================================================================
void PixelMatrix::drawContents( QPainter * paint, int, int, int , int  )
{
   string mthn = "[PixelMatrix::drawContents()]" ;
   
   if( skipDrawing )
   {
      skipDrawing = false ;
      return ;
   }
   
   int x1= 0;
   int y1= 0;
   
   int chip ;
   
   // Draw row and column headers
   QPen  myPen(QColor(0,0,0),0,Qt::SolidLine) ; 
   QFont headerFont(QString("Courier"),7) ;
   
   STDLINE("Painting",ACRed);
   paint->setBrush( *brush_label ); 
   paint->setPen(myPen);
   paint->setFont(headerFont) ;
   int border ; 
   if( getMapName() == "pixelData" && drawSwatch ) 
   {
      //Lorechip = theRenaissance->currentChipCB->currentText().toInt() ;
      chip = 0 ;
      // Draw column tags
      for (int i=0;i<Ncols/2;i++) {
         border = border_1 ;
         if( i<10 == 0 ) border = border_2 ;
         x1 = i*cellWidthX*2 + cellWidthX * 2 + border * 3 ;
         y1 =   cellWidthY*Nrows + border_2 * 2;
         paint->drawText(x1+cellWidthX/5, y1+(int)((double)cellWidthY*2), QString::number(i)) ;
         STDLINE("Painting text",ACRed);
      }
      // Draw row tags
      for (int i=0;i<Nrows/5;i++) {
         x1 = 0;
         y1 = (i+1)*cellWidthY*5 + cellWidthY -border_1 *2  ;
         paint->drawText(x1+2, y1+(int)((double)cellWidthY/1.3), QString::number((Nrows/5-i)*5-5) );      }
   }
   else if( getMapName() == "pixelMap" && drawSwatch) {
      //LOREchip = theRenaissance->chipCB->currentText().toInt() ;
      chip = 0 ;
      // Draw column tags
      for (int i=0;i<Ncols/2;i++) {
         border = border_1 ;
         if( i<10 == 0 ) border = border_2 ;
         x1 = i*cellWidthX*2 + cellWidthX * 2 + border * 3 ;
         y1 =   cellWidthY*Nrows + border_2 * 26 ;
         paint->drawText(x1+cellWidthX/5, y1+(int)((double)cellWidthY*2), QString::number(i)) ;
      }
      // Draw row tags
      for (int i=0;i<Nrows/5;i++) {
         x1 = 0;
         y1 = (i+1)*cellWidthY*5 + cellWidthY -border_1 *2  ;
         paint->drawText(x1+2, y1+(int)((double)cellWidthY/1.3), QString::number((Nrows/5-i)*5-5) );      }
   }

   if( getMapName() == "pixelData" && drawSwatch )
   {
      // Draw color swatch
      brush_filled->setColor(QColor(224,255,255)) ; // +
      paint->setBrush( *brush_filled );             // |---> Color swatch background palette
      paint->drawRect(488, 1, 60, 503);             // +
      
      brush_filled->setColor(QColor(255,0,225)) ;   // +
      paint->setBrush( *brush_filled );             // |---> Underflow
      paint->drawRect(492, 495, cellWidthX, 6);     // |
      paint->drawText(502, 500, "Und." );           // +
      
      brush_filled->setColor(QColor(0,0,0)) ;       // +
      paint->setBrush( *brush_filled );             // |---> Overflow
      paint->drawRect(492,   4, cellWidthX, 6);     // |
      paint->drawText(502,  10, "Ovf." );           // +
      
      double fromRInd = 0 ;
      double toRInd   = 255 ;
      /*
      if( theRenaissance->useCutsCB->isChecked())
      {
         if( theRenaissance->showPeakDistributionPB->isOn()   || theRenaissance->showPeakDistributionPlotPB->isOn())
         { 
            fromRInd  = theRenaissance->thrMinCutLE->text().toDouble() ;
            toRInd    = theRenaissance->thrMaxCutLE->text().toDouble() ;
         }
         if( theRenaissance->showNoiseDistributionPB->isOn()  || theRenaissance->showNoiseDistributionPlotPB->isOn())
         { 
            fromRInd  = theRenaissance->noiseMinCutLE->text().toDouble() ;
            toRInd    = theRenaissance->noiseMaxCutLE->text().toDouble() ;
         }
         if( theRenaissance->showCLDistributionPB->isOn()     || theRenaissance->showCLDistributionPlotPB->isOn())
         { 
            fromRInd  = theRenaissance->CLMinCutLE->text().toDouble() ;
            toRInd    = theRenaissance->CLMaxCutLE->text().toDouble() ;
         }
         if( theRenaissance->showChi2DistributionPB->isOn()   || theRenaissance->showChi2DistributionPlotPB->isOn())
         { 
            fromRInd  = theRenaissance->Chi2MinCutLE->text().toDouble() ;
            toRInd    = theRenaissance->Chi2MaxCutLE->text().toDouble() ;
         }
         if( theRenaissance->showFitStsDistributionPB->isOn() || theRenaissance->showFitStsDistributionPlotPB->isOn())
         { 
            fromRInd  = theRenaissance->fitStsMinCutLE->text().toDouble() ;
            toRInd    = theRenaissance->fitStsMaxCutLE->text().toDouble() ;
         }
      }
      */
      int step = 5 ;
      double dynamicRange = (toRInd - fromRInd) / (double)colorSwatch.size() ;
      // The full color swatch    
      for( int ind=3; ind<(int)colorSwatch.size()-3; ind++ )
      {
         int rInd = (int)colorSwatch.size() - ind - 1 ; // Reverse index
         x1 = 535 ;
//         y1 = ind*4 + 4 ;
         y1 = ind*4;
         brush_filled->setColor(colorSwatch[rInd]) ;
         paint->setBrush( *brush_filled );
         paint->drawRect(x1,y1, cellWidthX, 6); 
         if( ind % step == 0 && ind > 0)
         {
            x1 = 500 ;
//            y1 = (int)((double)ind*4) + 10;
            y1 = (int)((double)ind*4);
            double n = dynamicRange * rInd + fromRInd;
            paint->drawText(x1, y1, QString::number(n,'f',2));   
         }
      }
   }
   
   myPen.setColor(QColor(125,80,24)) ;
   if( getMapName() == "pixelMap" && drawDc ) 
   {
      // Draw double column markers
      int dc = 0 ;
      for (int i=0;i<Ncols;i+=2) 
      {
         if( dcState[chip][dc] )
         {
            paint->setBrush( *brush_dcol ); 
         } else {
            paint->setBrush( *brush_dcolNo ); 
         }
         x1 = i*(cellWidthX) + cellWidthX*2 + (int)((double)cellWidthX*2./3.)  ;
         y1 = cellWidthY + border_2 ;
         paint->drawRect(x1,y1, cellWidthX - border_1, cellWidthY*Nrows + border_2 * 3); 
         dc++ ;
      }
   }
   
   // Draw cell content
   int border_x ;   
   int border_y ;   
   for (int i=0;i<Ncols;i++) {
      border_x = border_2 ;
      if( i%2 == 0 ) border_x = border_1 ;
      for (int j=0;j<Nrows;j++) {
         border_y = border_2 ;
         x1 = i*cellWidthX + cellWidthX*2 + border_x ;
//         y1 = j*cellWidthY + cellWidthY   + border_y ;
         y1 = (Nrows-j-1)*cellWidthY + cellWidthY   + border_y ;
         
         if( getMapName() == "pixelData" )
         { 
            //LOREchip = theRenaissance->currentChipCB->currentText().toInt() ;
            chip = 0 ;
            if ( values[chip][i][j] == 0)
            {
               brush_filled->setColor(theColorMap[chip][j][i]) ;
               paint->setBrush( *brush_filled );
            } else {
               paint->setBrush( *brush_selected ); 
            }
            paint->drawRect(x1,y1, cellWidthX - border_2, cellWidthY - border_2); 
         }
         
         if( getMapName() == "pixelMap" )
         {
            //            brush_filled->setColor(cellDisabled) ;
            if ( enableMask[chip][i][j] == 0)
            {
               brush_filled->setColor(cellDisabled) ;
               if( injectMask[chip][i][j] == INJECT_WITH_CAPACITOR )
               {
                  brush_filled->setColor(disableCellButInjectWithCapacitor) ;
               }
               if( injectMask[chip][i][j] == INJECT_WITH_BUMP_BOND )
               {
                  brush_filled->setColor(disableCellButInjectWithBumpBond) ;
               } 
               //               if( injectMask[i][j] == INJECT_DISABLED )
               //               {
               //                  brush_filled->setColor(salmon) ;
               //               } 
            } else {
               if(        injectMask[chip][i][j] == INJECT_WITH_CAPACITOR ) {
                  brush_filled->setColor(enableCellAndInjectWithCapacitor) ;
               } else if( injectMask[chip][i][j] == INJECT_WITH_BUMP_BOND ) {
                  brush_filled->setColor(enableCellAndInjectWithBumpBond) ;
               } else if( injectMask[chip][i][j] == INJECT_DISABLED ) {               
                  brush_filled->setColor(enableCellButInjectDisabled) ;
               } else {
                  brush_filled->setColor(salmon) ;
               }
            }
            paint->setBrush( *brush_filled ); 
            paint->drawRect(x1,y1, cellWidthX - border_2 -1, cellWidthY - border_2); 
         }
      }
   }
   
   drawDc     = true ;
   drawSwatch = true ;
}
/*
//==================================================================================================
void PixelMatrix::clipRegion(int *x,int *y,int clipx,int clipy,int , int ) {
   if (*x < clipx) *x=clipx;
   if (*y < clipy) *y=clipy;
   // if (*x+cellWidth > clipx+clipw-1 ) *x=clipx+clipw-1;
   // if (*y > clipy+cliph-1) *y=clipy+cliph-1;
}
//==================================================================================================
bool PixelMatrix::eventFilter( QObject * watched, QEvent * event )
{ 
   string mthn = "[PixelMatrix::eventFilter()]" ;
   int chip ;
   
   QMouseEvent* mouseEvent = NULL ;
   
   if ( event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress) 
   {
      mouseEvent = (QMouseEvent*)event;
      int posX = mouseEvent->x() ;
      int posY = mouseEvent->y()+3 ;
      int dX = abs(posX - lastX) ;
      int dY = abs(posY - lastY) ;
      theRenaissance->PeakResultLE ->setText(QString::number(posX,'f',2)) ;
      theRenaissance->NoiseResultLE->setText(QString::number(posY,'f',2)) ;
      lastX = posX ;
      lastY = posY ;
      if( dX <= 1 && dY<= 1 ) return true ; 
      int row = ( posY - cellWidthY) / ( cellWidthY ) - 1 ;
      int col = ( posX - cellWidthX) / ( cellWidthX ) - 1 ;
      if (row < 0) row = 0 ;
      if (row > Nrows - 1) row = Nrows - 1 ;
      if (col < 0) col = 0 ;
      if (col > Ncols - 1) col = Ncols - 1 ;
      row = Nrows - row - 1 ;
      
      if( getMapName() == "pixelData" )
      {
         chip = theRenaissance->currentChipCB->currentText().toInt() ;
         theRenaissance->currentRowLE->setText(QString::number(row)) ;
         theRenaissance->currentColLE->setText(QString::number(col)) ;
         drawHistogram(chip, row, col); 
         getFitResults(chip, row, col) ;
         theRenaissance->PeakResultLE ->setText(QString::number(fitResults[chip][row][col]["Peak"] ,'f',2)) ;
         theRenaissance->NoiseResultLE->setText(QString::number(fitResults[chip][row][col]["Noise"],'f',2)) ;
         theRenaissance->clResultLE   ->setText(QString::number(fitResults[chip][row][col]["Prob"] ,'g',2)) ;
         theRenaissance->chi2ResultLE ->setText(QString::number(fitResults[chip][row][col]["Chi2"] ,'f',2)) ;
         theRenaissance->redLE        ->setText(QString::number((theColorMap[chip][row][col]).red())) ;
         theRenaissance->greenLE      ->setText(QString::number((theColorMap[chip][row][col]).green())) ;
         theRenaissance->blueLE       ->setText(QString::number((theColorMap[chip][row][col]).blue())) ;
         
         if( fitResults[chip][row][col]["fitSts"] == 1 )
         {
            theRenaissance->fitStsLE->setText("Approximation only, not accurate") ;
            theRenaissance->fitStsLE->setPaletteBackgroundColor(QColor(255,0,0)) ;
         }
         if( fitResults[chip][row][col]["fitSts"] == 2 )
         {
            theRenaissance->fitStsLE->setText("Full matrix, but forced positive-definite") ;
            theRenaissance->fitStsLE->setPaletteBackgroundColor(QColor(255,234,3)) ;
         }
         if( fitResults[chip][row][col]["fitSts"] == 3 )
         {
            theRenaissance->fitStsLE->setText("Full accurate covariance matrix") ;
            theRenaissance->fitStsLE->setPaletteBackgroundColor(QColor(0,255,0)) ;
         }
         
         for (int r=0; r<Nrows; r++) {
            for (int c=0; c<Ncols; c++) {
               values[chip][c][r] = 0 ;
               if (r == row && c == col) values[chip][c][r] = 1 ;
            }
         }
         viewport()->update();      
      }
      if( getMapName() == "pixelMap" )
      {
         chip = theRenaissance->chipCB->currentText().toInt() ;
         drawDc = false ;
         theRenaissance->currentPMColLE->setText(QString::number(col)) ;
         theRenaissance->currentPMRowLE->setText(QString::number(row)) ;
         if( theRenaissance->bumpBondInjectCB->isChecked() )
         {
            bumpSelection =  INJECT_WITH_BUMP_BOND ;
         } else {
            bumpSelection =  INJECT_WITH_CAPACITOR ;
         }
         if( mouseEvent != NULL && mouseEvent->button() == Qt::NoButton) 
         {
            minRow = min(row,minRow) ;
            maxRow = max(row,maxRow) ;
            minCol = min(col,minCol) ;
            maxCol = max(col,maxCol) ;
            for (int r=minRow; r<=maxRow; r++) {
               for (int c=minCol; c<=maxCol; c++) {
                  
                  if( enableMask[chip][c][r] != buttonSelection )
                  {
                     enableMask[chip][c][r] = buttonSelection ;
                     // hardware call
                     theDaqManager->enablePixel(theDaqManager->getCurrentCard(),
                                                theDaqManager->getCurrentTbm(),
                                                chip,
                                                r,c,buttonSelection);
                  }
                  
                  if( buttonSelection == ACTIVATEBUTTON && theRenaissance->injectionCB->isChecked() )
                  {
                     if( injectMask[chip][c][r] != bumpSelection )
                     {
                        injectMask[chip][c][r] = bumpSelection ;
                        // hardware call
                        theDaqManager->injectPixel(theDaqManager->getCurrentCard(),
                                                     theDaqManager->getCurrentTbm(),
                                                     chip,
                                                     r,c,bumpSelection);
                     }
                  }
               }
            }
         } 
         if( mouseEvent != NULL && mouseEvent->button() == Qt::LeftButton) 
         {
            minCol = col ;
            maxCol = col ;
            minRow = row ;
            maxRow = row ;
            buttonSelection = ACTIVATEBUTTON ;
            if( enableMask[chip][col][row] != buttonSelection )
            {
               enableMask[chip][col][row] = buttonSelection ;
               // hardware call
               
               theDaqManager->enablePixel(theDaqManager->getCurrentCard(),
                                          theDaqManager->getCurrentTbm(),
                                          chip,
                                          row,col,buttonSelection);
            }
            if( theRenaissance->injectionCB->isChecked() )
            {
               injectMask[chip][col][row] = buttonSelection * bumpSelection;
               // hardware call
               cout << theFormatter->getFormatted(mthn) 
                     << "Injection type: " 
                     << bumpSelection
                     << endl;
               theDaqManager->injectPixel(theDaqManager->getCurrentCard(),
                                            theDaqManager->getCurrentTbm(),
                                            chip,
                                            row,col,bumpSelection);
            }
         } 
         if( mouseEvent != NULL && mouseEvent->button() == Qt::RightButton) 
         {
            minCol = 1000 ;
            maxCol = 0 ;
            minRow = 1000 ;
            maxRow = 0 ;
            buttonSelection = DEACTIVATEBUTTON ;
            if( enableMask[chip][col][row] != buttonSelection )
            {
               enableMask[chip][col][row] = buttonSelection ;
               // hardware call
               theDaqManager->enablePixel(theDaqManager->getCurrentCard(),
                                          theDaqManager->getCurrentTbm(),
                                          chip,
                                          row,col,buttonSelection);
            }
         } 
         //         if( mouseEvent != NULL && mouseEvent->button() == Qt::MidButton) 
         //         {
         //            minCol = col ;
         //            maxCol = col ;
         //            minRow = row ;
         //            maxRow = row ;
         //            if( mouseEvent->state()  == 257) 
         //            {
         //               selecting = 0 ;
         //            } else {
         //               selecting = 1 ;
         //            }
         //         } 
      }
      viewport()->update();      
   }
   return QScrollView::eventFilter( watched, event );
}
//==================================================================================================
void PixelMatrix::drawHistogram(int chip, int row, int col)
{
   string mthn = "[PixelMatrix::drawHistogram()]" ;
   stringstream msg ;
 
   TH1F * theHistogram ;
   if( theRenaissance->sCurveLinearityToggleCB->isChecked())
   {
      theHistogram = hm->getSCurveTestHistogram( chip, row, col ) ;
   } else {
      theHistogram = hm->getLinearityHistogram(  chip, row, col ) ;
   }
   if( theHistogram == NULL )
   {
      msg.str("") ;
      msg << " Histogram for chip "
            << chip
            << " row "
            << row
            << " and col "
            << col
            << " is unavailable (incomplete data set)." ;
      theMw->statusBar()->message(mthn + QString(msg.str().c_str())) ;
      HistoCanvas->GetCanvas()->Clear() ;
      HistoCanvas->GetCanvas()->Modified();
      HistoCanvas->GetCanvas()->Update();
      update() ;
      return ;
   } else {
      msg.str("") ;
      msg << " Drawing histogram for chip "
            << chip
            << " row "
            << row
            << " and col "
            << col ;
      theMw->statusBar()->message(mthn + QString(msg.str().c_str())) ;
   }
   
   HistoCanvas->GetCanvas()->cd() ;
   QString optionList = "" ;
    
    
   for( int i=0; i<(int)theRenaissance->monoDOptionsBG->count(); i++ )
   {
      if(               theRenaissance->monoDOptionsBG->find(i) != NULL &&
          ((QCheckBox *)theRenaissance->monoDOptionsBG->find(i))->isChecked() )
      {
         optionList += theRenaissance->monoDOptionsBG->find(i)->text() ;
         optionList += " " ;
      }
   }
       
   TF1 * f ;
   TF1 * f1 = NULL;
   if( theRenaissance->sCurveLinearityToggleCB->isChecked())
   {
      if( theRenaissance->histogramCB->isChecked() ) optionList+= "HIST " ;
      f = theHistogram->GetFunction(theRenaissance->fitFunctionCB->currentText().ascii()) ;
   } else {
 //     f = theHistogram->GetFunction("line") ;
      f = theHistogram->GetFunction("linearGain") ;
      f1 = theHistogram->GetFunction("gainSaturation") ;
   }
      
   gStyle->SetPalette(1,0) ;
   theHistogram->SetStats(theRenaissance->statisticsBoxCB->isChecked()) ;
   theHistogram->Draw(optionList.ascii()) ;
   theHistogram->SetLabelSize(0.06,"X") ;
   theHistogram->SetLabelSize(0.06,"Y") ;
   theHistogram->SetFillColor(theRenaissance->xMLhistogramBackgroundColor->getValue()) ;
   theHistogram->SetMarkerStyle( theRenaissance->markerStyleCB->currentText().toInt()) ;
   theHistogram->SetMarkerSize(  theRenaissance->markerSizeLE ->text().toDouble()) ;
   theHistogram->SetMarkerColor( markerColorsMap[theRenaissance->markerColorCB->currentText().ascii()]) ;
   gStyle->SetOptFit(111111) ;
   gStyle->SetOptLogx(theRenaissance->logScaleLargeX->isChecked()) ;
   gStyle->SetOptLogy(theRenaissance->logScaleLargeY->isChecked()) ;
   gStyle->SetOptLogz(theRenaissance->logScaleLargeZ->isChecked()) ;
   
   if ( f != NULL ) 
   {
      {
         f->Draw("SAME") ;
         f->SetLineWidth(1) ;
         f->SetLineColor(2) ;
      }
   }
   if ( f1 != NULL ) 
   {
      {
         f1->Draw("SAME") ;
         f1->SetLineWidth(1) ;
         f1->SetLineColor(4) ;
      }
   }
   
   HistoCanvas->GetCanvas()->Modified();
   HistoCanvas->GetCanvas()->Update();
}
//==================================================================================================
void PixelMatrix::enableAllCells(int chip, bool state)
{
   for (int row=0; row<Nrows; row++) {
      for (int col=0; col<Ncols; col++) {
         enableMask[chip][col][row] = (int)state ;
         theDaqManager->enablePixel(theDaqManager->getCurrentCard(),
                                    theDaqManager->getCurrentTbm(),
                                    chip,
                                    row,col,state);
      }
   }
   //   viewport()->update();      
   theMw->repaint() ;
   theMw->update() ;
   viewport()->repaint();      
}
//==================================================================================================
void PixelMatrix::enableAllDoubleColumns(int chip, bool state)
{
   for (int c=0; c<Ncols/2; c++ )
   {
      dcState[chip][c] = state ;
      theDaqManager->enableDoubleColumn(theDaqManager->getCurrentCard(),
                                        theDaqManager->getCurrentTbm(),
                                        chip,
                                        c,
                                        state) ;
   }  
   //   viewport()->update();      
   viewport()->repaint();      
}
//==================================================================================================
void PixelMatrix::dumpValues() {
   //   for (int j=0;j<Nrows;j++) {
   //      for (int i=0;i<Ncols;i++) {
   //         cout << values[i][j] << " ";
   //      }
   //      cout << endl;
   //   }
}
//==================================================================================================
void PixelMatrix::resize()
{
   int x = Ncols * ( cellWidthX ) ;
   int y = Nrows * ( cellWidthY ) ;
   cout << "[PixelMatrix::resize()]\tx: " << x << endl ;
   cout << "[PixelMatrix::resize()]\ty: " << y << endl ;
   resizeContents(x,y);
}
//==================================================================================================
void PixelMatrix::getLimitsFromHistogram(int chip)
{
   string mthn = "[PixelMatrix::getLimitsFromHistogram()]" ;
   int index = theRenaissance->singleCellValuesTL->selectedId() ;
   if( index > (int)distributionTypes.size() )
   {
      index -= 10 ;
   }
   cout << theFormatter->getFormatted(mthn)
         << "Getting limits from histogram of chip "
         << chip
         << " for "
         << distributionTypes[index]
         << endl ;
   
   double xlow = hm->getLowLimit(chip, distributionTypes[index]) ;
   double xhig = hm->getHigLimit(chip, distributionTypes[index]) ;
   
   if( theRenaissance->showPeakDistributionPlotPB->isOn())
   { 
      theRenaissance->thrMinCutLE   ->setText(QString::number(xlow,'f',2)) ;
      theRenaissance->thrMaxCutLE   ->setText(QString::number(xhig,'f',2)) ;
   }
   if( theRenaissance->showNoiseDistributionPlotPB->isOn())
   { 
      theRenaissance->noiseMinCutLE ->setText(QString::number(xlow,'f',2)) ;
      theRenaissance->noiseMaxCutLE ->setText(QString::number(xhig,'f',2)) ;
   }
   if( theRenaissance->showCLDistributionPlotPB->isOn())
   { 
      theRenaissance->CLMinCutLE    ->setText(QString::number(xlow,'f',2)) ;
      theRenaissance->CLMaxCutLE    ->setText(QString::number(xhig,'f',2)) ;
   }
   if( theRenaissance->showChi2DistributionPlotPB->isOn())
   { 
      theRenaissance->Chi2MinCutLE  ->setText(QString::number(xlow,'f',2)) ;
      theRenaissance->Chi2MaxCutLE  ->setText(QString::number(xhig,'f',2)) ;
   }
   if( theRenaissance->showFitStsDistributionPlotPB->isOn())
   { 
      theRenaissance->fitStsMinCutLE->setText(QString::number(xlow,'f',2)) ;
      theRenaissance->fitStsMaxCutLE->setText(QString::number(xhig,'f',2)) ;
   }
   
   
}
//==================================================================================================
void PixelMatrix::dcButton_released(int whichDc)
{
   int chip ;
   if( getMapName() == "pixelData" )
   {
      chip = theRenaissance->currentChipCB->currentText().toInt() ;
   } else {
      chip = theRenaissance->chipCB->currentText().toInt() ;
   }
   dcState[chip][whichDc] = !dcState[chip][whichDc] ;
   theDaqManager->enableDoubleColumn(theDaqManager->getCurrentCard(),
                                     theDaqManager->getCurrentTbm(),
                                     chip,
                                     whichDc,
                                     dcState[chip][whichDc]) ;
   //   viewport()->update();      
   viewport()->repaint();      
}
*/
//==================================================================================================
void PixelMatrix::rowButton_released(int whichRow)
{
    STDLINE(whichRow,ACGreen);
   whichRow = Nrows - whichRow - 1 ;
   int sts ;
   int chip ;
   if( getMapName() == "pixelData" )
   {
      chip = 0 ;
   } else {
      chip = 0 ;
   }
   STDLINE(whichRow,ACRed);
   /*
   if( theRenaissance->bumpBondInjectCB->isChecked() )
   {
      bumpSelection =  INJECT_WITH_BUMP_BOND ;
   } else {
      bumpSelection =  INJECT_WITH_CAPACITOR ;
   }
   rowEnableState[chip][whichRow] = !rowEnableState[chip][whichRow] ;
   if( theRenaissance->injectionCB->isChecked()) 
   {
      rowInjectState[chip][whichRow] = !rowInjectState[chip][whichRow] ;
   }
   if( rowEnableState[chip][whichRow] ) 
   { 
      sts = 1 ;
   } else {
      sts = 0 ;
   }
   
   bool injectIt = rowInjectState[chip][whichRow] && theRenaissance->injectionCB->isChecked() ;
   
   for (int c=0; c<Ncols; c++)
   {
      enableMask[chip][c][whichRow] = sts;
      
      if( injectIt ) injectMask[chip][c][whichRow] = bumpSelection ;
   }
   */
   //   viewport()->update();      
   viewport()->repaint();      
}
/*
//==================================================================================================
void PixelMatrix::colButton_released(int whichCol)
{
   int sts ;
   int chip ;
   if( getMapName() == "pixelData" )
   {
      chip = theRenaissance->currentChipCB->currentText().toInt() ;
   } else {
      chip = theRenaissance->chipCB->currentText().toInt() ;
   }
   if( theRenaissance->bumpBondInjectCB->isChecked() )
   {
      bumpSelection =  INJECT_WITH_BUMP_BOND ;
   } else {
      bumpSelection =  INJECT_WITH_CAPACITOR ;
   }
   colEnableState[chip][whichCol] = !colEnableState[chip][whichCol] ;
   if( theRenaissance->injectionCB->isChecked()) 
   {
      colInjectState[chip][whichCol] = !colInjectState[chip][whichCol] ;
   }
   if( colEnableState[chip][whichCol] ) 
   { 
      sts = 1 ;
   } else {
      sts = 0 ;
   }
   
   bool injectIt = colInjectState[chip][whichCol] && theRenaissance->injectionCB->isChecked() ;
   
   for (int r=0; r<Nrows; r++)
   {
      enableMask[chip][whichCol][r] = sts;
      
      if( injectIt ) injectMask[chip][whichCol][r] = bumpSelection ;
   }
   //   viewport()->update();      
   viewport()->repaint();      
}
*/
//============================================================================
void PixelMatrix::updateThisViewport(bool state)
{
   // state = 0:bump-bond injection,  1:capacitor injection
   int chip ;
   if( getMapName() == "pixelData" )
   {
      chip = 0 ;
   } else {
      chip = 0 ;
      for( int c=0; c<Ncols; c++ )
      {
         for( int r=0; r<Nrows; r++ )
         {
            if( injectMask[chip][c][r] != INJECT_DISABLED )
            {
               if( state ) 
               {
                  injectMask[chip][c][r] = INJECT_WITH_BUMP_BOND ;
               } else {
                  injectMask[chip][c][r] = INJECT_WITH_CAPACITOR ;
               }
            }
         }
      }
   }
   //   viewport()->update();      
   viewport()->repaint();      
}
/*
//============================================================================
void PixelMatrix::clearAllInjections(int chip)
{
   for (int c=0; c<Ncols; c++)
   {
      for( int r=0; r<Nrows; r++)
      {
         injectMask[chip][c][r] = 0 ;
      }
   }
   theDaqManager
         ->getPciBus()
         ->getCard(  theRenaissance->cardCB->currentText().toULong())
         ->getDevice(theRenaissance->TBMCB ->currentText().toULong())
         ->getDetector(chip)
         ->clearCal() ;
   viewport()->repaint();      
}
//============================================================================
void PixelMatrix::reportCalibrationResults()
{
   string mthn = "[PixelMatrix::reportCalibrationResults()]" ;
   for( int chipList=0; chipList<(int)chipSelectionList.size(); chipList++)
   {
      int chip = chipSelectionList[chipList] ;
      theRenaissance->finalSCurvesResultsT->setRowHeight(chip,12) ;
      
      pair<double, double> threshold ;
      pair<double, double> noise ;
      threshold = hm->getPeakSigmaGlobalFit(chip, "Peak") ;
      noise     = hm->getPeakSigmaGlobalFit(chip, "Noise") ;
      theRenaissance->finalSCurvesResultsT->setText( chip, 0,QString::number(threshold.first, 'f',1)) ;
      theRenaissance->finalSCurvesResultsT->setText( chip, 1,QString::number(threshold.second,'f',1)) ;
      theRenaissance->finalSCurvesResultsT->setText( chip, 2,QString::number(noise.first,     'f',1)) ;
      theRenaissance->finalSCurvesResultsT->setText( chip, 3,QString::number(noise.second,    'f',1)) ;
      
      pair<int, int> lowHighPeak ;
      pair<int, int> lowHighNoise ;
      lowHighPeak  = hm->getExceedGlobalFit(chip, "Peak") ;
      lowHighNoise = hm->getExceedGlobalFit(chip, "Noise") ;
      theRenaissance->finalSCurvesResultsT->setText( chip, 4,QString::number(lowHighPeak.first  )) ;
      theRenaissance->finalSCurvesResultsT->setText( chip, 5,QString::number(lowHighPeak.second )) ;
      theRenaissance->finalSCurvesResultsT->setText( chip, 6,QString::number(lowHighNoise.first )) ;
      theRenaissance->finalSCurvesResultsT->setText( chip, 7,QString::number(lowHighNoise.second)) ;
      
      pair<double, double> kurtSkewPeak ;
      pair<double, double> kurtSkewNoise ;
      kurtSkewPeak  = hm->getShapeGlobalFit(chip, "Peak") ;
      kurtSkewNoise = hm->getShapeGlobalFit(chip, "Noise") ;
      theRenaissance->finalSCurvesResultsT->setText( chip, 8,QString::number(kurtSkewPeak.first  ,'f',1)) ;
      theRenaissance->finalSCurvesResultsT->setText( chip, 9,QString::number(kurtSkewPeak.second ,'f',1)) ;
      theRenaissance->finalSCurvesResultsT->setText( chip,10,QString::number(kurtSkewNoise.first ,'f',1)) ;
      theRenaissance->finalSCurvesResultsT->setText( chip,11,QString::number(kurtSkewNoise.second,'f',1)) ;
   }
   
}
//============================================================================
void PixelMatrix::prendoDentro(const QString & )
{
}
//====================================================================
void PixelMatrix::ErrorGenerator (TH1F * Data, TArrayD * errorData)
{
   string mthn = "[PixelMatrix::ErrorGenerator()]" ;
   
   double effo;
   
   errorData->Set(Data->GetNbinsX()+1);
   double kevents = (double)Data->GetEntries() ;
   if( kevents == 0 ) 
   {

  //    cout << theFormatter->getFormatted(mthn)
  //          << "No entries for histogram "
  //          << Data->GetTitle()
  //          << endl ;

      for ( int i = 0; i < Data->GetNbinsX()+1; i++ )
         errorData->AddAt(0.001, i);
      return ;
   }    
   effo = 0.5/kevents;
   errorData->AddAt(TMath::Sqrt(effo*(1.0-effo)/kevents), 0);
   
   bool firstNonZero = false ;   
   for ( int i = 1; i < Data->GetNbinsX()+1; i++ )
   {
      effo = Data->GetBinContent(i);
      if (effo == 0)
      {
         if( !firstNonZero )
         {
            effo = 0.5/kevents;
         } else {
            effo = 0.;
         }
      }
      else
      {
         firstNonZero = true ;
         if (effo == 1)
         {
            effo = 1. - 0.5/kevents;
         }
      }
      errorData->AddAt(TMath::Sqrt(effo*(1.0-effo)/kevents), i);
   }
}

//================================================================================
double PixelMatrix::SCurve(double * x, double * par)
{
   double answer;
   double erf_arg;
   double sqrt2 = sqrt(2.);
   double ynorm;
   
   ynorm = (x[0] - par[0]) / par[1];
   if (ynorm>0)
   {
      erf_arg = -ynorm/sqrt2;
      answer = TMath::Erfc(erf_arg)/2.;
   }
   else
   {
      erf_arg = ynorm/sqrt2;
      answer = 1. - TMath::Erfc(erf_arg)/2.;
   }
   
   return answer;
}
//================================================================================
double PixelMatrix::Gaussian (double * x, double * par)
{
   Double_t arg = 0;
   if (par[2] != 0) arg = (x[0] - par[1])/par[2];
   
   Double_t fitval = par[0]*TMath::Exp(-0.5*arg*arg);
   return fitval;
}
*/
