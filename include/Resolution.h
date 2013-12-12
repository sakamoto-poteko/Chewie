#ifndef RESOLUTION_H
#define RESOLUTION_H

#include "Analysis.h"

#include <TTreeFormula.h>

#include <vector>
#include <iostream>


class TH1F           ;
class TTreeFormula   ;
class AnalysisManager;
class WindowsManager ;
class PlanesMapping  ;

class Resolution : public Analysis
{
public:
    Resolution  (AnalysisManager* analysisManager = 0, int nOfThreads = 1);
    ~Resolution (void                                                    );

    void beginJob            (void                                                                );
    void analyze             (const Data&                       data    , int threadNumber        );
    void endJob              (void                                                                );
    void setCutsFormula      (std::map<std::string,std::string> cutsList, std::vector<TTree*> tree);
    bool passStandardCuts    (int planeID                               , const Data& data        );

    void getInFile           (TFile *                                                             ){}

private:
    void book                (void                                                      );
    void destroy             (void                                                      );

    void predictedErrors     (bool pass, int planeID, const Data& data, int threadNumber);
    void calculateXresiduals (bool pass, int planeID, const Data& data, int threadNumber);
    void calculateYresiduals (bool pass, int planeID, const Data& data, int threadNumber);
    void xResolution         (bool pass, int planeID, const Data& data, int threadNumber);
    void yResolution         (bool pass, int planeID, const Data& data, int threadNumber);

    PlanesMapping*                                     thePlaneMapping_    ;
    std::map< std::string,std::vector<TTreeFormula*> > cutsFormulas_       ;
    const WindowsManager*                              theWindowsManager_  ;

    std::vector<TH1F*>    hXresiduals_                ;
    std::vector<TH1F*>    hXresidualCalculated_       ;
    std::vector<TH1F*>    hXresidualsClusterSize1_    ;
    std::vector<TH1F*>    hXresidualsClusterSize2_    ;
    std::vector<TH1F*>    hYresiduals_                ;
    std::vector<TH1F*>    hYresidualCalculated_       ;
    std::vector<TH1F*>    hYresidualsClusterSize1_    ;
    std::vector<TH1F*>    hYresidualsClusterSize2_    ;
    std::vector<TH1F*>    hPredictedXErrors_          ;
    std::vector<TH1F*>    hPredictedYErrors_          ;
    std::vector<TH1F*>    hSigmaXIntercept_           ;
    std::vector<TH1F*>    hSigmaYIntercept_           ;
    std::vector<TH1F*>    hSigmaXSlope_               ;
    std::vector<TH1F*>    hSigmaYSlope_               ;
};

#endif // RESOLUTION_H
