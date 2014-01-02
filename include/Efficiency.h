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
    void scanEfficiency   (int planeID, int value  , const Data& data, int threadNumber);

    void setErrorsBar     (int  planeID                                                );

    PlanesMapping*                                     thePlaneMapping_  ;
    std::map< std::string,std::vector<TTreeFormula*> > cutsFormulas_     ;
    const WindowsManager*                              theWindowsManager_;
    XmlParser*                                         theXmlParser_     ;

    //Histograms
    std::vector<TH1F*>                 hEfficiency_                   ;
    std::vector<TH1F*>                 hEfficiencyNorm_               ;
    std::vector<TH1F*>                 hWindowEfficiency_             ;
    std::vector<TH1F*>                 hWindowEfficiencyNorm_         ;
    std::vector<TH1F*>                 h1DXcellEfficiencyFirstHit_    ;
    std::vector<TH1F*>                 h1DXcellEfficiencySecondHit_   ;
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

    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsEvenRowsNorm_ ;
    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsOddRowsNorm_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsEvenRowsNorm_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsOddRowsNorm_   ;
    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsEvenRows_ ;
    std::vector<TH2F*>                  hCellEfficiencyEvenColumnsOddRows_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsEvenRows_  ;
    std::vector<TH2F*>                  hCellEfficiencyOddColumnsOddRows_   ;
    std::vector<TH2F*>                  h4CellEfficiency_                   ;

    std::map< int,std::vector<TH1F*> > scanEfficiencyNorm_            ;
    std::map< int,std::vector<TH1F*> > scanEfficiencyHistos_          ;
    std::vector<TGraph*>               scanGraph_                     ;

    RectWindow rectWindow;
};

#endif
