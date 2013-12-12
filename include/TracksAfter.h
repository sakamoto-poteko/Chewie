#ifndef TRACKSAFTER_H
#define TRACKSAFTER_H

#include "Analysis.h"

#include <TTreeFormula.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TNtuple.h>

#include <vector>

class TH1F           ;
class TH2F           ;
class AnalysisManager;
class PlanesMapping  ;
class XmlParser      ;
class XmlPlane       ;

class TracksAfter : public Analysis
{
public:
    TracksAfter(AnalysisManager* analysisManager = 0, int nOfThreads = 1);
        ~TracksAfter (void                                                    );

        void   beginJob            (void                                                                   );
        void   analyze             (const Data&                       data    , int threadNumber           );
        void   endJob              (void                                                                   );
        void   setCutsFormula      (std::map<std::string,std::string>         , std::vector<TTree*>        ){}
        bool   passStandardCuts    (int planeID                               , const Data& data           );
        bool   passCalibrationsCut (int planeID, const Data &data                                          );
        void   positionUnfolded    (int planeID                                                            );
        float  findMaxHisto2DX     (int planeID, float charge                                              );
        float  findMaxHisto2DY     (int planeID, float charge                                              );
        void   unfold              (int planeID, float charge, float xp, float yp,  int threadNumber       );
        void   testPredictedFunc   (void                                                                   );
        void   fitEtaFunc          (int planeID, std::string type                                          );
        void   loadEtaFunc         (std::string type                                                       );
        double invertInHisto       (int planeID, double etaValue, std::string coordinate, std::string type );
        double invertInHistoSingle (double etaValue, TH1F * histo                                          );
        double calculateInHisto    (double value,    TH1F * histo                                          );
        void   calculateXResiduals (const Data& data, int planeID, int threadNumber                        );
        void   calculateYResiduals (const Data& data, int planeID, int threadNumber                        );
        void   profileChi2Par      (double * par, int parNumber, double liminf, double limsup              );
        void   computeEtaDerivative(int planeID                                                            );

    private:
        void getInFile           (TFile * infile);
        void book                (void          );
        void destroy             (void          );
        void preBook             (void          );

        PlanesMapping*                                     thePlaneMapping_       ;
        std::map< std::string,std::vector<TTreeFormula*> > cutsFormulas_          ;
        TFile *                                            inFile_;

        std::vector<TH1*>     hXdistribution1D_      ;
        std::vector<TH1*>     hXdistribution2D_      ;
        std::vector<TH2*>     hXChargedistribution1D_;
        std::vector<TH2*>     hXChargedistribution2D_;
        std::vector<TH1*>     hYdistribution1D_      ;
        std::vector<TH1*>     hYdistribution2D_      ;
        std::vector<TH2*>     hYChargedistribution1D_;
        std::vector<TH2*>     hYChargedistribution2D_;

        std::vector<TH2F*>     XProbabilityFunc_;
        std::vector<TH2F*>     YProbabilityFunc_;

        std::vector<TH2F*>     XYReconstructed_           ;
        std::vector<TH1F*>     XResidualsReconstructed_   ;
        std::vector<TH1F*>     YResidualsReconstructed_   ;

        std::vector<TH1F*>     hXAsimmetry_;
        std::vector<TH1F*>     hYAsimmetry_;
        std::vector<TH1F*>     hXAsimmetryLE2_;
        std::vector<TH1F*>     hYAsimmetryLE2_;
        std::vector<TH1F*>     hXAsimmetryInv_;
        std::vector<TH1F*>     hYAsimmetryInv_;
        std::vector<TH1F*>     hXEtaDistribution_;
        std::vector<TH1F*>     hYEtaDistribution_;
        std::vector<TH1F*>     projXSize2;
        std::vector<TH1F*>     projYSize2;

        std::vector<TF1*>      XAsimmetryFunc_   ;
        std::vector<TF1*>      YAsimmetryFunc_   ;
        std::vector<TF1*>      XAsimmetryFuncInv_;
        std::vector<TF1*>      YAsimmetryFuncInv_;

        std::vector<TH1F*>     hXResidualsAsimmetry_;
        std::vector<TH1F*>     hYResidualsAsimmetry_;
        std::vector<TH1F*>     hXResidualsAsimmetryLE2_;
        std::vector<TH1F*>     hYResidualsAsimmetryLE2_;

        TH1F *                funcPredicted_          ;
        TH1F *                funcPredicted2_         ;
        TH1F *                funcPredicted3_         ;

        std::vector<TH1F*>    computedEtaFuncXSize2_  ;
        std::vector<TH1F*>    computedEtaFuncYSize2_  ;
        std::vector<TH1F*>    computedEtaFuncXSizeLE2_;
        std::vector<TH1F*>    computedEtaFuncYSizeLE2_;
        std::vector<TH1F*>    computedEtaDerivativeXFitted_ ;
        std::vector<TH1F*>    computedEtaDerivativeYFitted_ ;
        std::vector<TH1F*>    computedEtaDerivativeXData_ ;
        std::vector<TH1F*>    computedEtaDerivativeYData_ ;
        std::vector<TH1F*>    computedEtaDerivativeXFittedSize2_ ;
        std::vector<TH1F*>    computedEtaDerivativeYFittedSize2_ ;
        std::vector<TH1F*>    computedEtaDerivativeXDataSize2_ ;
        std::vector<TH1F*>    computedEtaDerivativeYDataSize2_ ;
        std::vector<TH1F*>    computedEtaDerivativeXFittedSize2Inv_ ;
        std::vector<TH1F*>    computedEtaDerivativeYFittedSize2Inv_ ;
        std::vector<TH1F*>    computedEtaDerivativeXDataSize2Inv_ ;
        std::vector<TH1F*>    computedEtaDerivativeYDataSize2Inv_ ;
        std::vector<TH1F*>    computedEtaDerivativeXDataSize2InvNorm_ ;
        std::vector<TH1F*>    computedEtaDerivativeYDataSize2InvNorm_ ;

        std::vector<TH1F*>    chi2ProfilesPar_;

};

#endif // TRACKSAFTER_H
