#ifndef _PIXELMATRIX
#define _PIXELMATRIX

#include <qwidget.h>
#include <QScrollArea>
#include <qspinbox.h>
#include <qstatusbar.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qregexp.h>
#include <map>
#include <qstring.h>
#include <TQtWidget.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TKey.h>
#include <TStyle.h>
#include <TLine.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TVirtualFitter.h>

//#include "HManager.h"
//#include "Chronos.h"
//#include "Constants.h"


#define NCHANS 256

class QBrush;
class QColor;
class QPainter;
class QPen;
class Formatter;
class QGroupBox;

typedef std::map<int, std::map<int, std::map<int, int> > >                        map4Int ;
typedef std::map<int, std::map<int, std::map<int, std::map<QString, double> > > > cellValues ;
typedef std::map<int, std::map<int, std::map<int, int> > >                        map4Int ;
typedef std::map<int, std::map<int, std::map<int, double> > >                     map3IntDouble ;

class PixelMatrix : public QScrollArea
{
   
   Q_OBJECT
   Q_PROPERTY( QString theMapName  READ getMapName     WRITE setMapName     )
   Q_PROPERTY( int Ncols           READ getNcols       WRITE setNcols       )
   Q_PROPERTY( int Nrows           READ getNrows       WRITE setNrows       )
   Q_PROPERTY( int cellWidthX      READ getcellWidthX  WRITE setcellWidthX  )
   Q_PROPERTY( int cellWidthY      READ getcellWidthY  WRITE setcellWidthY  )
   Q_PROPERTY( int border_1        READ getBorder_1    WRITE setBorder_1    )
   Q_PROPERTY( int border_2        READ getBorder_2    WRITE setBorder_2    )
  
public:
   PixelMatrix( QWidget* parent = 0);
   virtual ~PixelMatrix();
   
//   bool    eventFilter(QObject *,QEvent *) ;
   
   //void    init(QMainWindow * mw, TQtWidget *, TQtWidget *) ;
   void    init() ;
   void    destroy(void) ;
   
   void    resize(void) ;
   void    setupColorSwatch(void) ;
   int     getNcols ()        const  {return Ncols;      }
   int     getNrows ()        const  {return Nrows;      }
   int     getcellWidthX ()   const  {return cellWidthX; }
   int     getcellWidthY ()   const  {return cellWidthY; }
   int     getBorder_1()      const  {return border_1;   }
   int     getBorder_2()      const  {return border_2;   }
   QString getMapName()       const  {return theMapName; }
   
   void    setNcols(int i)           {Ncols       = i;}
   void    setNrows(int i)           {Nrows       = i;}
   void    setcellWidthX(int i)      {cellWidthX  = i;}
   void    setcellWidthY(int i)      {cellWidthY  = i;}
   void    setBorder_1(int i)        {border_1    = i;}
   void    setBorder_2(int i)        {border_2    = i;}
   void    setMapName(QString m)     {theMapName  = m;}
//   void    setRootFile(QString f)  {rootFile    = f;}
   map4Int getEnableMask()           {return enableMask;}
   map4Int getInjectMask()           {return injectMask;}
   void    drawHistogram(         int, int, int ) ;
   void    Calibrate(QString,     unsigned long trim=0);
   void    CalibrateGain( double fromCal, double toCal, double calSteps, unsigned long trim=0);
   void    display(                    QString);
   void    computeCurrentSwatch(  int, QString);
   void    displayPlot(           int, QString);
   void    displayModulePlot(     int);
   void    getLimitsFromHistogram(int) ;
   void    dumpValues(            void);
//   void    save(QString);
   void    fillGlobalPlot(        int, QString) ;
   void    computeFractions(      int, QString) ;
   void    updateThisViewport(    bool) ;
//   void    clearAllInjections(    int ) ;
   void    enableAllCells(        int, bool) ;
   void    enableAllDoubleColumns(int, bool) ;
   void    changeColorSwatch(     int, QColor) ;
   void    setActiveChipList(std::vector<int> list) {chipSelectionList = list;}
   void    doFits(                void) ;
   bool    doLinearFits(          void) ;
   void    restoreChi2AndFitSts(  void) ;
//   void    setVCalDelDefaults(map<int, int>) ;
   
public slots:
//   void    dcButton_released( int) ;
   void    rowButton_released(int) ;
//   void    colButton_released(int) ;
//   void    prendoDentro(const QString &) ;
//   void    reportCalibrationResults(void) ;
   
private:
   
   enum {
      DEACTIVATEBUTTON = 0x0,
      ACTIVATEBUTTON   = 0x1 
   } ;
   
   enum {
      INJECT_DISABLED       = 0x0,
      INJECT_WITH_CAPACITOR = 0x1,
      INJECT_WITH_BUMP_BOND = 0x2
   } ;
   
   void   polish(                  void) ;
   void   estimateSCurve(          int, int, int ) ;
   void   fitSCurve(               int, int, int ) ;
   double computeChisquare(        int, int, int) ;
   
//   static double    SCurve(   double *, double *) ;
//   static double    Gaussian( double *, double *) ;

   bool             dataFromFile ;
   
   void             ErrorGenerator(TH1F *, TArrayD *) ;
   void             getFitResults(int, int, int) ;
   void             getHistogramsFromFile(QString) ;
   QWidget*         parent_;
   QWidget*         placeHolder_;
   Formatter      * theFormatter ;
   //Chronos        * theTimer ;
   //QMainWindow    * theMw ;
   TQtWidget      * HistoCanvas ;
   TQtWidget      * bigHistoCanvas ;
   
   TArrayD        * errorData ;
   TVirtualFitter * fitter ;
   
   QGroupBox   * dcButtonGroup ;
   QButtonGroup   * rowButtonGroup ;
   QGroupBox   * colButtonGroup ;
   
   //DaqManager     * theDaqManager ;
   
   TF1            * FitFun ;
   TF1            * FitFunSCurve ;
   TF1            * FitFunGaussian ;
   TH1F           * tempHist ;
   TH1F           * global1DPlot ;
   TH2F           * global2DPlot ;
   TH2F           * modulePlot ;
   
   TLine          * line ;
   
   QString          msg ;   
   QString          currentGlobalPlot ;
   QBrush         * brush_white;
   QBrush         * brush_selected;
   QBrush         * brush_label;
   QBrush         * brush_dcol;
   QBrush         * brush_dcolNo;
   QBrush         * brush_filled;
   QBrush         * brush_dense4Pattern;
   QWidget        * thisParent ;
   QColor           parentBgColor;
   QString          theMapName ;
   QString          rootFile ;
    
   QColor           white ;   
   QColor           black ;   
   QColor           red ;   
   QColor           green ;   
   QColor           darkGreen ;   
   QColor           blue ;
   QColor           paleGreen ;   
   QColor           yellow ;
   QColor           acquaMarina ;
   QColor           salmon ;
   QColor           grey ;
   QColor           darkGrey ;
   QColor           disableCellButInjectWithCapacitor ;
   QColor           enableCellAndInjectWithCapacitor ;
   QColor           disableCellButInjectWithBumpBond ;
   QColor           enableCellAndInjectWithBumpBond ;
   QColor           enableCellButInjectDisabled;
   QColor           cellDisabled;
    
   std::vector<QString>           distributionTypes ;
   std::vector<QColor>            colorSwatch ;
   std::vector<int>               chipSelectionList ;
   std::map<int, std::map<int, bool> > dcState ;
   std::map<int, std::map<int, bool> > rowEnableState ;
   std::map<int, std::map<int, bool> > colEnableState ;
   std::map<int, std::map<int, bool> > rowInjectState ;
   std::map<int, std::map<int, bool> > colInjectState ;
   std::map<std::string, int>          markerColorsMap ;
//   map<int, int>             calDelDefaults ;
   
//   Renaissance * theRenaissance ;
//   HManager    * hm ;
   
   int Nrows;
   int Ncols;
   int cellWidthX;
   int cellWidthY;
   int border_1;
   int border_2 ;
   int clipx;
   int clipy;
   
   int buttonSelection ;
   int bumpSelection ;
   int minRow ;
   int maxRow ;
   int minCol ;
   int maxCol ;
   
   int mouseStartRow;
   int mouseStartCol;
   
   int lastX ;
   int lastY ;
   
   cellValues fitResults ;
   
   bool isControlPressed;
   bool drawDc;
   bool drawSwatch;
   bool skipDrawing;
   
//   QMap<int, QMap<int, double> > theErf ;
   std::map<int, std::map<int, std::map<int, QColor> > > theColorMap ;
   std::map<int, QPushButton *> dcButton ;
   std::map<int, QPushButton *> rowButton ;
   std::map<int, QPushButton *> colButton ;
   
   map4Int             values;
   map4Int             enableMask;
   map4Int             injectMask;
   map3IntDouble       initPeak ;
   map3IntDouble       initNoise ;
   map3IntDouble       chi2T ;
   map4Int             fitSts ;
   map4Int             lowSbin ;
   map4Int             higSbin ;
   
   void clipRegion (int *x,int *y,int clipx,int clipy,int clipw, int cliph);
   
protected:
   virtual void paintEvent(QPaintEvent *e);
   void drawContents(QPainter  * p, int clipx, int clipy, int clipw, int cliph );
};

#endif
