#include "Data.h"

#include <TTree.h>
#include <TBranch.h>

//Index
//using namespace std;

//==========================================================================================================
Data::Data()
{ 
}

//==========================================================================================================
Data::~Data()
{
}

//==========================================================================================================
void Data::setBranchAddress(TTree* tree)
{
  tree->SetBranchAddress("eventNumber"                     , &eventNumber_  	               );
  tree->SetBranchAddress("runNumber"                       , &runNumber_    	               );
  tree->SetBranchAddress("numberOfTracks"                  , &numberOfTracks_	               );
  tree->SetBranchAddress("trackNumber"                     , &trackNumber_  	               );
  tree->SetBranchAddress("numberOfTelescopeHits"           , &numberOfTelescopeHits_           );
  tree->SetBranchAddress("numberOfTelescopeClustersSizeLE2", &numberOfTelescopeClustersSizeLE2_);
  tree->SetBranchAddress("ndof"                            , &ndof_ 		                   );
  tree->SetBranchAddress("chi2"                            , &chi2_ 		                   );
  tree->SetBranchAddress("xIntercept"                      , &xIntercept_		               );
  tree->SetBranchAddress("xSigmaIntercept"                 , &xSigmaIntercept_	               );
  tree->SetBranchAddress("yIntercept"                      , &yIntercept_		               );
  tree->SetBranchAddress("ySigmaIntercept"                 , &ySigmaIntercept_	               );
  tree->SetBranchAddress("xSlope"                          , &xSlope_		                   );
  tree->SetBranchAddress("xSigmaSlope"                     , &xSigmaSlope_  	               );
  tree->SetBranchAddress("ySlope"                          , &ySlope_		                   );
  tree->SetBranchAddress("ySigmaSlope"                     , &ySigmaSlope_  	               );
  tree->SetBranchAddress("hasHit"                          , &hasHit_		                   );
  tree->SetBranchAddress("belongsToTrack"                  , &belongsToTrack_	               );
  tree->SetBranchAddress("isInDetector"                    , &isInDetector_ 	               );
  tree->SetBranchAddress("isInDetectorUnconstrained"       , &isInDetectorUnconstrained_       );
  tree->SetBranchAddress("ndofUnconstrained"  	           , &ndofUnconstrained_               );
  tree->SetBranchAddress("chi2Unconstrained"               , &chi2Unconstrained_               );
  tree->SetBranchAddress("xInterceptUnconstrained"         , &xInterceptUnconstrained_         );
  tree->SetBranchAddress("xSigmaInterceptUnconstrained"    , &xSigmaInterceptUnconstrained_    );
  tree->SetBranchAddress("yInterceptUnconstrained"         , &yInterceptUnconstrained_         );
  tree->SetBranchAddress("ySigmaInterceptUnconstrained"    , &ySigmaInterceptUnconstrained_    );
  tree->SetBranchAddress("xSlopeUnconstrained"             , &xSlopeUnconstrained_             );
  tree->SetBranchAddress("xSigmaSlopeUnconstrained"        , &xSigmaSlopeUnconstrained_        );
  tree->SetBranchAddress("ySlopeUnconstrained"             , &ySlopeUnconstrained_             );
  tree->SetBranchAddress("ySigmaSlopeUnconstrained"        , &ySigmaSlopeUnconstrained_        );
  tree->SetBranchAddress("clusterSize"                     , &clusterSize_  	               );
  tree->SetBranchAddress("numberOfCols"                    , &numberOfCols_ 	               );
  tree->SetBranchAddress("numberOfRows"                    , &numberOfRows_ 	               );
  tree->SetBranchAddress("clusterCharge"                   , &clusterCharge_	               );
  tree->SetBranchAddress("meanCol"                         , &meanCol_		                   );
  tree->SetBranchAddress("meanRow"                         , &meanRow_		                   );
  tree->SetBranchAddress("colPredicted"                    , &colPredicted_ 	               );
  tree->SetBranchAddress("rowPredicted"                    , &rowPredicted_ 	               );
  tree->SetBranchAddress("xPitchLocal"                     , &xPitchLocal_  	               );
  tree->SetBranchAddress("yPitchLocal"                     , &yPitchLocal_  	               );
  tree->SetBranchAddress("xPitchGlobal"                    , &xPitchGlobal_ 	               );
  tree->SetBranchAddress("yPitchGlobal"                    , &yPitchGlobal_ 	               );
  tree->SetBranchAddress("xMeasuredLocal"                  , &xMeasuredLocal_	               );
  tree->SetBranchAddress("yMeasuredLocal"                  , &yMeasuredLocal_	               );
  tree->SetBranchAddress("xMeasuredGlobal"                 , &xMeasuredGlobal_	               );
  tree->SetBranchAddress("yMeasuredGlobal"                 , &yMeasuredGlobal_	               );
  tree->SetBranchAddress("xPredictedLocal"                 , &xPredictedLocal_	               );
  tree->SetBranchAddress("yPredictedLocal"                 , &yPredictedLocal_	               );
  tree->SetBranchAddress("xPredictedGlobal"                , &xPredictedGlobal_	               );
  tree->SetBranchAddress("yPredictedGlobal"                , &yPredictedGlobal_	               );
  tree->SetBranchAddress("xErrorMeasuredLocal"             , &xErrorMeasuredLocal_             );
  tree->SetBranchAddress("yErrorMeasuredLocal"             , &yErrorMeasuredLocal_             );
  tree->SetBranchAddress("xErrorMeasuredGlobal"            , &xErrorMeasuredGlobal_            );
  tree->SetBranchAddress("yErrorMeasuredGlobal"            , &yErrorMeasuredGlobal_            );
  tree->SetBranchAddress("xErrorPredictedLocal"            , &xErrorPredictedLocal_            );
  tree->SetBranchAddress("yErrorPredictedLocal"            , &yErrorPredictedLocal_            );
  tree->SetBranchAddress("xErrorPredictedGlobal"           , &xErrorPredictedGlobal_           );
  tree->SetBranchAddress("yErrorPredictedGlobal"           , &yErrorPredictedGlobal_           );
  tree->SetBranchAddress("xTrackResidualLocal"             , &xTrackResidualLocal_             );
  tree->SetBranchAddress("yTrackResidualLocal"             , &yTrackResidualLocal_             );
  tree->SetBranchAddress("xTrackResidualGlobal"            , &xTrackResidualGlobal_            );
  tree->SetBranchAddress("yTrackResidualGlobal"            , &yTrackResidualGlobal_            );
  tree->SetBranchAddress("xPixelResidualLocal"             , &xPixelResidualLocal_             );
  tree->SetBranchAddress("yPixelResidualLocal"             , &yPixelResidualLocal_             );
  tree->SetBranchAddress("xPixelResidualGlobal"            , &xPixelResidualGlobal_            );
  tree->SetBranchAddress("yPixelResidualGlobal"            , &yPixelResidualGlobal_            );
  tree->SetBranchAddress("xPixelResidualLocalUnconstrained", &xPixelResidualLocalUnconstrained_);
  tree->SetBranchAddress("yPixelResidualLocalUnconstrained", &yPixelResidualLocalUnconstrained_);
  tree->SetBranchAddress("xPixelPitchLocalUnconstrained"   , &xPixelPitchLocalUnconstrained_   );
  tree->SetBranchAddress("yPixelPitchLocalUnconstrained"   , &yPixelPitchLocalUnconstrained_   );
  tree->SetBranchAddress("clusterPixelRow"                 , &clusterPixelRow_	               );
  tree->SetBranchAddress("clusterPixelCol"                 , &clusterPixelCol_	               );
  tree->SetBranchAddress("clusterPixelCharge"              , &clusterPixelCharge_	           );
  tree->SetBranchAddress("xClusterPixelCenterLocal"        , &xClusterPixelCenterLocal_        );
  tree->SetBranchAddress("yClusterPixelCenterLocal"        , &yClusterPixelCenterLocal_        );
  tree->SetBranchAddress("xClusterPixelCenterGlobal"       , &xClusterPixelCenterGlobal_       );
  tree->SetBranchAddress("yClusterPixelCenterGlobal"       , &yClusterPixelCenterGlobal_       );
  tree->SetBranchAddress("isPixelCalibrated"               , &isPixelCalibrated_	           );
};

//==========================================================================================================
void Data::branch(TTree* tree)
{
  tree->Branch("eventNumber"                     , &eventNumber_		             , "eventNumber/I" 		                   );
  tree->Branch("runNumber"                       , &runNumber_   		             , "eventNumber/I" 		                   );
  tree->Branch("numberOfTracks"                  , &numberOfTracks_ 	             , "numberOfTracks/I"		               );
  tree->Branch("trackNumber"                     , &trackNumber_		             , "trackNumber/I"                         );
  tree->Branch("numberOfTelescopeHits"           , &numberOfTelescopeHits_           , "numberOfTelescopeHits/I"               );
  tree->Branch("numberOfTelescopeClustersSizeLE2", &numberOfTelescopeClustersSizeLE2_, "numberOfTelescopeClustersSizeLE2/I"    );
  tree->Branch("ndof"	                         , &ndof_			                 , "ndof/I"			                       );
  tree->Branch("chi2"	                         , &chi2_			                 , "chi2/F"			                       );
  tree->Branch("xIntercept"                      , &xIntercept_		                 , "xIntercept/D"  		                   );
  tree->Branch("xSigmaIntercept"                 , &xSigmaIntercept_	             , "xSigmaIntercept/D"		               );
  tree->Branch("yIntercept"                      , &yIntercept_		                 , "yIntercept/D"  	                       );
  tree->Branch("ySigmaIntercept"                 , &ySigmaIntercept_	             , "ySigmaIntercept/D"		               );
  tree->Branch("xSlope"                          , &xSlope_ 		                 , "xSlope/D"			                   );
  tree->Branch("xSigmaSlope"                     , &xSigmaSlope_		             , "xSigmaSlope/D" 		                   );
  tree->Branch("ySlope"                          , &ySlope_ 		                 , "ySlope/D"			                   );
  tree->Branch("ySigmaSlope"                     , &ySigmaSlope_		             , "ySigmaSlope/D" 		                   );
  tree->Branch("hasHit"                          , &hasHit_ 		                 , "hasHit[10]/O"  		                   );
  tree->Branch("belongsToTrack"                  , &belongsToTrack_ 	             , "belongsToTrack[10]/O"  	               );
  tree->Branch("isInDetector"                    , &isInDetector_		             , "isInDetector[10]/O"		               );
  tree->Branch("isInDetectorUnconstrained"       , &isInDetectorUnconstrained_	     , "isInDetectorUnconstrained[10]/O"       );
  tree->Branch("ndofUnconstrained"	             , &ndofUnconstrained_	             , "ndofUnconstrained[10]/I"	           );
  tree->Branch("chi2Unconstrained"	             , &chi2Unconstrained_	             , "chi2Unconstrained[10]/F"	           );
  tree->Branch("xInterceptUnconstrained"         , &xInterceptUnconstrained_         , "xInterceptUnconstrained[10]/D"         );
  tree->Branch("xSigmaInterceptUnconstrained"    , &xSigmaInterceptUnconstrained_    , "xSigmaInterceptUnconstrained[10]/D"    );
  tree->Branch("yInterceptUnconstrained"         , &yInterceptUnconstrained_         , "yInterceptUnconstrained[10]/D"         );
  tree->Branch("ySigmaInterceptUnconstrained"    , &ySigmaInterceptUnconstrained_    , "ySigmaInterceptUnconstrained[10]/D"    );
  tree->Branch("xSlopeUnconstrained"	         , &xSlopeUnconstrained_	         , "xSlopeUnconstrained[10]/D"	           );
  tree->Branch("xSigmaSlopeUnconstrained"        , &xSigmaSlopeUnconstrained_        , "xSigmaSlopeUnconstrained[10]/D"        );
  tree->Branch("ySlopeUnconstrained"	         , &ySlopeUnconstrained_	         , "ySlopeUnconstrained[10]/D"	           );
  tree->Branch("ySigmaSlopeUnconstrained"        , &ySigmaSlopeUnconstrained_        , "ySigmaSlopeUnconstrained[10]/D"        );
  tree->Branch("clusterSize"	                 , &clusterSize_		             , "clusterSize[10]/I"		               );
  tree->Branch("numberOfCols"	                 , &numberOfCols_		             , "numberOfCols[10]/i"		               );
  tree->Branch("numberOfRows"	                 , &numberOfRows_		             , "numberOfRows[10]/i"		               );
  tree->Branch("clusterCharge"                   , &clusterCharge_  	             , "clusterCharge[10]/I"		           );
  tree->Branch("meanCol"	                     , &meanCol_		                 , "meanCol[10]/F" 		                   );
  tree->Branch("meanRow"	                     , &meanRow_		                 , "meanRow[10]/F" 		                   );
  tree->Branch("colPredicted"                    , &colPredicted_		             , "colPredicted[10]/I"		               );
  tree->Branch("rowPredicted"                    , &rowPredicted_		             , "rowPredicted[10]/I"		               );
  tree->Branch("xPitchLocal"                     , &xPitchLocal_		             , "xPitchLocal[10]/F"		               );
  tree->Branch("yPitchLocal"                     , &yPitchLocal_		             , "yPitchLocal[10]/F"		               );
  tree->Branch("xPitchGlobal"                    , &xPitchGlobal_		             , "xPitchGlobal[10]/F"		               );
  tree->Branch("yPitchGlobal"                    , &yPitchGlobal_		             , "yPitchGlobal[10]/F"		               );
  tree->Branch("xMeasuredLocal"	                 , &xMeasuredLocal_ 	             , "xMeasuredLocal[10]/F"  	               );
  tree->Branch("yMeasuredLocal"	                 , &yMeasuredLocal_ 	             , "yMeasuredLocal[10]/F"  	               );
  tree->Branch("xMeasuredGlobal"                 , &xMeasuredGlobal_	             , "xMeasuredGlobal[10]/F" 	               );
  tree->Branch("yMeasuredGlobal"                 , &yMeasuredGlobal_	             , "yMeasuredGlobal[10]/F" 	               );
  tree->Branch("xPredictedLocal"                 , &xPredictedLocal_	             , "xPredictedLocal[10]/F" 	               );
  tree->Branch("yPredictedLocal"                 , &yPredictedLocal_	             , "yPredictedLocal[10]/F" 	               );
  tree->Branch("xPredictedGlobal"                , &xPredictedGlobal_	             , "xPredictedGlobal[10]/F"	               );
  tree->Branch("yPredictedGlobal"                , &yPredictedGlobal_	             , "yPredictedGlobal[10]/F"	               );
  tree->Branch("xErrorMeasuredLocal"             , &xErrorMeasuredLocal_	         , "xErrorMeasuredLocal[10]/F"	           );
  tree->Branch("yErrorMeasuredLocal"             , &yErrorMeasuredLocal_	         , "yErrorMeasuredLocal[10]/F"	           );
  tree->Branch("xErrorMeasuredGlobal"            , &xErrorMeasuredGlobal_	         , "xErrorMeasuredGlobal[10]/F"	           );
  tree->Branch("yErrorMeasuredGlobal"            , &yErrorMeasuredGlobal_	         , "yErrorMeasuredGlobal[10]/F"	           );
  tree->Branch("xErrorPredictedLocal"            , &xErrorPredictedLocal_	         , "xErrorPredictedLocal[10]/F"	           );
  tree->Branch("yErrorPredictedLocal"            , &yErrorPredictedLocal_	         , "yErrorPredictedLocal[10]/F"	           );
  tree->Branch("xErrorPredictedGlobal"           , &xErrorPredictedGlobal_           , "xErrorPredictedGlobal[10]/F"	       );
  tree->Branch("yErrorPredictedGlobal"           , &yErrorPredictedGlobal_           , "yErrorPredictedGlobal[10]/F"	       );
  tree->Branch("xTrackResidualLocal"             , &xTrackResidualLocal_	         , "xTrackResidualLocal[10]/F"	           );
  tree->Branch("yTrackResidualLocal"             , &yTrackResidualLocal_	         , "yTrackResidualLocal[10]/F"	           );
  tree->Branch("xTrackResidualGlobal"            , &xTrackResidualGlobal_	         , "xTrackResidualGlobal[10]/F"	           );
  tree->Branch("yTrackResidualGlobal"            , &yTrackResidualGlobal_	         , "yTrackResidualGlobal[10]/F"	           );
  tree->Branch("xPixelResidualLocal"             , &xPixelResidualLocal_	         , "xPixelResidualLocal[10]/F"	           );
  tree->Branch("yPixelResidualLocal"             , &yPixelResidualLocal_	         , "yPixelResidualLocal[10]/F"	           );
  tree->Branch("xPixelResidualGlobal"            , &xPixelResidualGlobal_	         , "xPixelResidualGlobal[10]/F"	           );
  tree->Branch("yPixelResidualGlobal"            , &yPixelResidualGlobal_	         , "yPixelResidualGlobal[10]/F"	           );
  tree->Branch("xPixelResidualLocalUnconstrained", &xPixelResidualLocalUnconstrained_, "xPixelResidualLocalUnconstrained[10]/F");
  tree->Branch("yPixelResidualLocalUnconstrained", &yPixelResidualLocalUnconstrained_, "yPixelResidualLocalUnconstrained[10]/F");
  tree->Branch("xPixelPitchLocalUnconstrained"   , &xPixelPitchLocalUnconstrained_	 , "xPixelPitchLocalUnconstrained[10]/F"   );
  tree->Branch("yPixelPitchLocalUnconstrained"   , &yPixelPitchLocalUnconstrained_	 , "yPixelPitchLocalUnconstrained[10]/F"   );
  tree->Branch("clusterPixelRow"                 , &clusterPixelRow_	             , "clusterPixelRow[4][10]/I"	           );
  tree->Branch("clusterPixelCol"                 , &clusterPixelCol_	             , "clusterPixelCol[4][10]/I"	           );
  tree->Branch("clusterPixelCharge"              , &clusterPixelCharge_	             , "clusterPixelCharge[4][10]/I"	       );
  tree->Branch("xClusterPixelCenterLocal"        , &xClusterPixelCenterLocal_        , "xClusterPixelCenterLocal[4][10]/F"     );
  tree->Branch("yClusterPixelCenterLocal"        , &yClusterPixelCenterLocal_        , "yClusterPixelCenterLocal[4][10]/F"     );
  tree->Branch("xClusterPixelCenterGlobal"       , &xClusterPixelCenterGlobal_       , "xClusterPixelCenterGlobal[4][10]/F"    );
  tree->Branch("yClusterPixelCenterGlobal"       , &yClusterPixelCenterGlobal_       , "yClusterPixelCenterGlobal[4][10]/F"    );
  tree->Branch("isPixelCalibrated"               , &isPixelCalibrated_	             , "isPixelCalibrated[4][10]/O"	           );
}

