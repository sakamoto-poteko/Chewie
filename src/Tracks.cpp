#include "Tracks.h"
#include "AnalysisManager.h"
#include "ThreadUtilities.h"
#include "PlanesMapping.h"
#include "XmlParser.h"
#include "XmlAnalysis.h"
#include "XmlPlane.h"
#include "MessageTools.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TThread.h>
#include <TFile.h>

#include <iostream>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Tracks::Tracks(AnalysisManager* analysisManager, int nOfThreads) :
        Analysis           (analysisManager, nOfThreads)
        , thePlaneMapping_ (0                          )
{
    thePlaneMapping_ = new PlanesMapping();

    hChi2_            = 0;
    hXSlope_          = 0;
    hYSlope_          = 0;
    hXIntercept_      = 0;
    hYIntercept_      = 0;
    hSigmaXSlope_     = 0;
    hSigmaYSlope_     = 0;
    hSigmaXIntercept_ = 0;
    hSigmaYIntercept_ = 0;

    cols_.push_back(10);
    cols_.push_back(15);
    cols_.push_back(20);
    cols_.push_back(25);
    cols_.push_back(27);
    cols_.push_back(30);
    cols_.push_back(33);
    cols_.push_back(36);
    cols_.push_back(39);
    cols_.push_back(41);
    cols_.push_back(43);
    cols_.push_back(45);
    cols_.push_back(48);
    cols_.push_back(49);
    cols_.push_back(50);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Tracks::~Tracks(void)
{
    destroy();
    if(thePlaneMapping_)
        delete thePlaneMapping_;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tracks::destroy(void)
{
    if(Analysis::fDoNotDelete_) return;
    /*if(hXSlope_)
    {
        delete hXSlope_;
        hXSlope_ = 0;
    }

    if(hSigmaXSlope_)
    {
        delete hSigmaXSlope_;
        hSigmaXSlope_ = 0;
    }

    if(hYSlope_)
    {
        delete hYSlope_;
        hYSlope_ = 0;
    }

    if(hSigmaYSlope_)
    {
        delete hSigmaYSlope_;
        hSigmaYSlope_ = 0;
    }

    if(hXIntercept_)
    {
        delete hXIntercept_;
        hXIntercept_ = 0;
    }

    if(hSigmaXIntercept_)
    {
        delete hSigmaXIntercept_;
        hSigmaXIntercept_ = 0;
    }

    if(hYIntercept_)
    {
        delete hYIntercept_;
        hYIntercept_ = 0;
    }

    if(hSigmaYIntercept_)
    {
        delete hSigmaYIntercept_;
        hSigmaYIntercept_ = 0;
    }

*/
    for(unsigned int p=0; p<hClusterHits_.size(); p++)
    {
        if(hClusterHits_[p])
        {
            delete hClusterHits_[p];
        }
        if(hTracksProjRC_[p])
        {
            delete hTracksProjRC_[p];
        }
        if(hTracksProjXY_[p])
        {
            delete hTracksProjXY_[p];
        }
        if(hTracksProjXYPix_[p])
        {
            delete hTracksProjXYPix_[p];
        }
        if(hXError_[p])
        {
            delete hXError_[p];
        }
        if(hYError_[p])
        {
            delete hYError_[p];
        }
    }
    hClusterHits_.clear();
    hTracksProjRC_.clear();
    hTracksProjXY_.clear();
    hTracksProjXYPix_.clear();
    hXError_.clear();
    hYError_.clear();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tracks::setCutsFormula(std::map<std::string, std::string>, std::vector<TTree *>)
{
    std::stringstream ss;

    STDLINE("",ACWhite);
    ss << "No cuts applied for Tracks analysis!";
    STDLINE(ss.str(),ACRed);
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Tracks::passStandardCuts(int  , const Data& data)
{
    XmlParser* theParser = theAnalysisManager_->getXmlParser();
    if(theParser->getAnalysesFromString("Charge")->applyStandardCuts())
        return true;

    int minHits = atoi(theParser->getAnalysesFromString("Charge")->getMinHits().c_str());;//To calculate efficiency on the telescope

    int hitsontelescope = 0;
    bool HitsClusterLE2 = true;
    for (unsigned int i = 0; i < 8; i++) {
        if (data.getHasHit(i) == true) hitsontelescope++;
        if (data.getClusterSize(i) > 2) {
            HitsClusterLE2 = false;
        }
    }

    if(hitsontelescope >= minHits)
        return true;
    else
        return false;
/*
    XmlParser* theParser = theAnalysisManager_->getXmlParser();

    if(!theParser->getAnalyses()["Tracks"]->applyStandardCuts())
        return true;

    unsigned int  minHits = atoi(theParser->getAnalyses()["Tracks"]->getMinHits().c_str());
    unsigned int  hits    = 0;

    for(unsigned int pp=0; pp<thePlaneMapping_->getNumberOfPlanes(); pp++)
    {
        if(data.getHasHit(pp) && thePlaneMapping_->getPlaneName(pp).find("Telescope") != std::string::npos && data.getClusterSize(pp)<=2)
        {
            if(data.getClusterSize(pp)==1)
                hits++;
            if(data.getClusterSize(pp)==2)
            {
                if(data.getClusterPixelCol(0,pp)==data.getClusterPixelCol(1,pp) || data.getClusterPixelRow(0,pp)==data.getClusterPixelRow(1,pp))
                    hits++;
            }
        }
    }

    if(hits>=minHits)
        return true;
    else
        return false;
*/
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tracks::beginJob(void)
{
    book();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Tracks::PositionUnfolded_prepare   (int planeID, const Data &data, int threadNumber)
{
    if (!data.getHasHit(planeID)) return;

//    float pixelLengthX = 150;
//    float pixelLengthY = 100;

    int size = data.getClusterSize(planeID);
    if (size > 2) return;

    if (size == 1) {
        THREADED(hXdistribution1D_[planeID])->Fill(data.getXPixelResidualLocal(planeID));
        THREADED(hXChargedistribution1D_[planeID])->Fill(data.getXPixelResidualLocal(planeID), data.getClusterCharge(planeID));
        THREADED(hYdistribution1D_[planeID])->Fill(data.getYPixelResidualLocal(planeID));
        THREADED(hYChargedistribution1D_[planeID])->Fill(data.getYPixelResidualLocal(planeID), data.getClusterCharge(planeID));
    }

    if (size == 2) {
        THREADED(hXdistribution2D_[planeID])->Fill(data.getXPixelResidualLocal(planeID));
        THREADED(hXChargedistribution2D_[planeID])->Fill(data.getXPixelResidualLocal(planeID), data.getClusterCharge(planeID));
        THREADED(hYdistribution2D_[planeID])->Fill(data.getYPixelResidualLocal(planeID));
        THREADED(hYChargedistribution2D_[planeID])->Fill(data.getYPixelResidualLocal(planeID), data.getClusterCharge(planeID));
    }

    return;
}


void Tracks::PositionUnfolded_book ()
{
    std::string hName;
    std::string hTitle;
    std::string planeName;

    int binSize = 5;
    int pixelLengthX = 150;
    int pixelLengthY = 100;
    int nBinsX = pixelLengthX/binSize;    //150um
    int nBinsY = pixelLengthY/binSize;    //100um originally, changed binning to 5x5um/bin

    for (unsigned int i = 0; i < thePlaneMapping_->getNumberOfPlanes(); ++i) {
        planeName = thePlaneMapping_->getPlaneName(i);

        theAnalysisManager_->cd("Tracks");
        theAnalysisManager_->mkdir("Other");
        ++nBinsX;
        ++nBinsY;

    }
}

void Tracks::numberOfTracksAnalysis (const Data& data, int threadNumber)
{
    THREADED(hNumberOfTracks_  )->Fill(data.getNumberOfTracks()                          );
    THREADED(hRunNumber_       )->Fill(data.getRunNumber     ()                          );
    THREADED(hEventNumber_     )->Fill(data.getEventNumber   ()                          );
    THREADED(hTracksVsEvNumber_)->Fill(data.getEventNumber   (), data.getNumberOfTracks());

    THREADED(mEvNumberRun_[data.getRunNumber()])->Fill(data.getEventNumber());
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Tracks::analyze(const Data &data, int threadNumber)
{
    numberOfTracksAnalysis(data, threadNumber);

    if(!passStandardCuts(0,data)) return;

    int xPredicted = 0;
    int yPredicted = 0;

    //Manual inputs for DUT0 beam center
    int beamCenterX = 30;
    int beamCenterY = 30;

    THREADED(hChi2_      )->Fill(data.getChi2());
    THREADED(hXSlope_    )->Fill(data.getXSlope());
    THREADED(hYSlope_    )->Fill(data.getYSlope());
    THREADED(hXIntercept_)->Fill(data.getXIntercept());
    THREADED(hYIntercept_)->Fill(data.getYIntercept());

    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {
        if (data.getIsInDetector(p))
//        if (data.getHasHit(p))
        {
            THREADED(hTracksProjRC_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
            THREADED(hTracksProjXY_[p])->Fill(data.getXPredictedLocal(p),data.getYPredictedLocal(p));

            if (data.getXPixelResidualLocal(p)<=0)
            {
                xPredicted = (data.getXPitchLocal(p)/2) + abs(data.getXPixelResidualLocal(p));
            }
            else if (data.getXPixelResidualLocal(p)>0)
            {
                xPredicted = (data.getXPitchLocal(p)/2) - data.getXPixelResidualLocal(p);
            }
            if (data.getYPixelResidualLocal(p)<=0)
            {
                yPredicted = (data.getYPitchLocal(p)/2) + abs(data.getYPixelResidualLocal(p));
            }
            else if (data.getYPixelResidualLocal(p)>0)
            {
                yPredicted = (data.getYPitchLocal(p)/2) - data.getYPixelResidualLocal(p);
            }

            THREADED(hTracksProjXYPix_[p])->Fill(xPredicted,yPredicted);
            THREADED(hTracksProjXPixError_[p])->Fill(xPredicted, data.getXErrorPredictedLocal(p));
            THREADED(hTracksProjYPixError_[p])->Fill(yPredicted, data.getYErrorPredictedLocal(p));
            THREADED(hXError_[p])->Fill(data.getXErrorPredictedLocal(p));
            THREADED(hYError_[p])->Fill(data.getYErrorPredictedLocal(p));

            if (data.getRowPredicted(p)>=beamCenterY && data.getRowPredicted(p)<79)
            {
                THREADED(hTracksProjXYPixAboveBeam_[p])->Fill(xPredicted,yPredicted);
                if (data.getColPredicted(p)<beamCenterX && data.getColPredicted(p)>=1)
                {
                    THREADED(hTracksProjXYPixLeftOfBeam_[p])->Fill(xPredicted,yPredicted);
                    THREADED(hTracksProjXYPixQuad2_[p])->Fill(xPredicted,yPredicted);
                    THREADED(hTracksProjRCQuad2_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                    THREADED(hTracksProjRCRecon_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                }
                else if (data.getColPredicted(p)>=beamCenterX && data.getColPredicted(p)<51)
                {
                    THREADED(hTracksProjXYPixRightOfBeam_[p])->Fill(xPredicted,yPredicted);
                    THREADED(hTracksProjXYPixQuad1_[p])->Fill(xPredicted,yPredicted);
                    THREADED(hTracksProjRCQuad1_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                    THREADED(hTracksProjRCRecon_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                }
                else
                {
                    THREADED(hTracksProjRCEdges_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                    THREADED(hTracksProjRCRecon_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                }
            }
            else if (data.getRowPredicted(p)<beamCenterY && data.getRowPredicted(p)>=1)
            {
                THREADED(hTracksProjXYPixBelowBeam_[p])->Fill(xPredicted,yPredicted);
                if (data.getColPredicted(p)<beamCenterX && data.getColPredicted(p)>=1)
                {
                    THREADED(hTracksProjXYPixLeftOfBeam_[p])->Fill(xPredicted,yPredicted);
                    THREADED(hTracksProjXYPixQuad3_[p])->Fill(xPredicted,yPredicted);
                    THREADED(hTracksProjRCQuad3_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                    THREADED(hTracksProjRCRecon_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                }
                else if (data.getColPredicted(p)>=beamCenterX && data.getColPredicted(p)<51)
                {
                    THREADED(hTracksProjXYPixRightOfBeam_[p])->Fill(xPredicted,yPredicted);
                    THREADED(hTracksProjXYPixQuad4_[p])->Fill(xPredicted,yPredicted);
                    THREADED(hTracksProjRCQuad4_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                    THREADED(hTracksProjRCRecon_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                }
                else
                {
                    THREADED(hTracksProjRCEdges_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                    THREADED(hTracksProjRCRecon_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                }
            }
            else
            {
                THREADED(hTracksProjRCEdges_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
                THREADED(hTracksProjRCRecon_[p])->Fill(data.getColPredicted(p),data.getRowPredicted(p));
            }

            if(data.getHasHit(p))
            {
                THREADED(ClusterSizeHits_[p])->Fill(data.getColPredicted(p), data.getRowPredicted(p), data.getClusterSize(p));
                THREADED(hClusterHitsNorm_[p])->Fill(data.getColPredicted(p), data.getRowPredicted(p));

                for(int h=0; h<data.getClusterSize(p) && h<4; h++)
                {
                    THREADED(hClusterHits_[p])->Fill(data.getClusterPixelCol(h,p),data.getClusterPixelRow(h,p));
                }
            }

            for (std::map<int, TH2F*>::iterator it = hTrackHitsCols_[p].begin(); it != hTrackHitsCols_[p].end(); ++it)
                if (data.getColPredicted(p) == it->first) THREADED(it->second)->Fill(data.getXPredictedLocal(p),data.getYPredictedLocal(p));
        }

        PositionUnfolded_prepare (p, data, threadNumber);
    }

    THREADED(hSigmaXIntercept_)->Fill(data.getXSigmaIntercept());
    THREADED(hSigmaYIntercept_)->Fill(data.getYSigmaIntercept());
    THREADED(hSigmaXSlope_    )->Fill(data.getXSigmaSlope());
    THREADED(hSigmaYSlope_    )->Fill(data.getYSigmaSlope());

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tracks::endJob(void)
{
    ADD_THREADED(hChi2_            );
    ADD_THREADED(hXSlope_          );
    ADD_THREADED(hSigmaXSlope_     );
    ADD_THREADED(hYSlope_          );
    ADD_THREADED(hSigmaYSlope_     );
    ADD_THREADED(hXIntercept_      );
    ADD_THREADED(hSigmaXIntercept_ );
    ADD_THREADED(hYIntercept_      );
    ADD_THREADED(hSigmaYIntercept_ );
    ADD_THREADED(hNumberOfTracks_  );
    ADD_THREADED(hRunNumber_       );
    ADD_THREADED(hEventNumber_     );
    ADD_THREADED(hTracksVsEvNumber_);

    for (std::vector<int>::iterator it = fRunNumbers_.begin(); it != fRunNumbers_.end(); ++it)
    {
        ADD_THREADED(mEvNumberRun_[*it]);
    }

    hChi2_            ->GetXaxis()->SetTitle("chi2/DOF"              );
    hXSlope_          ->GetXaxis()->SetTitle("X Slope (rad)"         );
    hYSlope_          ->GetXaxis()->SetTitle("Y Slope (rad)"         );
    hSigmaXSlope_     ->GetXaxis()->SetTitle("X Slope error (rad)"   );
    hSigmaYSlope_     ->GetXaxis()->SetTitle("Y Slope error (rad)"   );
    hXIntercept_      ->GetXaxis()->SetTitle("X Intercept (um)"      );
    hYIntercept_      ->GetXaxis()->SetTitle("Y Intercept (um)"      );
    hSigmaXIntercept_ ->GetXaxis()->SetTitle("X Intercept error (um)");
    hSigmaYIntercept_ ->GetXaxis()->SetTitle("Y Intercept error (um)");
    hNumberOfTracks_  ->GetXaxis()->SetTitle("Number of tracks"      );
    hRunNumber_       ->GetXaxis()->SetTitle("Run Number"            );
    hEventNumber_     ->GetXaxis()->SetTitle("Event Number"          );
    hTracksVsEvNumber_->GetXaxis()->SetTitle("Event Number"          );

    hTracksVsEvNumber_->Divide(hEventNumber_);

    for (std::vector<int>::iterator it = fRunNumbers_.begin(); it != fRunNumbers_.end(); ++it)
    {
        mEvNumberRun_[*it]->GetXaxis()->SetTitle("Event Number");
    }

    TH1D * hTracksProjXYPixPX_;
    TH1D * hTracksProjXYPixPY_;

    for (unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {

        ADD_THREADED(hClusterHits_[p]);
        hClusterHits_[p]->GetXaxis()->SetTitle("Col");
        hClusterHits_[p]->GetYaxis()->SetTitle("Row");
        ADD_THREADED(ClusterSizeHits_[p]);
        ClusterSizeHits_[p]->GetXaxis()->SetTitle("Col");
        ClusterSizeHits_[p]->GetYaxis()->SetTitle("Row");
        ADD_THREADED(hClusterHitsNorm_[p]);
        hClusterHitsNorm_[p]->GetXaxis()->SetTitle("Col");
        hClusterHitsNorm_[p]->GetYaxis()->SetTitle("Row");
        ADD_THREADED(hTracksProjRC_[p]);
        hTracksProjRC_[p]->GetXaxis()->SetTitle("Col");
        hTracksProjRC_[p]->GetYaxis()->SetTitle("Row");
        ADD_THREADED(hTracksProjXYPix_[p]);
        hTracksProjXYPix_[p]->GetXaxis()->SetTitle("X (um)");
        hTracksProjXYPix_[p]->GetYaxis()->SetTitle("Y (um)");
        ADD_THREADED(hTracksProjXY_[p]);
        hTracksProjXY_[p]->GetXaxis()->SetTitle("X (um)");
        hTracksProjXY_[p]->GetYaxis()->SetTitle("Y (um)");

        ADD_THREADED(hXError_[p]);
        hXError_[p]->GetXaxis()->SetTitle("Error on x");
        hXError_[p]->GetYaxis()->SetTitle("Number of events");
        ADD_THREADED(hYError_[p]);
        hYError_[p]->GetXaxis()->SetTitle("Error on y");
        hYError_[p]->GetYaxis()->SetTitle("Number of events");

        ADD_THREADED(hTracksProjXYPixAboveBeam_[p]);
        hTracksProjXYPixAboveBeam_[p]->GetXaxis()->SetTitle("X (um)");
        hTracksProjXYPixAboveBeam_[p]->GetYaxis()->SetTitle("Y (um)");
        ADD_THREADED(hTracksProjXYPixBelowBeam_[p]);
        hTracksProjXYPixBelowBeam_[p]->GetXaxis()->SetTitle("X (um)");
        hTracksProjXYPixBelowBeam_[p]->GetYaxis()->SetTitle("Y (um)");
        ADD_THREADED(hTracksProjXYPixLeftOfBeam_[p]);
        hTracksProjXYPixLeftOfBeam_[p]->GetXaxis()->SetTitle("X (um)");
        hTracksProjXYPixLeftOfBeam_[p]->GetYaxis()->SetTitle("Y (um)");
        ADD_THREADED(hTracksProjXYPixRightOfBeam_[p]);
        hTracksProjXYPixRightOfBeam_[p]->GetXaxis()->SetTitle("X (um)");
        hTracksProjXYPixRightOfBeam_[p]->GetYaxis()->SetTitle("Y (um)");
        ADD_THREADED(hTracksProjXYPixQuad1_[p]);
        hTracksProjXYPixQuad1_[p]->GetXaxis()->SetTitle("X (um)");
        hTracksProjXYPixQuad1_[p]->GetYaxis()->SetTitle("Y (um)");
        ADD_THREADED(hTracksProjXYPixQuad2_[p]);
        hTracksProjXYPixQuad2_[p]->GetXaxis()->SetTitle("X (um)");
        hTracksProjXYPixQuad2_[p]->GetYaxis()->SetTitle("Y (um)");
        ADD_THREADED(hTracksProjXYPixQuad3_[p]);
        hTracksProjXYPixQuad3_[p]->GetXaxis()->SetTitle("X (um)");
        hTracksProjXYPixQuad3_[p]->GetYaxis()->SetTitle("Y (um)");
        ADD_THREADED(hTracksProjXYPixQuad4_[p]);
        hTracksProjXYPixQuad4_[p]->GetXaxis()->SetTitle("X (um)");
        hTracksProjXYPixQuad4_[p]->GetYaxis()->SetTitle("Y (um)");

        ADD_THREADED(hTracksProjRCQuad1_[p]);
        hTracksProjRCQuad1_[p]->GetXaxis()->SetTitle("Col");
        hTracksProjRCQuad1_[p]->GetYaxis()->SetTitle("Row");
        ADD_THREADED(hTracksProjRCQuad2_[p]);
        hTracksProjRCQuad2_[p]->GetXaxis()->SetTitle("Col");
        hTracksProjRCQuad2_[p]->GetYaxis()->SetTitle("Row");
        ADD_THREADED(hTracksProjRCQuad3_[p]);
        hTracksProjRCQuad3_[p]->GetXaxis()->SetTitle("Col");
        hTracksProjRCQuad3_[p]->GetYaxis()->SetTitle("Row");
        ADD_THREADED(hTracksProjRCQuad4_[p]);
        hTracksProjRCQuad4_[p]->GetXaxis()->SetTitle("Col");
        hTracksProjRCQuad4_[p]->GetYaxis()->SetTitle("Row");
        ADD_THREADED(hTracksProjRCRecon_[p]);
        hTracksProjRCRecon_[p]->GetXaxis()->SetTitle("Col");
        hTracksProjRCRecon_[p]->GetYaxis()->SetTitle("Row");
        ADD_THREADED(hTracksProjRCEdges_[p]);
        hTracksProjRCEdges_[p]->GetXaxis()->SetTitle("Col");
        hTracksProjRCEdges_[p]->GetYaxis()->SetTitle("Row");

        ADD_THREADED(hTracksProjXPixError_[p]);
        hTracksProjXPixError_[p]->GetXaxis()->SetTitle("X (um)");
        hTracksProjXPixError_[p]->GetYaxis()->SetTitle("X Error (um)");
        ADD_THREADED(hTracksProjYPixError_[p]);
        hTracksProjYPixError_[p]->GetXaxis()->SetTitle("Y (um)");
        hTracksProjYPixError_[p]->GetYaxis()->SetTitle("Y Error (um)");

        // Normalize hTracksProjX(Y)PixError w/ X&Y projections of hTracksProjXYPix_;
        hTracksProjXYPixPX_ = hTracksProjXYPix_[p]->ProjectionX();
        hTracksProjXYPixPY_ = hTracksProjXYPix_[p]->ProjectionY();
        hTracksProjXPixError_[p]->Divide(hTracksProjXYPixPX_);
        hTracksProjYPixError_[p]->Divide(hTracksProjXYPixPY_);
        delete hTracksProjXYPixPX_;
        delete hTracksProjXYPixPY_;

        ADD_THREADED(hXdistribution1D_[p]);
        ADD_THREADED(hXdistribution2D_[p]);
        ADD_THREADED(hXChargedistribution1D_[p]);
        ADD_THREADED(hXChargedistribution2D_[p]);
        ADD_THREADED(hYdistribution1D_[p]);
        ADD_THREADED(hYdistribution2D_[p]);
        ADD_THREADED(hYChargedistribution1D_[p]);
        ADD_THREADED(hYChargedistribution2D_[p]);

        for (std::map<int, TH2F*>::iterator it = hTrackHitsCols_[p].begin(); it != hTrackHitsCols_[p].end(); ++it)
            ADD_THREADED(it->second);

        hXdistribution1D_[p]->GetXaxis()->SetTitle("x position (um)");
        hXdistribution2D_[p]->GetXaxis()->SetTitle("x position (um)");
        hXChargedistribution1D_[p]->GetXaxis()->SetTitle("x position (um)");
        hXChargedistribution1D_[p]->GetYaxis()->SetTitle("Charge");
        hXChargedistribution2D_[p]->GetXaxis()->SetTitle("x position (um)");
        hXChargedistribution2D_[p]->GetYaxis()->SetTitle("Charge");
        hYdistribution1D_[p]->GetXaxis()->SetTitle("y position (um)");
        hYdistribution2D_[p]->GetXaxis()->SetTitle("y position (um)");
        hYChargedistribution1D_[p]->GetXaxis()->SetTitle("y position (um)");
        hYChargedistribution1D_[p]->GetYaxis()->SetTitle("Charge");
        hYChargedistribution2D_[p]->GetXaxis()->SetTitle("y position (um)");
        hYChargedistribution2D_[p]->GetYaxis()->SetTitle("Charge");

        for (std::map<int, TH2F*>::iterator it = hTrackHitsCols_[p].begin(); it != hTrackHitsCols_[p].end(); ++it)
        {
            it->second->GetXaxis()->SetTitle("x position (um)");
            it->second->GetYaxis()->SetTitle("y position (um)");
        }

        ClusterSizeHits_[p]->Divide(hClusterHitsNorm_[p]);

    }

//    PositionUnfolded_book();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tracks::book(void)
{
    destroy();

    std::string hName;
    std::string hTitle;
    std::string planeName;

    theAnalysisManager_->cd("/");
    theAnalysisManager_->mkdir("Tracks");

    hName  = "hChi2_"               ;
    hTitle = "Chi2/DOF distribution";
    hChi2_ = NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),1000,0,50));

    theAnalysisManager_->mkdir("X");

    hName  = "hXSlope_"            ;
    hTitle = "X slopes distribution";
    hXSlope_ = NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 2000, -0.001, 0.001));

    hName  = "hSigmaXSlope_";
    hTitle = "X slope errors distribution";
    hSigmaXSlope_ = NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 1000, 0, 0.001));

    hName  = "hXIntercept_";
    hTitle = "X intercepts distribution";
    hXIntercept_ = NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 100, -10, 10));

    hName  = "hSigmaXIntercept_";
    hTitle = "X intercept errors distribution";
    hSigmaXIntercept_ = NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 10000, 0, 10));

    theAnalysisManager_->cd("Tracks");
    theAnalysisManager_->mkdir("Y");

    hName  = "hYSlope_"            ;
    hTitle = "Y slope distribution";
    hYSlope_ = NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 2000, -0.1, 0.1));

    hName  = "hSigmaYSlope_";
    hTitle = "Y slope errors distribution";
    hSigmaYSlope_ = NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 1000, 0, 0.001));

    hName  = "hYIntercept_";
    hTitle = "Y intercept distribution";
    hYIntercept_ = NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 100, -10, 10));

    hName  = "hSigmaYIntercept_";
    hTitle = "Y intercept errors distribution";
    hSigmaYIntercept_ = NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 10000, 0, 10));

    theAnalysisManager_->cd("Tracks");
    theAnalysisManager_->mkdir("Numbers");

    hName = "NumberOfTracks_";
    hTitle = "Distribution of number of tracks";
    hNumberOfTracks_ = NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 5, 0, 5));

    hName = "RunNumber_" + planeName;
    hTitle = "Number of events for each run number";
    hRunNumber_ = NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), (int)(*fRunNumbers_.begin() - *(fRunNumbers_.end()-1) + 1), *fRunNumbers_.begin(), *(fRunNumbers_.end()-1)+1));

    hName = "EventNumber_" + planeName;
    hTitle = "Event number";
    hEventNumber_ = NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 40000, 0, 400000));

    hName = "TracksVsEvNumber_" + planeName;
    hTitle = "Number of tracks vs Event number";
    hTracksVsEvNumber_ = NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 40000, 0, 400000));

    std::stringstream sss;
    for (std::vector<int>::iterator it = fRunNumbers_.begin(); it != fRunNumbers_.end(); ++it)
    {
        sss.str("");
        sss << *it;
        hName = "EventNumberRun_" + sss.str();
        hTitle = "Event number distribution for run number # " + sss.str();
        mEvNumberRun_[*it] = NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 40000, 0, 400000));
    }

    XmlParser* theParser = theAnalysisManager_->getXmlParser();
    int nBinsX = 0;
    int nBinsY = 0;
    for (unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes();p++)
    {
        planeName = thePlaneMapping_->getPlaneName(p);
        int xChipsNumber = atoi((((theParser->getPlanes())[thePlaneMapping_->getPlaneName(p)]->getChipsNumber()).first).c_str());
        int yChipsNumber = atoi((((theParser->getPlanes())[thePlaneMapping_->getPlaneName(p)]->getChipsNumber()).second).c_str());
        nBinsX = xChipsNumber*52;
        nBinsY = yChipsNumber*80;

        theAnalysisManager_->cd("Tracks");
        theAnalysisManager_->mkdir(planeName);

        hName = "ClusterHits_" + planeName;
        hTitle = "Beam profile of cluster hits" + planeName;
        hClusterHits_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, nBinsX, nBinsY, 0, nBinsY)));

        hName = "ClusterSizeHits_" + planeName;
        hTitle = "Medium cluster size distribution on revelator " + planeName;
        ClusterSizeHits_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, nBinsX, nBinsY, 0, nBinsY)));

        hName = "ClusterHitsNorm_" + planeName;
        hTitle = "Distribution of reconstructed events on revelator " + planeName;
        hClusterHitsNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, nBinsX, nBinsY, 0, nBinsY)));

        hName = "TrackProjectionRowCol_" + planeName;
        hTitle = "Row/Col Track projections " + planeName;
        hTracksProjRC_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, nBinsX, nBinsY, 0, nBinsY)));

        int binSize = 5;
        int pixelLengthX = 150;
        int pixelLengthY = 100;
        nBinsX = pixelLengthX/binSize;    //150um
        nBinsY = pixelLengthY/binSize;    //100um originally, changed binning to 5x5um/bin

        hName = "XError" + planeName;
        hTitle = "Error on x cooridnate" + planeName;
        hXError_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 500, 0, 10)));
        hName = "YError" + planeName;
        hTitle = "Error on y cooridnate" + planeName;
        hYError_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 500, 0, 10)));

        hName = "TrackProjectionXYPixel_" + planeName;
        hTitle = "XY track projections per pixel " + planeName;
        hTracksProjXYPix_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, pixelLengthX, nBinsY, 0, pixelLengthY)));

        // Histograms showing track projection on individual pixels above, below, to either side of beam center, and in quads around center. Only done on DUT0.
        hName = "TrackProjectionXYPixelAboveBeam_" + planeName;
        hTitle = "XY track projections per pixel above center of beam " + planeName;
        hTracksProjXYPixAboveBeam_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, pixelLengthX, nBinsY, 0, pixelLengthY)));
        hName = "TrackProjectionXYPixelBelowBeam_" + planeName;
        hTitle = "XY track projections per pixel below center of beam " + planeName;
        hTracksProjXYPixBelowBeam_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, pixelLengthX, nBinsY, 0, pixelLengthY)));
        hName = "TrackProjectionXYPixelLeftOfBeam_" + planeName;
        hTitle = "XY track projections per pixel left of center of beam " + planeName;
        hTracksProjXYPixLeftOfBeam_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, pixelLengthX, nBinsY, 0, pixelLengthY)));
        hName = "TrackProjectionXYPixelRightOfBeam_" + planeName;
        hTitle = "XY track projections per pixel right of center of beam " + planeName;
        hTracksProjXYPixRightOfBeam_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, pixelLengthX, nBinsY, 0, pixelLengthY)));

        hName = "TrackProjectionXYPixelQuad1_" + planeName;
        hTitle = "XY track projections per pixel (Quad1) " + planeName;
        hTracksProjXYPixQuad1_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, pixelLengthX, nBinsY, 0, pixelLengthY)));
        hName = "TrackProjectionXYPixelQuad2_" + planeName;
        hTitle = "XY track projections per pixel (Quad2) " + planeName;
        hTracksProjXYPixQuad2_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, pixelLengthX, nBinsY, 0, pixelLengthY)));
        hName = "TrackProjectionXYPixelQuad3_" + planeName;
        hTitle = "XY track projections per pixel (Quad3) " + planeName;
        hTracksProjXYPixQuad3_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, pixelLengthX, nBinsY, 0, pixelLengthY)));
        hName = "TrackProjectionXYPixelQuad4_" + planeName;
        hTitle = "XY track projections per pixel (Quad4) " + planeName;
        hTracksProjXYPixQuad4_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, pixelLengthX, nBinsY, 0, pixelLengthY)));

        // Histograms showing error in track projection in x and y on each pixel position
        hName = "TrackProjectionXPixelError_" + planeName;
        hTitle = "Error in Track Projection in X Direction " + planeName;
        hTracksProjXPixError_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), nBinsX, 0, pixelLengthX)));
        hName = "TrackProjectionYPixelError_" + planeName;
        hTitle = "Error in Track Projection in Y Direction " + planeName;
        hTracksProjYPixError_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), nBinsY, 0, pixelLengthY)));

        //Histograms supporting Unfolding function
        hName = "Xdistribution1D_" + planeName;
        hTitle = "Distribution of hits for x, cluster size 1";
        hXdistribution1D_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), nBinsX, -pixelLengthX/2, pixelLengthX/2)));
        hName = "Xdistribution2D_" + planeName;
        hTitle = "Distribution of hits for x, cluster size 2";
        hXdistribution2D_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), nBinsX, -pixelLengthX/2, pixelLengthX/2)));
        hName = "XChargedistribution1D_" + planeName;
        hTitle = "Distribution of charge vs x, cluster size 1";
        hXChargedistribution1D_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, -pixelLengthX/2, pixelLengthX/2, 500, 0, 80000)));
        hName = "XChargedistribution2D_" + planeName;
        hTitle = "Distribution of charge vs x, cluster size 2";
        hXChargedistribution2D_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, -pixelLengthX/2, pixelLengthX/2, 500, 0, 80000)));
        hName = "Ydistribution1D_" + planeName;
        hTitle = "Distribution of hits for y, cluster size 1";
        hYdistribution1D_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), nBinsY, -pixelLengthY/2, pixelLengthY/2)));
        hName = "Ydistribution2D_" + planeName;
        hTitle = "Distribution of hits for y, cluster size 2";
        hYdistribution2D_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), nBinsY, -pixelLengthY/2, pixelLengthY/2)));
        hName = "YChargedistribution1D_" + planeName;
        hTitle = "Distribution of charge vs y, cluster size 1";
        hYChargedistribution1D_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsY, -pixelLengthY/2, pixelLengthY/2, 500, 0, 80000)));
        hName = "YChargedistribution2D_" + planeName;
        hTitle = "Distribution of charge vs y, cluster size 2";
        hYChargedistribution2D_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsY, -pixelLengthY/2, pixelLengthY/2, 500, 0, 80000)));

        theAnalysisManager_->mkdir("Column scan");

        std::map<int, TH2F*> aMap;
        std::stringstream ssd;
        for (std::vector<int>::iterator cs = cols_.begin(); cs != cols_.end(); ++cs)
        {
            ssd.str("");
            ssd << "TrackHitsCol_col" << *cs << "_" << planeName;
            hName = ssd.str();
            ssd.str("");
            ssd << "XY track projections for column #" << *cs << ", " << planeName;
            hTitle = ssd.str();
            aMap[*cs] = NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(),  nBinsX, 0, pixelLengthX, nBinsY, 0, pixelLengthY));
        }
        hTrackHitsCols_.push_back(aMap);

        theAnalysisManager_->cd("Tracks/" + planeName);

        // Histograms showing track projection on Row/Col of ROC per quadrant.
        nBinsX = xChipsNumber*52;
        nBinsY = yChipsNumber*80;
        hName = "TrackProjectionRowColQuad1_" + planeName;
        hTitle = "Row/Col Track projections (Quad1) " + planeName;
        hTracksProjRCQuad1_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, nBinsX, nBinsY, 0, nBinsY)));
        hName = "TrackProjectionRowColQuad2_" + planeName;
        hTitle = "Row/Col Track projections (Quad2) " + planeName;
        hTracksProjRCQuad2_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, nBinsX, nBinsY, 0, nBinsY)));
        hName = "TrackProjectionRowColQuad3_" + planeName;
        hTitle = "Row/Col Track projections (Quad3) " + planeName;
        hTracksProjRCQuad3_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, nBinsX, nBinsY, 0, nBinsY)));
        hName = "TrackProjectionRowColQuad4_" + planeName;
        hTitle = "Row/Col Track projections (Quad4) " + planeName;
        hTracksProjRCQuad4_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, nBinsX, nBinsY, 0, nBinsY)));
        hName = "TrackProjectionRowColRecon_" + planeName;
        hTitle = "Row/Col Track projections (reconstructed) " + planeName;
        hTracksProjRCRecon_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, nBinsX, nBinsY, 0, nBinsY)));
        hName = "TrackProjectionRowColEdges_" + planeName;
        hTitle = "Row/Col Track projections (edges) " + planeName;
        hTracksProjRCEdges_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, nBinsX, nBinsY, 0, nBinsY)));

        hName = "TrackProjectionXY_" + planeName;
        hTitle = "XY track projections " + planeName;
        nBinsX = xChipsNumber*52*15;
        nBinsY = yChipsNumber*80*10;
        hTracksProjXY_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, 0, nBinsX*10, nBinsY, 0, nBinsY*10)));

    }

}
