#include "Resolution.h"
#include "AnalysisManager.h"
#include "WindowsManager.h"
#include "Window.h"
#include "ThreadUtilities.h"
#include "PlanesMapping.h"
#include "MessageTools.h"
#include "XmlParser.h"
#include "XmlAnalysis.h"

#include <TH1F.h>
#include <TThread.h>
#include <TFile.h>
#include <TTreeFormula.h>

#include <iostream>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Resolution::Resolution(AnalysisManager* analysisManager, int nOfThreads) :
    Analysis          (analysisManager,
                       nOfThreads      )
  , thePlaneMapping_  (0               )
  , theWindowsManager_(0               )
{
    thePlaneMapping_ = new PlanesMapping();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Resolution::~Resolution(void)
{
    if(thePlaneMapping_)
        delete thePlaneMapping_;

    destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resolution::destroy(void)
{
    if(Analysis::fDoNotDelete_) return;

    for(std::vector<TH1F*>::iterator it=hXresiduals_            .begin(); it!=hXresiduals_            .end(); it++) delete *it; hXresiduals_            .clear();
    for(std::vector<TH1F*>::iterator it=hXresidualCalculated_   .begin(); it!=hXresidualCalculated_   .end(); it++) delete *it; hXresidualCalculated_   .clear();
    for(std::vector<TH1F*>::iterator it=hXresidualsClusterSize1_.begin(); it!=hXresidualsClusterSize1_.end(); it++) delete *it; hXresidualsClusterSize1_.clear();
    for(std::vector<TH1F*>::iterator it=hXresidualsClusterSize2_.begin(); it!=hXresidualsClusterSize2_.end(); it++) delete *it; hXresidualsClusterSize2_.clear();

    for(std::vector<TH1F*>::iterator it=hYresiduals_            .begin(); it!=hYresiduals_            .end(); it++) delete *it; hYresiduals_            .clear();
    for(std::vector<TH1F*>::iterator it=hYresidualCalculated_   .begin(); it!=hYresidualCalculated_   .end(); it++) delete *it; hYresidualCalculated_   .clear();
    for(std::vector<TH1F*>::iterator it=hYresidualsClusterSize1_.begin(); it!=hYresidualsClusterSize1_.end(); it++) delete *it; hYresidualsClusterSize1_.clear();
    for(std::vector<TH1F*>::iterator it=hYresidualsClusterSize2_.begin(); it!=hYresidualsClusterSize2_.end(); it++) delete *it; hYresidualsClusterSize2_.clear();

    for(std::vector<TH1F*>::iterator it=hPredictedXErrors_      .begin(); it!=hPredictedXErrors_      .end(); it++) delete *it; hPredictedXErrors_      .clear();
    for(std::vector<TH1F*>::iterator it=hPredictedYErrors_      .begin(); it!=hPredictedYErrors_      .end(); it++) delete *it; hPredictedYErrors_      .clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resolution::predictedErrors(bool pass, int planeID, const Data& data, int threadNumber)
{
    if(!pass)
        return;

    THREADED(hPredictedXErrors_[planeID-8])->Fill(data.getXErrorPredictedGlobal(planeID));
    THREADED(hPredictedYErrors_[planeID-8])->Fill(data.getYErrorPredictedGlobal(planeID));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resolution::calculateXresiduals(bool pass, int planeID, const Data &data, int threadNumber)
{
    if( !pass || !data.getIsInDetector( planeID ) || !data.getHasHit( planeID ) || data.getClusterSize( planeID ) != 2 )
        return;

    float maxPitchX = 150;

    if( data.getXPitchLocal( planeID ) > maxPitchX )
        return;

    float Xp = 0;

    if(data.getXPixelResidualLocal( planeID ) > 0)
        Xp = data.getXPitchLocal( planeID )/2 - data.getXPixelResidualLocal( planeID );
    else if(data.getXPixelResidualLocal( planeID ) <= 0)
        Xp = -(data.getXPixelResidualLocal( planeID ) + data.getXPitchLocal( planeID )/2);

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted   ( planeID) ;
    int           col       = data.getColPredicted   ( planeID );

    if( !theWindow->checkWindow(col,row) ) {
        return;
    }

    for(int h=0; h<2; ++h)
    {
        if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID)) //hits are in the window
            || !data.getIsPixelCalibrated(h,planeID)                                                          //pixels are calibrated
            ||  data.getClusterPixelRow  (h,planeID) != row )                                                 //hits are on the same row (sharing is along the row - x direction)
            return;
    }

    int   hitID       =     -1 ;
    int   totalCharge =      0 ;
    int   chargeLeft  =      0 ;
    int   chargeRight =      0 ;
    float Asimmetry   =      0 ;
    float slope       = -13.65 ;
    float intercept   =  1.385 ;
    //float slope       = -5.181 ;
    //float intercept   =  1.228 ;
    float Xm                   ;   

    for(int h=0; h<2; ++h)
    {
        if(data.getClusterPixelCol(h,planeID) == col)//mi assicuro che ci sia la cella predetta in uno dei due hit
        {
            hitID   = h   ;
            break;
        }
    }
    if( hitID == -1 )
        return;

    for(int h=0; h<2; ++h)
    {
        if(data.getXPixelResidualLocal(planeID) > 0 && (col - data.getClusterPixelCol(h,planeID)) == 1)//il secondo hit e' a SN della predetta
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
        else
            return;
        /*else if(data.getXPixelResidualLocal(planeID) > 0 && (col - data.getClusterPixelCol(h,planeID)) == -1)
        {
            chargeLeft  = data.getClusterPixelCharge(hitID,planeID);
            chargeRight = data.getClusterPixelCharge(h    ,planeID);
            Xp = - data.getXPixelResidualLocal(planeID) - data.getXPitchLocal(planeID)/2;
            break;
        }
        else if(data.getXPixelResidualLocal(planeID) < 0 && (col - data.getClusterPixelCol(h,planeID)) ==  1)
        {
            chargeLeft  = data.getClusterPixelCharge(h    ,planeID);
            chargeRight = data.getClusterPixelCharge(hitID,planeID);
            Xp = -(data.getXPixelResidualLocal(planeID) - data.getXPitchLocal(planeID)/2);
            break;
        }*/
    }

    totalCharge = chargeLeft + chargeRight;
    Asimmetry   = (float)(chargeLeft - chargeRight)/totalCharge;

    if(Asimmetry >= -0.7 && Asimmetry <= 0.7 && totalCharge <= 30000)
    {
        Xm = slope*Asimmetry + intercept;
        THREADED(hXresidualCalculated_ [planeID])->Fill( Xm - Xp );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resolution::calculateYresiduals(bool pass, int planeID, const Data &data, int threadNumber)
{
    if( !pass || !data.getIsInDetector( planeID ) || !data.getHasHit( planeID ) || data.getClusterSize( planeID ) != 2 )
        return;

    float maxPitchY = 100;

    if( data.getYPitchLocal( planeID ) > maxPitchY )
        return;

    float Yp = 0;

    if(data.getYPixelResidualLocal( planeID ) > 0)
        Yp = data.getYPitchLocal( planeID )/2 - data.getYPixelResidualLocal( planeID );
    else if(data.getYPixelResidualLocal( planeID ) <= 0)
        Yp = -(data.getYPixelResidualLocal( planeID ) + data.getYPitchLocal( planeID )/2);

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    int           row       = data.getRowPredicted   ( planeID );
    int           col       = data.getColPredicted   ( planeID );

    if( !theWindow->checkWindow(col,row) )
        return;

    for(int h=0; h<2; ++h)
    {
        if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID)) // hits are in the window
            || !data.getIsPixelCalibrated(h,planeID)                                                          //pixels are calibrated
            ||  data.getClusterPixelCol    (h,planeID) !=  col )                                              //hits are on the same column (sharing is along the column - y direction)
            return;
    }

    int   hitID       =      -1 ;
    int   totalCharge =       0 ;
    int   chargeLeft  =       0 ;
    int   chargeRight =       0 ;
    float Asimmetry   =       0 ;
    float slope       =  -12.13 ;
    float intercept   = -0.5596 ;
    //float slope       =  -5.14  ;
    //float intercept   = -0.8479 ;
    float Ym                    ;

    for(int h=0; h<2; ++h)
    {
        if(data.getClusterPixelRow(h,planeID) == row)//mi assicuro che ci sia la cella predetta in uno dei due hit
        {
            hitID   = h   ;
            break;
        }
    }
    if( hitID == -1)
        return;

    for(int h=0; h<2; ++h)
    {
        if(data.getYPixelResidualLocal(planeID) > 0 && (row - data.getClusterPixelRow(h,planeID)) == 1)//il secondo hit e' a SN della predetta
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
        else
            return;
        /*else if(data.getYPixelResidualLocal(planeID) > 0 && (row - data.getClusterPixelRow(h,planeID)) == -1)
        {
            chargeLeft  = data.getClusterPixelCharge(hitID,planeID);
            chargeRight = data.getClusterPixelCharge(h    ,planeID);
            Yp = - data.getYPixelResidualLocal(planeID) - data.getYPitchLocal(planeID)/2;
            break;
        }
        else if(data.getYPixelResidualLocal(planeID) < 0 && (row - data.getClusterPixelRow(h,planeID)) ==  1)
        {
            chargeLeft  = data.getClusterPixelCharge(h    ,planeID);
            chargeRight = data.getClusterPixelCharge(hitID,planeID);
            Yp = -(data.getYPixelResidualLocal(planeID) - data.getYPitchLocal(planeID)/2);
            break;
        }*/
    }

    totalCharge = chargeLeft + chargeRight;
    Asimmetry  = (float)(chargeLeft - chargeRight)/totalCharge;

    if(Asimmetry >= -0.7 && Asimmetry <= 0.7 && totalCharge <= 30000)
    {
        Ym = slope*Asimmetry + intercept;
        THREADED(hYresidualCalculated_ [planeID])->Fill( Ym - Yp );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resolution::xResolution(bool pass, int planeID, const Data& data, int threadNumber)
{
    if(!pass || !data.getBelongsToTrack(planeID))
        return;

    int   size    = data.getClusterSize(planeID);
    int   maxHits = 2;

    if(size > maxHits)
        return;

    const Window* theWindow  = theWindowsManager_->getWindow(planeID);

    for(int h=0; h<size; h++)
    {
        if( !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID)) ) //i due hit devono stare nella finestra
            return;
    }

    THREADED(hXresiduals_[planeID])->Fill(data.getXTrackResidualLocal(planeID));

    if(size == 1)
        THREADED(hXresidualsClusterSize1_[planeID])->Fill(data.getXTrackResidualLocal(planeID));
    if(size == 2)
    {
        if(data.getClusterPixelRow(0,planeID) != data.getClusterPixelRow(1,planeID))
            return;

        THREADED(hXresidualsClusterSize2_[planeID])->Fill(data.getXTrackResidualLocal(planeID));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resolution::yResolution(bool pass, int planeID, const Data& data, int threadNumber)
{
     if(!pass || !data.getBelongsToTrack(planeID))
        return;

    int   size    = data.getClusterSize(planeID);
    int   maxHits = 2;

    if(size > maxHits)
        return;

    const Window* theWindow = theWindowsManager_->getWindow(planeID);

    for(int h=0; h<size; h++)
    {
        if( !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID)) ) //i due hit devono stare nella finestra
            return;
    }

    THREADED(hYresiduals_[planeID])->Fill(data.getYTrackResidualLocal(planeID));

    if(size == 1)
        THREADED(hYresidualsClusterSize1_[planeID])->Fill(data.getYTrackResidualLocal(planeID));
    if(size == 2)
    {
        if(data.getClusterPixelCol(0,planeID) != data.getClusterPixelCol(1,planeID))
            return;

        THREADED(hYresidualsClusterSize2_[planeID])->Fill(data.getYTrackResidualLocal(planeID));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resolution::setCutsFormula(std::map<std::string,std::string> cutsList, std::vector<TTree*> tree)
{
    std::vector<TTreeFormula*> formulasVector;

    for(std::map<std::string,std::string>::iterator it=cutsList.begin(); it!=cutsList.end(); it++)
    {
        if((it->first) == "main cut" && (it->second).size()==0)
            STDLINE("WARNING: no main cut set in resolution analysis!! Default value = true!", ACRed);

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
bool Resolution::passStandardCuts(int planeID, const Data &data)
{
    XmlParser* theParser = theAnalysisManager_->getXmlParser();
    if(theParser->getAnalysesFromString("Charge")->applyStandardCuts()) {
        return true;
    }

    int minHits = 7;//To calculate efficiency on the telescope
    int excludeMe = 0;
    if(thePlaneMapping_->getPlaneName(planeID).find("Dut") != std::string::npos)//Dut case
        minHits = atoi(theParser->getAnalysesFromString("Charge")->getMinHits().c_str());
    else if(data.getHasHit(planeID) && data.getClusterSize(planeID)<=2)//Telescope case
        excludeMe = 1;

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
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resolution::beginJob(void)
{
    theWindowsManager_ = theAnalysisManager_->getWindowsManager();
    book();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resolution::analyze(const Data& data, int threadNumber)//WARNING: You can't change this name (threadNumber) or the MACRO THREAD won't compile
{   
    bool mainCut;
    if(cutsFormulas_.find("main cut")!=cutsFormulas_.end())
        mainCut = cutsFormulas_["main cut"][threadNumber]->EvalInstance();
    else
        mainCut = true;

    if(!mainCut)
        return;

    bool errorsCut;
    if(cutsFormulas_.find("errors")!=cutsFormulas_.end())
        errorsCut = cutsFormulas_["errors"][threadNumber]->EvalInstance();
    else
        errorsCut = mainCut;

    bool xResolutionCut;
    if(cutsFormulas_.find("X resolution")!=cutsFormulas_.end())
        xResolutionCut = cutsFormulas_["X resolution"][threadNumber]->EvalInstance();
    else
        xResolutionCut = mainCut;

    bool yResolutionCut;
    if(cutsFormulas_.find("Y resolution")!=cutsFormulas_.end())
        yResolutionCut = cutsFormulas_["Y resolution"][threadNumber]->EvalInstance();
    else
        yResolutionCut = mainCut;

    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {
        if(!passStandardCuts(p,data))
            continue;
        \
        calculateXresiduals(xResolutionCut,p,data,threadNumber);
        calculateYresiduals(yResolutionCut,p,data,threadNumber);
        xResolution(xResolutionCut,p,data,threadNumber);
        yResolution(yResolutionCut,p,data,threadNumber);

       if(thePlaneMapping_->getPlaneName(p).find("Dut")!=std::string::npos)
           predictedErrors(errorsCut,p,data,threadNumber);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resolution::endJob(void)
{
    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {
        ADD_THREADED(hXresiduals_            [p]);
        ADD_THREADED(hXresidualCalculated_   [p]);
        ADD_THREADED(hXresidualsClusterSize1_[p]);
        ADD_THREADED(hXresidualsClusterSize2_[p]);
        ADD_THREADED(hYresiduals_            [p]);
        ADD_THREADED(hYresidualCalculated_   [p]);
        ADD_THREADED(hYresidualsClusterSize1_[p]);
        ADD_THREADED(hYresidualsClusterSize2_[p]);

        hXresiduals_            [p]->GetXaxis()->SetTitle("x residual (um)");
        hXresidualCalculated_   [p]->GetXaxis()->SetTitle("x residual (um)");
        hXresidualsClusterSize1_[p]->GetXaxis()->SetTitle("x residual (um)");
        hXresidualsClusterSize2_[p]->GetXaxis()->SetTitle("x residual (um)");
        hYresiduals_            [p]->GetXaxis()->SetTitle("y residual (um)");
        hYresidualCalculated_   [p]->GetXaxis()->SetTitle("y residual (um)");
        hYresidualsClusterSize1_[p]->GetXaxis()->SetTitle("y residual (um)");
        hYresidualsClusterSize2_[p]->GetXaxis()->SetTitle("y residual (um)");

        if(thePlaneMapping_->getPlaneName(p).find("Dut")!=std::string::npos)
        {
            ADD_THREADED(hPredictedXErrors_[p-8]);
            ADD_THREADED(hPredictedYErrors_[p-8]);

            hPredictedXErrors_[p-8]->GetXaxis()->SetTitle("x error (um)");
            hPredictedYErrors_[p-8]->GetXaxis()->SetTitle("y error (um)");
        }

        if(p == 9)
        {
            std::stringstream ss;
            STDLINE("Single hits residuals:", ACRed);
            ss.str(""); ss << "X Residuals: " << hXresidualsClusterSize1_[p]->GetMean();
            STDLINE(ss.str(),ACGreen);
            ss.str(""); ss << "Y Residuals: " << hYresidualsClusterSize1_[p]->GetMean();
            STDLINE(ss.str(),ACGreen);
            STDLINE("",ACWhite);

            STDLINE("Double hits residuals:", ACRed);
            ss.str(""); ss << "X Residuals: " << hXresidualsClusterSize2_[p]->GetMean();
            STDLINE(ss.str(),ACGreen);
            ss.str(""); ss << "Y Residuals: " << hYresidualsClusterSize2_[p]->GetMean();
            STDLINE(ss.str(),ACGreen);
            STDLINE("",ACWhite);

            STDLINE("Single and double hits residuals:", ACRed);
            ss.str(""); ss << "X Residuals: " << hXresiduals_[p]->GetMean();
            STDLINE(ss.str(),ACGreen);
            ss.str(""); ss << "Y Residuals: " << hYresiduals_[p]->GetMean();
            STDLINE(ss.str(),ACGreen);
            STDLINE("",ACWhite);
        }
    }       
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Resolution::book(void)
{
    destroy();

    std::string hName;
    std::string hTitle;
    std::string planeName;

    theAnalysisManager_->cd("/");
    theAnalysisManager_->mkdir("Resolution");

    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {
        planeName = thePlaneMapping_->getPlaneName(p);
        theAnalysisManager_->cd("Resolution");
        theAnalysisManager_->mkdir(planeName);

        theAnalysisManager_->mkdir("X Residuals");

        hName  = "hXresiduals_"                               + planeName;
        hTitle = "X residuals  "                              + planeName;
        hXresiduals_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 125, -500, 500)));

        hName  = "hXresidualCalculated_"                      + planeName;
        hTitle = "X residuals calculated from asimmetry fit " + planeName;
        hXresidualCalculated_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 125, -500, 500)));

        hName  = "hXresidualsClusterSize1_"                   + planeName;
        hTitle = "X residuals cluster size 1 "                + planeName;
        hXresidualsClusterSize1_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 125, -500, 500)));

        hName  = "hXresidualsClusterSize2_"                   + planeName;
        hTitle = "X residuals cluster size 2 "                + planeName;
        hXresidualsClusterSize2_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 125, -500, 500)));

        theAnalysisManager_->cd("Resolution/" + planeName);

        theAnalysisManager_->mkdir("Y Residuals");

        hName  = "hYresiduals_"                               + planeName;
        hTitle = "Y residuals "                               + planeName;
        hYresiduals_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 125, -500, 500)));

        hName  = "hYresidualCalculated_"                      + planeName;
        hTitle = "Y residuals calculated from asimmetry fit " + planeName;
        hYresidualCalculated_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 125, -500, 500)));

        hName  = "hYresidualsClusterSize1_"                   + planeName;
        hTitle = "Y residuals cluster size 1 "                + planeName;
        hYresidualsClusterSize1_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 125, -500, 500)));

        hName  = "hYresidualsClusterSize2_"                   + planeName;
        hTitle = "Y residuals cluster size 2 "                + planeName;
        hYresidualsClusterSize2_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 125, -500, 500)));

        if(planeName.find("Dut")!=std::string::npos)
        {
            theAnalysisManager_->cd("Resolution/" + planeName);

            theAnalysisManager_->mkdir("Errors");

            hName  = "hPredictedXErrors_"                     + planeName;
            hTitle = "predicted X errors "                    + planeName;
            hPredictedXErrors_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 10000, 0, 10)));

            hName  = "hPredictedYErrors_"                     + planeName;
            hTitle = "predicted Y errors "                    + planeName;
            hPredictedYErrors_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(), 10000, 0, 10)));
        }
    }
}

