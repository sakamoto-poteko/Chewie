#ifndef CALIBRATIONSMANAGER_H
#define CALIBRATIONSMANAGER_H

#include "EventManager.h"

#include "TFile.h"

#include <map>
#include <string>

#define NPARS     4
/*#define NCOLS    52
#define NROWS    80
#define FIRSTROW  0
#define LASTROW  79
#define FIRSTCOL  0
#define LASTCOL  51*/

class Geometry     ;
class PlanesMapping;
class TH1F         ;
class TH2F         ;
class TF1          ;

class CalibrationsManager
{
public:
    CalibrationsManager  (EventManager* eventManager = 0);
    ~CalibrationsManager (void                          );

    void   openCalibPlotsFile (std::string fileName);
    void   copyHistograms     (void                );

    TH2F*  getParHisto        (int         parID   ){return h2DparsPlots_[parID];}

    void   setDut             (std::string dut     ){dut_        =      dut;}
    void   setInFileName      (std::string fileName){inFileName_ = fileName;}
    void   setWindow          (void                );
    void   bookParsPlots      (void                );
    void   bookCalibHistos    (void                );
    void   makeParsPlots      (void                );
    void   makeCalibHistos    (void                );
    void   drawFitFunction    (void                );
    void   destroyCalibHistos (void                );
    void   destroyParsPlots   (void                );

private:
    EventManager*                        theEventManager_             ;
    Geometry*                            theGeometry_                 ;
    PlanesMapping*                       thePlanesMapping_            ;
    std::string                          inFileName_                  ;
    TFile*                               calibPlotsFile_              ;
    std::string                          dut_                         ;
    int                                  firstRow_                    ;
    int                                  lastRow_                     ;
    int                                  firstCol_                    ;
    int                                  lastCol_                     ;

    TF1*                                 calFunk_                     ;
    TF1*                                 calFunkInv_                  ;
    TH1F*                                h1DparsPlots_         [NPARS];
    TH2F*                                h2DparsPlots_         [NPARS];
    std::map< int, std::map<int,TH1F*> > hPixelCalibration_           ;
    TH1F*                                hThreashold_                 ;
    TH2F*                                h2DThreashold_               ;
    TH1F*                                h1Dresidual_                 ;
    TH2F*                                h2Dresidual_                 ;
    TH2F*                                hCorrelations_ [NPARS][NPARS];
    TH2F*                                hFirstBin_                   ;
    TH2F*                                hLastBin_                    ;
};

#endif // CALIBRATIONSMANAGER_H
