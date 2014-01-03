/**
  * File: Charge.cpp
  *
  * Currently maintined by Afa.L Cheng <alpha@tomatoeskit.org>
 **/

#include "Charge.h"
#include "AnalysisManager.h"
#include "ThreadUtilities.h"
#include "PlanesMapping.h"
#include "WindowsManager.h"
#include "CalibrationsManager.h"
#include "Window.h"
#include "MessageTools.h"
#include "Utilities.h"
#include "XmlParser.h"
#include "XmlPlane.h"
#include "XmlAnalysis.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TThread.h>
#include <TFile.h>
#include <TTreeFormula.h>
#include <TMath.h>
#include <TF1.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TVectorT.h>
#include <TMap.h>

#include <iostream>
#include <map>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Charge::Charge(AnalysisManager* analysisManager, int nOfThreads) :
    Analysis          (analysisManager, nOfThreads)
  , thePlaneMapping_  (0)
  , langaus_          (0)
  , theWindowsManager_(0)
  , threashold_       (0)
  , maxCharge_        (0)
  , minTotCharge_     (0)
  , maxTotCharge_     (0)
{
    thePlaneMapping_ = new PlanesMapping();

    for(int p=0; p<4; p++)
    {
        parMin_      [p] = 0;
        parMax_      [p] = 0;
        isMinToLimit_[p] = 0;
        isMaxToLimit_[p] = 0;
    }
    totEventsControl_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Charge::~Charge(void)
{
    if(thePlaneMapping_)
    {
        delete thePlaneMapping_;
        thePlaneMapping_ = 0;
    }

    if(langaus_)
    {
        delete langaus_;
        langaus_ = 0;
    }

    destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::destroy(void)
{
    if(Analysis::fDoNotDelete_) return;

    /*--------------------------------------------------------------------------------------------Cluster size--------------------------------------------------------------------------------------------------*/
    for(std::vector<TH1F*>::iterator it=hClusterSize_                           .begin(); it!=hClusterSize_                           .end(); it++) delete *it; hClusterSize_                           .clear();
    for(std::vector<TH1F*>::iterator it=hClusterSizeCuts_                       .begin(); it!=hClusterSizeCuts_                       .end(); it++) delete *it; hClusterSizeCuts_                       .clear();
    for(std::vector<TH1F*>::iterator it=hClusterSizeCutsPlus_                   .begin(); it!=hClusterSizeCutsPlus_                   .end(); it++) delete *it; hClusterSizeCutsPlus_                   .clear();
    for(std::vector<TH1F*>::iterator it=hNumberOfCols_                          .begin(); it!=hNumberOfCols_                          .end(); it++) delete *it; hNumberOfCols_                          .clear();
    for(std::vector<TH1F*>::iterator it=hNumberOfRows_                          .begin(); it!=hNumberOfRows_                          .end(); it++) delete *it; hNumberOfRows_                          .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution1s_             .begin(); it!=hClusterSizeDistribution1s_             .end(); it++) delete *it; hClusterSizeDistribution1s_             .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution1sOROC_         .begin(); it!=hClusterSizeDistribution1sOROC_         .end(); it++) delete *it; hClusterSizeDistribution1sOROC_         .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution1sEROC_         .begin(); it!=hClusterSizeDistribution1sEROC_         .end(); it++) delete *it; hClusterSizeDistribution1sEROC_         .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution1sOREC_         .begin(); it!=hClusterSizeDistribution1sOREC_         .end(); it++) delete *it; hClusterSizeDistribution1sOREC_         .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution1sEREC_         .begin(); it!=hClusterSizeDistribution1sEREC_         .end(); it++) delete *it; hClusterSizeDistribution1sEREC_         .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution1sOddColumns_   .begin(); it!=hClusterSizeDistribution1sOddColumns_   .end(); it++) delete *it; hClusterSizeDistribution1sOddColumns_   .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution1sEvenColumns_  .begin(); it!=hClusterSizeDistribution1sEvenColumns_  .end(); it++) delete *it; hClusterSizeDistribution1sEvenColumns_  .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution1s4Cells_       .begin(); it!=hClusterSizeDistribution1s4Cells_       .end(); it++) delete *it; hClusterSizeDistribution1s4Cells_       .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution2s_             .begin(); it!=hClusterSizeDistribution2s_             .end(); it++) delete *it; hClusterSizeDistribution2s_             .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution2sOROC_         .begin(); it!=hClusterSizeDistribution2sOROC_         .end(); it++) delete *it; hClusterSizeDistribution2sOROC_         .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution2sEROC_         .begin(); it!=hClusterSizeDistribution2sEROC_         .end(); it++) delete *it; hClusterSizeDistribution2sEROC_         .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution2sOREC_         .begin(); it!=hClusterSizeDistribution2sOREC_         .end(); it++) delete *it; hClusterSizeDistribution2sOREC_         .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution2sEREC_         .begin(); it!=hClusterSizeDistribution2sEREC_         .end(); it++) delete *it; hClusterSizeDistribution2sEREC_         .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution2sOddColumns_   .begin(); it!=hClusterSizeDistribution2sOddColumns_   .end(); it++) delete *it; hClusterSizeDistribution2sOddColumns_   .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution2sEvenColumns_  .begin(); it!=hClusterSizeDistribution2sEvenColumns_  .end(); it++) delete *it; hClusterSizeDistribution2sEvenColumns_  .clear();
    for(std::vector<TH2F*>::iterator it=hClusterSizeDistribution2s4Cells_       .begin(); it!=hClusterSizeDistribution2s4Cells_       .end(); it++) delete *it; hClusterSizeDistribution2s4Cells_       .clear();

    /*-----------------------------------------------------------------------------------------Landau distributions---------------------------------------------------------------------------------------------*/
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize1_                    .begin(); it!=hLandauClusterSize1_                    .end(); it++) delete *it; hLandauClusterSize1_                    .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize2_                    .begin(); it!=hLandauClusterSize2_                    .end(); it++) delete *it; hLandauClusterSize2_                    .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize2sameCol_             .begin(); it!=hLandauClusterSize2sameCol_             .end(); it++) delete *it; hLandauClusterSize2sameCol_             .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize2sameRow_             .begin(); it!=hLandauClusterSize2sameRow_             .end(); it++) delete *it; hLandauClusterSize2sameRow_             .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize3_                    .begin(); it!=hLandauClusterSize3_                    .end(); it++) delete *it; hLandauClusterSize2_                    .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize3sameCol_             .begin(); it!=hLandauClusterSize3sameCol_             .end(); it++) delete *it; hLandauClusterSize2sameCol_             .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize3sameRow_             .begin(); it!=hLandauClusterSize3sameRow_             .end(); it++) delete *it; hLandauClusterSize2sameRow_             .clear();
    for(std::vector<TH1F*>::iterator it=hCellLandau_                            .begin(); it!=hCellLandau_                            .end(); it++) delete *it; hCellLandau_                            .clear();

    /*-------------------------------------------------------------------------------------------2D cell charge-------------------------------------------------------------------------------------------------*/
    for(std::vector<TH2F*>::iterator it=h2DCellCharge_                          .begin(); it!=h2DCellCharge_                          .end(); it++) delete *it; h2DCellCharge_                          .clear();
    for(std::vector<TH2F*>::iterator it=h2DCellChargeNum_                       .begin(); it!=h2DCellChargeNum_                       .end(); it++) delete *it; h2DCellChargeNum_                       .clear();
    for(std::vector<TH2F*>::iterator it=h2DallTracks_                           .begin(); it!=h2DallTracks_                           .end(); it++) delete *it; h2DallTracks_                           .clear();

    // START Odd/Even Cell Charge
    // Odd Rows
    for(std::vector<TH2F*>::iterator it=h2DCellChargeEvenColumnsOddRows_        .begin(); it!=h2DCellChargeEvenColumnsOddRows_        .end(); it++) delete *it; h2DCellChargeEvenColumnsOddRows_        .clear();
    for(std::vector<TH2F*>::iterator it=h2DCellChargeOddColumnsOddRows_         .begin(); it!=h2DCellChargeOddColumnsOddRows_         .end(); it++) delete *it; h2DCellChargeOddColumnsOddRows_         .clear();
    for(std::vector<TH2F*>::iterator it=h2DEvenTracksOddRows_                   .begin(); it!=h2DEvenTracksOddRows_                   .end(); it++) delete *it; h2DEvenTracksOddRows_                   .clear();
    for(std::vector<TH2F*>::iterator it=h2DOddTracksOddRows_                    .begin(); it!=h2DOddTracksOddRows_                    .end(); it++) delete *it; h2DOddTracksOddRows_                    .clear();
    // Even Rows
    for(std::vector<TH2F*>::iterator it=h2DCellChargeEvenColumnsEvenRows_       .begin(); it!=h2DCellChargeEvenColumnsEvenRows_       .end(); it++) delete *it; h2DCellChargeEvenColumnsEvenRows_       .clear();
    for(std::vector<TH2F*>::iterator it=h2DCellChargeOddColumnsEvenRows_        .begin(); it!=h2DCellChargeOddColumnsEvenRows_        .end(); it++) delete *it; h2DCellChargeOddColumnsEvenRows_        .clear();
    for(std::vector<TH2F*>::iterator it=h2DEvenTracksEvenRows_                  .begin(); it!=h2DEvenTracksEvenRows_                  .end(); it++) delete *it; h2DEvenTracksEvenRows_                  .clear();
    for(std::vector<TH2F*>::iterator it=h2DOddTracksEvenRows_                   .begin(); it!=h2DOddTracksEvenRows_                   .end(); it++) delete *it; h2DOddTracksEvenRows_                   .clear();
    // Row independent
    for(std::vector<TH2F*>::iterator it=h2DCellChargeOddColumns_                .begin(); it!=h2DCellChargeOddColumns_                .end(); it++) delete *it; h2DCellChargeOddColumns_                .clear();
    for(std::vector<TH2F*>::iterator it=h2DCellChargeEvenColumns_               .begin(); it!=h2DCellChargeEvenColumns_               .end(); it++) delete *it; h2DCellChargeEvenColumns_               .clear();
    // 4 Cell
    for(std::vector<TH2F*>::iterator it=h4CellChargeFullRange_                  .begin(); it!=h4CellChargeFullRange_                  .end(); it++) delete *it; h4CellChargeFullRange_                  .clear();

    // END Odd/Even Cell Charge
    for(std::vector<TH2F*>::iterator it=h2DCellChargeNorm_                      .begin(); it!=h2DCellChargeNorm_                      .end(); it++) delete *it; h2DCellChargeNorm_                      .clear();
    for(std::vector<TH2F*>::iterator it=h4CellsCharge_                          .begin(); it!=h4CellsCharge_                          .end(); it++) delete *it; h4CellsCharge_                          .clear();
    for(std::vector<TH2F*>::iterator it=h4CellsAllTracks_                       .begin(); it!=h4CellsAllTracks_                       .end(); it++) delete *it; h4CellsAllTracks_                       .clear();
    for(std::vector<TH2F*>::iterator it=h4CellsChargeNorm_                      .begin(); it!=h4CellsChargeNorm_                      .end(); it++) delete *it; h4CellsChargeNorm_                      .clear();
    for(std::vector<TH2F*>::iterator it=h4Hits_                                 .begin(); it!=h4Hits_                                 .end(); it++) delete *it; h4Hits_                                 .clear();
    for(std::vector<TH2F*>::iterator it=h4HitsCharge_                           .begin(); it!=h4HitsCharge_                           .end(); it++) delete *it; h4HitsCharge_                           .clear();
    for(std::vector<TH2F*>::iterator it=hCellChargeCoarse_                      .begin(); it!=hCellChargeCoarse_                      .end(); it++) delete *it; hCellChargeCoarse_                      .clear();
    for(std::vector<TH2F*>::iterator it=hCellChargeCoarseNorm_                  .begin(); it!=hCellChargeCoarseNorm_                  .end(); it++) delete *it; hCellChargeCoarseNorm_                  .clear();
    for(std::vector<TH2F*>::iterator it=hHitsNotOnRowCol_                       .begin(); it!=hHitsNotOnRowCol_                       .end(); it++) delete *it; hHitsNotOnRowCol_                       .clear();
    for(std::vector<TH1F*>::iterator it=hCutsControl_                           .begin(); it!=hCutsControl_                           .end(); it++) delete *it; hCutsControl_                           .clear();
    for(std::vector<TH1F*>::iterator it=hChargeNotOnRowCol_                     .begin(); it!=hChargeNotOnRowCol_                     .end(); it++) delete *it; hChargeNotOnRowCol_                     .clear();

    /*----------------------------------------------2D cell charge Landau--------------------------------------------*/
    for(std::vector<std::vector<TH1F*> >::iterator it=hCellChargeCoarseLandau_.begin(); it!=hCellChargeCoarseLandau_.end(); it++)
        for(std::vector<TH1F*>::iterator it2=it->begin(); it2!=it->end(); it2++) delete *it2;
    hCellChargeCoarseLandau_.clear();

    /*------------------------------------------------------------------------------------2D cell charge - X coordinate-----------------------------------------------------------------------------------------*/
    for(std::vector<TH2F*>::iterator it=h2DXcellCharge_                         .begin(); it!=h2DXcellCharge_                         .end(); it++) delete *it; h2DXcellCharge_                         .clear();
    for(std::vector<TH2F*>::iterator it=h2DXcellChargeSumLE2_                   .begin(); it!=h2DXcellChargeSumLE2_                   .end(); it++) delete *it; h2DXcellChargeSumLE2_                   .clear();
    for(std::vector<TH2F*>::iterator it=h2DXcellSingleHits_                     .begin(); it!=h2DXcellSingleHits_                     .end(); it++) delete *it; h2DXcellSingleHits_                     .clear();
    for(std::vector<TH2F*>::iterator it=h2DXcellDoubleHits_                     .begin(); it!=h2DXcellDoubleHits_                     .end(); it++) delete *it; h2DXcellDoubleHits_                     .clear();
    for(std::vector<TH2F*>::iterator it=hHitsNotONRowColVsXSlope_               .begin(); it!=hHitsNotONRowColVsXSlope_               .end(); it++) delete *it; hHitsNotONRowColVsXSlope_               .clear();

    /*------------------------------------------------------------------------------------2D cell charge - Y coordinate-----------------------------------------------------------------------------------------*/
    for(std::vector<TH2F*>::iterator it=h2DYcellCharge_                         .begin(); it!=h2DYcellCharge_                         .end(); it++) delete *it; h2DYcellCharge_                         .clear();
    for(std::vector<TH2F*>::iterator it=h2DYcellChargeSumLE2_                   .begin(); it!=h2DYcellChargeSumLE2_                   .end(); it++) delete *it; h2DYcellChargeSumLE2_                   .clear();
    for(std::vector<TH2F*>::iterator it=h2DYcellSingleHits_                     .begin(); it!=h2DYcellSingleHits_                     .end(); it++) delete *it; h2DYcellSingleHits_                     .clear();
    for(std::vector<TH2F*>::iterator it=h2DYcellDoubleHits_                     .begin(); it!=h2DYcellDoubleHits_                     .end(); it++) delete *it; h2DYcellDoubleHits_                     .clear();
    for(std::vector<TH2F*>::iterator it=hHitsNotONRowColVsYSlope_               .begin(); it!=hHitsNotONRowColVsYSlope_               .end(); it++) delete *it; hHitsNotONRowColVsYSlope_               .clear();

    /*------------------------------------------------------------------------------------1D cell charge - X coordinate----------------------------------------------------------------------------------------*/
    for(std::vector<TH1F*>::iterator it=h1DXcellCharge_                         .begin(); it!=h1DXcellCharge_                         .end(); it++) delete *it; h1DXcellCharge_                         .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellChargeNormToAll_                .begin(); it!=h1DXcellChargeNormToAll_                .end(); it++) delete *it; h1DXcellChargeNormToAll_                .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellChargeSumLE2_                   .begin(); it!=h1DXcellChargeSumLE2_                   .end(); it++) delete *it; h1DXcellChargeSumLE2_                   .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellChargeSumLE2NormToAll_          .begin(); it!=h1DXcellChargeSumLE2NormToAll_          .end(); it++) delete *it; h1DXcellChargeSumLE2NormToAll_          .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellChargeSumLE3_                   .begin(); it!=h1DXcellChargeSumLE3_                   .end(); it++) delete *it; h1DXcellChargeSumLE3_                   .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellChargeSumLE3NormToAll_          .begin(); it!=h1DXcellChargeSumLE3NormToAll_          .end(); it++) delete *it; h1DXcellChargeSumLE3NormToAll_          .clear();
    for(std::vector<TH1F*>::iterator it=h1DXallTracks_                          .begin(); it!=h1DXallTracks_                          .end(); it++) delete *it; h1DXallTracks_                          .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellChargeNorm_                     .begin(); it!=h1DXcellChargeNorm_                     .end(); it++) delete *it; h1DXcellChargeNorm_                     .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellChargeSumLE2Norm_               .begin(); it!=h1DXcellChargeSumLE2Norm_               .end(); it++) delete *it; h1DXcellChargeSumLE2Norm_               .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellChargeSumLE3Norm_               .begin(); it!=h1DXcellChargeSumLE3Norm_               .end(); it++) delete *it; h1DXcellChargeSumLE3Norm_               .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellSingleHits_                     .begin(); it!=h1DXcellSingleHits_                     .end(); it++) delete *it; h1DXcellSingleHits_                     .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellDoubleHits_                     .begin(); it!=h1DXcellDoubleHits_                     .end(); it++) delete *it; h1DXcellDoubleHits_                     .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcell3Hits_                          .begin(); it!=h1DXcell3Hits_                          .end(); it++) delete *it; h1DXcell3Hits_                          .clear();

    /*------------------------------------------------------------------------------------1D charge-Y residuals------------------------------------------------------------------------------------------------*/
    for(std::vector<TH1F*>::iterator it=h1DYcellCharge_                         .begin(); it!=h1DYcellCharge_                         .end(); it++) delete *it; h1DYcellCharge_                         .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellChargeNormToAll_                .begin(); it!=h1DYcellChargeNormToAll_                .end(); it++) delete *it; h1DYcellChargeNormToAll_                .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellChargeSumLE2_                   .begin(); it!=h1DYcellChargeSumLE2_                   .end(); it++) delete *it; h1DYcellChargeSumLE2_                   .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellChargeSumLE2NormToAll_          .begin(); it!=h1DYcellChargeSumLE2NormToAll_          .end(); it++) delete *it; h1DYcellChargeSumLE2NormToAll_          .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellChargeSumLE3_                   .begin(); it!=h1DYcellChargeSumLE3_                   .end(); it++) delete *it; h1DYcellChargeSumLE3_                   .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellChargeSumLE3NormToAll_          .begin(); it!=h1DYcellChargeSumLE3NormToAll_          .end(); it++) delete *it; h1DYcellChargeSumLE3NormToAll_          .clear();
    for(std::vector<TH1F*>::iterator it=h1DYallTracks_                          .begin(); it!=h1DYallTracks_                          .end(); it++) delete *it; h1DYallTracks_                          .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellChargeNorm_                     .begin(); it!=h1DYcellChargeNorm_                     .end(); it++) delete *it; h1DYcellChargeNorm_                     .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellChargeSumLE2Norm_               .begin(); it!=h1DYcellChargeSumLE2Norm_               .end(); it++) delete *it; h1DYcellChargeSumLE2Norm_               .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellChargeSumLE3Norm_               .begin(); it!=h1DYcellChargeSumLE3Norm_               .end(); it++) delete *it; h1DYcellChargeSumLE3Norm_               .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellSingleHits_                     .begin(); it!=h1DYcellSingleHits_                     .end(); it++) delete *it; h1DYcellSingleHits_                     .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellDoubleHits_                     .begin(); it!=h1DYcellDoubleHits_                     .end(); it++) delete *it; h1DYcellDoubleHits_                     .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcell3Hits_                          .begin(); it!=h1DYcell3Hits_                          .end(); it++) delete *it; h1DYcell3Hits_                          .clear();

    /*----------------------------------------------------------------------------------------X Asimmetry------------------------------------------------------------------------------------------------------*/
    for(std::vector<TH1F*>::iterator it=hXasimmetry_                            .begin(); it!=hXasimmetry_                            .end(); it++) delete *it; hXasimmetry_                            .clear();
    for(std::vector<TH2F*>::iterator it=h2DXAsimmetryLandau_                    .begin(); it!=h2DXAsimmetryLandau_                    .end(); it++) delete *it; h2DXAsimmetryLandau_                    .clear();
    for(std::vector<TH2F*>::iterator it=h2DXcellChargeAsimmetry_                .begin(); it!=h2DXcellChargeAsimmetry_                .end(); it++) delete *it; h2DXcellChargeAsimmetry_                .clear();
    for(std::vector<TH2F*>::iterator it=h2DXcellChargeAsimmetryInv_             .begin(); it!=h2DXcellChargeAsimmetryInv_             .end(); it++) delete *it; h2DXcellChargeAsimmetryInv_             .clear();
    for(std::vector<TH2F*>::iterator it=h2DXcellChargeAsimmetryInv_             .begin(); it!=h2DXcellChargeAsimmetryInv_             .end(); it++) delete *it; h2DXcellChargeAsimmetryInv_             .clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellChargeAsimmetry_                .begin(); it!=h1DXcellChargeAsimmetry_                .end(); it++) delete *it; h1DXcellChargeAsimmetry_                .clear();
    for(std::vector<TH2F*>::iterator it=h2DXcellChargeAsimmetryUnconstrained_   .begin(); it!=h2DXcellChargeAsimmetryUnconstrained_   .end(); it++) delete *it; h2DXcellChargeAsimmetryUnconstrained_   .clear();
    for(std::vector<TH2F*>::iterator it=h2DXcellChargeAsimmetryUnconstrainedInv_.begin(); it!=h2DXcellChargeAsimmetryUnconstrainedInv_.end(); it++) delete *it; h2DXcellChargeAsimmetryUnconstrainedInv_.clear();
    for(std::vector<TH1F*>::iterator it=h1DXcellChargeAsimmetryUnconstrained_   .begin(); it!=h1DXcellChargeAsimmetryUnconstrained_   .end(); it++) delete *it; h1DXcellChargeAsimmetryUnconstrained_   .clear();
    for(std::vector<TH2F*>::iterator it=h2DXCellChargeAsimmetrySizeLE2_         .begin(); it!=h2DXCellChargeAsimmetrySizeLE2_         .end(); it++) delete *it; h2DXCellChargeAsimmetrySizeLE2_         .clear();
    for(std::vector<TH1F*>::iterator it=h1DXCellChargeAsimmetrySizeLE2_         .begin(); it!=h1DXCellChargeAsimmetrySizeLE2_         .end(); it++) delete *it; h1DXCellChargeAsimmetrySizeLE2_         .clear();

    /*----------------------------------------------------------------------------------------Y Asimmetry------------------------------------------------------------------------------------------------------*/
    for(std::vector<TH1F*>::iterator it=hYasimmetry_                            .begin(); it!=hYasimmetry_                            .end(); it++) delete *it; hYasimmetry_                            .clear();
    for(std::vector<TH2F*>::iterator it=h2DYAsimmetryLandau_                    .begin(); it!=h2DYAsimmetryLandau_                    .end(); it++) delete *it; h2DYAsimmetryLandau_                    .clear();
    for(std::vector<TH2F*>::iterator it=h2DYcellChargeAsimmetry_                .begin(); it!=h2DYcellChargeAsimmetry_                .end(); it++) delete *it; h2DYcellChargeAsimmetry_                .clear();
    for(std::vector<TH2F*>::iterator it=h2DYcellChargeAsimmetryInv_             .begin(); it!=h2DYcellChargeAsimmetryInv_             .end(); it++) delete *it; h2DYcellChargeAsimmetryInv_             .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellChargeAsimmetry_                .begin(); it!=h1DYcellChargeAsimmetry_                .end(); it++) delete *it; h1DYcellChargeAsimmetry_                .clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellChargeAsimmetryInv_             .begin(); it!=h1DYcellChargeAsimmetryInv_             .end(); it++) delete *it; h1DYcellChargeAsimmetryInv_             .clear();
    for(std::vector<TH2F*>::iterator it=h2DYAsimmetryLandau_                    .begin(); it!=h2DYAsimmetryLandau_                    .end(); it++) delete *it; h2DYAsimmetryLandau_                    .clear();
    for(std::vector<TH2F*>::iterator it=h2DYcellChargeAsimmetryUnconstrained_   .begin(); it!=h2DYcellChargeAsimmetryUnconstrained_   .end(); it++) delete *it; h2DYcellChargeAsimmetryUnconstrained_   .clear();
    for(std::vector<TH2F*>::iterator it=h2DYcellChargeAsimmetryUnconstrainedInv_.begin(); it!=h2DYcellChargeAsimmetryUnconstrainedInv_.end(); it++) delete *it; h2DYcellChargeAsimmetryUnconstrainedInv_.clear();
    for(std::vector<TH1F*>::iterator it=h1DYcellChargeAsimmetryUnconstrained_   .begin(); it!=h1DYcellChargeAsimmetryUnconstrained_   .end(); it++) delete *it; h1DYcellChargeAsimmetryUnconstrained_   .clear();
    for(std::vector<TH2F*>::iterator it=h2DYCellChargeAsimmetrySizeLE2_         .begin(); it!=h2DYCellChargeAsimmetrySizeLE2_         .end(); it++) delete *it; h2DYCellChargeAsimmetrySizeLE2_         .clear();
    for(std::vector<TH1F*>::iterator it=h1DYCellChargeAsimmetrySizeLE2_         .begin(); it!=h1DYCellChargeAsimmetrySizeLE2_         .end(); it++) delete *it; h1DYCellChargeAsimmetrySizeLE2_         .clear();

    // START Window

    /*--------------------------------------------------------------------------------------------Cluster size--------------------------------------------------------------------------------------------------*/
    for(std::vector<TH1F*>::iterator it=hClusterSizeWindowed_                           .begin(); it!=hClusterSizeWindowed_                           .end(); it++) delete *it; hClusterSizeWindowed_                           .clear();
    for(std::vector<TH1F*>::iterator it=hClusterSizeCutsWindowed_                       .begin(); it!=hClusterSizeCutsWindowed_                       .end(); it++) delete *it; hClusterSizeCutsWindowed_                       .clear();
    for(std::vector<TH1F*>::iterator it=hNumberOfColsWindowed_                          .begin(); it!=hNumberOfColsWindowed_                          .end(); it++) delete *it; hNumberOfColsWindowed_                          .clear();
    for(std::vector<TH1F*>::iterator it=hNumberOfRowsWindowed_                          .begin(); it!=hNumberOfRowsWindowed_                          .end(); it++) delete *it; hNumberOfRowsWindowed_                          .clear();

    /*-----------------------------------------------------------------------------------------Landau distributions---------------------------------------------------------------------------------------------*/
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize1Windowed_                    .begin(); it!=hLandauClusterSize1Windowed_                    .end(); it++) delete *it; hLandauClusterSize1Windowed_                    .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize2Windowed_                    .begin(); it!=hLandauClusterSize2Windowed_                    .end(); it++) delete *it; hLandauClusterSize2Windowed_                    .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize2sameColWindowed_             .begin(); it!=hLandauClusterSize2sameColWindowed_             .end(); it++) delete *it; hLandauClusterSize2sameColWindowed_             .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize2sameRowWindowed_             .begin(); it!=hLandauClusterSize2sameRowWindowed_             .end(); it++) delete *it; hLandauClusterSize2sameRowWindowed_             .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize3Windowed_                    .begin(); it!=hLandauClusterSize3Windowed_                    .end(); it++) delete *it; hLandauClusterSize2Windowed_                    .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize3sameColWindowed_             .begin(); it!=hLandauClusterSize3sameColWindowed_             .end(); it++) delete *it; hLandauClusterSize2sameColWindowed_             .clear();
    for(std::vector<TH1F*>::iterator it=hLandauClusterSize3sameRowWindowed_             .begin(); it!=hLandauClusterSize3sameRowWindowed_             .end(); it++) delete *it; hLandauClusterSize2sameRowWindowed_             .clear();
    for(std::vector<TH1F*>::iterator it=hCellLandauWindowed_                            .begin(); it!=hCellLandauWindowed_                            .end(); it++) delete *it; hCellLandauWindowed_                            .clear();
    for(std::vector<TH1F*>::iterator it=hWindowCellLandauWindowed_                      .begin(); it!=hWindowCellLandauWindowed_                      .end(); it++) delete *it; hWindowCellLandauWindowed_                      .clear();

    /*-------------------------------------------------------------------------------------------2D cell charge-------------------------------------------------------------------------------------------------*/
    for(std::vector<TH2F*>::iterator it=h2DCellChargeWindowed_                          .begin(); it!=h2DCellChargeWindowed_                          .end(); it++) delete *it; h2DCellChargeWindowed_                          .clear();
    for(std::vector<TH2F*>::iterator it=h2DCellChargeNumWindowed_                       .begin(); it!=h2DCellChargeNumWindowed_                       .end(); it++) delete *it; h2DCellChargeNumWindowed_                       .clear();
    for(std::vector<TH2F*>::iterator it=h2DallTracksWindowed_                           .begin(); it!=h2DallTracksWindowed_                           .end(); it++) delete *it; h2DallTracksWindowed_                           .clear();

    // START Odd/Even Cell Charge
    // Odd Rows
    for(std::vector<TH2F*>::iterator it=h2DCellChargeEvenColumnsOddRowsWindowed_        .begin(); it!=h2DCellChargeEvenColumnsOddRowsWindowed_        .end(); it++) delete *it; h2DCellChargeEvenColumnsOddRowsWindowed_        .clear();
    for(std::vector<TH2F*>::iterator it=h2DCellChargeOddColumnsOddRowsWindowed_         .begin(); it!=h2DCellChargeOddColumnsOddRowsWindowed_         .end(); it++) delete *it; h2DCellChargeOddColumnsOddRowsWindowed_         .clear();
    for(std::vector<TH2F*>::iterator it=h2DEvenTracksOddRowsWindowed_                   .begin(); it!=h2DEvenTracksOddRowsWindowed_                   .end(); it++) delete *it; h2DEvenTracksOddRowsWindowed_                   .clear();
    for(std::vector<TH2F*>::iterator it=h2DOddTracksOddRowsWindowed_                    .begin(); it!=h2DOddTracksOddRowsWindowed_                    .end(); it++) delete *it; h2DOddTracksOddRowsWindowed_                    .clear();
    // Even Rows
    for(std::vector<TH2F*>::iterator it=h2DCellChargeEvenColumnsEvenRowsWindowed_       .begin(); it!=h2DCellChargeEvenColumnsEvenRowsWindowed_       .end(); it++) delete *it; h2DCellChargeEvenColumnsEvenRowsWindowed_       .clear();
    for(std::vector<TH2F*>::iterator it=h2DCellChargeOddColumnsEvenRowsWindowed_        .begin(); it!=h2DCellChargeOddColumnsEvenRowsWindowed_        .end(); it++) delete *it; h2DCellChargeOddColumnsEvenRowsWindowed_        .clear();
    for(std::vector<TH2F*>::iterator it=h2DEvenTracksEvenRowsWindowed_                  .begin(); it!=h2DEvenTracksEvenRowsWindowed_                  .end(); it++) delete *it; h2DEvenTracksEvenRowsWindowed_                  .clear();
    for(std::vector<TH2F*>::iterator it=h2DOddTracksEvenRowsWindowed_                   .begin(); it!=h2DOddTracksEvenRowsWindowed_                   .end(); it++) delete *it; h2DOddTracksEvenRowsWindowed_                   .clear();
    // Row independent
    for(std::vector<TH2F*>::iterator it=h2DCellChargeOddColumnsWindowed_                .begin(); it!=h2DCellChargeOddColumnsWindowed_                .end(); it++) delete *it; h2DCellChargeOddColumnsWindowed_                .clear();
    for(std::vector<TH2F*>::iterator it=h2DCellChargeEvenColumnsWindowed_               .begin(); it!=h2DCellChargeEvenColumnsWindowed_               .end(); it++) delete *it; h2DCellChargeEvenColumnsWindowed_               .clear();
    // 4 Cell
    for(std::vector<TH2F*>::iterator it=h4CellChargeFullRangeWindowed_                  .begin(); it!=h4CellChargeFullRangeWindowed_                  .end(); it++) delete *it; h4CellChargeFullRangeWindowed_                  .clear();

    // END Odd/Even Cell Charge
    for(std::vector<TH2F*>::iterator it=h2DCellChargeNormWindowed_                      .begin(); it!=h2DCellChargeNormWindowed_                      .end(); it++) delete *it; h2DCellChargeNormWindowed_                      .clear();
    for(std::vector<TH2F*>::iterator it=h4CellsChargeWindowed_                          .begin(); it!=h4CellsChargeWindowed_                          .end(); it++) delete *it; h4CellsChargeWindowed_                          .clear();
    for(std::vector<TH2F*>::iterator it=h4CellsAllTracksWindowed_                       .begin(); it!=h4CellsAllTracksWindowed_                       .end(); it++) delete *it; h4CellsAllTracksWindowed_                       .clear();
    for(std::vector<TH2F*>::iterator it=h4CellsChargeNormWindowed_                      .begin(); it!=h4CellsChargeNormWindowed_                      .end(); it++) delete *it; h4CellsChargeNormWindowed_                      .clear();
    for(std::vector<TH2F*>::iterator it=h4HitsWindowed_                                 .begin(); it!=h4HitsWindowed_                                 .end(); it++) delete *it; h4HitsWindowed_                                 .clear();
    for(std::vector<TH2F*>::iterator it=h4HitsChargeWindowed_                           .begin(); it!=h4HitsChargeWindowed_                           .end(); it++) delete *it; h4HitsChargeWindowed_                           .clear();
    for(std::vector<TH2F*>::iterator it=hCellChargeCoarseWindowed_                      .begin(); it!=hCellChargeCoarseWindowed_                      .end(); it++) delete *it; hCellChargeCoarseWindowed_                      .clear();
    for(std::vector<TH2F*>::iterator it=hCellChargeCoarseNormWindowed_                  .begin(); it!=hCellChargeCoarseNormWindowed_                  .end(); it++) delete *it; hCellChargeCoarseNormWindowed_                  .clear();
    for(std::vector<TH2F*>::iterator it=hHitsNotOnRowColWindowed_                       .begin(); it!=hHitsNotOnRowColWindowed_                       .end(); it++) delete *it; hHitsNotOnRowColWindowed_                       .clear();
    for(std::vector<TH1F*>::iterator it=hCutsControlWindowed_                           .begin(); it!=hCutsControlWindowed_                           .end(); it++) delete *it; hCutsControlWindowed_                           .clear();
    for(std::vector<TH1F*>::iterator it=hChargeNotOnRowColWindowed_                     .begin(); it!=hChargeNotOnRowColWindowed_                     .end(); it++) delete *it; hChargeNotOnRowColWindowed_                     .clear();


    // END Window

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Charge::langausFit(TH1* histo, double *fitParameters)
{
    TAxis* xAxis           ;
    double range           ;
    double integral        ;
    double gausPar      [3];
    double landauPar    [3];
    double fitRange     [2];
    double startValues  [4];
    double parsLowLimit [4];
    double parsHighLimit[4];

    TF1* landau = new TF1("myLandau","landau",0,60000);
    TF1* gaus   = new TF1("myGaus"  ,"gaus"  ,0,60000);

    // NOTE: Changed lower bound from .4 to .2
    fitRange[0]=0.2*(histo->GetMean());
    fitRange[1]=1.8*(histo->GetMean());

    gaus->SetRange(fitRange[0],fitRange[1]);
    histo->Fit(gaus,"0QR");
    for(int p=0; p<3; p++)
        gausPar[p] = gaus->GetParameter(p);

    landau->SetRange(fitRange[0],fitRange[1]);
    histo->Fit(landau,"0QR");
    for(int p=0; p<3; p++)
        landauPar[p] = landau->GetParameter(p);

    xAxis    = histo->GetXaxis();
    range    = xAxis->GetXmax() - xAxis->GetXmin();
    integral = ((histo->Integral())*range)/(histo->GetNbinsX());

    startValues[0]=landauPar[2];
    startValues[1]=landauPar[1];
    startValues[2]=integral    ;
    startValues[3]=gausPar[2]  ;

    parsLowLimit [0] = startValues[0] - 0.68*startValues[0];
    parsHighLimit[0] = startValues[0] + 0.68*startValues[0];
    parsLowLimit [1] = startValues[1] - 0.68*startValues[1];
    parsHighLimit[1] = startValues[1] + 0.68*startValues[1];
    parsLowLimit [2] = startValues[2] - 0.68*startValues[2];
    parsHighLimit[2] = startValues[2] + 0.68*startValues[2];
    parsLowLimit [3] = startValues[3] - 0.68*startValues[3];
    parsHighLimit[3] = startValues[3] + 0.68*startValues[3];

    langaus_->SetRange(fitRange[0],fitRange[1]);
    langaus_->SetParameters(startValues);

    for (int p=0; p<4; p++) {
        langaus_->SetParLimits(p, parsLowLimit[p], parsHighLimit[p]);
    }

    TFitResultPtr r = histo->Fit(langaus_,"QRBL");
    int fitStatus = r;

    langaus_->GetParameters(fitParameters);

    return fitStatus;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::fitCharge(int planeID)
{
    std::stringstream ss;
    double fitParameters[4];

    /*--------------------------------------------------------*/
    STDLINE("All single hits:",ACWhite);
    STDLINE("",ACWhite);

    langausFit(hLandauClusterSize1_[planeID],fitParameters);

    ss.str(""); ss << "Width: " << fitParameters[0];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "MPV: " << fitParameters[1];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "Area: " << fitParameters[2];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "GSigma: " << fitParameters[3];
    STDLINE(ss.str(),ACGreen);

    STDLINE("",ACWhite);

    /*--------------------------------------------------------*/
    STDLINE("Single hits in the cell fiducial window:",ACWhite);
    STDLINE("",ACWhite);

    langausFit(hCellLandau_[planeID],fitParameters);
    // FIXME: Fit windowLandau

    ss.str(""); ss << "Width: " << fitParameters[0];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "MPV: " << fitParameters[1];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "Area: " << fitParameters[2];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "GSigma: " << fitParameters[3];
    STDLINE(ss.str(),ACGreen);

    STDLINE("",ACWhite);

    /*--------------------------------------------------------*/
    STDLINE("Single pixel in the cell fiducial window", ACWhite);
    STDLINE("", ACWhite);

    langausFit(hCellLandauSinglePixel_[planeID], fitParameters);

    ss.str(""); ss << "Width: " << fitParameters[0];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "MPV: " << fitParameters[1];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "Area: " << fitParameters[2];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "GSigma: " << fitParameters[3];
    STDLINE(ss.str(),ACGreen);

    STDLINE("",ACWhite);

    /*--------------------------------------------------------*/
    STDLINE("All double hits:",ACWhite);
    STDLINE("",ACWhite);

    langausFit(hLandauClusterSize2_[planeID],fitParameters);

    ss.str(""); ss << "Width: " << fitParameters[0];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "MPV: " << fitParameters[1];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "Area: " << fitParameters[2];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "GSigma: " << fitParameters[3];
    STDLINE(ss.str(),ACGreen);

    STDLINE("",ACWhite);

    /*--------------------------------------------------------*/
    STDLINE("Double hits on the same row:",ACWhite);
    STDLINE("",ACWhite);

    langausFit(hLandauClusterSize2sameRow_[planeID],fitParameters);

    ss.str(""); ss << "Width: " << fitParameters[0];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "MPV: " << fitParameters[1];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "Area: " << fitParameters[2];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "GSigma: " << fitParameters[3];
    STDLINE(ss.str(),ACGreen);

    STDLINE("",ACWhite);

    /*--------------------------------------------------------*/
    STDLINE("Double hits on the same column:",ACWhite);
    STDLINE("",ACWhite);

    langausFit(hLandauClusterSize2sameCol_[planeID],fitParameters);

    ss.str(""); ss << "Width: " << fitParameters[0];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "MPV: " << fitParameters[1];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "Area: " << fitParameters[2];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "GSigma: " << fitParameters[3];
    STDLINE(ss.str(),ACGreen);

    STDLINE("",ACWhite);

    /*--------------------------------------------------------*/
    STDLINE("All 3 hits:",ACWhite);
    STDLINE("",ACWhite);

    langausFit(hLandauClusterSize3sameCol_[planeID],fitParameters);

    ss.str(""); ss << "Width: " << fitParameters[0];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "MPV: " << fitParameters[1];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "Area: " << fitParameters[2];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "GSigma: " << fitParameters[3];
    STDLINE(ss.str(),ACGreen);

    STDLINE("",ACWhite);

    /*--------------------------------------------------------*/
    STDLINE("3 hits on the same row:",ACWhite);
    STDLINE("",ACWhite);

    langausFit(hLandauClusterSize3sameRow_[planeID],fitParameters);

    ss.str(""); ss << "Width: " << fitParameters[0];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "MPV: " << fitParameters[1];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "Area: " << fitParameters[2];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "GSigma: " << fitParameters[3];
    STDLINE(ss.str(),ACGreen);

    STDLINE("",ACWhite);

    /*--------------------------------------------------------*/
    STDLINE("3 hits on the same column:",ACWhite);
    STDLINE("",ACWhite);

    langausFit(hLandauClusterSize3sameCol_[planeID],fitParameters);

    ss.str(""); ss << "Width: " << fitParameters[0];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "MPV: " << fitParameters[1];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "Area: " << fitParameters[2];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "GSigma: " << fitParameters[3];
    STDLINE(ss.str(),ACGreen);

    STDLINE("",ACWhite);

    /*--------------------------------------------------------*/
    STDLINE("3 hits on the same row:",ACWhite);
    STDLINE("",ACWhite);

    langausFit(hLandauClusterSize3sameRow_[planeID],fitParameters);

    ss.str(""); ss << "Width: " << fitParameters[0];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "MPV: " << fitParameters[1];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "Area: " << fitParameters[2];
    STDLINE(ss.str(),ACGreen);

    ss.str(""); ss << "GSigma: " << fitParameters[3];
    STDLINE(ss.str(),ACGreen);

    STDLINE("",ACWhite);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::setErrorsBar(int planeID)
{
    std::stringstream hName;
    std::string planeName;

    planeName = thePlaneMapping_->getPlaneName(planeID);
    theAnalysisManager_->cd("/Charge/" + planeName + "/Xasimmetry");

    hName.str(""); hName << "h1DXcellChargeAsimmetry_" << planeName;
    h1DXcellChargeAsimmetry_.push_back((TH1F*)h2DXcellChargeAsimmetry_[planeID]->ProfileX(hName.str().c_str(),1,-1));

    hName.str(""); hName << "h1DXcellChargeAsimmetryInv_" << planeName;
    h1DXcellChargeAsimmetryInv_.push_back((TH1F*)h2DXcellChargeAsimmetryInv_[planeID]->ProfileX(hName.str().c_str(),1,-1));

    hName.str(""); hName << "h1DXcellChargeAsimmetryUnconstrained_" << planeName;
    h1DXcellChargeAsimmetryUnconstrained_.push_back((TH1F*)h2DXcellChargeAsimmetryUnconstrained_[planeID]->ProfileX(hName.str().c_str(),1,-1));

    hName.str(""); hName << "h1DXcellChargeAsimmetryUnconstrainedInv_" << planeName;
    h1DXcellChargeAsimmetryUnconstrainedInv_.push_back((TH1F*)h2DXcellChargeAsimmetryUnconstrainedInv_[planeID]->ProfileX(hName.str().c_str(),1,-1));

    hName.str(""); hName << "h1DXcellChargeAsimmetrySizeLE2_" << planeName;
    h1DXCellChargeAsimmetrySizeLE2_.push_back((TH1F*)h2DXCellChargeAsimmetrySizeLE2_[planeID]->ProfileX(hName.str().c_str(),1,-1));

    theAnalysisManager_->cd("/Charge/" + planeName + "/Yasimmetry");

    hName.str(""); hName << "h1DYcellChargeAsimmetry_" << planeName;
    h1DYcellChargeAsimmetry_.push_back((TH1F*)h2DYcellChargeAsimmetry_[planeID]->ProfileX(hName.str().c_str(),1,-1));

    hName.str(""); hName << "h1DYcellChargeAsimmetryInv_" << planeName;
    h1DYcellChargeAsimmetryInv_.push_back((TH1F*)h2DYcellChargeAsimmetryInv_[planeID]->ProfileX(hName.str().c_str(),1,-1));

    hName.str(""); hName << "h1DYcellChargeAsimmetryUnconstrained_" << planeName;
    h1DYcellChargeAsimmetryUnconstrained_.push_back((TH1F*)h2DYcellChargeAsimmetryUnconstrained_[planeID]->ProfileX(hName.str().c_str(),1,-1));

    hName.str(""); hName << "h1DYcellChargeAsimmetryUnconstrainedInv_" << planeName;
    h1DYcellChargeAsimmetryUnconstrainedInv_.push_back((TH1F*)h2DYcellChargeAsimmetryUnconstrainedInv_[planeID]->ProfileX(hName.str().c_str(),1,-1));

    hName.str(""); hName << "h1DYcellChargeAsimmetrySizeLE2_" << planeName;
    h1DYCellChargeAsimmetrySizeLE2_.push_back((TH1F*)h2DYCellChargeAsimmetrySizeLE2_[planeID]->ProfileX(hName.str().c_str(),1,-1));

    theAnalysisManager_->cd("/Charge/" + planeName + "/XcellCharge2D");

    hName.str(""); hName << "hHitsNotONRowColVsXSlopeProfileX_" << planeName;
    hHitsNotONRowColVsXSlopeProfileX_.push_back((TH1F*)hHitsNotONRowColVsXSlope_[planeID]->ProfileX(hName.str().c_str(),150,-150));

    hName.str(""); hName << "hHitsNotONRowColVsXSlopeProfileY_" << planeName;
    hHitsNotONRowColVsXSlopeProfileY_.push_back((TH1F*)hHitsNotONRowColVsXSlope_[planeID]->ProfileY(hName.str().c_str(),0.000015,-0.000015));

    theAnalysisManager_->cd("/Charge/" + planeName + "/YcellCharge2D");

    hName.str(""); hName << "hHitsNotONRowColVsYSlopeProfileX_" << planeName;
    hHitsNotONRowColVsYSlopeProfileX_.push_back((TH1F*)hHitsNotONRowColVsYSlope_[planeID]->ProfileX(hName.str().c_str(),150,-150));

    hName.str(""); hName << "hHitsNotONRowColVsYSlopeProfileY_" << planeName;
    hHitsNotONRowColVsYSlopeProfileY_.push_back((TH1F*)hHitsNotONRowColVsYSlope_[planeID]->ProfileY(hName.str().c_str(),0.000015,-0.000015));

    theAnalysisManager_->cd("/Charge/" + planeName + "/XcellCharge1D");

    double binError;

    int nBinsX = h2DXcellCharge_[planeID]->GetNbinsX();
    TH1D* hXchargeTmp[nBinsX];
    for(int bX=1; bX<=nBinsX; ++bX)
    {
        hName.str(""); hName << "Xcharge_Proj_bin_" << bX;
        hXchargeTmp[bX-1] = h2DXcellCharge_[planeID]->ProjectionY(hName.str().c_str(),bX,bX);
        if(hXchargeTmp[bX-1]->GetEntries()!=0)
        {
            binError = hXchargeTmp[bX-1]->GetRMS()/sqrt(hXchargeTmp[bX-1]->GetEntries());
            h1DXcellCharge_[planeID]->SetBinError(bX,binError);
        }
        else
            continue;
    }

    for(int p=0; p<nBinsX; p++) hXchargeTmp[p]->Delete("0");

    nBinsX = h2DXcellChargeSumLE2_[planeID]->GetNbinsX();
    TH1D* hXchargeSumLE2Tmp[nBinsX];
    for(int bX=1; bX<=nBinsX; ++bX)
    {
        hName.str(""); hName << "XchargeSumLE2_Proj_bin_" << bX;
        hXchargeSumLE2Tmp[bX-1] = h2DXcellChargeSumLE2_[planeID]->ProjectionY(hName.str().c_str(),bX,bX);
        if(hXchargeSumLE2Tmp[bX-1]->GetEntries()!=0)
        {
            binError = hXchargeSumLE2Tmp[bX-1]->GetRMS()/sqrt(hXchargeSumLE2Tmp[bX-1]->GetEntries());
            h1DXcellChargeSumLE2_[planeID]->SetBinError(bX,binError);
        }
        else
            continue;
    }

    for(int p=0; p<nBinsX; p++) hXchargeSumLE2Tmp[p]->Delete("0");

    nBinsX = h2DXcellChargeSumLE3_[planeID]->GetNbinsX();
    TH1D* hXchargeSumLE3Tmp[nBinsX];
    for(int bX=1; bX<=nBinsX; ++bX)
    {
        hName.str(""); hName << "XchargeSumLE3_Proj_bin_" << bX;
        hXchargeSumLE3Tmp[bX-1] = h2DXcellChargeSumLE3_[planeID]->ProjectionY(hName.str().c_str(),bX,bX);
        if(hXchargeSumLE3Tmp[bX-1]->GetEntries()!=0)
        {
            binError = hXchargeSumLE3Tmp[bX-1]->GetRMS()/sqrt(hXchargeSumLE3Tmp[bX-1]->GetEntries());
            h1DXcellChargeSumLE3_[planeID]->SetBinError(bX,binError);
        }
        else
            continue;
    }

    for(int p=0; p<nBinsX; p++) hXchargeSumLE3Tmp[p]->Delete("0");

    theAnalysisManager_->cd("/Charge/" + planeName + "/YcellCharge1D");

    int nBinsY = h2DYcellCharge_[planeID]->GetNbinsX();
    TH1D* hYchargeTmp[nBinsX];
    for(int bY=1; bY<=nBinsY; ++bY)
    {
        hName.str(""); hName << "Ycharge_Proj_bin_" << bY;
        hYchargeTmp[bY-1] = h2DYcellCharge_[planeID]->ProjectionY(hName.str().c_str(),bY,bY);
        if(hYchargeTmp[bY-1]->GetEntries()!=0)
        {
            binError = hYchargeTmp[bY-1]->GetRMS()/sqrt(hYchargeTmp[bY-1]->GetEntries());
            h1DYcellCharge_[planeID]->SetBinError(bY,binError);
        }
        else
            continue;
    }

    for(int p=0; p<nBinsY; p++) hYchargeTmp[p]->Delete("0");

    nBinsY = h2DYcellChargeSumLE2_[planeID]->GetNbinsX();
    TH1D* hYchargeSumLE2Tmp[nBinsX];
    for(int bY=1; bY<=nBinsY; ++bY)
    {
        hName.str(""); hName << "YchargeSumLE2_Proj_bin_" << bY;
        hYchargeSumLE2Tmp[bY-1] = h2DYcellChargeSumLE2_[planeID]->ProjectionY(hName.str().c_str(),bY,bY);
        if(hYchargeSumLE2Tmp[bY-1]->GetEntries()!=0)
        {
            binError = hYchargeSumLE2Tmp[bY-1]->GetRMS()/sqrt(hYchargeSumLE2Tmp[bY-1]->GetEntries());
            h1DYcellChargeSumLE2_[planeID]->SetBinError(bY,binError);
        }
        else
            continue;
    }

    for(int p=0; p<nBinsY; p++) hYchargeSumLE2Tmp[p]->Delete("0");

    nBinsY = h2DYcellChargeSumLE3_[planeID]->GetNbinsX();
    TH1D* hYchargeSumLE3Tmp[nBinsX];
    for(int bY=1; bY<=nBinsY; ++bY)
    {
        hName.str(""); hName << "YchargeSumLE3_Proj_bin_" << bY;
        hYchargeSumLE3Tmp[bY-1] = h2DYcellChargeSumLE3_[planeID]->ProjectionY(hName.str().c_str(),bY,bY);
        if(hYchargeSumLE3Tmp[bY-1]->GetEntries()!=0)
        {
            binError = hYchargeSumLE3Tmp[bY-1]->GetRMS()/sqrt(hYchargeSumLE3Tmp[bY-1]->GetEntries());
            h1DYcellChargeSumLE3_[planeID]->SetBinError(bY,binError);
        }
        else
            continue;
    }

    for(int p=0; p<nBinsY; p++) hYchargeSumLE3Tmp[p]->Delete("0");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::clusterSize(bool, int planeID, const Data& data, int threadNumber)
{
    if( !data.getHasHit(planeID) )
        return;

    THREADED(hClusterSize_ [planeID])->Fill(data.getClusterSize(planeID));
    THREADED(hNumberOfCols_[planeID])->Fill(data.getNumberOfCols(planeID));
    THREADED(hNumberOfRows_[planeID])->Fill(data.getNumberOfRows(planeID));

    // START Window

    if (customWindow.checkCustomWindow(data.getXPixelResidualLocal(planeID), data.getYPixelResidualLocal(planeID))) {
        THREADED(hClusterSizeWindowed_ [planeID])->Fill(data.getClusterSize(planeID));
        THREADED(hNumberOfColsWindowed_[planeID])->Fill(data.getNumberOfCols(planeID));
        THREADED(hNumberOfRowsWindowed_[planeID])->Fill(data.getNumberOfRows(planeID));
    }

    // END Window

    if(!data.getIsInDetector(planeID) )
        return;

    float maxPitchX = 150;
    float maxPitchY = 100;

    if( data.getXPitchLocal(planeID) > maxPitchX || data.getYPitchLocal(planeID) > maxPitchY )
        return;

    // [Afa:] The code is not used. Commented it out
/*
    float xRes = 0;
    float yRes = 0;

    if( data.getXPixelResidualLocal(planeID) > 0 )
        xRes = data.getXPitchLocal(planeID)/2 - data.getXPixelResidualLocal(planeID);
    else if( data.getXPixelResidualLocal(planeID) <= 0 )
        xRes = -(data.getXPixelResidualLocal(planeID) + data.getXPitchLocal(planeID)/2);

    if( data.getYPixelResidualLocal(planeID) > 0 )
        yRes = data.getYPitchLocal(planeID)/2 - data.getYPixelResidualLocal(planeID);
    else if( data.getYPixelResidualLocal(planeID) <= 0 )
        yRes = -(data.getYPixelResidualLocal(planeID) + data.getYPitchLocal(planeID)/2);
*/

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted(planeID)         ;
    int           col       = data.getColPredicted(planeID)         ;

    if( !theWindow->checkWindow(col,row) )
        return;

    int size = data.getClusterSize(planeID);

    if (!data.getHasHit(planeID))
        return;
    if (size > 3)
        return;

    for (int h = 0; h < size; ++h) { // Traverse the cluster, find the corresponding pixel in cluster
        if (data.getClusterPixelRow(h, planeID) == row && data.getClusterPixelCol(h, planeID) == col
                && data.getIsPixelCalibrated (h, planeID)
                && data.getClusterPixelCharge(h, planeID) > threashold_
                && data.getClusterPixelCharge(h, planeID) < maxCharge_) {

            switch (size) {
            case 2:
                THREADED(hClusterSizeDistribution2s_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
                // START Cluster Size Distr..clusterSize() Fill Size 2
                if (col % 2 == 0) { // Even columns
                    if (row % 2 == 0) {
                        THREADED(hClusterSizeDistribution2sEREC_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
                    } else {
                        THREADED(hClusterSizeDistribution2sOREC_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
                    }
                } else { // Odd columns
                    if (row % 2 == 0) {
                        THREADED(hClusterSizeDistribution2sEROC_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
                    } else {
                        THREADED(hClusterSizeDistribution2sOROC_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
                    }
                }
                break;
            case 1:
                THREADED(hClusterSizeDistribution1s_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
                // START Cluster Size Distr..clusterSize() Fill Size 1
                if (col % 2 == 0) { // Even columns
                    if (row % 2 == 0) {
                        THREADED(hClusterSizeDistribution1sEREC_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
                    } else {
                        THREADED(hClusterSizeDistribution1sOREC_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
                    }
                } else { // Odd columns
                    if (row % 2 == 0) {
                        THREADED(hClusterSizeDistribution1sEROC_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
                    } else {
                        THREADED(hClusterSizeDistribution1sOROC_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
                    }
                }
                break;
            default:
                break;
            }
            // The break below is for "for"
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::clusterLandau(bool pass, int planeID, const Data& data, int threadNumber)
{
    if( !pass || !data.getHasHit(planeID) )
        return;

    int size = data.getClusterSize(planeID);

    if( size > 3 )
        return;

    const Window* theWindow = theWindowsManager_->getWindow(planeID);

    for(int h=0; h<size; ++h)
    {
        if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID)) //hits must fall in the window
               || !data.getIsPixelCalibrated(h,planeID)                                                          //pixels must be calibrated
               ||  data.getClusterPixelCharge(h,planeID) < threashold_   )                                       //charge must be over threashold  ---> Doesn't work with old data
            return;
    }

    int charge = data.getClusterCharge(planeID);

    if( size == 1 )
        THREADED(hLandauClusterSize1_[planeID])->Fill(charge);
    else if( size == 2 )
        THREADED(hLandauClusterSize2_[planeID])->Fill(charge);
    else if( size == 3 )
        THREADED(hLandauClusterSize3_[planeID])->Fill(charge);

    THREADED(hClusterSizeCuts_ [planeID])->Fill(data.getClusterSize(planeID));

    // START Window
    if (customWindow.checkCustomWindow(data.getXPixelResidualLocal(planeID), data.getYPixelResidualLocal(planeID))) {
        if( size == 1 )
            THREADED(hLandauClusterSize1Windowed_[planeID])->Fill(charge);
        else if( size == 2 )
            THREADED(hLandauClusterSize2Windowed_[planeID])->Fill(charge);
        else if( size == 3 )
            THREADED(hLandauClusterSize3Windowed_[planeID])->Fill(charge);

        THREADED(hClusterSizeCutsWindowed_ [planeID])->Fill(data.getClusterSize(planeID));
    }
    // END Window

    bool cellLandauCut = true;
    if(cutsFormulas_.find("cell Landau") != cutsFormulas_.end()) {
            cellLandauCut = cutsFormulas_["cell Landau"][threadNumber]->EvalInstance();
    }

    if (cellLandauCut) THREADED(hClusterSizeCutsPlus_[planeID])->Fill(size);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::cellLandau(bool pass, int planeID, const Data& data, int threadNumber)
{
    if( !pass || !data.getHasHit(planeID) )
        return;

    int size = data.getClusterSize(planeID);
    if (size > 4) return;
    const Window* theWindow = theWindowsManager_->getWindow(planeID);

    for(int h=0; h<size; ++h)
    {
        if(!theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID))
                || !data.getIsPixelCalibrated (h,planeID))
            return;
        THREADED(hCellLandauSinglePixel_[planeID])->Fill(data.getClusterPixelCharge(h, planeID));
        if (customWindow.checkCustomWindow(data.getXPixelResidualLocal(planeID), data.getYPixelResidualLocal(planeID)))
            THREADED(hCellLandauSinglePixelWindowed_[planeID])->Fill(data.getClusterPixelCharge(h, planeID));
    }

    if( size != 1 )
        return;
    // Why the graph description says "single hits"?
    if( theWindow->checkWindow(data.getClusterPixelCol(0,planeID),data.getClusterPixelRow(0,planeID))
           && data.getIsPixelCalibrated(0,planeID)
           && data.getClusterCharge(planeID) > threashold_  ) {
        THREADED(hCellLandau_[planeID])->Fill(data.getClusterCharge(planeID));
        if (customWindow.checkCustomWindow(data.getXPixelResidualLocal(planeID), data.getYPixelResidualLocal(planeID)))
            THREADED(hCellLandauWindowed_[planeID])->Fill(data.getClusterCharge(planeID));
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::Xlandau(bool pass, int planeID, const Data &data, int threadNumber)
{
    if( !pass || !data.getHasHit(planeID) || data.getClusterSize(planeID) < 2 )
        return;

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted(planeID)         ;
    int           col       = data.getColPredicted(planeID)         ;

    if( !theWindow->checkWindow(col,row) )
        return;

    int size = data.getClusterSize(planeID);
    if (size > 4) return;
    for(int h=0; h<size; ++h)
    {
        if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID)) //hits must fall in the window
               ||  data.getClusterPixelRow   (h,planeID) != row                                               //hits must be on the same row (sharing is along the row - x direction)
               || !data.getIsPixelCalibrated (h,planeID)                                                      //pixels must be calibrated
               ||  data.getClusterPixelCharge(h,planeID) < threashold_  )                                     //charge must be over threashold
            return;
    }

    if( size == 2)
        THREADED(hLandauClusterSize2sameRow_[planeID])->Fill(data.getClusterCharge(planeID));
    else if( size == 3)
        THREADED(hLandauClusterSize3sameRow_[planeID])->Fill(data.getClusterCharge(planeID));

    if (customWindow.checkCustomWindow(data.getXPixelResidualLocal(planeID), data.getYPixelResidualLocal(planeID))) {
        if( size == 2)
            THREADED(hLandauClusterSize2sameRowWindowed_[planeID])->Fill(data.getClusterCharge(planeID));
        else if( size == 3)
            THREADED(hLandauClusterSize3sameRowWindowed_[planeID])->Fill(data.getClusterCharge(planeID));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::Ylandau(bool pass, int planeID, const Data &data, int threadNumber)
{
    if( !pass || !data.getHasHit(planeID) || data.getClusterSize(planeID) < 2 )
        return;

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted(planeID)         ;
    int           col       = data.getColPredicted(planeID)         ;

    if( !theWindow->checkWindow(col,row) )
        return;

    int size = data.getClusterSize(planeID);
    if (size > 4) return;
    for(int h=0; h<size; ++h)
    {
        if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID)) //hits must fall in the window
               ||  data.getClusterPixelCol   (h,planeID) !=  col                                              //hits must be on the same column (sharing is along the column - y direction)
               || !data.getIsPixelCalibrated (h,planeID)                                                      //pixels must be calibrated
               ||  data.getClusterPixelCharge(h,planeID) < threashold_ )                                      //charge must be over threashold
            return;
    }

    if( size ==2 )
        THREADED(hLandauClusterSize2sameCol_[planeID])->Fill(data.getClusterCharge(planeID));
    else if( size == 3 )
        THREADED(hLandauClusterSize3sameCol_[planeID])->Fill(data.getClusterCharge(planeID));

    if (customWindow.checkCustomWindow(data.getXPixelResidualLocal(planeID), data.getYPixelResidualLocal(planeID)))  {
        if( size ==2 )
            THREADED(hLandauClusterSize2sameCol_[planeID])->Fill(data.getClusterCharge(planeID));
        else if( size == 3 )
            THREADED(hLandauClusterSize3sameCol_[planeID])->Fill(data.getClusterCharge(planeID));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::cellCharge(bool pass, int planeID, const Data& data, int threadNumber)
{
    if( !pass || !data.getIsInDetector(planeID) )
        return;

    float maxPitchX = 150;
    float maxPitchY = 100;

    if( data.getXPitchLocal(planeID) > maxPitchX || data.getYPitchLocal(planeID) > maxPitchY )
        return;

    float xRes = 0;
    float yRes = 0;

    if( data.getXPixelResidualLocal(planeID) > 0 )
        xRes = data.getXPitchLocal(planeID)/2 - data.getXPixelResidualLocal(planeID);
    else if( data.getXPixelResidualLocal(planeID) <= 0 )
        xRes = -(data.getXPixelResidualLocal(planeID) + data.getXPitchLocal(planeID)/2);

    if( data.getYPixelResidualLocal(planeID) > 0 )
        yRes = data.getYPitchLocal(planeID)/2 - data.getYPixelResidualLocal(planeID);
    else if( data.getYPixelResidualLocal(planeID) <= 0 )
        yRes = -(data.getYPixelResidualLocal(planeID) + data.getYPitchLocal(planeID)/2);

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted(planeID)         ;
    int           col       = data.getColPredicted(planeID)         ;

    // WARNING: No Window HERE
    // Why? 'Cause it looks better
    // Odd/Even tracks no window
    if (row % 2 == 0) { // Even row
        if (col % 2 == 0) {
            THREADED(h2DEvenTracksEvenRows_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
        } else {
            THREADED(h2DOddTracksEvenRows_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
        }
    } else { // Odd row
        if (col % 2 == 0) {
            THREADED(h2DEvenTracksOddRows_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
        } else {
            THREADED(h2DOddTracksOddRows_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
        }
    }

    // START Window
    bool withinCustomWindow = customWindow.checkCustomWindow(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
    if (withinCustomWindow) {
        if (row % 2 == 0) { // Even row
            if (col % 2 == 0) {
                THREADED(h2DEvenTracksEvenRowsWindowed_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
            } else {
                THREADED(h2DOddTracksEvenRowsWindowed_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
            }
        } else { // Odd row
            if (col % 2 == 0) {
                THREADED(h2DEvenTracksOddRowsWindowed_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
            } else {
                THREADED(h2DOddTracksOddRowsWindowed_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
            }
        }
    }

    // END Window

    bool withinWindow = theWindow->checkWindow(col,row);
    if(!withinWindow) {
        oddEvenCellHelperOutOfWindow(data, planeID, row, col, threadNumber);
    }

    THREADED(h2DallTracks_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));

    if (data.getClusterSize(planeID) == 2)
    {
        THREADED(h2DCellChargeNormSize2_[planeID])->Fill(xRes, yRes);
        if      (data.getRowPredicted(planeID) > 60) THREADED(h2DCellChargeNormSize2Up_  [planeID])->Fill(xRes, yRes);
        else if (data.getRowPredicted(planeID) < 58) THREADED(h2DCellChargeNormSize2Down_[planeID])->Fill(xRes, yRes);
    }
    else if (data.getClusterSize(planeID) == 1)
    {
        THREADED(h2DCellChargeNormSize1_[planeID])->Fill(xRes, yRes);
        if      (data.getRowPredicted(planeID) > 60) THREADED(h2DCellChargeNormSize1Up_  [planeID])->Fill(xRes, yRes);
        else if (data.getRowPredicted(planeID) < 58) THREADED(h2DCellChargeNormSize1Down_[planeID])->Fill(xRes, yRes);
    }
    THREADED(h4CellsAllTracks_[planeID])->Fill(xRes,yRes);

    // START Window
    if (withinCustomWindow) {
        THREADED(h2DallTracksWindowed_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));

        if (data.getClusterSize(planeID) == 2)
        {
            THREADED(h2DCellChargeNormSize2Windowed_[planeID])->Fill(xRes, yRes);
            if      (data.getRowPredicted(planeID) > 60) THREADED(h2DCellChargeNormSize2UpWindowed_  [planeID])->Fill(xRes, yRes);
            else if (data.getRowPredicted(planeID) < 58) THREADED(h2DCellChargeNormSize2DownWindowed_[planeID])->Fill(xRes, yRes);
        }
        else if (data.getClusterSize(planeID) == 1)
        {
            THREADED(h2DCellChargeNormSize1Windowed_[planeID])->Fill(xRes, yRes);
            if      (data.getRowPredicted(planeID) > 60) THREADED(h2DCellChargeNormSize1UpWindowed_  [planeID])->Fill(xRes, yRes);
            else if (data.getRowPredicted(planeID) < 58) THREADED(h2DCellChargeNormSize1DownWindowed_[planeID])->Fill(xRes, yRes);
        }
        THREADED(h4CellsAllTracksWindowed_[planeID])->Fill(xRes,yRes);
    }
    // END Window

    int size = data.getClusterSize(planeID);

    if( !data.getHasHit(planeID) || size > 4 )
        return;

    for(int h=0; h<size; h++)
    {
        THREADED(hCutsControl_[planeID])->Fill(0);
        if (data.getIsPixelCalibrated (h,planeID))
        {
            THREADED(hCutsControl_[planeID])->Fill(1);
            if (data.getClusterPixelRow(h,planeID) == row && data.getClusterPixelCol(h,planeID) == col)
            {
                THREADED(hCutsControl_[planeID])->Fill(2);
                if (data.getClusterSize(planeID) < 2)
                {
                    THREADED(hCutsControl_[planeID])->Fill(3);
                }
            }
            else
            {
                THREADED(hCutsControl_[planeID])->Fill(4);
                if (data.getClusterSize(planeID) < 2) THREADED(hCutsControl_[planeID])->Fill(5);
                THREADED(hHitsNotOnRowCol_[planeID])->Fill(xRes, yRes);
                if (yRes > 20 || yRes < -20) THREADED(hHitsNotOnRowColProjX_[planeID])->Fill(xRes);
                if (xRes > 30 || xRes < -30) THREADED(hHitsNotOnRowColProjY_[planeID])->Fill(yRes);
                THREADED(hHitsNotONRowColVsXSlope_[planeID])->Fill(xRes, data.getXSlopeUnconstrained(planeID));
                THREADED(hHitsNotONRowColVsYSlope_[planeID])->Fill(yRes, data.getYSlopeUnconstrained(planeID));
                THREADED(hChargeNotOnRowCol_[planeID])->Fill(data.getClusterPixelCharge(h, planeID));
            }
        }
    }

    THREADED(h2DCellChargeNorm_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));

    // START Window

    if (withinCustomWindow) {
        for(int h=0; h<size; h++)
        {
            THREADED(hCutsControlWindowed_[planeID])->Fill(0);
            if (data.getIsPixelCalibrated (h,planeID))
            {
                THREADED(hCutsControlWindowed_[planeID])->Fill(1);
                if (data.getClusterPixelRow(h,planeID) == row && data.getClusterPixelCol(h,planeID) == col)
                {
                    THREADED(hCutsControlWindowed_[planeID])->Fill(2);
                    if (data.getClusterSize(planeID) < 2)
                    {
                        THREADED(hCutsControlWindowed_[planeID])->Fill(3);
                    }
                }
                else
                {
                    THREADED(hCutsControlWindowed_[planeID])->Fill(4);
                    if (data.getClusterSize(planeID) < 2) THREADED(hCutsControlWindowed_[planeID])->Fill(5);
                    THREADED(hHitsNotOnRowColWindowed_[planeID])->Fill(xRes, yRes);
                    THREADED(hChargeNotOnRowColWindowed_[planeID])->Fill(data.getClusterPixelCharge(h, planeID));
                }
            }
        }

        THREADED(h2DCellChargeNormWindowed_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
    }

    // END Window


    for(int h=0; h<size; h++)
    {
        if(    data.getClusterPixelRow   (h,planeID) == row
               && data.getClusterPixelCol   (h,planeID) == col
               && data.getIsPixelCalibrated (h,planeID)
               && data.getClusterPixelCharge(h,planeID) > threashold_
               && data.getClusterPixelCharge(h,planeID) < maxCharge_
               )
        {
            THREADED(h4CellsChargeNorm_[planeID])->Fill(xRes,yRes);
            THREADED(h2DCellCharge_            [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));

            // This is for those within the window
            if (row % 2 == 0) { // Even rows
                if (col % 2 == 0)
                    THREADED(h2DCellChargeEvenColumnsEvenRows_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                else
                    THREADED(h2DCellChargeOddColumnsEvenRows_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
            } else { // Odd rows
                if (col % 2 == 0)
                    THREADED(h2DCellChargeEvenColumnsOddRows_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                else
                    THREADED(h2DCellChargeOddColumnsOddRows_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
            }

            THREADED(h2DCellChargeNum_         [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
            if (planeID == 9) ++totEventsControl_;
            THREADED(h4CellsCharge_          [planeID])->Fill(xRes,yRes,data.getClusterPixelCharge(h,planeID));
            THREADED(hCellChargeCoarse_      [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
            THREADED(hCellChargeCoarseNorm_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
            TH2F* histo = THREADED(hCellChargeCoarse_[planeID]);
            int histoN = (histo->GetXaxis()->FindBin(data.getXPixelResidualLocal(planeID))-1)*histo->GetNbinsY() + histo->GetYaxis()->FindBin(data.getYPixelResidualLocal(planeID))-1;


            if (histoN < hCellChargeCoarseLandau_[planeID].size())
                THREADED(hCellChargeCoarseLandau_[planeID][histoN])->Fill(data.getClusterPixelCharge(h,planeID)); //DOESN'T ALWAYS WORK...

            // START Window

            if (withinCustomWindow) {
                THREADED(h4CellsChargeNormWindowed_[planeID])->Fill(xRes,yRes);
                THREADED(h2DCellChargeWindowed_            [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));

                // This is for those within the window
                if (row % 2 == 0) { // Even rows
                    if (col % 2 == 0)
                        THREADED(h2DCellChargeEvenColumnsEvenRowsWindowed_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                    else
                        THREADED(h2DCellChargeOddColumnsEvenRowsWindowed_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                } else { // Odd rows
                    if (col % 2 == 0)
                        THREADED(h2DCellChargeEvenColumnsOddRowsWindowed_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                    else
                        THREADED(h2DCellChargeOddColumnsOddRowsWindowed_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                }

                THREADED(h2DCellChargeNumWindowed_         [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                THREADED(h4CellsChargeWindowed_          [planeID])->Fill(xRes,yRes,data.getClusterPixelCharge(h,planeID));
                THREADED(hCellChargeCoarseWindowed_      [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                THREADED(hCellChargeCoarseNormWindowed_  [planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
            }
            // END Window

            break;
        }
    }

    if( size == 4 )
    {
        for(int h=0; h<size; ++h)
        {
            if( !theWindow->checkWindow( data.getClusterPixelCol(h,planeID) , data.getClusterPixelRow(h,planeID) ) ||
                    !data.getIsPixelCalibrated(h,planeID) ||
                    data.getClusterPixelCharge(h,planeID) < threashold_ ||
                    data.getClusterPixelCharge(h,planeID) > maxCharge_ ) return;
        }

        bool isPredictedIn = false;
        int  firstHit;
        for(int h=0; h<size; ++h)
        {
            if(data.getClusterPixelCol(h,planeID) == col && data.getClusterPixelRow(h,planeID) == row)
            {
                isPredictedIn = true;
                firstHit = h;
                break;
            }
        }

        if(!isPredictedIn)
            return;

        bool isXmin = false;
        bool isXmax = false;
        bool isYmin = false;
        bool isYmax = false;
        for(int h=0; h<size; ++h)
        {
            if(h == firstHit)
                continue;

            if( data.getXClusterPixelCenterLocal(firstHit,planeID) > data.getXClusterPixelCenterLocal(h,planeID) )
            {
                isXmin = false;
                isXmax = true;
            }
            if( data.getXClusterPixelCenterLocal(firstHit,planeID) < data.getXClusterPixelCenterLocal(h,planeID) )
            {
                isXmin = true;
                isXmax = false;
            }
            if( data.getYClusterPixelCenterLocal(firstHit,planeID) > data.getYClusterPixelCenterLocal(h,planeID) )
            {
                isYmin = false;
                isYmax = true;
            }
            if( data.getYClusterPixelCenterLocal(firstHit,planeID) < data.getYClusterPixelCenterLocal(h,planeID) )
            {
                isYmin = true;
                isYmax = false;
            }
        }

        int secondHit = 0;
        int thirdHit  = 0;
        for(int h=0; h<size; ++h)
        {
            if( h != firstHit && row == data.getClusterPixelRow(h,planeID) &&
                    (col - data.getClusterPixelCol(h,planeID)) != 0 &&
                    abs(col - data.getClusterPixelCol(h,planeID)) <= 1 )
            {
                secondHit = h;
                break;
            }
        }
        for(int h=0; h<size; ++h)
        {
            if( h != firstHit && h!= secondHit && col == data.getClusterPixelCol(h,planeID) &&
                    (row - data.getClusterPixelRow(h,planeID)) != 0 &&
                    abs(row - data.getClusterPixelRow(h,planeID)) <= 1 )
            {
                thirdHit = h;
                break;
            }
        }

        for(int h=0; h<size; ++h)
        {
            if( h != firstHit && h != secondHit && h != thirdHit &&
                    data.getClusterPixelRow(h,planeID) == data.getClusterPixelRow(thirdHit,planeID) &&
                    data.getClusterPixelCol(h,planeID) == data.getClusterPixelCol(secondHit,planeID))
            {
                if(isYmax && isXmin)
                {
                    xRes = -data.getXPixelResidualLocal(planeID) - data.getXPitchLocal(planeID)/2;
                    yRes = -data.getYPixelResidualLocal(planeID) + data.getYPitchLocal(planeID)/2;
                }
                else if(isYmin && isXmin)
                {
                    xRes = -data.getXPixelResidualLocal(planeID) - data.getXPitchLocal(planeID)/2;
                    yRes = -data.getYPixelResidualLocal(planeID) - data.getYPitchLocal(planeID)/2;
                }
                else if(isYmax && isXmax)
                {
                    xRes = -data.getXPixelResidualLocal(planeID) + data.getXPitchLocal(planeID)/2;
                    yRes = -data.getYPixelResidualLocal(planeID) + data.getYPitchLocal(planeID)/2;
                }
                else if(isYmin && isXmax)
                {
                    xRes = -data.getXPixelResidualLocal(planeID) + data.getXPitchLocal(planeID)/2;
                    yRes = -data.getYPixelResidualLocal(planeID) - data.getYPitchLocal(planeID)/2;
                }

                /*if(planeID==9)
                {
                    std::cout << "event: " << data.getEventNumber() << " - predicted row: " << row << " - predicted col: " << col << std::endl;
                    std::cout << "hit: " << firstHit << " - row: " << data.getClusterPixelRow(firstHit,planeID) << " - col: " << data.getClusterPixelCol(firstHit,planeID) << std::endl;
                    std::cout << "hit: " << secondHit << " - row: " << data.getClusterPixelRow(secondHit,planeID) << " - col: " << data.getClusterPixelCol(secondHit,planeID) << std::endl;
                    std::cout << "hit: " << thirdHit << " - row: " << data.getClusterPixelRow(thirdHit,planeID) << " - col: " << data.getClusterPixelCol(thirdHit,planeID) << std::endl;
                    std::cout << "hit: " << h << " - row: " << data.getClusterPixelRow(h,planeID) << " - col: " << data.getClusterPixelCol(h,planeID) << std::endl;
                    std::cout << "isXmax: " << isXmax << " - isXmin: " << isXmin << " - isYmax: " << isYmax << " - isYmin: " << isYmin << std::endl;
                    std::cout << "cell X: " << data.getXPixelResidualLocal(planeID) << " -----> " << xRes << std::endl;
                    std::cout << "cell Y: " << data.getYPixelResidualLocal(planeID) << " -----> " << yRes << std::endl;
                }*/
                THREADED(h4Hits_      [planeID])->Fill(xRes,yRes);
                THREADED(h4HitsCharge_[planeID])->Fill(xRes,yRes,data.getClusterPixelCharge(firstHit,planeID));

                if (withinCustomWindow) {
                    THREADED(h4HitsWindowed_      [planeID])->Fill(xRes,yRes);
                    THREADED(h4HitsChargeWindowed_[planeID])->Fill(xRes,yRes,data.getClusterPixelCharge(firstHit,planeID));
                }
                break;
            }
        }

    }

}

/**
 * @brief Charge::oddEvenCellHelper It's just a helper for those event out of window
 * @param data
 * @param planeID
 * @param row
 * @param col
 */
void Charge::oddEvenCellHelperOutOfWindow(const Data &data, int planeID, int row, int col, int threadNumber)
{
    // Cell Charge Even/Odd Columns
    // Will go here no matter within the window
    // For those out of window
    int size = data.getClusterSize(planeID);

    if( !data.getHasHit(planeID) || size > 4 )
        return;

    for(int h=0; h<size; h++)
    {
        if(    data.getClusterPixelRow   (h,planeID) == row
               && data.getClusterPixelCol   (h,planeID) == col
               && data.getIsPixelCalibrated (h,planeID)
               && data.getClusterPixelCharge(h,planeID) > threashold_
               && data.getClusterPixelCharge(h,planeID) < maxCharge_
               )
        {
            if (row % 2 == 0) { // Even rows
                if (col % 2 == 0)
                    THREADED(h2DCellChargeEvenColumnsEvenRows_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                else
                    THREADED(h2DCellChargeOddColumnsEvenRows_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
            } else { // Odd rows
                if (col % 2 == 0)
                    THREADED(h2DCellChargeEvenColumnsOddRows_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                else
                    THREADED(h2DCellChargeOddColumnsOddRows_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
            }

            if (customWindow.checkCustomWindow(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID))) {
                if (row % 2 == 0) { // Even rows
                    if (col % 2 == 0)
                        THREADED(h2DCellChargeEvenColumnsEvenRowsWindowed_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                    else
                        THREADED(h2DCellChargeOddColumnsEvenRowsWindowed_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                } else { // Odd rows
                    if (col % 2 == 0)
                        THREADED(h2DCellChargeEvenColumnsOddRowsWindowed_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                    else
                        THREADED(h2DCellChargeOddColumnsOddRowsWindowed_[planeID])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
                }
            }
        }
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::XchargeDivision(bool pass, int planeID, const Data& data, int threadNumber)
{
    if( !pass || !data.getIsInDetector(planeID) )
        return;

    float maxPitchX = 150;
    float maxPitchY = 100;

    if( data.getXPitchLocal(planeID) > maxPitchX || data.getYPitchLocal(planeID) > maxPitchY )
        return;

    float xRes = 0;
    float yRes = 0;

    if(data.getXPixelResidualLocal(planeID) > 0)
        xRes = data.getXPitchLocal(planeID)/2 - data.getXPixelResidualLocal(planeID);
    else if(data.getXPixelResidualLocal(planeID) <= 0)
        xRes = -(data.getXPixelResidualLocal(planeID) + data.getXPitchLocal(planeID)/2);

    if(data.getYPixelResidualLocal(planeID) > 0)
        yRes = data.getYPitchLocal(planeID)/2 - data.getYPixelResidualLocal(planeID);
    else if(data.getYPixelResidualLocal(planeID) <= 0)
        yRes = -(data.getYPixelResidualLocal(planeID) + data.getYPitchLocal(planeID)/2);

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted(planeID)         ;
    int           col       = data.getColPredicted(planeID)         ;

    if( !theWindow->checkWindow(col,row) )
        return;

    THREADED(h1DXallTracks_[planeID])->Fill(xRes);

    int size = data.getClusterSize(planeID);
    if( data.getHasHit(planeID) && size <= 3 )
    {
        for(int h=0; h<size; ++h)
        {
            if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID))  //hits are in the window
                   || !data.getIsPixelCalibrated(h,planeID)                                                           //pixels are calibrated
                   ||  data.getClusterPixelRow    (h,planeID) != row                                                  //hits are on the same row (sharing is along the row - x direction)
                   ||  data.getClusterPixelCharge (h,planeID) < threashold_                                           //charge is over threshold
                   ||  data.getClusterPixelCharge (h,planeID) > maxCharge_   )                                        //maximum allowed charge for this physics
                return;
        }

        int hitID = -1;
        for(int h=0; h<size; ++h)
        {
            if( data.getClusterPixelRow(h,planeID) == row && data.getClusterPixelCol(h,planeID) == col )
            {
                THREADED(h2DXcellCharge_          [planeID])->Fill(xRes,data.getClusterPixelCharge(h,planeID));
                THREADED(h1DXcellCharge_          [planeID])->Fill(xRes,data.getClusterPixelCharge(h,planeID));
                THREADED(h1DXcellChargeNormToAll_ [planeID])->Fill(xRes,data.getClusterPixelCharge(h,planeID));
                THREADED(h1DXcellChargeNorm_[planeID])->Fill(xRes);
                hitID = h;
                break;
            }
        }

        if( hitID == -1 )
            return;

        if( size == 1 )
        {
            THREADED(h2DXcellSingleHits_ [planeID])->Fill(xRes,yRes);
            THREADED(h1DXcellSingleHits_ [planeID])->Fill(xRes);
        }
        else if( size == 2 )
        {
            for(int h=0; h<size; ++h)
            {
                if( h == hitID )
                    continue;
                else if(    (data.getXPixelResidualLocal(planeID)  < 0  && (col - data.getClusterPixelCol(h,planeID)) ==  1)
                            || (data.getXPixelResidualLocal(planeID)  > 0  && (col - data.getClusterPixelCol(h,planeID)) == -1) )
                    return;
            }

            THREADED(h1DXcellDoubleHits_[planeID])->Fill(xRes);
            THREADED(h2DXcellDoubleHits_[planeID])->Fill(xRes,yRes);
        }

        if( data.getClusterCharge(planeID) <= maxTotCharge_ )
        {
            if( size <= 2 )
            {
                THREADED(h1DXcellChargeSumLE2Norm_      [planeID])->Fill(xRes);
                THREADED(h1DXcellChargeSumLE2_          [planeID])->Fill(xRes,data.getClusterCharge(planeID));
                THREADED(h1DXcellChargeSumLE2NormToAll_ [planeID])->Fill(xRes,data.getClusterCharge(planeID));
                THREADED(h2DXcellChargeSumLE2_          [planeID])->Fill(xRes,data.getClusterCharge(planeID));
            }
            if( size <= 3 )
            {
                THREADED(h1DXcellChargeSumLE3Norm_      [planeID])->Fill(xRes);
                THREADED(h1DXcellChargeSumLE3_          [planeID])->Fill(xRes,data.getClusterCharge(planeID));
                THREADED(h1DXcellChargeSumLE3NormToAll_ [planeID])->Fill(xRes,data.getClusterCharge(planeID));
                THREADED(h2DXcellChargeSumLE3_          [planeID])->Fill(xRes,data.getClusterCharge(planeID));
            }
            if( size == 3 )
                THREADED(h1DXcell3Hits_                 [planeID])->Fill(yRes);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::Xasimmetry(bool pass, int planeID, const Data& data, int threadNumber)
{
    if( !pass || !data.getIsInDetector(planeID))
        return;

    float maxPitchX = 150;

    if( data.getXPitchLocal(planeID) > maxPitchX )
        return;

    float xRes = 0;

    if(data.getXPixelResidualLocal(planeID) > 0)
        xRes = data.getXPitchLocal(planeID)/2 - data.getXPixelResidualLocal(planeID);
    else if(data.getXPixelResidualLocal(planeID) <= 0)
        xRes = -(data.getXPixelResidualLocal(planeID) + data.getXPitchLocal(planeID)/2);

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted(planeID)         ;
    int           col       = data.getColPredicted(planeID)         ;

    if( !theWindow->checkWindow(col,row) )
        return;

    int size = data.getClusterSize(planeID);
    if (size > 4) return;

    int  hitID = -1;
    for(int h=0; h<size; ++h)
    {
        if( data.getClusterPixelRow(h,planeID) == row && data.getClusterPixelCol(h,planeID) == col )
        {
            hitID = h;
            break;
        }
    }

    if( hitID == -1 )
        return;

/*    if (data.getHasHit(planeID) && size == 1)
    {
        for(int h=0; h<size; ++h)
        {
            if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID))  //hits are in the window
                   || !data.getIsPixelCalibrated(h,planeID)                                                        //pixels are calibrated
                   ||  data.getClusterPixelRow    (h,planeID) != row                                               //hits are on the same row (sharing is along the row - x direction)
                   ||  data.getClusterPixelCharge (h,planeID) <  threashold_                                       //charge is over threshold
                   ||  data.getClusterPixelCharge (h,planeID) >  maxCharge_   )                                    //maximum allowed charge for this physics
                return;
        }

        if (fabs(data.getXPixelResidualLocal(planeID)) > fabs(data.getYPixelResidualLocal(planeID)))
        {
            if (xRes >  0 && data.getClusterCharge(planeID) >= minTotCharge_ && data.getClusterCharge(planeID) <= maxTotCharge_) THREADED(h2DXCellChargeAsimmetrySizeLE2_[planeID])->Fill(xRes, -1);
            if (xRes <= 0 && data.getClusterCharge(planeID) >= minTotCharge_ && data.getClusterCharge(planeID) <= maxTotCharge_) THREADED(h2DXCellChargeAsimmetrySizeLE2_[planeID])->Fill(xRes,  1);
        }

    } */

    for(int h=0; h<size; ++h)
    {
        if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID))  //hits are in the window
               || !data.getIsPixelCalibrated(h,planeID)                                                        //pixels are calibrated
               ||  data.getClusterPixelRow    (h,planeID) != row                                               //hits are on the same row (sharing is along the row - x direction)
               ||  data.getClusterPixelCharge (h,planeID) <  threashold_                                       //charge is over threshold
               ||  data.getClusterPixelCharge (h,planeID) >  maxCharge_   )                                    //maximum allowed charge for this physics
            return;
    }

    if( data.getHasHit(planeID) && size == 2 )
    {
        float Asimmetry   = 0;
        int   totalCharge = 0;
        int   chargeLeft  = 0;
        int   chargeRight = 0;
        for(int h=0; h<size; ++h)
        {
            if( h == hitID )
                continue;
            else if(data.getXPixelResidualLocal(planeID) > 0 && (col - data.getClusterPixelCol(h,planeID)) == 1)//il secondo hit e' a SN della predetta
            {
                chargeLeft  = data.getClusterPixelCharge(h    ,planeID);
                chargeRight = data.getClusterPixelCharge(hitID,planeID);
                break;
            }
            else if(data.getXPixelResidualLocal(planeID) <= 0 && (col - data.getClusterPixelCol(h,planeID)) == -1)//il secondo hit e' a DX della predetta
            {
                chargeLeft  = data.getClusterPixelCharge(hitID,planeID);
                chargeRight = data.getClusterPixelCharge(h    ,planeID);
                break;
            }
            else if(data.getXPixelResidualLocal(planeID) > 0 && (col - data.getClusterPixelCol(h,planeID)) == -1)
            {
                chargeLeft  = data.getClusterPixelCharge(hitID,planeID);
                chargeRight = data.getClusterPixelCharge(h    ,planeID);
                xRes = - data.getXPixelResidualLocal(planeID) - data.getXPitchLocal(planeID)/2;
                break;
            }
            else if(data.getXPixelResidualLocal(planeID) < 0 && (col - data.getClusterPixelCol(h,planeID)) ==  1)
            {
                chargeLeft  = data.getClusterPixelCharge(h    ,planeID);
                chargeRight = data.getClusterPixelCharge(hitID,planeID);
                xRes = -(data.getXPixelResidualLocal(planeID) - data.getXPitchLocal(planeID)/2);
                break;
            }
            else
                return;
        }

        totalCharge = chargeLeft + chargeRight;
        Asimmetry = (float)(chargeLeft - chargeRight)/totalCharge;
        float Asimmetry0 = (float)(chargeLeft/totalCharge);

        if( totalCharge >= minTotCharge_ && totalCharge <= maxTotCharge_ )
        {
            THREADED(h2DXcellChargeAsimmetry_                [planeID])->Fill(xRes, Asimmetry);
            THREADED(h2DXCellChargeAsimmetrySizeLE2_         [planeID])->Fill(xRes, Asimmetry);
            THREADED(h2DXcellChargeAsimmetryInv_             [planeID])->Fill(Asimmetry, xRes);
            THREADED(h2DXAsimmetryLandau_                    [planeID])->Fill(totalCharge, Asimmetry);
            THREADED(hXasimmetry_                            [planeID])->Fill(Asimmetry);
            THREADED(hXasimmetry0_                           [planeID])->Fill(Asimmetry0);
        }
    }

        if( data.getHasHit(planeID) && size == 1 )
        {
            float Asimmetry   = 0;
            int   totalCharge = 0;

            if(data.getClusterPixelCol(0,planeID) == col)//la cella colpita coincide con la predetta
            {
                if(data.getXPixelResidualLocal(planeID) > 0)//la traccia ha colpito a sn
                    Asimmetry = -1;
                else if(data.getXPixelResidualLocal(planeID) <= 0)//la traccia ha colpito a dx
                    Asimmetry = 1;
            }
            else if(data.getXPixelResidualLocal(planeID) >  0 && (col - data.getClusterPixelCol(0,planeID)) ==  1)//la cella colpita e' a SN della predetta
            {
                Asimmetry = -1;
                xRes = - data.getXPixelResidualLocal(planeID) - data.getXPitchLocal(planeID)/2;
            }
            else if(data.getXPixelResidualLocal(planeID) <= 0 && (col - data.getClusterPixelCol(0,planeID)) == -1)//la cella colpita e' a DX della predetta
            {
                Asimmetry = 1;
                xRes = -(data.getXPixelResidualLocal(planeID) - data.getXPitchLocal(planeID)/2);
            }
            else
                return;

            totalCharge = data.getClusterPixelCharge(0,planeID);

            THREADED(h2DXCellChargeAsimmetrySizeLE2_[planeID])->Fill(xRes, Asimmetry);
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::XasimmetryUnconstr(bool pass, int planeID, const Data& data, int threadNumber)
{
    if( !pass || !data.getIsInDetector(planeID))
        return;

    float maxPitchX = 150;

    if( data.getXPixelPitchLocalUnconstrained(planeID) > maxPitchX )
        return;

    float xResUnconstrained = 0  ;

    if(data.getXPixelResidualLocalUnconstrained(planeID) > 0)
        xResUnconstrained = data.getXPixelPitchLocalUnconstrained(planeID)/2 - data.getXPixelResidualLocalUnconstrained(planeID);
    else if(data.getXPixelResidualLocalUnconstrained(planeID) <= 0)
        xResUnconstrained = -(data.getXPixelResidualLocalUnconstrained(planeID) + data.getXPixelPitchLocalUnconstrained(planeID)/2);

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted(planeID)         ;
    int           col       = data.getColPredicted(planeID)         ;

    if( !theWindow->checkWindow(col,row) )
        return;

    int size = data.getClusterSize(planeID);
    if( data.getHasHit(planeID) && size == 2 )
    {
        for(int h=0; h<size; ++h)
        {
            if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID))  //hits are in the window
                   || !data.getIsPixelCalibrated(h,planeID)                                                           //pixels are calibrated
                   ||  data.getClusterPixelRow    (h,planeID) != row                                                  //hits are on the same row (sharing is along the row - x direction)
                   ||  data.getClusterPixelCharge (h,planeID) < threashold_                                           //charge is over threshold
                   ||  data.getClusterPixelCharge (h,planeID) > maxCharge_   )                                        //maximum allowed charge for this physics
                return;
        }

        int  hitID = -1;
        for(int h=0; h<size; ++h)
        {
            if( data.getClusterPixelRow(h,planeID) == row && data.getClusterPixelCol(h,planeID) == col )
            {
                hitID = h;
                break;
            }
        }

        if( hitID == -1 )
            return;

        float Asimmetry   = 0;
        int   totalCharge = 0;
        int   chargeLeft  = 0;
        int   chargeRight = 0;
        for(int h=0; h<size; ++h)
        {
            if( h == hitID )
                continue;
            else if(data.getXPixelResidualLocalUnconstrained(planeID) > 0 && (col - data.getClusterPixelCol(h,planeID)) == 1)//il secondo hit e' a SN della predetta
            {
                chargeLeft  = data.getClusterPixelCharge(h    ,planeID);
                chargeRight = data.getClusterPixelCharge(hitID,planeID);
                break;
            }
            else if(data.getXPixelResidualLocalUnconstrained(planeID) <= 0 && (col - data.getClusterPixelCol(h,planeID)) == -1)//il secondo hit e' a DX della predetta
            {
                chargeLeft  = data.getClusterPixelCharge(hitID,planeID);
                chargeRight = data.getClusterPixelCharge(h    ,planeID);
                break;
            }
            else if(data.getXPixelResidualLocalUnconstrained(planeID) > 0 && (col - data.getClusterPixelCol(h,planeID)) == -1)
            {
                chargeLeft        = data.getClusterPixelCharge(hitID,planeID);
                chargeRight       = data.getClusterPixelCharge(h    ,planeID);
                xResUnconstrained = - data.getXPixelResidualLocalUnconstrained(planeID) - data.getXPixelPitchLocalUnconstrained(planeID)/2;
                break;
            }
            else if(data.getXPixelResidualLocalUnconstrained(planeID) < 0 && (col - data.getClusterPixelCol(h,planeID)) ==  1)
            {
                chargeLeft        = data.getClusterPixelCharge(h    ,planeID);
                chargeRight       = data.getClusterPixelCharge(hitID,planeID);
                xResUnconstrained = -(data.getXPixelResidualLocalUnconstrained(planeID) - data.getXPixelPitchLocalUnconstrained(planeID)/2);
                break;
            }
            else
                return;
        }

        totalCharge = chargeLeft + chargeRight;
        Asimmetry = (float)(chargeLeft - chargeRight)/totalCharge;

        if( totalCharge >= minTotCharge_ && totalCharge <= maxTotCharge_ )
        {
            THREADED(h2DXcellChargeAsimmetryUnconstrained_   [planeID])->Fill(xResUnconstrained, Asimmetry);
            THREADED(h2DXcellChargeAsimmetryUnconstrainedInv_[planeID])->Fill(Asimmetry, xResUnconstrained);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::YchargeDivision(bool pass, int planeID, const Data& data, int threadNumber)
{
    if( !pass || !data.getIsInDetector(planeID))
        return;

    float maxPitchX = 150;
    float maxPitchY = 100;

    if( data.getXPitchLocal(planeID) > maxPitchX || data.getYPitchLocal(planeID) > maxPitchY )
        return;

    float xRes = 0;
    float yRes = 0;

    if(data.getXPixelResidualLocal(planeID) > 0)
        xRes = data.getXPitchLocal(planeID)/2 - data.getXPixelResidualLocal(planeID);
    else if(data.getXPixelResidualLocal(planeID) <= 0)
        xRes = -(data.getXPixelResidualLocal(planeID) + data.getXPitchLocal(planeID)/2);

    if(data.getYPixelResidualLocal(planeID) > 0)
        yRes = data.getYPitchLocal(planeID)/2 - data.getYPixelResidualLocal(planeID);
    else if(data.getYPixelResidualLocal(planeID) <= 0)
        yRes = -(data.getYPixelResidualLocal(planeID) + data.getYPitchLocal(planeID)/2);

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted(planeID)         ;
    int           col       = data.getColPredicted(planeID)         ;

    if( !theWindow->checkWindow(col,row) )
        return;

    THREADED(h1DYallTracks_[planeID])->Fill(yRes);

    int size = data.getClusterSize(planeID);
    if( data.getHasHit(planeID) && data.getClusterSize(planeID) <= 3 )
    {
        for(int h=0; h<size; ++h)
        {
            if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID)) // hits are in the window
                   || !data.getIsPixelCalibrated(h,planeID)                                                       //pixels are calibrated
                   ||  data.getClusterPixelCol    (h,planeID) !=  col                                             //hits are on the same column (sharing is along the column - y direction)
                   ||  data.getClusterPixelCharge (h,planeID) <  threashold_                                      //charge is over threshold
                   ||  data.getClusterPixelCharge (h,planeID) > maxCharge_    )                                   //maximum allowed charge for this physics
                return;
        }

        bool hitID = -1;
        for(int h=0; h<size; ++h)
        {
            if(data.getClusterPixelRow(h,planeID) == row && data.getClusterPixelCol(h,planeID) == col)
            {
                THREADED(h1DYcellCharge_          [planeID])->Fill(yRes,data.getClusterPixelCharge(h,planeID));
                THREADED(h1DYcellChargeNormToAll_ [planeID])->Fill(yRes,data.getClusterPixelCharge(h,planeID));
                THREADED(h2DYcellCharge_          [planeID])->Fill(yRes,data.getClusterPixelCharge(h,planeID));
                THREADED(h1DYcellChargeNorm_[planeID])->Fill(yRes);
                hitID = h;
                break;
            }
        }

        if( hitID == -1 )
            return;

        if( size == 1 )
        {
            THREADED(h1DYcellSingleHits_ [planeID])->Fill(yRes);
            THREADED(h2DYcellSingleHits_ [planeID])->Fill(xRes,yRes);
        }
        else if( size == 2 )
        {
            for(int h=0; h<size; ++h)
            {
                if( h == hitID )
                    continue;
                else if(    (data.getYPixelResidualLocal(planeID) < 0 && (row - data.getClusterPixelRow(h,planeID)) ==  1)
                            || (data.getYPixelResidualLocal(planeID) > 0 && (row - data.getClusterPixelRow(h,planeID)) == -1)  )
                    return;

            }

            THREADED(h1DYcellDoubleHits_[planeID])->Fill(yRes);
            THREADED(h2DYcellDoubleHits_[planeID])->Fill(xRes,yRes);
        }

        if( data.getClusterCharge(planeID) <= maxTotCharge_ )
        {
            if( size <= 2 )
            {

                THREADED(h1DYcellChargeSumLE2_         [planeID])->Fill(yRes,data.getClusterCharge(planeID));
                THREADED(h1DYcellChargeSumLE2NormToAll_[planeID])->Fill(yRes,data.getClusterCharge(planeID));
                THREADED(h1DYcellChargeSumLE2Norm_     [planeID])->Fill(yRes);
                THREADED(h2DYcellChargeSumLE2_         [planeID])->Fill(yRes,data.getClusterCharge(planeID));
            }
            if ( size <= 3 )
            {
                THREADED(h1DYcellChargeSumLE3_         [planeID])->Fill(yRes,data.getClusterCharge(planeID));
                THREADED(h1DYcellChargeSumLE3NormToAll_[planeID])->Fill(yRes,data.getClusterCharge(planeID));
                THREADED(h1DYcellChargeSumLE3Norm_     [planeID])->Fill(yRes);
                THREADED(h2DYcellChargeSumLE3_         [planeID])->Fill(yRes,data.getClusterCharge(planeID));
            }
            if( size == 3 )
                THREADED(h1DYcell3Hits_                [planeID])->Fill(yRes);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::Yasimmetry(bool pass, int planeID, const Data& data, int threadNumber)
{
    if( !pass || !data.getIsInDetector(planeID))
        return;

    float maxPitchY = 100;

    if( data.getYPitchLocal(planeID) > maxPitchY )
        return;

    float yRes = 0;

    if(data.getYPixelResidualLocal(planeID) > 0)
        yRes = data.getYPitchLocal(planeID)/2 - data.getYPixelResidualLocal(planeID);
    else if(data.getYPixelResidualLocal(planeID) <= 0)
        yRes = -(data.getYPixelResidualLocal(planeID) + data.getYPitchLocal(planeID)/2);

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted(planeID)         ;
    int           col       = data.getColPredicted(planeID)         ;

    if( !theWindow->checkWindow(col,row) )
        return;

    int size = data.getClusterSize(planeID);
    if (size > 4) return;

    int hitID = -1;
    for(int h=0; h<size; ++h)
    {
        if( data.getClusterPixelRow(h,planeID) == row && data.getClusterPixelCol(h,planeID) == col )
        {
            hitID = h;
            break;
        }
    }

    if( hitID == -1 )
        return;

/*    if (data.getHasHit(planeID) && size == 1)
    {
        for(int h=0; h<size; ++h)
        {
            if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID))  //hits are in the window
                   || !data.getIsPixelCalibrated(h,planeID)                                                        //pixels are calibrated
                   ||  data.getClusterPixelRow    (h,planeID) != row                                               //hits are on the same row (sharing is along the row - x direction)
                   ||  data.getClusterPixelCharge (h,planeID) <  threashold_                                       //charge is over threshold
                   ||  data.getClusterPixelCharge (h,planeID) >  maxCharge_   )                                    //maximum allowed charge for this physics
                return;
        }

        if (fabs(data.getXPixelResidualLocal(planeID)) < fabs(data.getYPixelResidualLocal(planeID)))
        {
            if (yRes >  0 && data.getClusterCharge(planeID) >= minTotCharge_ && data.getClusterCharge(planeID) <= maxTotCharge_) THREADED(h2DYCellChargeAsimmetrySizeLE2_[planeID])->Fill(yRes, -1);
            if (yRes <= 0 && data.getClusterCharge(planeID) >= minTotCharge_ && data.getClusterCharge(planeID) <= maxTotCharge_) THREADED(h2DYCellChargeAsimmetrySizeLE2_[planeID])->Fill(yRes,  1);
        }
    }
*/
    for(int h=0; h<size; ++h)
    {
        if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID))  // hits are in the window
               || !data.getIsPixelCalibrated(h,planeID)                                                        //pixels are calibrated
               ||  data.getClusterPixelCol    (h,planeID) != col                                               //hits are on the same column (sharing is along the column - y direction)
               ||  data.getClusterPixelCharge (h,planeID) <  threashold_                                       //charge is over threshold
               ||  data.getClusterPixelCharge (h,planeID) >  maxCharge_    )                                   //maximum allowed charge for this physics
            return;
    }

    if( data.getHasHit(planeID) && size == 2 )
    {
        float Asimmetry   = 0;
        int   totalCharge = 0;
        int   chargeLeft  = 0;
        int   chargeRight = 0;
        for(int h=0; h<size; ++h)
        {
            if( h == hitID )
                continue;
            else if(data.getYPixelResidualLocal(planeID) > 0 && (row - data.getClusterPixelRow(h,planeID)) == 1)//il secondo hit e' a SN della predetta
            {
                chargeLeft  = data.getClusterPixelCharge(h    ,planeID);
                chargeRight = data.getClusterPixelCharge(hitID,planeID);
                break;
            }
            else if(data.getYPixelResidualLocal(planeID) <= 0 && (row - data.getClusterPixelRow(h,planeID)) == -1)//il secondo hit e' a DX della predetta
            {
                chargeLeft  = data.getClusterPixelCharge(hitID,planeID);
                chargeRight = data.getClusterPixelCharge(h    ,planeID);
                break;
            }
            else if(data.getYPixelResidualLocal(planeID) > 0 && (row - data.getClusterPixelRow(h,planeID)) == -1)
            {
                chargeLeft  = data.getClusterPixelCharge(hitID,planeID);
                chargeRight = data.getClusterPixelCharge(h    ,planeID);
                yRes = - data.getYPixelResidualLocal(planeID) - data.getYPitchLocal(planeID)/2;
                break;
            }
            else if(data.getYPixelResidualLocal(planeID) < 0 && (row - data.getClusterPixelRow(h,planeID)) ==  1)
            {
                chargeLeft  = data.getClusterPixelCharge(h    ,planeID);
                chargeRight = data.getClusterPixelCharge(hitID,planeID);
                yRes = -(data.getYPixelResidualLocal(planeID) - data.getYPitchLocal(planeID)/2);
                break;
            }
            else
                return;
        }

        totalCharge = chargeLeft + chargeRight;
        Asimmetry = (float)(chargeLeft - chargeRight)/totalCharge;
        float Asimmetry0 = (float)(chargeLeft/totalCharge);

        if( totalCharge >= minTotCharge_ && totalCharge <= maxTotCharge_ )
        {
            THREADED(h2DYAsimmetryLandau_                    [planeID])->Fill(totalCharge,Asimmetry);
            THREADED(hYasimmetry_                            [planeID])->Fill(Asimmetry);
            THREADED(hYasimmetry0_                           [planeID])->Fill(Asimmetry0);
            THREADED(h2DYcellChargeAsimmetry_                [planeID])->Fill(yRes, Asimmetry);
            THREADED(h2DYCellChargeAsimmetrySizeLE2_         [planeID])->Fill(yRes, Asimmetry);
            THREADED(h2DYcellChargeAsimmetryInv_             [planeID])->Fill(Asimmetry, yRes);
        }
    }
        if(data.getHasHit(planeID) && size == 1)
        {
            float Asimmetry   = 0;
            int   totalCharge = 0;

            if(data.getClusterPixelRow(0,planeID) == row)//la cella colpita coincide con la predetta
            {
                if(data.getYPixelResidualLocal(planeID) > 0)//la traccia ha colpito a sn
                    Asimmetry = -1;
                else if(data.getYPixelResidualLocal(planeID) <= 0)//la traccia ha colpito a dx
                    Asimmetry =  1;
            }
            else if(data.getYPixelResidualLocal(planeID) >  0 && (row - data.getClusterPixelRow(0,planeID)) ==  1)//la cella colpita e' a SN della predetta
            {
                Asimmetry = -1;
                yRes = - data.getYPixelResidualLocal(planeID) - data.getYPitchLocal(planeID)/2;
            }
            else if(data.getYPixelResidualLocal(planeID) <= 0 && (row - data.getClusterPixelRow(0,planeID)) == -1)//la cella colpita e' a DX della predetta
            {
                Asimmetry =  1;
                yRes = -(data.getYPixelResidualLocal(planeID) - data.getYPitchLocal(planeID)/2);
            }
            else
                return;

            totalCharge = data.getClusterPixelCharge(0,planeID);

            THREADED(h2DYCellChargeAsimmetrySizeLE2_[planeID])->Fill(yRes, Asimmetry);
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::YasimmetryUnconstr(bool pass, int planeID, const Data& data, int threadNumber)
{
    if( !pass || !data.getIsInDetector(planeID))
        return;

    float maxPitchY = 100;

    if( data.getYPixelPitchLocalUnconstrained(planeID) > maxPitchY )
        return;

    float yResUnconstrained  = 0;

    if(data.getYPixelResidualLocalUnconstrained(planeID) > 0)
        yResUnconstrained = data.getYPixelPitchLocalUnconstrained(planeID)/2 - data.getYPixelResidualLocalUnconstrained(planeID);
    else if(data.getYPixelResidualLocalUnconstrained(planeID) <= 0)
        yResUnconstrained = -(data.getYPixelResidualLocalUnconstrained(planeID) + data.getYPixelPitchLocalUnconstrained(planeID)/2);

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted(planeID)         ;
    int           col       = data.getColPredicted(planeID)         ;

    if( !theWindow->checkWindow(col,row) )
        return;

    int size = data.getClusterSize(planeID);
    if( data.getHasHit(planeID) && size == 2 )
    {
        for(int h=0; h<size; ++h)
        {
            if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID))  // hits are in the window
                   || !data.getIsPixelCalibrated(h,planeID)                                                        //pixels are calibrated
                   ||  data.getClusterPixelCol    (h,planeID) != col                                               //hits are on the same column (sharing is along the column - y direction)
                   ||  data.getClusterPixelCharge (h,planeID) <  threashold_                                       //charge is over threshold
                   ||  data.getClusterPixelCharge (h,planeID) >  maxCharge_    )                                   //maximum allowed charge for this physics
                return;
        }

        int hitID = -1;
        for(int h=0; h<size; ++h)
        {
            if( data.getClusterPixelRow(h,planeID) == row && data.getClusterPixelCol(h,planeID) == col )
            {
                hitID = h;
                break;
            }
        }

        if( hitID == -1 )
            return;

        float Asimmetry   = 0;
        int   totalCharge = 0;
        int   chargeLeft  = 0;
        int   chargeRight = 0;
        for(int h=0; h<size; ++h)
        {
            if( h == hitID )
                continue;
            else if(data.getYPixelResidualLocalUnconstrained(planeID) > 0 && (row - data.getClusterPixelRow(h,planeID)) == 1)//il secondo hit e' a SN della predetta
            {
                chargeLeft  = data.getClusterPixelCharge(h    ,planeID);
                chargeRight = data.getClusterPixelCharge(hitID,planeID);
                break;
            }
            else if(data.getYPixelResidualLocalUnconstrained(planeID) <= 0 && (row - data.getClusterPixelRow(h,planeID)) == -1)//il secondo hit e' a DX della predetta
            {
                chargeLeft  = data.getClusterPixelCharge(hitID,planeID);
                chargeRight = data.getClusterPixelCharge(h    ,planeID);
                break;
            }
            else if(data.getYPixelResidualLocalUnconstrained(planeID) > 0 && (row - data.getClusterPixelRow(h,planeID)) == -1)
            {
                chargeLeft        = data.getClusterPixelCharge(hitID,planeID);
                chargeRight       = data.getClusterPixelCharge(h    ,planeID);
                yResUnconstrained = - data.getYPixelResidualLocalUnconstrained(planeID) - data.getYPixelPitchLocalUnconstrained(planeID)/2;
                break;
            }
            else if(data.getYPixelResidualLocalUnconstrained(planeID) < 0 && (row - data.getClusterPixelRow(h,planeID)) ==  1)
            {
                chargeLeft        = data.getClusterPixelCharge(h    ,planeID);
                chargeRight       = data.getClusterPixelCharge(hitID,planeID);
                yResUnconstrained = -(data.getYPixelResidualLocalUnconstrained(planeID) - data.getYPixelPitchLocalUnconstrained(planeID)/2);
                break;
            }
            else
                return;
        }

        totalCharge = chargeLeft + chargeRight;
        Asimmetry = (float)(chargeLeft - chargeRight)/totalCharge;

        if( totalCharge >= minTotCharge_ && totalCharge <= maxTotCharge_ )
        {
            THREADED(h2DYcellChargeAsimmetryUnconstrained_   [planeID])->Fill(yResUnconstrained, Asimmetry);
            THREADED(h2DYcellChargeAsimmetryUnconstrainedInv_[planeID])->Fill(Asimmetry, yResUnconstrained);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::setCutsFormula(std::map<std::string,std::string> cutsList,std::vector<TTree*> tree)
{
    std::vector<TTreeFormula*> formulasVector;

    for(std::map<std::string,std::string>::iterator it=cutsList.begin(); it!=cutsList.end(); it++)
    {
        if((it->first) == "main cut" && (it->second).size()==0)
            STDLINE("WARNING: no main cut set in charge analysis!! Default value = true!", ACRed);

        formulasVector.clear();
        if((it->second).size()!=0)
        {
            for(unsigned int t=0; t<tree.size(); t++)
            {
                formulasVector.push_back(new TTreeFormula((it->second).c_str(),(it->second).c_str(),tree[t]));
            }
            cutsFormulas_[it->first] = formulasVector;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::setParsLimits(void)
{
    std::stringstream ss;

    XmlParser* theParser = theAnalysisManager_->getXmlParser();

    for(unsigned int pl=0; pl<thePlaneMapping_->getNumberOfPlanes(); pl++)
    {
        if(thePlaneMapping_->getPlaneName(pl).find("Dut") != std::string::npos
                && (theParser->getPlanes())[thePlaneMapping_->getPlaneName(pl)]->useCalibrations())
        {
            for(int p=0; p<4; p++)
            {
                parMin_      [p] = theParser->getAnalysesFromString("Charge")->getParLimits(p,true).first;
                parMax_      [p] = theParser->getAnalysesFromString("Charge")->getParLimits(p,true).second;
                isMinToLimit_[p] = theParser->getAnalysesFromString("Charge")->isParToLimit(p).first;
                isMaxToLimit_[p] = theParser->getAnalysesFromString("Charge")->isParToLimit(p).second;
                h2DparsPlots_[p] = theCalibrationsManager_->getParHisto(p);
                ss.str("");
                ss << "Par" << p << "\t min: " << parMin_[p] << " - limit: " << (int)isMinToLimit_[p];
                ss << " - max: " << parMax_[p] << " - limit: " << (int)isMaxToLimit_[p];
                STDLINE(ss.str(),ACWhite);
            }
            break;
        }
        else
            continue;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Charge::passCalibrationsCut(int planeID, const Data &data)
{
    XmlParser* theParser = theAnalysisManager_->getXmlParser();

    if(!(theParser->getPlanes())[thePlaneMapping_->getPlaneName(planeID)]->useCalibrations())
        return true;

    //std::stringstream ss;

    //isMinToLimit = true  - isMaxToLimit = true  : min <= parValue <= max
    //isMinToLimit = false - isMaxToLimit = true  : parValue <= max
    //isMinToLimit = true  - isMaxToLimit = false : parValue >= min
    //isMinToLimit = false - isMaxToLimit = false : nothing!

    int    row;
    int    col;
    bool   pass = false;
    double parValue;
    if (data.getClusterSize(planeID) > 4) return false;

    for(int h=0; h<data.getClusterSize(planeID); h++)
    {
        row = data.getClusterPixelRow(h,planeID);
        col = data.getClusterPixelCol(h,planeID);
        for(int p=0; p<4; p++)
        {
            parValue = h2DparsPlots_[p]->GetBinContent(h2DparsPlots_[p]->GetXaxis()->FindBin(col),h2DparsPlots_[p]->GetYaxis()->FindBin(row));
            if(isMinToLimit_[p]==false && isMaxToLimit_[p]==false)
                continue;
            else if(isMinToLimit_[p]==true && isMaxToLimit_[p]== true)
            {
                if(parValue>parMin_[p] && parValue<parMax_[p])
                    pass = true;
            }
            else if(isMinToLimit_[p]==true && isMaxToLimit_[p]==false)
            {
                if(parValue>parMin_[p])
                    pass = true;
            }
            else if(isMinToLimit_[p]==false && isMaxToLimit_[p]==true)
            {
                if(parValue<parMax_[p])
                    pass = true;
            }
        }
    }

    return pass;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::MeanChargePositionRN (bool pass, int planeID, const Data &data, int threadNumber)
{
    if (!pass || !data.getIsInDetector(planeID))
       return;

    float maxPitchX = 150;
    float maxPitchY = 100;

    if( data.getXPitchLocal(planeID) > maxPitchX || data.getYPitchLocal(planeID) > maxPitchY )
        return;

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted(planeID)         ;
    int           col       = data.getColPredicted(planeID)         ;

    if( !theWindow->checkWindow(col,row) )
        return;

    int size = data.getClusterSize(planeID);

    if( !data.getHasHit(planeID) || size > 4 )
        return;

    for(int h=0; h<size; h++)
    {
        if(    data.getClusterPixelRow   (h,planeID) == row
               && data.getClusterPixelCol   (h,planeID) == col
               && data.getIsPixelCalibrated (h,planeID)
               && data.getClusterPixelCharge(h,planeID) > threashold_
               && data.getClusterPixelCharge(h,planeID) < maxCharge_   )
        {
            THREADED(mXYNorm_      [planeID][data.getRunNumber()])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID));
            THREADED(mXYMeanCharge_[planeID][data.getRunNumber()])->Fill(data.getXPixelResidualLocal(planeID),data.getYPixelResidualLocal(planeID),data.getClusterPixelCharge(h,planeID));
//            if (planeID == 9) ++totEventsControl_;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::NormalizeEtaDistributionSize2 (int p)
{
    std::stringstream hName;
    std::string planeName = thePlaneMapping_->getPlaneName(p);

    theAnalysisManager_->cd("/Charge/" + planeName + "/XcellCharge1D");

    hName.str(""); hName << "projX_" << planeName;
    projX_.push_back((TH1F*)h2DCellChargeNormSize2_[p]->ProjectionX(hName.str().c_str(), -75, 75));

    hName.str(""); hName << "projXSize1_" << planeName;
    projXSize1_.push_back((TH1F*)h2DCellChargeNormSize1_[p]->ProjectionX(hName.str().c_str(), -75, 75));

    hName.str(""); hName << "projXUp_" << planeName;
    projXUp_.push_back((TH1F*)h2DCellChargeNormSize2Up_[p]->ProjectionX(hName.str().c_str(), -75, 75));

    hName.str(""); hName << "projXSize1Up_" << planeName;
    projXSize1Up_.push_back((TH1F*)h2DCellChargeNormSize1Up_[p]->ProjectionX(hName.str().c_str(), -75, 75));

    hName.str(""); hName << "projXDown_" << planeName;
    projXDown_.push_back((TH1F*)h2DCellChargeNormSize2Down_[p]->ProjectionX(hName.str().c_str(), -75, 75));

    hName.str(""); hName << "projXSize1Down_" << planeName;
    projXSize1Down_.push_back((TH1F*)h2DCellChargeNormSize1Down_[p]->ProjectionX(hName.str().c_str(), -75, 75));

    theAnalysisManager_->cd("/Charge/" + planeName + "/Xasimmetry");

    hName.str(""); hName << "h1DXEtaDistribution_" << planeName;
    h1DXEtaDistribution_.push_back((TH1F*)h2DXCellChargeAsimmetrySizeLE2_[p]->ProjectionY(hName.str().c_str(), -150, 150));

    theAnalysisManager_->cd("/Charge/" + planeName + "/YcellCharge1D");

    hName.str(""); hName << "projY_" << planeName;
    projY_.push_back((TH1F*)h2DCellChargeNormSize2_[p]->ProjectionY(hName.str().c_str(), -50, 50));

    hName.str(""); hName << "projYSize1_" << planeName;
    projYSize1_.push_back((TH1F*)h2DCellChargeNormSize1_[p]->ProjectionY(hName.str().c_str(), -50, 50));

    hName.str(""); hName << "projYUp_" << planeName;
    projYUp_.push_back((TH1F*)h2DCellChargeNormSize2Up_[p]->ProjectionY(hName.str().c_str(), -50, 50));

    hName.str(""); hName << "projYSize1Up_" << planeName;
    projYSize1Up_.push_back((TH1F*)h2DCellChargeNormSize1Up_[p]->ProjectionY(hName.str().c_str(), -50, 50));

    hName.str(""); hName << "projYDown_" << planeName;
    projYDown_.push_back((TH1F*)h2DCellChargeNormSize2Down_[p]->ProjectionY(hName.str().c_str(), -50, 50));

    hName.str(""); hName << "projYSize1Down_" << planeName;
    projYSize1Down_.push_back((TH1F*)h2DCellChargeNormSize1Down_[p]->ProjectionY(hName.str().c_str(), -50, 50));

    theAnalysisManager_->cd("/Charge/" + planeName + "/Yasimmetry");

    hName.str(""); hName << "h1DYEtaDistribution_" << planeName;
    h1DYEtaDistribution_.push_back((TH1F*)h2DYCellChargeAsimmetrySizeLE2_[p]->ProjectionY(hName.str().c_str(),-75,75));

    for (int j = 1; j < h1DXEtaDistribution_[p]->GetXaxis()->GetNbins()+1; ++j)
    {
            h1DXEtaDistribution_[p]->SetBinContent(j, (double)h1DXEtaDistribution_[p]->GetBinContent(j)/((double)projX_[p]->GetBinContent(j)));//projX_[p]->FindBin(h2DXCellChargeAsimmetrySizeLE2_[p]->GetXaxis()->GetBinCenter(i)))));
    }
    h1DXEtaDistribution_[p]->Scale(projX_[p]->GetEntries()/(double)projX_[p]->GetXaxis()->GetNbins());

    for (int j = 1; j < h1DYEtaDistribution_[p]->GetXaxis()->GetNbins()+1; ++j)
    {
            h1DYEtaDistribution_[p]->SetBinContent(j, (double)h1DYEtaDistribution_[p]->GetBinContent(j)/((double)projY_[p]->GetBinContent(j)));//projX_[p]->FindBin(h2DXCellChargeAsimmetrySizeLE2_[p]->GetXaxis()->GetBinCenter(i)))));
    }
    h1DYEtaDistribution_[p]->Scale(projY_[p]->GetEntries()/(double)projY_[p]->GetXaxis()->GetNbins());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::NormalizeEtaDistribution (int p)
{
//    std::stringstream hName;
//    std::string planeName = thePlaneMapping_->getPlaneName(p);

    for (int j = 1; j < h2DXCellChargeAsimmetrySizeLE2_[p]->GetYaxis()->GetNbins()+1; ++j)
    {
        for (int i = 1; i < h2DXCellChargeAsimmetrySizeLE2_[p]->GetXaxis()->GetNbins()+1; ++i)
        {
            if (fabs((double)h2DXCellChargeAsimmetrySizeLE2_[p]->GetYaxis()->GetBinCenter(j) -1.) < 0.1 || fabs((double)h2DXCellChargeAsimmetrySizeLE2_[p]->GetYaxis()->GetBinCenter(j) +1.) < 0.1)
                h2DXCellChargeAsimmetrySizeLE2_[p]->SetBinContent(i, j, (double)h1DXCellChargeAsimmetrySizeLE2_[p]->GetBinContent(i, j)/(double)projXSize1_[p]->GetBinContent(i));
            else
                h2DXCellChargeAsimmetrySizeLE2_[p]->SetBinContent(i, j, (double)h1DXCellChargeAsimmetrySizeLE2_[p]->GetBinContent(i, j)/(double)projX_[p]->GetBinContent(i));
        }
    }

    for (int j = 1; j < h2DYCellChargeAsimmetrySizeLE2_[p]->GetYaxis()->GetNbins()+1; ++j)
    {
        for (int i = 1; i < h2DYCellChargeAsimmetrySizeLE2_[p]->GetXaxis()->GetNbins()+1; ++i)
        {
            if (fabs((double)h2DYCellChargeAsimmetrySizeLE2_[p]->GetYaxis()->GetBinCenter(j) -1.) < 0.1 || fabs((double)h2DYCellChargeAsimmetrySizeLE2_[p]->GetYaxis()->GetBinCenter(j) +1.) < 0.1)
                h2DYCellChargeAsimmetrySizeLE2_[p]->SetBinContent(i, j, (double)h1DYCellChargeAsimmetrySizeLE2_[p]->GetBinContent(i, j)/(double)projYSize1_[p]->GetBinContent(i));
            else
                h2DYCellChargeAsimmetrySizeLE2_[p]->SetBinContent(i, j, (double)h1DYCellChargeAsimmetrySizeLE2_[p]->GetBinContent(i, j)/(double)projY_[p]->GetBinContent(i));
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::NormalizeEtaInverse (int p)
{
    for (int j = 1; j < h2DXcellChargeAsimmetryInv_[p]->GetYaxis()->GetNbins()+1; ++j)
    {
        for (int i = 1; i < h2DXcellChargeAsimmetryInv_[p]->GetXaxis()->GetNbins()+1; ++i)
        {
                h2DXcellChargeAsimmetryInv_[p]->SetBinContent(i, j, (double)h2DXcellChargeAsimmetryInv_[p]->GetBinContent(i, j)/(double)projX_[p]->GetBinContent(j));
        }
    }

    h2DXcellChargeAsimmetryInv_[p]->Scale((double)projX_[p]->GetEntries()/(double)projX_[p]->GetXaxis()->GetNbins());

    for (int j = 1; j < h2DYcellChargeAsimmetryInv_[p]->GetYaxis()->GetNbins()+1; ++j)
    {
        for (int i = 1; i < h2DYcellChargeAsimmetryInv_[p]->GetXaxis()->GetNbins()+1; ++i)
        {
                h2DYcellChargeAsimmetryInv_[p]->SetBinContent(i, j, (double)h2DYcellChargeAsimmetryInv_[p]->GetBinContent(i, j)/(double)projY_[p]->GetBinContent(j));
        }
    }

    h2DYcellChargeAsimmetryInv_[p]->Scale((double)projY_[p]->GetEntries()/(double)projY_[p]->GetXaxis()->GetNbins());

    for (int j = 1; j < h2DXcellChargeAsimmetry_[p]->GetYaxis()->GetNbins()+1; ++j)
    {
        for (int i = 1; i < h2DXcellChargeAsimmetry_[p]->GetXaxis()->GetNbins()+1; ++i)
        {
                h2DXcellChargeAsimmetry_[p]->SetBinContent(i, j, (double)h2DXcellChargeAsimmetry_[p]->GetBinContent(i, j)/(double)projX_[p]->GetBinContent(i));
        }
    }

    h2DXcellChargeAsimmetry_[p]->Scale((double)projX_[p]->GetEntries()/(double)projX_[p]->GetXaxis()->GetNbins());

    for (int j = 1; j < h2DYcellChargeAsimmetry_[p]->GetYaxis()->GetNbins()+1; ++j)
    {
        for (int i = 1; i < h2DYcellChargeAsimmetry_[p]->GetXaxis()->GetNbins()+1; ++i)
        {
                h2DYcellChargeAsimmetry_[p]->SetBinContent(i, j, (double)h2DYcellChargeAsimmetry_[p]->GetBinContent(i, j)/(double)projY_[p]->GetBinContent(i));
        }
    }

    h2DYcellChargeAsimmetry_[p]->Scale((double)projY_[p]->GetEntries()/(double)projY_[p]->GetXaxis()->GetNbins());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::CalculateEtaDerivative (int p)
{
    for (int l = 1; l < h1DXEtaDerivativeDistribution_[p]->GetXaxis()->GetNbins(); ++l)
    {
        h1DXEtaDerivativeDistribution_[p]->SetBinContent(l, double(-h1DXCellChargeAsimmetrySizeLE2_[p]->GetBinContent(l+1) + h1DXCellChargeAsimmetrySizeLE2_[p]->GetBinContent(l))/h1DXCellChargeAsimmetrySizeLE2_[p]->GetBinWidth(l));
    }
    for (int l = 1; l < h1DYEtaDerivativeDistribution_[p]->GetXaxis()->GetNbins(); ++l)
    {
        h1DYEtaDerivativeDistribution_[p]->SetBinContent(l, double(-h1DYCellChargeAsimmetrySizeLE2_[p]->GetBinContent(l+1) + h1DYCellChargeAsimmetrySizeLE2_[p]->GetBinContent(l))/h1DYCellChargeAsimmetrySizeLE2_[p]->GetBinWidth(l));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::calculateMeanCharge ()
{
    for (unsigned int p = 0; p < thePlaneMapping_->getNumberOfPlanes(); ++p)
    {
        for (std::map<int, TH2F*>::iterator it = mXYMeanCharge_[p].begin(); it != mXYMeanCharge_[p].end(); ++it)
        {
            it->second->Divide(mXYNorm_[p][it->first]);

            hXMeanCharge_[p]->Fill(it->first, it->second->GetMean(1));
            hYMeanCharge_[p]->Fill(it->first, it->second->GetMean(2));
        }

        hXMeanCharge_[p]->GetXaxis()->SetTitle("Run number");
        hXMeanCharge_[p]->GetYaxis()->SetTitle("Mean x");
        hYMeanCharge_[p]->GetXaxis()->SetTitle("Run number");
        hYMeanCharge_[p]->GetYaxis()->SetTitle("Mean y");
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Charge::passStandardCuts(int planeID, const Data &data)
{
    return  true;
    XmlParser* theParser = theAnalysisManager_->getXmlParser();
    if(theParser->getAnalysesFromString("Charge")->applyStandardCuts())
        return true;

    int minHits = 7;//To calculate efficiency on the telescope
    int excludeMe = 0;
    if(thePlaneMapping_->getPlaneName(planeID).find("Dut") != std::string::npos) //Dut case
        minHits = atoi(theParser->getAnalysesFromString("Charge")->getMinHits().c_str());
    else if(data.getHasHit(planeID))//Telescope case
    {
        if(data.getClusterSize(planeID) == 1)
            excludeMe = 1;
        else if(data.getClusterSize(planeID) == 2
                && (data.getClusterPixelRow(0,planeID) == data.getClusterPixelRow(1,planeID)
                    || data.getClusterPixelCol(0,planeID) == data.getClusterPixelCol(1,planeID)))
            excludeMe = 1;
    }

    if(data.getNumberOfTelescopeHits()-excludeMe >= minHits) {
        return true;
    }
    else
        return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::beginJob(void)
{
    XmlParser* theParser = theAnalysisManager_->getXmlParser();
    threashold_   = theParser->getAnalysesFromString("Charge")->getThreashold();
    maxCharge_    = theParser->getAnalysesFromString("Charge")->getMaxCharge();
    minTotCharge_ = theParser->getAnalysesFromString("Charge")->getMinTotCharge();
    maxTotCharge_ = theParser->getAnalysesFromString("Charge")->getMaxTotCharge();
    theWindowsManager_      = theAnalysisManager_->getWindowsManager();
    theCalibrationsManager_ = theAnalysisManager_->getCalibrationsManager();

    langaus_ = new TF1("langaus",Utilities::langaus,0,60000,4);
    langaus_->SetParNames("Width","MPV","Area","GSigma");
    langaus_->SetLineColor(kBlue);

    book();

    setParsLimits();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::analyze(const Data& data, int threadNumber)//WARNING: You can't change this name (threadNumber) or the MACRO THREAD won't compile
{
    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
        clusterSize(1,p,data,threadNumber);

//    STDLINE("STAMPA 1", ACYellow);

    if(cutsFormulas_.find("main cut") != cutsFormulas_.end() && !cutsFormulas_["main cut"][threadNumber]->EvalInstance())
        return;

    bool clusterLandauCut = true;
    if(cutsFormulas_.find("cluster Landau") != cutsFormulas_.end())
        clusterLandauCut = cutsFormulas_["cluster Landau"][threadNumber]->EvalInstance();

    bool cellLandauCut = true;
    if(cutsFormulas_.find("cell Landau") != cutsFormulas_.end())
        cellLandauCut = cutsFormulas_["cell Landau"][threadNumber]->EvalInstance();

    bool cellChargeCut = true;
    if(cutsFormulas_.find("cell charge") != cutsFormulas_.end())
        cellChargeCut = cutsFormulas_["cell charge"][threadNumber]->EvalInstance();

    bool cellChargeXCut = true;
    if(cutsFormulas_.find("cell charge X") != cutsFormulas_.end())
        cellChargeXCut = cutsFormulas_["cell charge X"][threadNumber]->EvalInstance();

    bool cellChargeYCut = true;
    if(cutsFormulas_.find("cell charge Y") != cutsFormulas_.end())
        cellChargeYCut = cutsFormulas_["cell charge Y"][threadNumber]->EvalInstance();

    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {
        if(!passStandardCuts(p,data))
            continue;

        if(thePlaneMapping_->getPlaneName(p).find("Dut") != std::string::npos)
        {
            if(!passCalibrationsCut(p,data))
                return;
        }
//        STDLINE("STAMPA 2", ACYellow);
//        std::stringstream ss;
//        ss << "Plane: " << p << ", row: " << data.getRowPredicted(p) << ", col: " << data.getColPredicted(p);
//        if (p == 8 || p == 9) STDLINE(ss.str(), ACWhite);

        clusterLandau        (clusterLandauCut,p,data,threadNumber);
//        STDLINE("STAMPA 3", ACYellow);
        cellLandau           (cellLandauCut   ,p,data,threadNumber);
//        STDLINE("STAMPA 4", ACYellow);
        cellCharge           (cellChargeCut   ,p,data,threadNumber);
//        STDLINE("STAMPA 5", ACYellow);
        MeanChargePositionRN (cellChargeCut   ,p,data,threadNumber);
//        STDLINE("STAMPA 6", ACYellow);

        Xlandau           (cellChargeXCut  ,p,data,threadNumber);
//        STDLINE("STAMPA 7", ACYellow);
        XchargeDivision   (cellChargeXCut  ,p,data,threadNumber);
//        STDLINE("STAMPA 8", ACYellow);
        Xasimmetry        (cellChargeXCut  ,p,data,threadNumber);
//        STDLINE("STAMPA 9", ACYellow);
        XasimmetryUnconstr(cellChargeXCut  ,p,data,threadNumber);
//        STDLINE("STAMPA 10", ACYellow);

        Ylandau           (cellChargeYCut  ,p,data,threadNumber);
//        STDLINE("STAMPA 11", ACYellow);
        YchargeDivision   (cellChargeYCut  ,p,data,threadNumber);
//        STDLINE("STAMPA 12", ACYellow);
        Yasimmetry        (cellChargeYCut  ,p,data,threadNumber);
//        STDLINE("STAMPA 13", ACYellow);
        YasimmetryUnconstr(cellChargeYCut  ,p,data,threadNumber);
//        STDLINE("STAMPA 14", ACYellow);

//        STDLINE("STAMPA 15", ACYellow);

    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::endJob(void)
{
    std::stringstream ss;

    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {
        std::string planeName = thePlaneMapping_->getPlaneName(p);

        ADD_THREADED(hClusterSize_                            [p]);
        ADD_THREADED(hClusterSizeCuts_                        [p]);
        ADD_THREADED(hClusterSizeCutsPlus_                    [p]);
        ADD_THREADED(hNumberOfCols_                           [p]);
        ADD_THREADED(hNumberOfRows_                           [p]);
        ADD_THREADED(hClusterSizeDistribution1s_              [p]);
        ADD_THREADED(hClusterSizeDistribution1sOddColumns_    [p]);
        ADD_THREADED(hClusterSizeDistribution1sEvenColumns_   [p]);
        ADD_THREADED(hClusterSizeDistribution1sOROC_          [p]);
        ADD_THREADED(hClusterSizeDistribution1sEROC_          [p]);
        ADD_THREADED(hClusterSizeDistribution1sOREC_          [p]);
        ADD_THREADED(hClusterSizeDistribution1sEREC_          [p]);
        ADD_THREADED(hClusterSizeDistribution1s4Cells_        [p]);
        ADD_THREADED(hClusterSizeDistribution2s_              [p]);
        ADD_THREADED(hClusterSizeDistribution2sOddColumns_    [p]);
        ADD_THREADED(hClusterSizeDistribution2sEvenColumns_   [p]);
        ADD_THREADED(hClusterSizeDistribution2sOROC_          [p]);
        ADD_THREADED(hClusterSizeDistribution2sEROC_          [p]);
        ADD_THREADED(hClusterSizeDistribution2sOREC_          [p]);
        ADD_THREADED(hClusterSizeDistribution2sEREC_          [p]);
        ADD_THREADED(hClusterSizeDistribution2s4Cells_        [p]);


        ADD_THREADED(hLandauClusterSize1_                     [p]);
        ADD_THREADED(hLandauClusterSize2_                     [p]);
        ADD_THREADED(hLandauClusterSize2sameCol_              [p]);
        ADD_THREADED(hLandauClusterSize2sameRow_              [p]);
        ADD_THREADED(hLandauClusterSize3_                     [p]);
        ADD_THREADED(hLandauClusterSize3sameCol_              [p]);
        ADD_THREADED(hLandauClusterSize3sameRow_              [p]);
        ADD_THREADED(hCellLandau_                             [p]);
        ADD_THREADED(hCellLandauSinglePixel_                  [p]);

        ADD_THREADED(h2DCellCharge_                           [p]);

        ADD_THREADED(h2DCellChargeNum_                        [p]);
        ADD_THREADED(h2DallTracks_                            [p]);

        // START Odd/Even Cell
        // Odd row
        ADD_THREADED(h2DEvenTracksOddRows_                    [p]);
        ADD_THREADED(h2DOddTracksOddRows_                     [p]);

        ADD_THREADED(h2DCellChargeEvenColumnsOddRows_         [p]);
        ADD_THREADED(h2DCellChargeOddColumnsOddRows_          [p]);

        // Even row
        ADD_THREADED(h2DEvenTracksEvenRows_                   [p]);
        ADD_THREADED(h2DOddTracksEvenRows_                    [p]);

        ADD_THREADED(h2DCellChargeEvenColumnsEvenRows_        [p]);
        ADD_THREADED(h2DCellChargeOddColumnsEvenRows_         [p]);

        // row independent
        ADD_THREADED(h2DCellChargeOddColumns_                 [p]);
        ADD_THREADED(h2DCellChargeEvenColumns_                [p]);
        // 4 cells
        ADD_THREADED(h4CellChargeFullRange_                   [p]);
        // END Odd/Even Cell

        ADD_THREADED(h2DCellChargeNorm_                       [p]);
        ADD_THREADED(h2DCellChargeNormSize2_                  [p]);
        ADD_THREADED(h2DCellChargeNormSize1_                  [p]);
        ADD_THREADED(h2DCellChargeNormSize2Up_                [p]);
        ADD_THREADED(h2DCellChargeNormSize1Up_                [p]);
        ADD_THREADED(h2DCellChargeNormSize2Down_              [p]);
        ADD_THREADED(h2DCellChargeNormSize1Down_              [p]);
        ADD_THREADED(h4CellsCharge_                           [p]);
        ADD_THREADED(h4CellsAllTracks_                        [p]);
        ADD_THREADED(h4CellsChargeNorm_                       [p]);
        ADD_THREADED(h4Hits_                                  [p]);
        ADD_THREADED(h4HitsCharge_                            [p]);
        ADD_THREADED(hCutsControl_                            [p]);
        ADD_THREADED(hHitsNotOnRowCol_                        [p]);
        ADD_THREADED(hChargeNotOnRowCol_                      [p]);
        ADD_THREADED(hCellChargeCoarse_                       [p]);
        ADD_THREADED(hCellChargeCoarseNorm_                   [p]);
        for(std::vector<TH1F*>::iterator it=hCellChargeCoarseLandau_[p].begin(); it!=hCellChargeCoarseLandau_[p].end(); it++)
            ADD_THREADED(*it);

        ADD_THREADED(h1DXcellCharge_                          [p]);
        ADD_THREADED(h1DXcellChargeNormToAll_                 [p]);
        ADD_THREADED(h1DXcellChargeSumLE2_                    [p]);
        ADD_THREADED(h1DXcellChargeSumLE2NormToAll_           [p]);
        ADD_THREADED(h1DXcellChargeSumLE3_                    [p]);
        ADD_THREADED(h1DXcellChargeSumLE3NormToAll_           [p]);
        ADD_THREADED(h1DXallTracks_                           [p]);
        ADD_THREADED(h1DXcellChargeNorm_                      [p]);
        ADD_THREADED(h1DXcellChargeSumLE2Norm_                [p]);
        ADD_THREADED(h1DXcellChargeSumLE3Norm_                [p]);
        ADD_THREADED(h1DXcellSingleHits_                      [p]);
        ADD_THREADED(h1DXcellDoubleHits_                      [p]);
        ADD_THREADED(h1DXcell3Hits_                           [p]);

        ADD_THREADED(h2DXcellCharge_                          [p]);
        ADD_THREADED(h2DXcellChargeSumLE2_                    [p]);
        ADD_THREADED(h2DXcellChargeSumLE3_                    [p]);
        ADD_THREADED(h2DXcellSingleHits_                      [p]);
        ADD_THREADED(h2DXcellDoubleHits_                      [p]);
        ADD_THREADED(hHitsNotONRowColVsXSlope_                [p]);
        ADD_THREADED(hHitsNotOnRowColProjX_                   [p]);

        ADD_THREADED(h1DYcellCharge_                          [p]);
        ADD_THREADED(h1DYcellChargeNormToAll_                 [p]);
        ADD_THREADED(h1DYcellChargeSumLE2_                    [p]);
        ADD_THREADED(h1DYcellChargeSumLE2NormToAll_           [p]);
        ADD_THREADED(h1DYcellChargeSumLE3_                    [p]);
        ADD_THREADED(h1DYcellChargeSumLE3NormToAll_           [p]);
        ADD_THREADED(h1DYallTracks_                           [p]);
        ADD_THREADED(h1DYcellChargeNorm_                      [p]);
        ADD_THREADED(h1DYcellChargeSumLE2Norm_                [p]);
        ADD_THREADED(h1DYcellChargeSumLE3Norm_                [p]);
        ADD_THREADED(h1DYcellSingleHits_                      [p]);
        ADD_THREADED(h1DYcellDoubleHits_                      [p]);
        ADD_THREADED(h1DYcell3Hits_                           [p]);

        ADD_THREADED(h2DYcellCharge_                          [p]);
        ADD_THREADED(h2DYcellChargeSumLE2_                    [p]);
        ADD_THREADED(h2DYcellChargeSumLE3_                    [p]);
        ADD_THREADED(h2DYcellSingleHits_                      [p]);
        ADD_THREADED(h2DYcellDoubleHits_                      [p]);
        ADD_THREADED(hHitsNotONRowColVsYSlope_                [p]);
        ADD_THREADED(hHitsNotOnRowColProjY_                   [p]);

        ADD_THREADED(hXasimmetry_                             [p]);
        ADD_THREADED(hXasimmetry0_                            [p]);
        ADD_THREADED(h2DXAsimmetryLandau_                     [p]);
        ADD_THREADED(h2DXcellChargeAsimmetry_                 [p]);
        ADD_THREADED(h2DXcellChargeAsimmetryInv_              [p]);
        ADD_THREADED(h2DXcellChargeAsimmetryUnconstrained_    [p]);
        ADD_THREADED(h2DXcellChargeAsimmetryUnconstrainedInv_ [p]);
        ADD_THREADED(h2DXCellChargeAsimmetrySizeLE2_          [p]);

        ADD_THREADED(hYasimmetry_                             [p]);
        ADD_THREADED(hYasimmetry0_                            [p]);
        ADD_THREADED(h2DYAsimmetryLandau_                     [p]);
        ADD_THREADED(h2DYcellChargeAsimmetry_                 [p]);
        ADD_THREADED(h2DYcellChargeAsimmetryInv_              [p]);
        ADD_THREADED(h2DYcellChargeAsimmetryUnconstrained_    [p]);
        ADD_THREADED(h2DYcellChargeAsimmetryUnconstrainedInv_ [p]);
        ADD_THREADED(h2DYCellChargeAsimmetrySizeLE2_          [p]);

        for (std::vector<int>::iterator it = fRunNumbers_.begin(); it != fRunNumbers_.end(); ++it)
        {
            ADD_THREADED(mXYMeanCharge_[p][*it]);
            ADD_THREADED(mXYNorm_[p][*it]);
        }

        // START Window
        ADD_THREADED(hClusterSizeWindowed_                            [p]);
        ADD_THREADED(hClusterSizeCutsWindowed_                        [p]);
        ADD_THREADED(hNumberOfColsWindowed_                           [p]);
        ADD_THREADED(hNumberOfRowsWindowed_                           [p]);

        ADD_THREADED(hLandauClusterSize1Windowed_                     [p]);
        ADD_THREADED(hLandauClusterSize2Windowed_                     [p]);
        ADD_THREADED(hLandauClusterSize2sameColWindowed_              [p]);
        ADD_THREADED(hLandauClusterSize2sameRowWindowed_              [p]);
        ADD_THREADED(hLandauClusterSize3Windowed_                     [p]);
        ADD_THREADED(hLandauClusterSize3sameColWindowed_              [p]);
        ADD_THREADED(hLandauClusterSize3sameRowWindowed_              [p]);
        ADD_THREADED(hCellLandauWindowed_                             [p]);
        ADD_THREADED(hCellLandauSinglePixelWindowed_                  [p]);

        ADD_THREADED(h2DCellChargeWindowed_                           [p]);

        ADD_THREADED(h2DCellChargeNumWindowed_                        [p]);
        ADD_THREADED(h2DallTracksWindowed_                            [p]);

        // START Odd/Even Cell
        // Odd row
        ADD_THREADED(h2DEvenTracksOddRowsWindowed_                    [p]);
        ADD_THREADED(h2DOddTracksOddRowsWindowed_                     [p]);

        ADD_THREADED(h2DCellChargeEvenColumnsOddRowsWindowed_         [p]);
        ADD_THREADED(h2DCellChargeOddColumnsOddRowsWindowed_          [p]);

        // Even row
        ADD_THREADED(h2DEvenTracksEvenRowsWindowed_                   [p]);
        ADD_THREADED(h2DOddTracksEvenRowsWindowed_                    [p]);

        ADD_THREADED(h2DCellChargeEvenColumnsEvenRowsWindowed_        [p]);
        ADD_THREADED(h2DCellChargeOddColumnsEvenRowsWindowed_         [p]);

        // row independent
        ADD_THREADED(h2DCellChargeOddColumnsWindowed_                 [p]);
        ADD_THREADED(h2DCellChargeEvenColumnsWindowed_                [p]);
        // 4 cells
        ADD_THREADED(h4CellChargeFullRangeWindowed_                   [p]);
        // END Odd/Even Cell

        ADD_THREADED(h2DCellChargeNormWindowed_                       [p]);
        ADD_THREADED(h2DCellChargeNormSize2Windowed_                  [p]);
        ADD_THREADED(h2DCellChargeNormSize1Windowed_                  [p]);
        ADD_THREADED(h2DCellChargeNormSize2UpWindowed_                [p]);
        ADD_THREADED(h2DCellChargeNormSize1UpWindowed_                [p]);
        ADD_THREADED(h2DCellChargeNormSize2DownWindowed_              [p]);
        ADD_THREADED(h2DCellChargeNormSize1DownWindowed_              [p]);
        ADD_THREADED(h4CellsChargeWindowed_                           [p]);
        ADD_THREADED(h4CellsAllTracksWindowed_                        [p]);
        ADD_THREADED(h4CellsChargeNormWindowed_                       [p]);
        ADD_THREADED(h4HitsWindowed_                                  [p]);
        ADD_THREADED(h4HitsChargeWindowed_                            [p]);
        ADD_THREADED(hCutsControlWindowed_                            [p]);
        ADD_THREADED(hHitsNotOnRowColWindowed_                        [p]);
        ADD_THREADED(hChargeNotOnRowColWindowed_                      [p]);
        ADD_THREADED(hCellChargeCoarseWindowed_                       [p]);
        ADD_THREADED(hCellChargeCoarseNormWindowed_                   [p]);
        // End Window


        h2DCellCharge_                [p]->Divide(h2DallTracks_            [p]);

        // START Odd/Even Cell..endJob() Calc
        // Odd row
        h2DCellChargeEvenColumnsOddRows_    [p]->Divide(h2DEvenTracksOddRows_   [p]);
        h2DCellChargeOddColumnsOddRows_     [p]->Divide(h2DOddTracksOddRows_    [p]);
        // Even row
        h2DCellChargeEvenColumnsEvenRows_   [p]->Divide(h2DEvenTracksEvenRows_  [p]);
        h2DCellChargeOddColumnsEvenRows_    [p]->Divide(h2DOddTracksEvenRows_   [p]);

        // row independent
        h2DCellChargeOddColumns_            [p]->Add(h2DCellChargeOddColumnsOddRows_    [p], 0.5);
        h2DCellChargeOddColumns_            [p]->Add(h2DCellChargeOddColumnsEvenRows_   [p], 0.5);
        h2DCellChargeEvenColumns_           [p]->Add(h2DCellChargeEvenColumnsOddRows_   [p], 0.5);
        h2DCellChargeEvenColumns_           [p]->Add(h2DCellChargeEvenColumnsEvenRows_  [p], 0.5);
        // Generate 4 cell histogram
        int _cell_xnbins_charge = h2DCellChargeEvenColumnsEvenRows_[p]->GetNbinsX();
        int _cell_ynbins_charge = h2DCellChargeEvenColumnsEvenRows_[p]->GetNbinsY();
        // Get it once, since all are same

        // O Col O Row  -x, +y
        for (int i = 1; i <= _cell_xnbins_charge; ++i) {
            for (int j = 1; j <= _cell_ynbins_charge; ++j) {
                h4CellChargeFullRange_[p]->SetBinContent(i, j + _cell_ynbins_charge,
                                                         h2DCellChargeOddColumnsOddRows_[p]->GetBinContent(i, j));
            }
        }
        // O Col E Row  -x, -y
        for (int i = 1; i <= _cell_xnbins_charge; ++i) {
            for (int j = 1; j <= _cell_ynbins_charge; ++j) {
                h4CellChargeFullRange_[p]->SetBinContent(i, j,
                                                         h2DCellChargeOddColumnsEvenRows_[p]->GetBinContent(i, j));
            }
        }
        // E Col O Row  +x, +y
        for (int i = 1; i <= _cell_xnbins_charge; ++i) {
            for (int j = 1; j <= _cell_ynbins_charge; ++j) {
                h4CellChargeFullRange_[p]->SetBinContent(i + _cell_xnbins_charge, j + _cell_ynbins_charge,
                                                         h2DCellChargeEvenColumnsOddRows_[p]->GetBinContent(i, j));
            }
        }
        // E Col E Row  +x, -y
        for (int i = 1; i <= _cell_xnbins_charge; ++i) {
            for (int j = 1; j <= _cell_ynbins_charge; ++j) {
                h4CellChargeFullRange_[p]->SetBinContent(i + _cell_xnbins_charge, j,
                                                         h2DCellChargeEvenColumnsEvenRows_[p]->GetBinContent(i, j));
            }
        }
        // End Odd/Even Cell
        // START cluster size..endJob() Calc
        hClusterSizeDistribution1sEvenColumns_  [p]->Add(hClusterSizeDistribution1sEREC_[p], 0.5);
        hClusterSizeDistribution1sEvenColumns_  [p]->Add(hClusterSizeDistribution1sOREC_[p], 0.5);
        hClusterSizeDistribution1sOddColumns_   [p]->Add(hClusterSizeDistribution1sEROC_[p], 0.5);
        hClusterSizeDistribution1sOddColumns_   [p]->Add(hClusterSizeDistribution1sOROC_[p], 0.5);
        hClusterSizeDistribution2sEvenColumns_  [p]->Add(hClusterSizeDistribution2sEREC_[p], 0.5);
        hClusterSizeDistribution2sEvenColumns_  [p]->Add(hClusterSizeDistribution2sOREC_[p], 0.5);
        hClusterSizeDistribution2sOddColumns_   [p]->Add(hClusterSizeDistribution2sEROC_[p], 0.5);
        hClusterSizeDistribution2sOddColumns_   [p]->Add(hClusterSizeDistribution2sOROC_[p], 0.5);
        // 4 Cells

        // Generate 4 cell histogram
        int _cell_xnbins_cluster1 = hClusterSizeDistribution1sOROC_[p]->GetNbinsX();
        int _cell_ynbins_cluster1 = hClusterSizeDistribution1sOROC_[p]->GetNbinsY();
        // Get it once, since all are same

        // O Col O Row  -x, +y
        for (int i = 1; i <= _cell_xnbins_cluster1; ++i) {
            for (int j = 1; j <= _cell_ynbins_cluster1; ++j) {
                hClusterSizeDistribution1s4Cells_[p]->SetBinContent(i, j + _cell_ynbins_cluster1,
                                                                    hClusterSizeDistribution1sOROC_[p]->GetBinContent(i, j));
            }
        }
        // O Col E Row  -x, -y
        for (int i = 1; i <= _cell_xnbins_cluster1; ++i) {
            for (int j = 1; j <= _cell_ynbins_cluster1; ++j) {
                hClusterSizeDistribution1s4Cells_[p]->SetBinContent(i, j,
                                                                    hClusterSizeDistribution1sEROC_[p]->GetBinContent(i, j));
            }
        }
        // E Col O Row  +x, +y
        for (int i = 1; i <= _cell_xnbins_cluster1; ++i) {
            for (int j = 1; j <= _cell_ynbins_cluster1; ++j) {
                hClusterSizeDistribution1s4Cells_[p]->SetBinContent(i + _cell_xnbins_cluster1, j + _cell_ynbins_cluster1,
                                                                    hClusterSizeDistribution1sOREC_[p]->GetBinContent(i, j));
            }
        }
        // E Col E Row  +x, -y
        for (int i = 1; i <= _cell_xnbins_cluster1; ++i) {
            for (int j = 1; j <= _cell_ynbins_cluster1; ++j) {
                hClusterSizeDistribution1s4Cells_[p]->SetBinContent(i + _cell_xnbins_cluster1, j,
                                                                    hClusterSizeDistribution1sEREC_[p]->GetBinContent(i, j));
            }
        }
        // End Odd/Even Cell

        // Generate 4 cell histogram
        int _cell_xnbins_cluster2 = hClusterSizeDistribution2sOROC_[p]->GetNbinsX();
        int _cell_ynbins_cluster2 = hClusterSizeDistribution2sOROC_[p]->GetNbinsY();
        // Get it once, since all are same

        // O Col O Row  -x, +y
        for (int i = 1; i <= _cell_xnbins_cluster2; ++i) {
            for (int j = 1; j <= _cell_ynbins_cluster2; ++j) {
                hClusterSizeDistribution2s4Cells_[p]->SetBinContent(i, j + _cell_ynbins_cluster2,
                                                                    hClusterSizeDistribution2sOROC_[p]->GetBinContent(i, j));
            }
        }
        // O Col E Row  -x, -y
        for (int i = 1; i <= _cell_xnbins_cluster2; ++i) {
            for (int j = 1; j <= _cell_ynbins_cluster2; ++j) {
                hClusterSizeDistribution2s4Cells_[p]->SetBinContent(i, j,
                                                                    hClusterSizeDistribution2sEROC_[p]->GetBinContent(i, j));
            }
        }
        // E Col O Row  +x, +y
        for (int i = 1; i <= _cell_xnbins_cluster2; ++i) {
            for (int j = 1; j <= _cell_ynbins_cluster2; ++j) {
                hClusterSizeDistribution2s4Cells_[p]->SetBinContent(i + _cell_xnbins_cluster2, j + _cell_ynbins_cluster2,
                                                                    hClusterSizeDistribution2sOREC_[p]->GetBinContent(i, j));
            }
        }
        // E Col E Row  +x, -y
        for (int i = 1; i <= _cell_xnbins_cluster2; ++i) {
            for (int j = 1; j <= _cell_ynbins_cluster2; ++j) {
                hClusterSizeDistribution2s4Cells_[p]->SetBinContent(i + _cell_xnbins_cluster2, j,
                                                                    hClusterSizeDistribution2sEREC_[p]->GetBinContent(i, j));
            }
        }
        // End Odd/Even Cell
        // End cluster size


        h4CellsCharge_                [p]->Divide(h4CellsChargeNorm_       [p]);
        h4HitsCharge_                 [p]->Divide(h4Hits_                  [p]);

        h1DXcellCharge_               [p]->Divide(h1DXcellChargeNorm_      [p]);
        h1DXcellChargeNormToAll_      [p]->Divide(h1DXallTracks_           [p]);
        h1DXcellChargeSumLE2_         [p]->Divide(h1DXcellChargeSumLE2Norm_[p]);
        h1DXcellChargeSumLE2NormToAll_[p]->Divide(h1DXallTracks_           [p]);
        h1DXcellChargeSumLE3_         [p]->Divide(h1DXcellChargeSumLE3Norm_[p]);
        h1DXcellChargeSumLE3NormToAll_[p]->Divide(h1DXallTracks_           [p]);
        hCellChargeCoarse_            [p]->Divide(hCellChargeCoarseNorm_   [p]);

        h1DYcellCharge_               [p]->Divide(h1DYcellChargeNorm_      [p]);
        h1DYcellChargeNormToAll_      [p]->Divide(h1DYallTracks_           [p]);
        h1DYcellChargeSumLE2_         [p]->Divide(h1DYcellChargeSumLE2Norm_[p]);
        h1DXcellChargeSumLE2NormToAll_[p]->Divide(h1DYallTracks_           [p]);
        h1DYcellChargeSumLE3_         [p]->Divide(h1DYcellChargeSumLE3Norm_[p]);
        h1DXcellChargeSumLE3NormToAll_[p]->Divide(h1DYallTracks_           [p]);

        // START Window
        h2DCellChargeWindowed_                [p]->Divide(h2DallTracksWindowed_            [p]);

        // START Odd/Even Cell..endJob() Calc
        // Odd row
        h2DCellChargeEvenColumnsOddRowsWindowed_    [p]->Divide(h2DEvenTracksOddRowsWindowed_   [p]);
        h2DCellChargeOddColumnsOddRowsWindowed_     [p]->Divide(h2DOddTracksOddRowsWindowed_    [p]);
        // Even row
        h2DCellChargeEvenColumnsEvenRowsWindowed_   [p]->Divide(h2DEvenTracksEvenRowsWindowed_  [p]);
        h2DCellChargeOddColumnsEvenRowsWindowed_    [p]->Divide(h2DOddTracksEvenRowsWindowed_   [p]);

        // row independent
        h2DCellChargeOddColumnsWindowed_            [p]->Add(h2DCellChargeOddColumnsOddRowsWindowed_    [p], 0.5);
        h2DCellChargeOddColumnsWindowed_            [p]->Add(h2DCellChargeOddColumnsEvenRowsWindowed_   [p], 0.5);
        h2DCellChargeEvenColumnsWindowed_           [p]->Add(h2DCellChargeEvenColumnsOddRowsWindowed_   [p], 0.5);
        h2DCellChargeEvenColumnsWindowed_           [p]->Add(h2DCellChargeEvenColumnsEvenRowsWindowed_  [p], 0.5);
        // Generate 4 cell histogram
        int _window_cell_xbins_charge = h2DCellChargeEvenColumnsEvenRowsWindowed_[p]->GetNbinsX();
        int _window_cell_ybins_charge = h2DCellChargeEvenColumnsEvenRowsWindowed_[p]->GetNbinsY();
        // Get it once, since all are same

        // O Col O Row  -x, +y
        for (int i = 1; i <= _window_cell_xbins_charge; ++i) {
            for (int j = 1; j <= _window_cell_ybins_charge; ++j) {
                h4CellChargeFullRangeWindowed_[p]->SetBinContent(i, j + _window_cell_ybins_charge,
                                                                 h2DCellChargeOddColumnsOddRowsWindowed_[p]->GetBinContent(i, j));
            }
        }
        // O Col E Row  -x, -y
        for (int i = 1; i <= _window_cell_xbins_charge; ++i) {
            for (int j = 1; j <= _window_cell_ybins_charge; ++j) {
                h4CellChargeFullRangeWindowed_[p]->SetBinContent(i, j,
                                                                 h2DCellChargeOddColumnsEvenRowsWindowed_[p]->GetBinContent(i, j));
            }
        }
        // E Col O Row  +x, +y
        for (int i = 1; i <= _window_cell_xbins_charge; ++i) {
            for (int j = 1; j <= _window_cell_ybins_charge; ++j) {
                h4CellChargeFullRangeWindowed_[p]->SetBinContent(i + _window_cell_xbins_charge, j + _window_cell_ybins_charge,
                                                                 h2DCellChargeEvenColumnsOddRowsWindowed_[p]->GetBinContent(i, j));
            }
        }
        // E Col E Row  +x, -y
        for (int i = 1; i <= _window_cell_xbins_charge; ++i) {
            for (int j = 1; j <= _window_cell_ybins_charge; ++j) {
                h4CellChargeFullRangeWindowed_[p]->SetBinContent(i + _window_cell_xbins_charge, j,
                                                                 h2DCellChargeEvenColumnsEvenRowsWindowed_[p]->GetBinContent(i, j));
            }
        }
        // End Odd/Even Cell

        h4CellsChargeWindowed_                [p]->Divide(h4CellsChargeNormWindowed_       [p]);
        h4HitsChargeWindowed_                 [p]->Divide(h4HitsWindowed_                  [p]);
        // End Window

        ss.str(""); ss << "Fitting Landau distributions for plane " << planeName << " ... ";
        STDLINE(ss.str(),ACRed);
        STDLINE("",ACWhite);

        fitCharge(p);

        STDLINE("",ACWhite);

        NormalizeEtaDistributionSize2 (p);
        //        NormalizeEtaDistribution      (p);
        //        NormalizeEtaInverse(p);

        setErrorsBar(p);

        CalculateEtaDerivative(p);

        const float resXRange = 150;
        const float resYRange = 100;

        h1DXcellChargeAsimmetry_                 [p]->SetMinimum(-1);
        h1DXcellChargeAsimmetry_                 [p]->SetMaximum( 1);
        h1DXcellChargeAsimmetry_                 [p]->SetMarkerStyle(20);
        h1DXcellChargeAsimmetry_                 [p]->SetMarkerSize(0.6);

        h1DXcellChargeAsimmetryInv_              [p]->SetMinimum(-(resXRange/2));
        h1DXcellChargeAsimmetryInv_              [p]->SetMaximum(resXRange/2);
        h1DXcellChargeAsimmetryInv_              [p]->SetMarkerStyle(20);
        h1DXcellChargeAsimmetryInv_              [p]->SetMarkerSize(0.6);

        h1DYcellChargeAsimmetry_                 [p]->SetMinimum(-1);
        h1DYcellChargeAsimmetry_                 [p]->SetMaximum( 1);
        h1DYcellChargeAsimmetry_                 [p]->SetMarkerStyle(20);
        h1DYcellChargeAsimmetry_                 [p]->SetMarkerSize(0.6);

        h1DYcellChargeAsimmetryInv_              [p]->SetMinimum(-(resYRange/2));
        h1DYcellChargeAsimmetryInv_              [p]->SetMaximum(resYRange/2);
        h1DYcellChargeAsimmetryInv_              [p]->SetMarkerStyle(20);
        h1DYcellChargeAsimmetryInv_              [p]->SetMarkerSize(0.6);

        h1DXcellChargeAsimmetryUnconstrained_    [p]->SetMinimum(-1);
        h1DXcellChargeAsimmetryUnconstrained_    [p]->SetMaximum( 1);
        h1DXcellChargeAsimmetryUnconstrained_    [p]->SetMarkerStyle(20);
        h1DXcellChargeAsimmetryUnconstrained_    [p]->SetMarkerSize(0.6);

        h1DXcellChargeAsimmetryUnconstrainedInv_ [p]->SetMinimum(-(resXRange/2));
        h1DXcellChargeAsimmetryUnconstrainedInv_ [p]->SetMaximum(resXRange/2);
        h1DXcellChargeAsimmetryUnconstrainedInv_ [p]->SetMarkerStyle(20);
        h1DXcellChargeAsimmetryUnconstrainedInv_ [p]->SetMarkerSize(0.6);

        h1DXCellChargeAsimmetrySizeLE2_          [p]->SetMinimum(-1.1);
        h1DXCellChargeAsimmetrySizeLE2_          [p]->SetMaximum( 1.1);
        h1DXCellChargeAsimmetrySizeLE2_          [p]->SetMarkerStyle(20);
        h1DXCellChargeAsimmetrySizeLE2_          [p]->SetMarkerSize(0.6);

        h1DYcellChargeAsimmetryUnconstrained_    [p]->SetMinimum(-1);
        h1DYcellChargeAsimmetryUnconstrained_    [p]->SetMaximum( 1);
        h1DYcellChargeAsimmetryUnconstrained_    [p]->SetMarkerStyle(20);
        h1DYcellChargeAsimmetryUnconstrained_    [p]->SetMarkerSize(0.6);

        h1DYcellChargeAsimmetryUnconstrainedInv_ [p]->SetMinimum(-(resYRange/2));
        h1DYcellChargeAsimmetryUnconstrainedInv_ [p]->SetMaximum(resYRange/2);
        h1DYcellChargeAsimmetryUnconstrainedInv_ [p]->SetMarkerStyle(20);
        h1DYcellChargeAsimmetryUnconstrainedInv_ [p]->SetMarkerSize(0.6);

        h1DYCellChargeAsimmetrySizeLE2_          [p]->SetMinimum(-1.1);
        h1DYCellChargeAsimmetrySizeLE2_          [p]->SetMaximum( 1.1);
        h1DYCellChargeAsimmetrySizeLE2_          [p]->SetMarkerStyle(20);
        h1DYCellChargeAsimmetrySizeLE2_          [p]->SetMarkerSize(0.6);

        h1DXcellCharge_                          [p]->SetMarkerStyle(20);
        h1DXcellCharge_                          [p]->SetMarkerSize(0.6);
        h1DXcellCharge_                          [p]->SetMarkerColor(kBlack);
        h1DXcellCharge_                          [p]->SetLineColor(kBlack);

        h1DXcellChargeSumLE2_                    [p]->SetMarkerStyle(20);
        h1DXcellChargeSumLE2_                    [p]->SetMarkerSize(0.6);
        h1DXcellChargeSumLE2_                    [p]->SetMarkerColor(kRed);
        h1DXcellChargeSumLE2_                    [p]->SetLineColor(kRed);

        h1DXcellChargeSumLE3_                    [p]->SetMarkerStyle(20);
        h1DXcellChargeSumLE3_                    [p]->SetMarkerSize(0.6);
        h1DXcellChargeSumLE3_                    [p]->SetMarkerColor(kBlue);
        h1DXcellChargeSumLE3_                    [p]->SetLineColor(kBlue);

        h1DYcellCharge_                          [p]->SetMarkerStyle(20);
        h1DYcellCharge_                          [p]->SetMarkerSize(0.6);
        h1DYcellCharge_                          [p]->SetMarkerColor(kBlack);
        h1DYcellCharge_                          [p]->SetLineColor(kBlack);

        h1DYcellChargeSumLE2_                    [p]->SetMarkerStyle(20);
        h1DYcellChargeSumLE2_                    [p]->SetMarkerSize(0.6);
        h1DYcellChargeSumLE2_                    [p]->SetMarkerColor(kRed);
        h1DYcellChargeSumLE2_                    [p]->SetLineColor(kRed);

        h1DYcellChargeSumLE3_                    [p]->SetMarkerStyle(20);
        h1DYcellChargeSumLE3_                    [p]->SetMarkerSize(0.6);
        h1DYcellChargeSumLE3_                    [p]->SetMarkerColor(kBlue);
        h1DYcellChargeSumLE3_                    [p]->SetLineColor(kBlue);

        /*h1DXcellChargeNormToAll_                 [p]->SetMarkerStyle(20);
        h1DXcellChargeNormToAll_                 [p]->SetMarkerSize(0.6);
        h1DXcellChargeNormToAll_                 [p]->SetMarkerColor(kBlack);

        h1DXcellChargeSumLE2NormToAll_           [p]->SetMarkerStyle(20);
        h1DXcellChargeSumLE2NormToAll_           [p]->SetMarkerSize(0.6);
        h1DXcellChargeSumLE2NormToAll_           [p]->SetMarkerColor(kRed);

        h1DXcellChargeSumLE3NormToAll_           [p]->SetMarkerStyle(20);
        h1DXcellChargeSumLE3NormToAll_           [p]->SetMarkerSize(0.6);
        h1DXcellChargeSumLE3NormToAll_           [p]->SetMarkerColor(kMagenta);*/

        hClusterSize_                            [p]->GetXaxis()->SetTitle("cluster size"      );
        hClusterSizeCuts_                        [p]->GetXaxis()->SetTitle("cluster size"      );
        hClusterSizeCutsPlus_                    [p]->GetXaxis()->SetTitle("cluster size"      );
        hNumberOfCols_                           [p]->GetXaxis()->SetTitle("number of columns" );
        hNumberOfRows_                           [p]->GetXaxis()->SetTitle("number of rows"    );
        hClusterSizeDistribution1s_              [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution1s_              [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution1sOddColumns_    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution1sOddColumns_    [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution1sEvenColumns_   [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution1sEvenColumns_   [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution1sOROC_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution1sOROC_          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution1sEROC_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution1sEROC_          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution1sOREC_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution1sOREC_          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution1sEREC_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution1sEREC_          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution1s4Cells_        [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution1s4Cells_        [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution2s_              [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution2s_              [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution2sOddColumns_    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution2sOddColumns_    [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution2sEvenColumns_   [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution2sEvenColumns_   [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution2sOROC_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution2sOROC_          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution2sEROC_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution2sEROC_          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution2sOREC_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution2sOREC_          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution2sEREC_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution2sEREC_          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hClusterSizeDistribution2s4Cells_        [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hClusterSizeDistribution2s4Cells_        [p]->GetXaxis()->SetTitle("short pitch (um)"  );


        hLandauClusterSize1_                     [p]->GetXaxis()->SetTitle("charge (electrons)");
        hCellLandau_                             [p]->GetXaxis()->SetTitle("charge (electrons)");
        hCellLandauSinglePixel_                  [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize2_                     [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize2sameRow_              [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize2sameCol_              [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize3_                     [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize3sameRow_              [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize3sameCol_              [p]->GetXaxis()->SetTitle("charge (electrons)");

        h2DCellCharge_                           [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellCharge_                           [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        // START Odd/Even Cell Charge Axis name
        // Odd rows
        h2DCellChargeEvenColumnsOddRows_         [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeEvenColumnsOddRows_         [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DCellChargeOddColumnsOddRows_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeOddColumnsOddRows_          [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DEvenTracksOddRows_                    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DEvenTracksOddRows_                    [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DOddTracksOddRows_                     [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DOddTracksOddRows_                     [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        // Even rows
        h2DCellChargeEvenColumnsEvenRows_        [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeEvenColumnsEvenRows_        [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DCellChargeOddColumnsEvenRows_         [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeOddColumnsEvenRows_         [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DEvenTracksEvenRows_                   [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DEvenTracksEvenRows_                   [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DOddTracksEvenRows_                    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DOddTracksEvenRows_                    [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        // row independent
        h2DCellChargeEvenColumns_                [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeEvenColumns_                [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DCellChargeOddColumns_                 [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeOddColumns_                 [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        // 4 cells
        h4CellChargeFullRange_                   [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h4CellChargeFullRange_                   [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        // End Odd/Even Cell Charge Axis name
        h2DCellChargeNum_                        [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNum_                        [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DallTracks_                            [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DallTracks_                            [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DOddTracksOddRows_                            [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DOddTracksOddRows_                            [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNorm_                       [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNorm_                       [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize2_                  [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize2_                  [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize1_                  [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize1_                  [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize2Up_                [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize2Up_                [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize1Up_                [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize1Up_                [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize2Down_              [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize2Down_              [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize1Down_              [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize1Down_              [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h4CellsCharge_                           [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h4CellsCharge_                           [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h4CellsAllTracks_                        [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h4CellsAllTracks_                        [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h4CellsChargeNorm_                       [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h4CellsChargeNorm_                       [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        hCellChargeCoarse_                       [p]->GetXaxis()->SetTitle("x (um)"            );
        hCellChargeCoarse_                       [p]->GetYaxis()->SetTitle("y (um)"            );
        hCellChargeCoarseNorm_                   [p]->GetXaxis()->SetTitle("x (um)"            );
        hCellChargeCoarseNorm_                   [p]->GetYaxis()->SetTitle("y (um)"            );
        hHitsNotOnRowCol_                        [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hHitsNotOnRowCol_                        [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        //        hCutsControl_                            [p]-> attach some legend...!
        hChargeNotOnRowCol_                      [p]->GetXaxis()->SetTitle("charge (electrons)");

        h1DXcellCharge_                          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcellCharge_                          [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DXcellChargeSumLE2_                    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcellChargeSumLE2_                    [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DXcellChargeSumLE3_                    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcellChargeSumLE3_                    [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DXcellChargeNormToAll_                 [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcellChargeNormToAll_                 [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DXcellChargeSumLE2NormToAll_           [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcellChargeSumLE2NormToAll_           [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DXcellChargeSumLE3NormToAll_           [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcellChargeSumLE3NormToAll_           [p]->GetYaxis()->SetTitle("charge (electrons)");
        h2DXcellCharge_                          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DXcellCharge_                          [p]->GetYaxis()->SetTitle("charge (electrons)");
        h2DXcellChargeSumLE2_                    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DXcellChargeSumLE2_                    [p]->GetYaxis()->SetTitle("charge (electrons)");
        h2DXcellChargeSumLE3_                    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DXcellChargeSumLE3_                    [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DXallTracks_                           [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcellChargeNorm_                      [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcellSingleHits_                      [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcellDoubleHits_                      [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcell3Hits_                           [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DXcellSingleHits_                      [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DXcellSingleHits_                      [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DXcellDoubleHits_                      [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DXcellDoubleHits_                      [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        hHitsNotONRowColVsXSlope_                [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hHitsNotONRowColVsXSlope_                [p]->GetYaxis()->SetTitle("x slope (1/um)"    );
        hHitsNotOnRowColProjX_                   [p]->GetXaxis()->SetTitle("long pitch (um)"   );

        h1DYcellCharge_                          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h1DYcellCharge_                          [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DYcellChargeSumLE2_                    [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h1DYcellChargeSumLE2_                    [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DYcellChargeSumLE3_                    [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h1DYcellChargeSumLE3_                    [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DYcellChargeNormToAll_                 [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DYcellChargeNormToAll_                 [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DYcellChargeSumLE2NormToAll_           [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DYcellChargeSumLE2NormToAll_           [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DYcellChargeSumLE3NormToAll_           [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DYcellChargeSumLE3NormToAll_           [p]->GetYaxis()->SetTitle("charge (electrons)");
        h2DYcellCharge_                          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h2DYcellCharge_                          [p]->GetYaxis()->SetTitle("charge (electrons)");
        h2DYcellChargeSumLE2_                    [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h2DYcellChargeSumLE2_                    [p]->GetYaxis()->SetTitle("charge (electrons)");
        h1DYallTracks_                           [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h1DYcellChargeNorm_                      [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h1DYcellSingleHits_                      [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h1DYcellDoubleHits_                      [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h1DYcell3Hits_                           [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h2DYcellSingleHits_                      [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DYcellSingleHits_                      [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DYcellDoubleHits_                      [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DYcellDoubleHits_                      [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        hHitsNotONRowColVsYSlope_                [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        hHitsNotONRowColVsYSlope_                [p]->GetYaxis()->SetTitle("y slope (1/um)"    );
        hHitsNotOnRowColProjY_                   [p]->GetXaxis()->SetTitle("short pitch (um)"  );

        hXasimmetry_                             [p]->GetXaxis()->SetTitle("Asimmetry"         );
        hXasimmetry0_                            [p]->GetXaxis()->SetTitle("Asimmetry on one side");
        h2DXAsimmetryLandau_                     [p]->GetXaxis()->SetTitle("charge (electrons)");
        h2DXAsimmetryLandau_                     [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h2DXcellChargeAsimmetry_                 [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DXcellChargeAsimmetry_                 [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h1DXcellChargeAsimmetry_                 [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcellChargeAsimmetry_                 [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h2DXcellChargeAsimmetryInv_              [p]->GetXaxis()->SetTitle("Asimmetry"         );
        h2DXcellChargeAsimmetryInv_              [p]->GetYaxis()->SetTitle("long pitch (um)"   );
        h1DXcellChargeAsimmetryInv_              [p]->GetXaxis()->SetTitle("Asimmetry"         );
        h1DXcellChargeAsimmetryInv_              [p]->GetYaxis()->SetTitle("long pitch (um)"   );
        h2DXcellChargeAsimmetryUnconstrained_    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DXcellChargeAsimmetryUnconstrained_    [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h1DXcellChargeAsimmetryUnconstrained_    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXcellChargeAsimmetryUnconstrained_    [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h2DXcellChargeAsimmetryUnconstrainedInv_ [p]->GetXaxis()->SetTitle("Asimmetry"         );
        h2DXcellChargeAsimmetryUnconstrainedInv_ [p]->GetYaxis()->SetTitle("long pitch (um)"   );
        h1DXcellChargeAsimmetryUnconstrainedInv_ [p]->GetXaxis()->SetTitle("Asimmetry"         );
        h1DXcellChargeAsimmetryUnconstrainedInv_ [p]->GetYaxis()->SetTitle("long pitch (um)"   );
        h2DXCellChargeAsimmetrySizeLE2_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DXCellChargeAsimmetrySizeLE2_          [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h1DXCellChargeAsimmetrySizeLE2_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h1DXCellChargeAsimmetrySizeLE2_          [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h1DXEtaDistribution_                     [p]->GetXaxis()->SetTitle("Asimmetry"         );
        h1DXEtaDerivativeDistribution_           [p]->GetXaxis()->SetTitle("Long pitch (um)"   );
        h1DXEtaDerivativeDistribution_           [p]->GetYaxis()->SetTitle("dEta/dx"           );

        hYasimmetry_                             [p]->GetXaxis()->SetTitle("Asimmetry"         );
        hYasimmetry0_                            [p]->GetXaxis()->SetTitle("Asimmetry on one side");
        h2DYAsimmetryLandau_                     [p]->GetXaxis()->SetTitle("charge (electrons)");
        h2DYAsimmetryLandau_                     [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h2DYcellChargeAsimmetry_                 [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h2DYcellChargeAsimmetry_                 [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h1DYcellChargeAsimmetry_                 [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h1DYcellChargeAsimmetry_                 [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h2DYcellChargeAsimmetryInv_              [p]->GetXaxis()->SetTitle("Asimmetry"         );
        h2DYcellChargeAsimmetryInv_              [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h1DYcellChargeAsimmetryInv_              [p]->GetXaxis()->SetTitle("Asimmetry"         );
        h1DYcellChargeAsimmetryInv_              [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DYcellChargeAsimmetryUnconstrained_    [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h2DYcellChargeAsimmetryUnconstrained_    [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h1DYcellChargeAsimmetryUnconstrained_    [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h1DYcellChargeAsimmetryUnconstrained_    [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h2DYcellChargeAsimmetryUnconstrainedInv_ [p]->GetXaxis()->SetTitle("Asimmetry"         );
        h2DYcellChargeAsimmetryUnconstrainedInv_ [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h1DYcellChargeAsimmetryUnconstrainedInv_ [p]->GetXaxis()->SetTitle("Asimmetry"         );
        h1DYcellChargeAsimmetryUnconstrainedInv_ [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DYCellChargeAsimmetrySizeLE2_          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h2DYCellChargeAsimmetrySizeLE2_          [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h1DYCellChargeAsimmetrySizeLE2_          [p]->GetXaxis()->SetTitle("short pitch (um)"  );
        h1DYCellChargeAsimmetrySizeLE2_          [p]->GetYaxis()->SetTitle("Asimmetry"         );
        h1DYEtaDistribution_                     [p]->GetXaxis()->SetTitle("Asimmetry"         );
        h1DYEtaDerivativeDistribution_           [p]->GetXaxis()->SetTitle("Short pitch (um)"  );
        h1DYEtaDerivativeDistribution_           [p]->GetYaxis()->SetTitle("dEta/dy"           );


        // START Window

        hClusterSizeWindowed_                            [p]->GetXaxis()->SetTitle("cluster size"      );
        hClusterSizeCutsWindowed_                        [p]->GetXaxis()->SetTitle("cluster size"      );
        hNumberOfColsWindowed_                           [p]->GetXaxis()->SetTitle("number of columns" );
        hNumberOfRowsWindowed_                           [p]->GetXaxis()->SetTitle("number of rows"    );

        hLandauClusterSize1Windowed_                     [p]->GetXaxis()->SetTitle("charge (electrons)");
        hCellLandauWindowed_                             [p]->GetXaxis()->SetTitle("charge (electrons)");
        hCellLandauSinglePixelWindowed_                  [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize2Windowed_                     [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize2sameRowWindowed_              [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize2sameColWindowed_              [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize3Windowed_                     [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize3sameRowWindowed_              [p]->GetXaxis()->SetTitle("charge (electrons)");
        hLandauClusterSize3sameColWindowed_              [p]->GetXaxis()->SetTitle("charge (electrons)");

        h2DCellChargeWindowed_                           [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeWindowed_                           [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        // START Odd/Even Cell Charge Axis name
        // Odd rows
        h2DCellChargeEvenColumnsOddRowsWindowed_         [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeEvenColumnsOddRowsWindowed_         [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DCellChargeOddColumnsOddRowsWindowed_          [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeOddColumnsOddRowsWindowed_          [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DEvenTracksOddRowsWindowed_                    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DEvenTracksOddRowsWindowed_                    [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DOddTracksOddRowsWindowed_                     [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DOddTracksOddRowsWindowed_                     [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        // Even rows
        h2DCellChargeEvenColumnsEvenRowsWindowed_        [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeEvenColumnsEvenRowsWindowed_        [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DCellChargeOddColumnsEvenRowsWindowed_         [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeOddColumnsEvenRowsWindowed_         [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DEvenTracksEvenRowsWindowed_                   [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DEvenTracksEvenRowsWindowed_                   [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DOddTracksEvenRowsWindowed_                    [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DOddTracksEvenRowsWindowed_                    [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        // row independent
        h2DCellChargeEvenColumnsWindowed_                [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeEvenColumnsWindowed_                [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DCellChargeOddColumnsWindowed_                 [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeOddColumnsWindowed_                 [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        // 4 cells
        h4CellChargeFullRangeWindowed_                   [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h4CellChargeFullRangeWindowed_                   [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        // End Odd/Even Cell Charge Axis name
        h2DCellChargeNumWindowed_                        [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNumWindowed_                        [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DallTracksWindowed_                            [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DallTracksWindowed_                            [p]->GetYaxis()->SetTitle("short pitch (um)"  );

        h2DOddTracksOddRowsWindowed_                            [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DOddTracksOddRowsWindowed_                            [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormWindowed_                       [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormWindowed_                       [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize2Windowed_                  [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize2Windowed_                  [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize1Windowed_                  [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize1Windowed_                  [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize2UpWindowed_                [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize2UpWindowed_                [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize1UpWindowed_                [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize1UpWindowed_                [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize2DownWindowed_              [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize2DownWindowed_              [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h2DCellChargeNormSize1DownWindowed_              [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h2DCellChargeNormSize1DownWindowed_              [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h4CellsChargeWindowed_                           [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h4CellsChargeWindowed_                           [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h4CellsAllTracksWindowed_                        [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h4CellsAllTracksWindowed_                        [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        h4CellsChargeNormWindowed_                       [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        h4CellsChargeNormWindowed_                       [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        hCellChargeCoarseWindowed_                       [p]->GetXaxis()->SetTitle("x (um)"            );
        hCellChargeCoarseWindowed_                       [p]->GetYaxis()->SetTitle("y (um)"            );
        hCellChargeCoarseNormWindowed_                   [p]->GetXaxis()->SetTitle("x (um)"            );
        hCellChargeCoarseNormWindowed_                   [p]->GetYaxis()->SetTitle("y (um)"            );
        hHitsNotOnRowColWindowed_                        [p]->GetXaxis()->SetTitle("long pitch (um)"   );
        hHitsNotOnRowColWindowed_                        [p]->GetYaxis()->SetTitle("short pitch (um)"  );
        hChargeNotOnRowColWindowed_                      [p]->GetXaxis()->SetTitle("charge (electrons)");

        // END Window
    }
    calculateMeanCharge();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Charge::book(void)
{
    destroy();

    std::string hName;
    std::string hTitle;
    std::string planeName;

    float resXRange = 150;
    float resYRange = 100;

    theAnalysisManager_->cd("/");
    theAnalysisManager_->mkdir("Charge");

    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {
        planeName = thePlaneMapping_->getPlaneName(p);
        theAnalysisManager_->cd("Charge");
        theAnalysisManager_->mkdir(planeName);

        /*--------------------------------------------------------------------------cluster size-----------------------------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("ClusterSize");

        hName  = "hClusterSize_"                   + planeName;
        hTitle = "Cluster size distribution "      + planeName;
        hClusterSize_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 10, 0, 10)));

        hName  = "hClusterSizeCuts_"               + planeName;
        hTitle = "Cluster size distribution (standard cut) "+ planeName;
        hClusterSizeCuts_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 10, 0, 10)));

        hName  = "hClusterSizeCutsPlus_"               + planeName;
        hTitle = "Cluster size distribution (standard + fiducial area cut) "+ planeName;
        hClusterSizeCutsPlus_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 10, 0, 10)));

        hName  = "hNumberOfCols_"                  + planeName;
        hTitle = "Number of columns distribution " + planeName;
        hNumberOfCols_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 10, 0, 10)));

        hName  = "hNumberOfRows_"                  + planeName;
        hTitle = "Number of rows distribution "    + planeName;
        hNumberOfRows_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 10, 0, 10)));

        hName  = "hClusterSizeDistribution1s_"           + planeName;
        hTitle = "Size 1 cluster distribution on pixel " + planeName;
        hClusterSizeDistribution1s_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution1s4Cells_"           + planeName;
        hTitle = "Size 1 cluster distribution on pixel 4 Cells " + planeName;
        hClusterSizeDistribution1s4Cells_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2 * ((int)resXRange/5), -resXRange, resXRange, 2 * ((int)resYRange/5), -resYRange, resYRange)));

        hName  = "hClusterSizeDistribution1sOddColumns_"           + planeName;
        hTitle = "Size 1 cluster distribution on pixel odd columns " + planeName;
        hClusterSizeDistribution1sOddColumns_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution1sEvenColumns_"           + planeName;
        hTitle = "Size 1 cluster distribution on pixel even columns " + planeName;
        hClusterSizeDistribution1sEvenColumns_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution1sOROC_"           + planeName;
        hTitle = "Size 1 cluster distribution on pixel odd rows odd columns " + planeName;
        hClusterSizeDistribution1sOROC_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution1sEROC_"           + planeName;
        hTitle = "Size 1 cluster distribution on pixel even rows odd columns " + planeName;
        hClusterSizeDistribution1sEROC_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution1sOREC_"           + planeName;
        hTitle = "Size 1 cluster distribution on pixel odd rows even columns " + planeName;
        hClusterSizeDistribution1sOREC_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution1sEREC_"           + planeName;
        hTitle = "Size 1 cluster distribution on pixel even rows even columns " + planeName;
        hClusterSizeDistribution1sEREC_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution2s_"           + planeName;
        hTitle = "Size 2 cluster distribution on pixel " + planeName;
        hClusterSizeDistribution2s_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution2s4Cells_"           + planeName;
        hTitle = "Size 2 cluster distribution on pixel 4 cells " + planeName;
        hClusterSizeDistribution2s4Cells_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2 * ((int)resXRange/5), -resXRange, resXRange, 2 * ((int)resYRange/5), -resYRange, resYRange)));

        hName  = "hClusterSizeDistribution2sOddColumns_"           + planeName;
        hTitle = "Size 2 cluster distribution on pixel odd columns " + planeName;
        hClusterSizeDistribution2sOddColumns_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution2sEvenColumns_"           + planeName;
        hTitle = "Size 2 cluster distribution on pixel even columns " + planeName;
        hClusterSizeDistribution2sEvenColumns_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution2sOROC_"           + planeName;
        hTitle = "Size 2 cluster distribution on pixel odd rows odd columns " + planeName;
        hClusterSizeDistribution2sOROC_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution2sOREC_"           + planeName;
        hTitle = "Size 2 cluster distribution on pixel odd rows even columns " + planeName;
        hClusterSizeDistribution2sOREC_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution2sEROC_"           + planeName;
        hTitle = "Size 2 cluster distribution on pixel even rows odd columns " + planeName;
        hClusterSizeDistribution2sEROC_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "hClusterSizeDistribution2sEREC_"           + planeName;
        hTitle = "Size 2 cluster distribution on pixel even rows even columns " + planeName;
        hClusterSizeDistribution2sEREC_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        theAnalysisManager_->cd("Charge/" + planeName);

        /*----------------------------------------------------------------------Landau distributions-----------------------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("Landau");

        hName  = "hCellLandau_"                                               + planeName;
        hTitle = "Charge distribution in a fiducial window without size 1 cluster counted"  + planeName;
        hCellLandau_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hCellLandauSinglePixel_"                                               + planeName;
        hTitle = "Charge distribution for each pixel in any cluster in a fiducial window "  + planeName;
        hCellLandauSinglePixel_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize1_"                                       + planeName;
        hTitle = "Charge distribution only for size 1 cluster "                       + planeName;
        hLandauClusterSize1_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize2_"                                       + planeName;
        hTitle = "Charge distribution only for size 2 cluster "                   + planeName;
        hLandauClusterSize2_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize2sameCol_"                                + planeName;
        hTitle = "Charge distribution for 2 ajacent hits on the same column " + planeName;
        hLandauClusterSize2sameCol_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize2sameRow_"                                + planeName;
        hTitle = "Charge distribution for 2 ajacent hits on the same row "    + planeName;
        hLandauClusterSize2sameRow_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize3_"                                       + planeName;
        hTitle = "Charge distribution only for size 3 cluster "                   + planeName;
        hLandauClusterSize3_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize3sameCol_"                                + planeName;
        hTitle = "Charge distribution for 3 ajacent hits on the same column " + planeName;
        hLandauClusterSize3sameCol_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize3sameRow_"                                + planeName;
        hTitle = "Charge distribution for 3 ajacent hits on the same row "    + planeName;
        hLandauClusterSize3sameRow_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        theAnalysisManager_->cd("Charge/" + planeName);

        /*------------------------------------------------------------------------2D cell charge--------------------------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("2DCellCharge");

        hName  = "h2DallTracks_"                   + planeName;
        hTitle = "Cell charge normalization "      + planeName;
        h2DallTracks_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));


        hName  = "h2DCellChargeNorm_"              + planeName;
        hTitle = "Cell charge normalization 2"     + planeName;
        h2DCellChargeNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "h2DCellCharge_"                  + planeName;
        hTitle = "Cell charge 2D distribution "    + planeName;
        h2DCellCharge_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        // START Cell Charge Odd/Even
        // Odd rows:
        hName  = "h2DEvenTracksOddRows_"           + planeName;
        hTitle = "Cell charge normalization even columns odd rows no window "   + planeName;
        h2DEvenTracksOddRows_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DOddTracksOddRows_"            + planeName;
        hTitle = "Cell charge normalization odd columns odd rows no window "    + planeName;
        h2DOddTracksOddRows_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DCellChargeEvenColumnsOddRows_"+ planeName;
        hTitle = "Cell charge 2D distribution even columns odd rows no window " + planeName;
        h2DCellChargeEvenColumnsOddRows_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DCellChargeOddColumnsOddRows_" + planeName;
        hTitle = "Cell charge 2D distribution odd columns odd rows no window "  + planeName;
        h2DCellChargeOddColumnsOddRows_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        // Even rows:
        hName  = "h2DEvenTracksEvenRows_"           + planeName;
        hTitle = "Cell charge normalization even columns even rows no window "  + planeName;
        h2DEvenTracksEvenRows_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DOddTracksEvenRows_"            + planeName;
        hTitle = "Cell charge normalization odd columns even rows no window "   + planeName;
        h2DOddTracksEvenRows_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DCellChargeEvenColumnsEvenRows_"+ planeName;
        hTitle = "Cell charge 2D distribution even columns even rows no window "+ planeName;
        h2DCellChargeEvenColumnsEvenRows_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DCellChargeOddColumnsEvenRows_" + planeName;
        hTitle = "Cell charge 2D distribution even columns even rows no window "+ planeName;
        h2DCellChargeOddColumnsEvenRows_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        // row independent
        hName  = "h2DCellChargeEvenColumns_"+ planeName;
        hTitle = "Cell charge 2D distribution even columns no window "+ planeName;
        h2DCellChargeEvenColumns_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DCellChargeOddColumns_" + planeName;
        hTitle = "Cell charge 2D distribution even columns no window "+ planeName;
        h2DCellChargeOddColumns_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        // 4 Cells
        hName  = "h4CellChargeFullRange_"+ planeName;
        hTitle = "4 Cell charge 2D distribution full range no window "+ planeName;
        h4CellChargeFullRange_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(),  ((int)2 * resXRange/5), -resXRange, resXRange, ((int)2 * resYRange/5), -resYRange, resYRange)));


        // END Cell Charge Odd/Even

        hName  = "h2DCellChargeNum_"                          + planeName;
        hTitle = "Cell charge 2D distribution not normalized " + planeName;
        h2DCellChargeNum_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "h2DCellChargeNormSize2_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 2 "     + planeName;
        h2DCellChargeNormSize2_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h2DCellChargeNormSize1_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 1 "     + planeName;
        h2DCellChargeNormSize1_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h4CellsAllTracks_"               + planeName;
        hTitle = "4 cells charge normalization "   + planeName;
        h4CellsAllTracks_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h2DCellChargeNormSize2Up_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 2 on the upper part of the revelator"     + planeName;
        h2DCellChargeNormSize2Up_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h2DCellChargeNormSize1Up_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 1 on the upper part of the revelator"     + planeName;
        h2DCellChargeNormSize1Up_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h2DCellChargeNormSize2Down_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 2 on the lower part of the revelator"     + planeName;
        h2DCellChargeNormSize2Down_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h2DCellChargeNormSize1Down_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 1 on the lower part of the revelator"     + planeName;
        h2DCellChargeNormSize1Down_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "hCellChargeCoarse_"              + planeName;
        hTitle = "Cell charge 2D distribution "    + planeName;
        hCellChargeCoarse_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/10, -(resXRange/2), resXRange/2, (int)resYRange/10, -(resYRange/2), resYRange/2)));

        hName  = "hCellChargeCoarseNorm_"          + planeName;
        hTitle = "Cell charge normalization "      + planeName;
        hCellChargeCoarseNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/10, -(resXRange/2), resXRange/2, (int)resYRange/10, -(resYRange/2), resYRange/2)));

        hName  = "h4CellsChargeNorm_"              + planeName;
        hTitle = "4 cells charge normalization 2 " + planeName;
        h4CellsChargeNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "h4CellsCharge_"                  + planeName;
        hTitle = "4 cells charge 2D distribution " + planeName;
        h4CellsCharge_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "h4HitsCharge_"                   + planeName;
        hTitle = "4 hits charge 2D distribution "  + planeName;
        h4HitsCharge_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -resXRange, resXRange, (int)resYRange/5, -resYRange, resYRange)));

        hName  = "h4Hits_"                         + planeName;
        hTitle = "4 hits 2D distribution "         + planeName;
        h4Hits_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -resXRange, resXRange, (int)resYRange/5, -resYRange, resYRange)));

        hName  = "hCutsControl_"                             + planeName;
        hTitle = "Events surviving different level of cuts " + planeName;
        hCutsControl_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 6, 0, 6 )));

        hName  = "hHitsNotOnRowCol_"                  + planeName;
        hTitle = "Distribution of hits with the hit pixel not equal to the predicted "    + planeName;
        hHitsNotOnRowCol_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "hChargeNotOnRowCol_" + planeName;
        hTitle = "Charge distributionn for hits whose tracks don't point on the same pixel " + planeName;
        hChargeNotOnRowCol_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        theAnalysisManager_->cd("Charge/" + planeName);

        /*------------------------------------------------------------------------2D cell charge landau--------------------------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("2DCellChargeLandau");

        std::stringstream sName;
        hCellChargeCoarseLandau_.push_back(std::vector<TH1F*>());
        for(int x=0; x<(int)resXRange/10; x++)
            for(int y=0; y<(int)resYRange/10; y++)
            {
                sName.str("");
                sName << -(resXRange/2) + 5 + x*10 << "," << -(resYRange/2) + 5 + y*10;
                hName  = "hCellChargeCoarseLandau_"  + sName.str()  + planeName;
                hTitle = "Cell charge Landau ("      + sName.str()  + ")" + planeName;
                hCellChargeCoarseLandau_[hCellChargeCoarseLandau_.size()-1].push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));
            }


        theAnalysisManager_->cd("Charge/" + planeName);

        /*--------------------------------------------------------------------------2D cell charge - X coordinate---------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("XcellCharge2D");

        hName  = "h2DXcellCharge_"                                      + planeName;
        hTitle = "Predicted cell charge vs. X coordinate "              + planeName;
        h2DXcellCharge_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2, 100, -10000, 100000)));

        hName  = "h2DXcellChargeSumLE2_"                                + planeName;
        hTitle = "Up to 2 adjacent hits total charge vs. X coordinate " + planeName;
        h2DXcellChargeSumLE2_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2, 100, -10000, 100000)));

        hName  = "h2DXcellChargeSumLE3_"                                + planeName;
        hTitle = "Up to 3 adjacent hits total charge vs. X coordinate " + planeName;
        h2DXcellChargeSumLE3_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2, 100, -10000, 100000)));

        hName  = "h2DXcellDoubleHits_"                                  + planeName;
        hTitle = "Double hits distribution vs. X coordinate "           + planeName;
        h2DXcellDoubleHits_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "h2DXcellSingleHits_"                                  + planeName;
        hTitle = "Single hits distribution vs. X coordinate "           + planeName;
        h2DXcellSingleHits_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "hHitsNotONRowColVsXSlope_" + planeName;
        hTitle = "x coordinate vs x slope for hits whose tracks don't point at the same pixel" + planeName;
        hHitsNotONRowColVsXSlope_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2, 200, -0.000015,  0.000015)));

        hName  = "hHitsNotONRowColProjX_" + planeName;
        hTitle = "x distribution for hits whose tracks don't point at the same pixel" + planeName;
        hHitsNotOnRowColProjX_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        theAnalysisManager_->cd("Charge/" + planeName);

        /*------------------------------------------------------------------------2D cell charge - Y coordinate----------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("YcellCharge2D");

        hName  = "h2DYcellCharge_"                                      + planeName;
        hTitle = "Predicted cell charge vs. Y coordinate "              + planeName;
        h2DYcellCharge_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1., 100, -10000, 100000)));

        hName  = "h2DYcellChargeSumLE2_"                                + planeName;
        hTitle = "Up to 2 adjacent hits total charge vs. Y coordinate " + planeName;
        h2DYcellChargeSumLE2_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1., 100, -10000, 100000)));

        hName  = "h2DYcellChargeSumLE3_"                                + planeName;
        hTitle = "Up to 3 adjacent hits total charge vs. Y coordinate " + planeName;
        h2DYcellChargeSumLE3_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1., 100, -10000, 100000)));

        hName  = "h2DYcellDoubleHits_"                                  + planeName;
        hTitle = "Double hits vs. Y coordinate "                        + planeName;
        h2DYcellDoubleHits_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1., (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "h2DYcellSingleHits_"                                  + planeName;
        hTitle = "Single hits vs. Y coordinate "                        + planeName;
        h2DYcellSingleHits_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1., (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "hHitsNotONRowColVsYSlope_" + planeName;
        hTitle = "y coordinate vs y slope for hits whose tracks don't point at the same pixel" + planeName;
        hHitsNotONRowColVsYSlope_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resYRange/2,-(resYRange/2),resYRange/2, 200, -0.000015,  0.000015)));

        hName  = "hHitsNotONRowColProjY_" + planeName;
        hTitle = "y distribution for hits whose tracks don't point at the same pixel" + planeName;
        hHitsNotOnRowColProjY_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        theAnalysisManager_->cd("Charge/" + planeName);

        /*---------------------------------------------------------------------------1D cell charge - X coordinate--------------------------------------------------------------------- -----------------------*/
        theAnalysisManager_->mkdir("XcellCharge1D");

        hName  = "h1DXcellCharge_"                                                             + planeName;
        hTitle = "Predicted cell charge - X coordinate (normalized to hits) "                  + planeName;
        h1DXcellCharge_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellChargeNormToAll_"                                                    + planeName;
        hTitle = "Predicted cell charge - X coordinate (normalized to tracks) "                + planeName;
        h1DXcellChargeNormToAll_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellChargeSumLE2_"                                                       + planeName;
        hTitle = "Up to 2 adjacent hits total charge - X coordinate (normalized to hits) "     + planeName;
        h1DXcellChargeSumLE2_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellChargeSumLE2NormToAll_"                                              + planeName;
        hTitle = "Up to 2 adjacent hits total charge - X coordinate (normalized to tracks) "   + planeName;
        h1DXcellChargeSumLE2NormToAll_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellChargeSumLE3_"                                                       + planeName;
        hTitle = "Up to 3 adjacent hits total charge - X coordinate (normalized to hits) "     + planeName;
        h1DXcellChargeSumLE3_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellChargeSumLE3NormToAll_"                                              + planeName;
        hTitle = "Up to 3 adjacent hits total charge - X coordinate (normalized to tracks) "   + planeName;
        h1DXcellChargeSumLE3NormToAll_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXallTracks_"                                                              + planeName;
        hTitle = "All tracks normalization - X coordinate "                                    + planeName;
        h1DXallTracks_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellChargeNorm_"                                                         + planeName;
        hTitle = "Predicted cell charge - X coordinate - all hits normalization "                + planeName;
        h1DXcellChargeNorm_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellChargeSumLE2Norm_"                                                   + planeName;
        hTitle = "Up to 2 adjacent hits total charge - X coordinate - all hits normalization " + planeName;
        h1DXcellChargeSumLE2Norm_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellChargeSumLE3Norm_"                                                   + planeName;
        hTitle = "Up to 3 adjacent hits total charge - X coordinate - all hits normalization " + planeName;
        h1DXcellChargeSumLE3Norm_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellSingleHits_"                                                         + planeName;
        hTitle = "Single hits distribution - X coordinate "                                    + planeName;
        h1DXcellSingleHits_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellDoubleHits_"                                                         + planeName;
        hTitle = "Double hits distribution - X coordinate "                                    + planeName;
        h1DXcellDoubleHits_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcell3Hits_"                                                              + planeName;
        hTitle = "3 adjacent hits distribution - X coordinate "                                + planeName;
        h1DXcell3Hits_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resXRange/2,-(resXRange/2),resXRange/2)));

        theAnalysisManager_->cd("Charge/" + planeName);

        /*---------------------------------------------------------------------------1D cell charge - Y coordinate-------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("YcellCharge1D");

        hName  = "h1DYcellCharge_"                                                             + planeName;
        hTitle = "Predicted cell charge - Y coordinate (normalized to hits) "                  + planeName;
        h1DYcellCharge_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellChargeNormToAll_"                                                    + planeName;
        hTitle = "Predicted cell charge - Y coordinate (normalized to tracks) "                + planeName;
        h1DYcellChargeNormToAll_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellChargeSumLE2_"                                                       + planeName;
        hTitle = "Up to 2 adjacent hits total charge - Y coordinate (normalized to hits) "     + planeName;
        h1DYcellChargeSumLE2_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellChargeSumLE2NormToAll_"                                              + planeName;
        hTitle = "Up to 2 adjacent hits total charge - Y coordinate (normalized to tracks) "   + planeName;
        h1DYcellChargeSumLE2NormToAll_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellChargeSumLE3_"                                                       + planeName;
        hTitle = "Up to 3 adjacent hits total charge - Y coordinate (normalized to hits) "     + planeName;
        h1DYcellChargeSumLE3_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellChargeSumLE3NormToAll_"                                              + planeName;
        hTitle = "Up to 3 adjacent hits total charge - Y coordinate (normalized to tracks)"    + planeName;
        h1DYcellChargeSumLE3NormToAll_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYallTracks_"                                                              + planeName;
        hTitle = "All tracks normalization - Y coordinate "                                    + planeName;
        h1DYallTracks_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellChargeNorm_"                                                         + planeName;
        hTitle = "Predicted cell charge - Y coordinate - all hits normalization "              + planeName;
        h1DYcellChargeNorm_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellChargeSumLE2Norm_"                                                   + planeName;
        hTitle = "Up to 2 adjacent hits total charge - Y coordinate - all hits normalization " + planeName;
        h1DYcellChargeSumLE2Norm_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellChargeSumLE3Norm_"                                                   + planeName;
        hTitle = "Up to 3 adjacent hits total charge - Y coordinate - all hits normalization " + planeName;
        h1DYcellChargeSumLE3Norm_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellSingleHits_"                                                         + planeName;
        hTitle = "Single hits distribution - Y coordinate "                                    + planeName;
        h1DYcellSingleHits_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellDoubleHits_"                                                         + planeName;
        hTitle = "Double hits distribution - Y coordinate "                                    + planeName;
        h1DYcellDoubleHits_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcell3Hits_"                                                              + planeName;
        hTitle = "3 adjacent hits distribution - Y coordinate "                                + planeName;
        h1DYcell3Hits_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        theAnalysisManager_->cd("Charge/" + planeName);

        /*----------------------------------------------------------------------------X Asimmetry-----------------------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("Xasimmetry");

        hName  = "hXasimmetry_"                                                     + planeName;
        hTitle = "Distribution of charge asimmetry values in X coordinate "         + planeName;
        hXasimmetry_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 100, -1, 1)));

        hName  = "hXasimmetry0_"                                                     + planeName;
        hTitle = "Distribution of one-sided charge asimmetry values in X coordinate "         + planeName;
        hXasimmetry0_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 100, 0, 1)));

        hName  = "h2DXAsimmetryLandau_"                                             + planeName;
        hTitle = "L/R charge asimmetry vs charge - X coordinate "                   + planeName;
        h2DXAsimmetryLandau_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 92, 5000, 120000, 20, -1, 1)));

        hName  = "h2DXcellChargeAsimmetry_"                                         + planeName;
        hTitle = "L/R charge asimmetry - X coordinate "                             + planeName;
        h2DXcellChargeAsimmetry_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2, -(resXRange/2), resXRange/2, 102, -1.1, 1.1)));

        hName  = "h2DXcellChargeAsimmetryInv_"                                      + planeName;
        hTitle = "L/R charge asimmetry - X coordinate "                             + planeName;
        h2DXcellChargeAsimmetryInv_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 102, -1.1, 1.1, 2*(int)resXRange/2, -(resXRange/2), resXRange/2)));

        hName  = "h2DXcellChargeAsimmetryUnconstrained_"                            + planeName;
        hTitle = "L/R unconstrained charge asimmetry - X coordinate "               + planeName;
        h2DXcellChargeAsimmetryUnconstrained_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/2, -(resXRange/2), resXRange/2, 100, -1, 1)));

        hName  = "h2DXcellChargeAsimmetryUnconstrainedInv_"                         + planeName;
        hTitle = "L/R unconstrained charge asimmetry - X coordinate "               + planeName;
        h2DXcellChargeAsimmetryUnconstrainedInv_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 20, -1, 1, (int)resXRange/2, -(resXRange/2), resXRange/2)));

        hName  = "h2DXcellChargeAsimmetrySizeLE2_"                                  + planeName;
        hTitle = "L/R charge asimmetry for cluster size <= 2 "                      + planeName;
        h2DXCellChargeAsimmetrySizeLE2_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 102, -1.1, 1.1)));

        hName = "h1DXEtaDerivativeDistribution_" + planeName;
        hTitle = "Distribution of eta derivative function for coordinate x, " + planeName;
        h1DXEtaDerivativeDistribution_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5));

        theAnalysisManager_->cd("Charge/" + planeName);

        /*---------------------------------------------------------------------------Y Asimmetry-------------------------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("Yasimmetry");

        hName  = "hYasimmetry_"                                                     + planeName;
        hTitle = "Distribution of charge asimmetry values in Y coordinate "         + planeName;
        hYasimmetry_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 100, -1, 1)));

        hName  = "hYasimmetry0_"                                                     + planeName;
        hTitle = "Distribution of one-sided charge asimmetry values in Y coordinate "         + planeName;
        hYasimmetry0_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 100, 0, 1)));

        hName  = "h2DYAsimmetryLandau_"                                             + planeName;
        hTitle = "L/R charge asimmetry vs charge - Y coordinate "                   + planeName;
        h2DYAsimmetryLandau_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 92, 5000, 120000, 20, -1, 1)));

        hName  = "h2DYcellChargeAsimmetry_"                                         + planeName;
        hTitle = "L/R charge asimmetry - Y coordinate "                             + planeName;
        h2DYcellChargeAsimmetry_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resYRange/2 - 1, -(resYRange/2) + 1, resYRange/2 - 1, 102, -1.1, 1.1)));

        hName  = "h2DYcellChargeAsimmetryInv_"                                      + planeName;
        hTitle = "L/R charge asimmetry - Y coordinate "                             + planeName;
        h2DYcellChargeAsimmetryInv_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 102, -1.1, 1.1, 2*(int)resYRange/2 - 1, -(resYRange/2) + 1, resYRange/2 - 1)));

        hName  = "h2DYcellChargeAsimmetryUnconstrained_"                            + planeName;
        hTitle = "L/R unconstrained charge asimmetry - Y coordinate "               + planeName;
        h2DYcellChargeAsimmetryUnconstrained_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(),(int)resYRange/2 - 1, -(resYRange/2) + 1, resYRange/2 - 1, 100, -1, 1)));

        hName  = "h2DYcellChargeAsimmetryUnconstrainedInv_"                         + planeName;
        hTitle = "L/R unconstrained charge asimmetry - Y coordinate "               + planeName;
        h2DYcellChargeAsimmetryUnconstrainedInv_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 20, -1, 1, (int)resYRange/2 - 1, -(resYRange/2) + 1, resYRange/2 - 1)));

        hName  = "h2DYcellChargeAsimmetrySizeLE2_"                                  + planeName;
        hTitle = "L/R charge asimmetry for cluster size <= 2 "                      + planeName;
        h2DYCellChargeAsimmetrySizeLE2_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5, 102, -1.1, 1.1)));

        hName = "h1DYEtaDerivativeDistribution_" + planeName;
        hTitle = "Distribution of eta derivative function for coordinate y, " + planeName;
        h1DYEtaDerivativeDistribution_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5));

        theAnalysisManager_->cd("Charge/" + planeName);

        /*---------------------------------------------------------------------------Run Number-------------------------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("RunNumber");
/*
        TMap * aMapX = NEW_THREADED(TMap);
        std::stringstream ss;
        for (int i = 888; i < 921; ++i)
        {
            TObject * iObj;
            ss.str("");
            ss << i;
            iObj->SetName(ss.str().c_str());
            *aMapX(iObj) = new TVectorT<std::pair<Float_t, Float_t>* >;
        }
        mXMeanCharge_.push_back(NEW_THREADED(aMapX));

        hName = "XMeanCharge_" + planeName;
        hTitle = "Mean charge x position as function of run number, " + planeName;
        hXMeanCharge_.push_back(new TH1F(hName.c_str(), hTitle.c_str(), 35, 887, 922));

        TMap * aMapY = NEW_THREADED(TMap);
        for (int i = 888; i < 921; ++i)
        {
            TObject * iObj;
            ss.str("");
            ss << i;
            iObj->SetName(ss.str().c_str());
            *aMapY(iObj) = new TVectorT<std::pair<Float_t, Float_t>* >;
        }
        mYMeanCharge_.push_back(NEW_THREADED(aMapY);

        hName = "YMeanCharge_" + planeName;
        hTitle = "Mean charge y position as function of run number, " + planeName;
        hYMeanCharge_.push_back(new TH1F(hName.c_str(), hTitle.c_str(), 35, 887, 922));

        No time for this shit! */
        std::map<int, TH2F*> aMap;
        std::map<int, TH2F*> aMapNorm;
        std::stringstream sss;
        int minRunNumber = 1000000000;
        int maxRunNumber = -1;
        for (std::vector<int>::iterator it = fRunNumbers_.begin(); it != fRunNumbers_.end(); ++it)
        {
            sss.str("");
            sss << *it;
            hName = "xyChargeDistribution_run" + sss.str() + "_" + planeName;
            hTitle = "Charge distribution, run number #" + sss.str() + ", " + planeName;
            aMap[*it] = NEW_THREADED(TH2F (hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5));
            hName = "ChargeNormalization_run" + sss.str() + "_" + planeName;
            hTitle = "Normalization histogram, run number #" + sss.str() + ", " + planeName;
            aMapNorm[*it] = NEW_THREADED(TH2F (hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5));

            if (*it > maxRunNumber) maxRunNumber = *it;
            if (*it < minRunNumber) minRunNumber = *it;
        }
        mXYMeanCharge_.push_back(aMap);
        mXYNorm_      .push_back(aMapNorm);

        hName = "XMeanCharge_" + planeName;
        hTitle = "Mean charge along x as function of run number, " + planeName;
        hXMeanCharge_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), maxRunNumber-minRunNumber+1, minRunNumber, maxRunNumber+1));
        hName = "YMeanCharge_" + planeName;
        hTitle = "Mean charge along y as function of run number, " + planeName;
        hYMeanCharge_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), maxRunNumber-minRunNumber+1, minRunNumber, maxRunNumber+1));

        theAnalysisManager_->cd("Charge/" + planeName);

        // START Window
        theAnalysisManager_->mkdir("Windowed");
        /*--------------------------------------------------------------------------cluster size-----------------------------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("ClusterSize");

        hName  = "hClusterSizeWindowed_"                   + planeName;
        hTitle = "Cluster size distribution "      + planeName;
        hClusterSizeWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 10, 0, 10)));

        hName  = "hClusterSizeCutsWindowed_"               + planeName;
        hTitle = "Cluster size distribution (standard cut) "+ planeName;
        hClusterSizeCutsWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 10, 0, 10)));

        hName  = "hNumberOfColsWindowed_"                  + planeName;
        hTitle = "Number of columns distribution " + planeName;
        hNumberOfColsWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 10, 0, 10)));

        hName  = "hNumberOfRowsWindowed_"                  + planeName;
        hTitle = "Number of rows distribution "    + planeName;
        hNumberOfRowsWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 10, 0, 10)));

        theAnalysisManager_->cd("Charge/" + planeName + "/Windowed");

        /*----------------------------------------------------------------------Landau distributions-----------------------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("Landau");

        hName  = "hLandauClusterSize1Windowed_"                                       + planeName;
        hTitle = "Charge distribution only for size 1 cluster "                       + planeName;
        hLandauClusterSize1Windowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hCellLandauWindowed_"                                               + planeName;
        hTitle = "Charge distribution without size 1 cluster counted "  + planeName;
        hCellLandauWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hCellLandauSinglePixelWindowed_"                                               + planeName;
        hTitle = "Charge distribution for each pixel in any cluster in a fiducial window "  + planeName;
        hCellLandauSinglePixelWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize2Windowed_"                                       + planeName;
        hTitle = "Charge distribution only for size 2 cluster "                   + planeName;
        hLandauClusterSize2Windowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize2sameColWindowed_"                                + planeName;
        hTitle = "Charge distribution for 2 ajacent hits on the same column " + planeName;
        hLandauClusterSize2sameColWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize2sameRowWindowed_"                                + planeName;
        hTitle = "Charge distribution for 2 ajacent hits on the same row "    + planeName;
        hLandauClusterSize2sameRowWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize3Windowed_"                                       + planeName;
        hTitle = "Charge distribution only for size 3 cluster "                   + planeName;
        hLandauClusterSize3Windowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize3sameColWindowed_"                                + planeName;
        hTitle = "Charge distribution for 3 ajacent hits on the same column " + planeName;
        hLandauClusterSize3sameColWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        hName  = "hLandauClusterSize3sameRowWindowed_"                                + planeName;
        hTitle = "Charge distribution for 3 ajacent hits on the same row "    + planeName;
        hLandauClusterSize3sameRowWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        theAnalysisManager_->cd("Charge/" + planeName + "/Windowed");

        /*------------------------------------------------------------------------2D cell charge--------------------------------------------------------------------------------------------------------------*/
        theAnalysisManager_->mkdir("2DCellCharge");

        hName  = "h2DallTracksWindowed_"                   + planeName;
        hTitle = "Cell charge normalization "      + planeName;
        h2DallTracksWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));


        hName  = "h2DCellChargeNormWindowed_"              + planeName;
        hTitle = "Cell charge normalization 2"     + planeName;
        h2DCellChargeNormWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "h2DCellChargeWindowed_"                  + planeName;
        hTitle = "Cell charge 2D distribution "    + planeName;
        h2DCellChargeWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        // START Cell Charge Odd/Even
        // Odd rows:
        hName  = "h2DEvenTracksOddRowsWindowed_"           + planeName;
        hTitle = "Cell charge normalization even columns odd rows no window "   + planeName;
        h2DEvenTracksOddRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DOddTracksOddRowsWindowed_"            + planeName;
        hTitle = "Cell charge normalization odd columns odd rows no window "    + planeName;
        h2DOddTracksOddRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DCellChargeEvenColumnsOddRowsWindowed_"+ planeName;
        hTitle = "Cell charge 2D distribution even columns odd rows no window " + planeName;
        h2DCellChargeEvenColumnsOddRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DCellChargeOddColumnsOddRowsWindowed_" + planeName;
        hTitle = "Cell charge 2D distribution odd columns odd rows no window "  + planeName;
        h2DCellChargeOddColumnsOddRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        // Even rows:
        hName  = "h2DEvenTracksEvenRowsWindowed_"           + planeName;
        hTitle = "Cell charge normalization even columns even rows no window "  + planeName;
        h2DEvenTracksEvenRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DOddTracksEvenRowsWindowed_"            + planeName;
        hTitle = "Cell charge normalization odd columns even rows no window "   + planeName;
        h2DOddTracksEvenRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DCellChargeEvenColumnsEvenRowsWindowed_"+ planeName;
        hTitle = "Cell charge 2D distribution even columns even rows no window "+ planeName;
        h2DCellChargeEvenColumnsEvenRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DCellChargeOddColumnsEvenRowsWindowed_" + planeName;
        hTitle = "Cell charge 2D distribution even columns even rows no window "+ planeName;
        h2DCellChargeOddColumnsEvenRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        // row independent
        hName  = "h2DCellChargeEvenColumnsWindowed_"+ planeName;
        hTitle = "Cell charge 2D distribution even columns no window "+ planeName;
        h2DCellChargeEvenColumnsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        hName  = "h2DCellChargeOddColumnsWindowed_" + planeName;
        hTitle = "Cell charge 2D distribution even columns no window "+ planeName;
        h2DCellChargeOddColumnsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -(resXRange/2), resXRange/2, (int)resYRange/5, -(resYRange/2), resYRange/2)));

        // 4 Cells
        hName  = "h4CellChargeFullRangeWindowed_"+ planeName;
        hTitle = "4 Cell charge 2D distribution full range no window "+ planeName;
        h4CellChargeFullRangeWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(),  ((int)2 * resXRange/5), -resXRange, resXRange, ((int)2 * resYRange/5), -resYRange, resYRange)));


        // END Cell Charge Odd/Even

        hName  = "h2DCellChargeNumWindowed_"                          + planeName;
        hTitle = "Cell charge 2D distribution not normalized " + planeName;
        h2DCellChargeNumWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "h2DCellChargeNormSize2Windowed_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 2 "     + planeName;
        h2DCellChargeNormSize2Windowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h2DCellChargeNormSize1Windowed_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 1 "     + planeName;
        h2DCellChargeNormSize1Windowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h4CellsAllTracksWindowed_"               + planeName;
        hTitle = "4 cells charge normalization "   + planeName;
        h4CellsAllTracksWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h2DCellChargeNormSize2UpWindowed_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 2 on the upper part of the revelator"     + planeName;
        h2DCellChargeNormSize2UpWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h2DCellChargeNormSize1UpWindowed_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 1 on the upper part of the revelator"     + planeName;
        h2DCellChargeNormSize1UpWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h2DCellChargeNormSize2DownWindowed_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 2 on the lower part of the revelator"     + planeName;
        h2DCellChargeNormSize2DownWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));
        hName  = "h2DCellChargeNormSize1DownWindowed_"              + planeName;
        hTitle = "Cell charge normalization for cluster size 1 on the lower part of the revelator"     + planeName;
        h2DCellChargeNormSize1DownWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), 2*(int)resXRange/2 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, 2*(int)resYRange/2 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "hCellChargeCoarseWindowed_"              + planeName;
        hTitle = "Cell charge 2D distribution "    + planeName;
        hCellChargeCoarseWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/10, -(resXRange/2), resXRange/2, (int)resYRange/10, -(resYRange/2), resYRange/2)));

        hName  = "hCellChargeCoarseNormWindowed_"          + planeName;
        hTitle = "Cell charge normalization "      + planeName;
        hCellChargeCoarseNormWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/10, -(resXRange/2), resXRange/2, (int)resYRange/10, -(resYRange/2), resYRange/2)));

        hName  = "h4CellsChargeNormWindowed_"              + planeName;
        hTitle = "4 cells charge normalization 2 " + planeName;
        h4CellsChargeNormWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "h4CellsChargeWindowed_"                  + planeName;
        hTitle = "4 cells charge 2D distribution " + planeName;
        h4CellsChargeWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "h4HitsChargeWindowed_"                   + planeName;
        hTitle = "4 hits charge 2D distribution "  + planeName;
        h4HitsChargeWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -resXRange, resXRange, (int)resYRange/5, -resYRange, resYRange)));

        hName  = "h4HitsWindowed_"                         + planeName;
        hTitle = "4 hits 2D distribution "         + planeName;
        h4HitsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5, -resXRange, resXRange, (int)resYRange/5, -resYRange, resYRange)));

        hName  = "hCutsControlWindowed_"                             + planeName;
        hTitle = "Events surviving different level of cuts " + planeName;
        hCutsControlWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 6, 0, 6 )));

        hName  = "hHitsNotOnRowColWindowed_"                  + planeName;
        hTitle = "Distribution of hits with the hit pixel not equal to the predicted "    + planeName;
        hHitsNotOnRowColWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), (int)resXRange/5 - 1, -(resXRange/2) + 2.5, resXRange/2 - 2.5, (int)resYRange/5 - 1, -(resYRange/2) + 2.5, resYRange/2 - 2.5)));

        hName  = "hChargeNotOnRowColWindowed_" + planeName;
        hTitle = "Charge distributionn for hits whose tracks don't point on the same pixel " + planeName;
        hChargeNotOnRowColWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 200, -10000, 100000)));

        theAnalysisManager_->cd("Charge/" + planeName);

        // END Window
    }
}

