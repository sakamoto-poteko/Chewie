#ifndef _Efficiency_h_
#define _Efficiency_h_

#include "Analysis.h"

#include "rectwindow.h"

#include <vector>
#include <iostream>

class TH1F           ;
class TH2F           ;
class TGraph         ;
class TTreeFormula   ;
class AnalysisManager;
class WindowsManager ;
class PlanesMapping  ;
class XmlParser      ;

class Efficiency : public Analysis
{
public:
    Efficiency  (AnalysisManager* analysisManager = 0, int nOfThreads = 1);
    ~Efficiency (void);

    void beginJob         (void                                                                );
    void analyze          (const Data&                       data    , int threadNumber        );
    void endJob           (void                                                                );
    void setCutsFormula   (std::map<std::string,std::string> cutsList, std::vector<TTree*> tree);
    bool passStandardCuts (int planeID                               , const Data& data        );

    void getInFile        (TFile *                                                             ){}

private:
    void book             (void                                                        );
    void scanBook         (void                                                        );
    void destroy          (void                                                        );

    void planeEfficiency  (bool pass  , int planeID, const Data& data, int threadNumber);
    void cellEfficiency   (bool pass  , int planeID, const Data& data, int threadNumber);
    void XcellEfficiency  (bool pass  , int planeID, const Data& data, int threadNumber);
    void YcellEfficiency  (bool pass  , int planeID, const Data& data, int threadNumber);
    void XcellEfficiencyWindowed  (bool pass  , int planeID, const Data& data, int threadNumber);
    void YcellEfficiencyWindowed  (bool pass  , int planeID, const Data& data, int threadNumber);
    void scanEfficiency   (int planeID, int value  , const Data& data, int threadNumber);

    void setErrorsBar     (int  planeID                                                );

    PlanesMapping*                                     thePlaneMapping_  ;
    std::map< std::string,std::vector<TTreeFormula*> > cutsFormulas_     ;
    const WindowsManager*                              theWindowsManager_;
    XmlParser*                                         theXmlParser_     ;

    //Histograms
    std::vector<TH1F*>                 hEfficiency_                   ;
    std::vector<TH1F*>                 hEfficiencyNorm_               ;

    std::vector<TH1F*>                 h1DXcellEfficiencyFirstHit_    ;
    std::vector<TH1F*>                 h1DXcellEfficiencySecondHit_   ;
    std::vector<TH1F*>                 h1DXcellEfficiencyAllHit_   ;
    std::vector<TH1F*>                 h1DXcellEfficiencyNorm_        ;
    std::vector<TH1F*>                 h1DYcellEfficiencyFirstHit_    ;
    std::vector<TH1F*>                 h1DYcellEfficiencySecondHit_   ;
    std::vector<TH1F*>                 h1DYcellEfficiencyNorm_        ;
    std::vector<TH2F*>                 h2DEfficiency_                 ;
    std::vector<TH2F*>                 h2DEfficiencyNorm_             ;
    std::vector<TH2F*>                 h2DInefficiency_               ;
    std::vector<TH2F*>                 hCellEfficiencyNorm_           ;
    std::vector<TH2F*>                 hCellEfficiencyEvenColumnsNorm_;
    std::vector<TH2F*>                 hCellEfficiencyOddColumnsNorm_ ;
    std::vector<TH2F*>                 hCellEfficiency_               ;
    std::vector<TH2F*>                 hCellEfficiencyEvenColumns_    ;
    std::vector<TH2F*>                 hCellEfficiencyOddColumns_     ;
    std::vector<TH2F*>                 hCellInefficiency_             ;
    // START Edge
    std::vector<TH1F*>                 h1DXcellEfficiencyCol01Row0_    ;
    std::vector<TH1F*>                 h1DXcellEfficiencyCol01Row0Norm_;

    std::vector<TH1F*>                 h1DXcellEfficiencyCol01Row79_    ;
    std::vector<TH1F*>                 h1DXcellEfficiencyCol01Row79Norm_;

    std::vector<TH1F*>                 h1DXcellEfficiencyCol01Row40_    ;
    std::vector<TH1F*>                 h1DXcellEfficiencyCol01Row40Norm_;

    std::vector<TH1F*>                 h1DXcellEfficiencyCol01RowAll_    ;
    std::vector<TH1F*>                 h1DXcellEfficiencyCol01RowAllNorm_;

    std::vector<TH1F*>                 h1DXcellEfficiencyCol5051Row0_    ;
    std::vector<TH1F*>                 h1DXcellEfficiencyCol5051Row0Norm_;

    std::vector<TH1F*>                 h1DXcellEfficiencyCol5051Row40_    ;
    std::vector<TH1F*>                 h1DXcellEfficiencyCol5051Row40Norm_;

    std::vector<TH1F*>                 h1DXcellEfficiencyCol5051Row79_    ;
    std::vector<TH1F*>                 h1DXcellEfficiencyCol5051Row79Norm_;

    std::vector<TH1F*>                 h1DXcellEfficiencyCol5051RowAll_    ;
    std::vector<TH1F*>                 h1DXcellEfficiencyCol5051RowAllNorm_;

    std::vector<TH1F*>                 h1DYcellEfficiencyRow01Col0_    ;
    std::vector<TH1F*>                 h1DYcellEfficiencyRow01Col0Norm_;

    std::vector<TH1F*>                 h1DYcellEfficiencyRow01Col30_    ;
    std::vector<TH1F*>                 h1DYcellEfficiencyRow01Col30Norm_;

    std::vector<TH1F*>                 h1DYcellEfficiencyRow01Col52_    ;
    std::vector<TH1F*>                 h1DYcellEfficiencyRow01Col52Norm_;

    std::vector<TH1F*>                 h1DYcellEfficiencyRow01ColAll_    ;
    std::vector<TH1F*>                 h1DYcellEfficiencyRow01ColAllNorm_;

    std::vector<TH1F*>                 h1DYcellEfficiencyRow7879Col0_    ;
    std::vector<TH1F*>                 h1DYcellEfficiencyRow7879Col0Norm_;

    std::vector<TH1F*>                 h1DYcellEfficiencyRow7879Col30_    ;
    std::vector<TH1F*>                 h1DYcellEfficiencyRow7879Col30Norm_;

    std::vector<TH1F*>                 h1DYcellEfficiencyRow7879Col52_    ;
    std::vector<TH1F*>                 h1DYcellEfficiencyRow7879Col52Norm_;

    std::vector<TH1F*>                 h1DYcellEfficiencyRow7879ColAll_    ;
    std::vector<TH1F*>                 h1DYcellEfficiencyRow7879ColAllNorm_;
    // END Edge

    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsEvenRowsNorm_ ;
    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsOddRowsNorm_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsEvenRowsNorm_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsOddRowsNorm_   ;
    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsEvenRows_ ;
    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsOddRows_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsEvenRows_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsOddRows_   ;
    std::vector<TH2F*>                  h4CellEfficiency_                   ;
    std::vector<TH2F*>                  h4CellEfficiencyNorm_               ;

    // Window
    std::vector<TH1F*>                 h1DXcellEfficiencyFirstHitWindowed_    ;
    std::vector<TH1F*>                 h1DXcellEfficiencySecondHitWindowed_   ;
    std::vector<TH1F*>                 h1DXcellEfficiencyNormWindowed_        ;
    std::vector<TH1F*>                 h1DYcellEfficiencyFirstHitWindowed_    ;
    std::vector<TH1F*>                 h1DYcellEfficiencySecondHitWindowed_   ;
    std::vector<TH1F*>                 h1DYcellEfficiencyNormWindowed_        ;

    std::vector<TH1F*>                 hEfficiencyWindowed_             ;
    std::vector<TH1F*>                 hEfficiencyNormWindowed_         ;
    std::vector<TH2F*>                 h2DEfficiencyWindowed_                 ;
    std::vector<TH2F*>                 h2DEfficiencyNormWindowed_             ;
    std::vector<TH2F*>                 h2DInefficiencyWindowed_               ;
    std::vector<TH2F*>                 hCellEfficiencyNormWindowed_           ;
    std::vector<TH2F*>                 hCellEfficiencyEvenColumnsNormWindowed_;
    std::vector<TH2F*>                 hCellEfficiencyOddColumnsNormWindowed_ ;
    std::vector<TH2F*>                 hCellEfficiencyWindowed_               ;
    std::vector<TH2F*>                 hCellEfficiencyEvenColumnsWindowed_    ;
    std::vector<TH2F*>                 hCellEfficiencyOddColumnsWindowed_     ;
    std::vector<TH2F*>                 hCellInefficiencyWindowed_             ;

    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsEvenRowsNormWindowed_ ;
    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsOddRowsNormWindowed_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsEvenRowsNormWindowed_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsOddRowsNormWindowed_   ;
    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsEvenRowsWindowed_ ;
    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsOddRowsWindowed_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsEvenRowsWindowed_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsOddRowsWindowed_   ;
    std::vector<TH2F*>                  h4CellEfficiencyWindowed_                   ;
    std::vector<TH2F*>                  h4CellEfficiencyNormWindowed_               ;

    // END Window

    std::map< int,std::vector<TH1F*> > scanEfficiencyNorm_            ;
    std::map< int,std::vector<TH1F*> > scanEfficiencyHistos_          ;
    std::vector<TGraph*>               scanGraph_                     ;

    CustomPixelWindow customPixelWindow;
};

#endif
