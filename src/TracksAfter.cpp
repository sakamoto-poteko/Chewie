/**
    Modified TracksAfter.cpp

    Loading data to change value in fitEtaFunc(..)


    Currently maintined by Afa.L Cheng <alpha@tomatoeskit.org>
 **/

#include <QSettings>

#include "TracksAfter.h"

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

#include <iostream>
#include <cmath>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TracksAfter::TracksAfter(AnalysisManager* analysisManager, int nOfThreads) :
    Analysis          (analysisManager, nOfThreads)
  , thePlaneMapping_  (0),
    // Default value for Thickness, DepletionVoltage and AppliedVoltage
    DUT0_Thickness(500), DUT0_DepletionVoltage(5), DUT0_AppliedVoltage(500),
    DUT1_Thickness(500), DUT1_DepletionVoltage(5), DUT1_AppliedVoltage(500)
{
    thePlaneMapping_ = new PlanesMapping();
    inFile_ = 0;

/*    for(int p=0; p<4; p++)
    {
        parMin_      [p] = 0;
        parMax_      [p] = 0;
        isMinToLimit_[p] = 0;
        isMaxToLimit_[p] = 0;
    }
    totEventsControl_ = 0;   */

    // Must make sure file is correct. No error check here
    QSettings tracksAfterData("TracksAfterData", QSettings::IniFormat, this);
    tracksAfterData.beginGroup("DUT0");
    if (tracksAfterData.contains("Thickness"))
        DUT0_Thickness = tracksAfterData.value("Thickness").toInt();
    if (tracksAfterData.contains("DepletionVoltage"))
        DUT0_DepletionVoltage = tracksAfterData.value("DepletionVoltage").toInt();
    if (tracksAfterData.contains("AppliedVoltage"))
        DUT0_AppliedVoltage = tracksAfterData.value("AppliedVoltage").toInt();
    tracksAfterData.endGroup();

    tracksAfterData.beginGroup("DUT1");
    if (tracksAfterData.contains("Thickness"))
        DUT1_Thickness = tracksAfterData.value("Thickness").toInt();
    if (tracksAfterData.contains("DepletionVoltage"))
        DUT1_DepletionVoltage = tracksAfterData.value("DepletionVoltage").toInt();
    if (tracksAfterData.contains("AppliedVoltage"))
        DUT1_AppliedVoltage = tracksAfterData.value("AppliedVoltage").toInt();
    tracksAfterData.endGroup();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TracksAfter::~TracksAfter(void)
{
    if(thePlaneMapping_)
    {
        delete thePlaneMapping_;
        thePlaneMapping_ = 0;
    }

    destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TracksAfter::destroy(void)
{
    if(Analysis::fDoNotDelete_) return;

    /*--------------------------------------------------------------------------------------------Cluster size--------------------------------------------------------------------------------------------------*/
//    for(std::vector<TH1F*>::iterator it=hClusterSize_                           .begin(); it!=hClusterSize_                           .end(); it++) delete *it; hClusterSize_                           .clear();
}

bool TracksAfter::passCalibrationsCut(int planeID, const Data &data)
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
//    bool   pass = false;
    bool pass = true;
//    double parValue;
    for(int h=0; h<data.getClusterSize(planeID); h++)
    {
        row = data.getClusterPixelRow(h,planeID);
        col = data.getClusterPixelCol(h,planeID);
        for(int p=0; p<4; p++)
        {
/*            parValue = h2DparsPlots_[p]->GetBinContent(h2DparsPlots_[p]->GetXaxis()->FindBin(col),h2DparsPlots_[p]->GetYaxis()->FindBin(row));
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
            }  */
        }
    }

    return pass;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TracksAfter::getInFile (TFile * infile)
{
    if (inFile_) delete inFile_;
    inFile_ = infile;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TracksAfter::passStandardCuts(int planeID, const Data &data)
{
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
void TracksAfter::beginJob(void)
{
/*    XmlParser* theParser = theAnalysisManager_->getXmlParser();
    threashold_   = theParser->getAnalyses()["Tracks"]->getThreashold();
    maxTracks_    = theParser->getAnalyses()["Tracks"]->getMaxTracks();
    minTotTracks_ = theParser->getAnalyses()["Tracks"]->getMinTotTracks();
    maxTotTracks_ = theParser->getAnalyses()["Tracks"]->getMaxTotTracks();   */

    book();

//    setParsLimits();
}

void TracksAfter::positionUnfolded (int planeID)
{
    float pn1x, pc1x, pn2x, pc2x;
    float pn1y, pc1y, pn2y, pc2y;
/*    for (int i = 0; i < hXChargedistribution1D_[planeID]->GetNbinsX(); ++i) {
        for (int j = 0; j < hXChargedistribution1D_[planeID]->GetNbinsY(); ++j) {
            XProbabilityFunc_[planeID]->Fill();
        }
    }
*/
    float binSize = 5;
    float pixelLengthX = 150;
    float pixelLengthY = 100;

    if (!hXdistribution1D_[planeID]) std::cout << "Didn't get the Histo hXdistribution1D " << planeID << "!!!\n";
    if (!hXdistribution2D_[planeID]) std::cout << "Didn't get the Histo hXdistribution2D " << planeID << "!!!\n";
    if (!hXChargedistribution1D_[planeID]) std::cout << "Didn't get the Histo hXChargedistribution1D " << planeID << "!!!\n";
    if (!hXChargedistribution1D_[planeID]) std::cout << "Didn't get the Histo hXChargedistribution2D " << planeID << "!!!\n";
    if (!hYdistribution1D_[planeID]) std::cout << "Didn't get the Histo hYdistribution1D " << planeID << "!!!\n";
    if (!hYdistribution2D_[planeID]) std::cout << "Didn't get the Histo hYdistribution2D " << planeID << "!!!\n";
    if (!hYChargedistribution1D_[planeID]) std::cout << "Didn't get the Histo hYChargedistribution1D " << planeID << "!!!\n";
    if (!hYChargedistribution2D_[planeID]) std::cout << "Didn't get the Histo hYChargedistribution2D " << planeID << "!!!\n";

    for (float x = -pixelLengthX/2 + binSize/2; x < pixelLengthX/2 + binSize/2; x += binSize) {
        for (float c = 8; c < 80008; c += 16) {
            pn1x = hXdistribution1D_[planeID]->GetBinContent(hXdistribution1D_[planeID]->FindBin(x));
            pn2x = hXdistribution2D_[planeID]->GetBinContent(hXdistribution2D_[planeID]->FindBin(x));
            pc1x = hXChargedistribution1D_[planeID]->GetBinContent(hXChargedistribution1D_[planeID]->FindBin(x, c));
            pc2x = hXChargedistribution2D_[planeID]->GetBinContent(hXChargedistribution2D_[planeID]->FindBin(x, c));
            XProbabilityFunc_[planeID]->Fill(x, c, pn1x*pc1x + pn2x*pc2x);
        }
    }

    for (float y = -pixelLengthY/2 + binSize/2; y < pixelLengthY/2 + binSize/2; y += binSize) {
        for (float c = 8; c < 80008; c += 16) {
            pn1y = hYdistribution1D_[planeID]->GetBinContent(hYdistribution1D_[planeID]->FindBin(y));
            pn2y = hYdistribution2D_[planeID]->GetBinContent(hYdistribution2D_[planeID]->FindBin(y));
            pc1y = hYChargedistribution1D_[planeID]->GetBinContent(hYChargedistribution1D_[planeID]->FindBin(y, c));
            pc2y = hYChargedistribution2D_[planeID]->GetBinContent(hYChargedistribution2D_[planeID]->FindBin(y, c));
            YProbabilityFunc_[planeID]->Fill(y, c, pn1y*pc1y + pn2y*pc2y);
        }
    }

}

float TracksAfter::findMaxHisto2DX (int planeID, float charge)
{
    float binSize = 5;
    float pixelLengthX = 150;

    float x = -pixelLengthX/2 + binSize/2;
    float xmax = x;
    float val = XProbabilityFunc_[planeID]->GetBinContent(hXChargedistribution1D_[planeID]->FindBin(x, charge));
    float valmax = val;
    for (x += binSize; x < pixelLengthX/2 + binSize/2; x += binSize) {
        val = XProbabilityFunc_[planeID]->GetBinContent(hXChargedistribution1D_[planeID]->FindBin(x, charge));
        if (val > valmax) {
            valmax = val;
            xmax = x;
        }
    }
    return xmax;
}

float TracksAfter::findMaxHisto2DY (int planeID, float charge)
{
    float binSize = 5;
    float pixelLengthY = 100;

    float x = -pixelLengthY/2 + binSize/2;
    float xmax = x;
    float val = YProbabilityFunc_[planeID]->GetBinContent(hYChargedistribution1D_[planeID]->FindBin(x, charge));
    float valmax = val;
    for (x += binSize; x < pixelLengthY/2 + binSize/2; x += binSize) {
        val = YProbabilityFunc_[planeID]->GetBinContent(hYChargedistribution1D_[planeID]->FindBin(x, charge));
        if (val > valmax) {
            valmax = val;
            xmax = x;
        }
    }
    return xmax;
}

void TracksAfter::unfold (int planeID, float charge, float xp, float yp, int threadNumber)
{
    float xr = findMaxHisto2DX (planeID, charge);
    float yr = findMaxHisto2DY (planeID, charge);

    THREADED(XYReconstructed_[planeID])->Fill(xr, yr);
    THREADED(XResidualsReconstructed_[planeID])->Fill(xp - xr);
    THREADED(YResidualsReconstructed_[planeID])->Fill(yp - yr);
}

void TracksAfter::fitEtaFunc (int planeID, std::string type)
{
    std::stringstream ss;

    XAsimmetryFunc_[planeID]->SetLineColor(kRed);
    XAsimmetryFuncInv_[planeID]->SetLineColor(kBlue);

    XAsimmetryFunc_[planeID]->SetParName(0, "Thickness                        ");
    XAsimmetryFunc_[planeID]->SetParName(1, "2*mu*DepletionVoltage            ");
    XAsimmetryFunc_[planeID]->SetParName(2, "DepletionVoltage                 ");
    XAsimmetryFunc_[planeID]->SetParName(3, "AppliedVoltage + DepletionVoltage");
    XAsimmetryFunc_[planeID]->SetParName(4, "4*DiffusionConstant              ");
    XAsimmetryFunc_[planeID]->SetParName(5, "ScaleFactor                      ");

    XAsimmetryFuncInv_[planeID]->SetParName(0, "Intercept");
    XAsimmetryFuncInv_[planeID]->SetParName(1, "Slope    ");

    if (type == "Size2")
    {
        XAsimmetryFunc_[planeID]->SetRange(-20., 20.);

        XAsimmetryFunc_[planeID]->FixParameter(5, 1. );
/*        XAsimmetryFunc_[planeID]->SetParameter(4, 3 );
        XAsimmetryFunc_[planeID]->SetParLimits(4, 0.1, 100);
        XAsimmetryFunc_[planeID]->SetParameter(3, 525);
        XAsimmetryFunc_[planeID]->SetParLimits(3, 510, 600);
        XAsimmetryFunc_[planeID]->SetParameter(2, 25 );
        XAsimmetryFunc_[planeID]->SetParLimits(2, 10 , 100);
        XAsimmetryFunc_[planeID]->SetParameter(1, 2 );
        XAsimmetryFunc_[planeID]->SetParLimits(2, 0.1 , 100);
//        XAsimmetryFunc_[planeID]->SetParameter(0, 100);
//        XAsimmetryFunc_[planeID]->SetParLimits(0, 50 , 500);  */
        XAsimmetryFunc_[planeID]->FixParameter(0, 500);
        XAsimmetryFunc_[planeID]->FixParameter(1, 0.02);
        XAsimmetryFunc_[planeID]->FixParameter(2, 5.);
        XAsimmetryFunc_[planeID]->FixParameter(3, 505.);
        XAsimmetryFunc_[planeID]->FixParameter(4, 0.09);

    }

    else if (type == "SizeLE2")
    {
        XAsimmetryFunc_[planeID]->FixParameter(5, 1. );
/*        XAsimmetryFunc_[planeID]->SetParameter(4, 0.01 );
        XAsimmetryFunc_[planeID]->SetParLimits(4, 0.001, 3);
        XAsimmetryFunc_[planeID]->SetParameter(3, 525);
        XAsimmetryFunc_[planeID]->SetParLimits(3, 510, 600);
        XAsimmetryFunc_[planeID]->SetParameter(2, 25 );
        XAsimmetryFunc_[planeID]->SetParLimits(2, 10 , 100);
        XAsimmetryFunc_[planeID]->SetParameter(1, 0.5 );
        XAsimmetryFunc_[planeID]->SetParLimits(2, 0.01 , 9);
//        XAsimmetryFunc_[planeID]->SetParameter(0, 100);
//        XAsimmetryFunc_[planeID]->SetParLimits(0, 50 , 500);  */
        XAsimmetryFunc_[planeID]->FixParameter(0, 500);
        XAsimmetryFunc_[planeID]->FixParameter(1, 1.5);
        XAsimmetryFunc_[planeID]->FixParameter(2, 5.);
        XAsimmetryFunc_[planeID]->FixParameter(3, 505.);
        XAsimmetryFunc_[planeID]->FixParameter(4, 0.07);

    }

    else if (type == "SizeLE2Inv")
    {
        XAsimmetryFuncInv_[planeID]->SetLineColor(kBlue); //No need to worry about fixing/setting parameters, fitting a rect should be easy... It's just a check
    }

    else {
        STDLINE("Type of size constraint not recognized!", ACRed);
        return;
    }

    // NOTE: Additional settings start
    // X Axis
    if (planeID == 8 || planeID == 9) {
        // NOTE: Not sure planeID 9
        double mu = 0;
        if (type == "Size2")
            mu = 0.002;
        if (type == "SizeLE2")
            mu = 0.003;

        if (type != "SizeLE2Inv") {
            // Thickness
            XAsimmetryFunc_[planeID]->FixParameter(0, planeID == 8 ? DUT0_Thickness : DUT1_Thickness);

            // 2*mu*DepletionVoltage
            XAsimmetryFunc_[planeID]->FixParameter(1, 2 * mu * planeID == 8 ? DUT0_DepletionVoltage : DUT1_DepletionVoltage);

            // Depletion Voltage
            XAsimmetryFunc_[planeID]->FixParameter(2, planeID == 8 ? DUT0_DepletionVoltage : DUT1_DepletionVoltage);

            // AppliedVoltage + DepletionVoltage
            XAsimmetryFunc_[planeID]->FixParameter(3, planeID == 8 ? DUT0_AppliedVoltage : DUT1_AppliedVoltage + planeID == 8 ? DUT0_DepletionVoltage : DUT1_DepletionVoltage);
        }
    }

    // NOTE: Additional settings end

    STDLINE("Fitting eta function in coordinate x for plane " + thePlaneMapping_->getPlaneName(planeID) + ", size constraint: " + type, ACGreen);

    if (type == "Size2")
    {
        if (planeID == 9)
            hXAsimmetry_[planeID]->Fit(XAsimmetryFunc_[planeID], ""); // fitting method "QRBL"?
    }
    else if (type == "SizeLE2")
    {
        if (planeID == 9)
            hXAsimmetryLE2_[planeID]->Fit(XAsimmetryFunc_[planeID], "L"); // fitting method "QRBL"?
    }
    else if (type == "SizeLE2Inv")
    {
        if (planeID == 9)
            hXAsimmetryInv_[planeID]->Fit(XAsimmetryFuncInv_[planeID], "", "", -0.6, 0.6); // fitting method "QRBL"?
    }

    ss.str("");
    ss << XAsimmetryFunc_[planeID]->GetParameter(0) << " +/- " << XAsimmetryFunc_[planeID]->GetParError(0);
    STDLINE("Thickness (um):                         " + ss.str(), ACYellow);
    ss.str("");
    ss << XAsimmetryFunc_[planeID]->GetParameter(1) << " +/- " << XAsimmetryFunc_[planeID]->GetParError(1);
    STDLINE("2 * mobility * depletion voltage (1/s): " + ss.str(), ACYellow);
    ss.str("");
    ss << XAsimmetryFunc_[planeID]->GetParameter(2) << " +/- " << XAsimmetryFunc_[planeID]->GetParError(2);
    STDLINE("Depletion voltage (V):                  " + ss.str(), ACYellow);
    ss.str("");
    ss << XAsimmetryFunc_[planeID]->GetParameter(3) << " +/- " << XAsimmetryFunc_[planeID]->GetParError(3);
    STDLINE("Applied + depletion voltage (V):        " + ss.str(), ACYellow);
    ss.str("");
    ss << XAsimmetryFunc_[planeID]->GetParameter(4) << " +/- " << XAsimmetryFunc_[planeID]->GetParError(4);
    STDLINE("4 * Diffusion Constant (um*um/s):       " + ss.str(), ACYellow);

    YAsimmetryFunc_[planeID]->SetLineColor(kRed);
    YAsimmetryFuncInv_[planeID]->SetLineColor(kBlue);

    YAsimmetryFunc_[planeID]->SetParName(0, "Thickness                        ");
    YAsimmetryFunc_[planeID]->SetParName(1, "2*mu*DepletionVoltage            ");
    YAsimmetryFunc_[planeID]->SetParName(2, "DepletionVoltage                 ");
    YAsimmetryFunc_[planeID]->SetParName(3, "AppliedVoltage + DepletionVoltage");
    YAsimmetryFunc_[planeID]->SetParName(4, "4*DiffusionConstant              ");
    YAsimmetryFunc_[planeID]->SetParName(5, "ScaleFactor                      ");

    YAsimmetryFuncInv_[planeID]->SetParName(0, "Intercept");
    YAsimmetryFuncInv_[planeID]->SetParName(1, "Slope    ");

    if (type == "Size2")
    {
        YAsimmetryFunc_[planeID]->FixParameter(5, 1.);
/*        YAsimmetryFunc_[planeID]->SetParameter(4, 3. );
        YAsimmetryFunc_[planeID]->SetParLimits(4, 0.1, 100);
        YAsimmetryFunc_[planeID]->SetParameter(3, 525);
        YAsimmetryFunc_[planeID]->SetParLimits(3, 510, 600);
        YAsimmetryFunc_[planeID]->SetParameter(2, 25 );
        YAsimmetryFunc_[planeID]->SetParLimits(2, 10 , 100);
        YAsimmetryFunc_[planeID]->SetParameter(1, 2. );
        YAsimmetryFunc_[planeID]->SetParLimits(1, 0.1 , 100);
//        YAsimmetryFunc_[planeID]->SetParameter(0, 100);
//        YAsimmetryFunc_[planeID]->SetParLimits(0, 50 , 500); */
        YAsimmetryFunc_[planeID]->FixParameter(0, 500);
        YAsimmetryFunc_[planeID]->FixParameter(1, 0.02);
        YAsimmetryFunc_[planeID]->FixParameter(2, 5.);
        YAsimmetryFunc_[planeID]->FixParameter(3, 505.);
        YAsimmetryFunc_[planeID]->FixParameter(4, 0.09);

    }

    else if (type == "SizeLE2")
    {
        YAsimmetryFunc_[planeID]->FixParameter(5, 1.);
/*        YAsimmetryFunc_[planeID]->SetParameter(4, 0.01 );
        YAsimmetryFunc_[planeID]->SetParLimits(4, 0.001, 0.3);
        YAsimmetryFunc_[planeID]->SetParameter(3, 525);
        YAsimmetryFunc_[planeID]->SetParLimits(3, 510, 600);
        YAsimmetryFunc_[planeID]->SetParameter(2, 25 );
        YAsimmetryFunc_[planeID]->SetParLimits(2, 10 , 100);
        YAsimmetryFunc_[planeID]->SetParameter(1, 1 );
        YAsimmetryFunc_[planeID]->SetParLimits(1, 0.1 , 9);
//        YAsimmetryFunc_[planeID]->SetParameter(0, 100);
//        YAsimmetryFunc_[planeID]->SetParLimits(0, 50 , 500);  */
        YAsimmetryFunc_[planeID]->FixParameter(0, 500);
        YAsimmetryFunc_[planeID]->FixParameter(1, 1.5);
        YAsimmetryFunc_[planeID]->FixParameter(2, 5.);
        YAsimmetryFunc_[planeID]->FixParameter(3, 505.);
        YAsimmetryFunc_[planeID]->FixParameter(4, 0.07);

    }

    else if (type == "SizeLE2Inv")
    {
        XAsimmetryFuncInv_[planeID]->SetLineColor(kBlue); //No need to worry about fixing/setting parameters, fitting a rect should be easy... It's just a check
    }

    else {
        STDLINE("Type of size constraint not recognized!", ACRed);
        return;
    }

    // NOTE: Additional settings start
    // Y Axis
    if (planeID == 8 || planeID == 9) {
        // NOTE: Not sure planeID 9
        double mu = 0;
        if (type == "Size2")
            mu = 0.002;
        if (type == "SizeLE2")
            mu = 0.003;

        if (type != "SizeLE2Inv") {
            // Thickness
            YAsimmetryFunc_[planeID]->FixParameter(0, planeID == 8 ? DUT0_Thickness : DUT1_Thickness);

            // 2*mu*DepletionVoltage
            YAsimmetryFunc_[planeID]->FixParameter(1, 2 * mu * planeID == 8 ? DUT0_DepletionVoltage : DUT1_DepletionVoltage);

            // Depletion Voltage
            YAsimmetryFunc_[planeID]->FixParameter(2, planeID == 8 ? DUT0_DepletionVoltage : DUT1_DepletionVoltage);

            // AppliedVoltage + DepletionVoltage
            YAsimmetryFunc_[planeID]->FixParameter(3, planeID == 8 ? DUT0_AppliedVoltage : DUT1_AppliedVoltage + planeID == 8 ? DUT0_DepletionVoltage : DUT1_DepletionVoltage);
        }
    }

    // NOTE: Additional settings end


    STDLINE("Fitting eta function in coordinate y for plane " + thePlaneMapping_->getPlaneName(planeID) + ", size constraint: " + type, ACGreen);

    if (type == "Size2")
    {
        if (planeID == 9)
            hYAsimmetry_[planeID]->Fit(YAsimmetryFunc_[planeID], ""); // fitting method "QRBL"?
    }
    else if (type == "SizeLE2")
    {
        if (planeID == 9)
            hYAsimmetryLE2_[planeID]->Fit(YAsimmetryFunc_[planeID], "L"); // fitting method "QRBL"?
    }
    else if (type == "SizeLE2Inv")
    {
        if (planeID == 9)
            hYAsimmetryInv_[planeID]->Fit(YAsimmetryFuncInv_[planeID], "", "", -0.6, 0.6); // fitting method "QRBL"?
    }

    ss.str("");
    ss << YAsimmetryFunc_[planeID]->GetParameter(0) << " +/- " << YAsimmetryFunc_[planeID]->GetParError(0);
    STDLINE("Thickness (um):                         " + ss.str(), ACYellow);
    ss.str("");
    ss << YAsimmetryFunc_[planeID]->GetParameter(1) << " +/- " << YAsimmetryFunc_[planeID]->GetParError(1);
    STDLINE("2 * mobility * depletion voltage (1/s): " + ss.str(), ACYellow);
    ss.str("");
    ss << YAsimmetryFunc_[planeID]->GetParameter(2) << " +/- " << YAsimmetryFunc_[planeID]->GetParError(2);
    STDLINE("Depletion voltage (V):                  " + ss.str(), ACYellow);
    ss.str("");
    ss << YAsimmetryFunc_[planeID]->GetParameter(3) << " +/- " << YAsimmetryFunc_[planeID]->GetParError(3);
    STDLINE("Applied + depletion voltage (V):        " + ss.str(), ACYellow);
    ss.str("");
    ss << YAsimmetryFunc_[planeID]->GetParameter(4) << " +/- " << YAsimmetryFunc_[planeID]->GetParError(4);
    STDLINE("4 * Diffusion Constant (um*um/s):       " + ss.str(), ACYellow);

}

///////////JUST A TEST TO CHECK IF THE ETA FUNCTION WORKS/////////////////////////////////////////////////////////////

void TracksAfter::testPredictedFunc (void)
{
//    double binSize = 5;
//    double pixelLengthX = 150;
    double pixelLengthY = 100;
    double * xt = new double;
    int n = 0;
    double parameters[6] = {500.,0.5, 70., 570., 0.002, 1.};
    double parameters2[5] = {4.75, 9.81, 1312., 0., 98.};
    double parameters3[9] = {500.,6.5, 0.07, 1., 4.76, 9.28, 1312, 0, 98};
    double sum = 0;
    for (float x = -pixelLengthY/2; x < pixelLengthY/2; x += 1) {
        *xt = x;
        funcPredicted_->SetBinContent(n, Utilities::uniformCenterSmearedPlusConstantNorm(xt, parameters2));
        sum += Utilities::uniformCenterSmearedPlusConstantNorm(xt, parameters2)*1;
//        std::cout << "Test point: " << x << ", " << Utilities::etaFitFunc(xt, parameters) << "!!!!\n";
        funcPredicted2_->SetBinContent(n, Utilities::etaSmeared(xt, parameters3));
        funcPredicted3_->SetBinContent(n, Utilities::etaInverseFitFunc(xt, parameters));
        ++n;
    }
    std::stringstream ss;
    ss << "The area of the normalized function is " << sum;
    STDLINE(ss.str(), ACYellow);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TracksAfter::loadEtaFunc (std::string type)
{
    float x, y;

    double fitParameters[6];
    for (unsigned int p = 0; p < thePlaneMapping_->getNumberOfPlanes(); ++p)
    {
        STDLINE("Loading eta functions for plane " + thePlaneMapping_->getPlaneName(p), ACGreen);

        if (p != 9) continue;
        if (p != 10) continue;//to avoid any loading when not required

        if (type == "Size2")
        {
//            if (p != 10) continue;//to avoid any loading when not required

            XAsimmetryFunc_[p]->GetParameters(fitParameters);
            for (int n = 0; n < computedEtaFuncXSize2_[p]->GetNbinsX()+1; ++n)
            {
                x = computedEtaFuncXSize2_[p]->GetBinCenter(n);
                computedEtaFuncXSize2_[p]->Fill(x, XAsimmetryFunc_[p]->Eval(x));
            }

            YAsimmetryFunc_[p]->GetParameters(fitParameters);
            for (int m = 0; m < computedEtaFuncYSize2_[p]->GetNbinsX()+1; ++m)
            {
                y = computedEtaFuncYSize2_[p]->GetBinCenter(m);
                computedEtaFuncYSize2_[p]->Fill(y, YAsimmetryFunc_[p]->Eval(y));
            }
        }

        else if (type == "SizeLE2")
        {
            XAsimmetryFunc_[p]->GetParameters(fitParameters);
            for (int n = 0; n < computedEtaFuncXSizeLE2_[p]->GetNbinsX()+1; ++n)
            {
                x = computedEtaFuncXSizeLE2_[p]->GetBinCenter(n);
                computedEtaFuncXSizeLE2_[p]->Fill(x, XAsimmetryFunc_[p]->Eval(x));
            }

            YAsimmetryFunc_[p]->GetParameters(fitParameters);
            for (int m = 0; m < computedEtaFuncYSizeLE2_[p]->GetNbinsX()+1; ++m)
            {
                y = computedEtaFuncYSizeLE2_[p]->GetBinCenter(m);
                computedEtaFuncYSizeLE2_[p]->Fill(y, YAsimmetryFunc_[p]->Eval(y));
            }
        }

        else {
            STDLINE("Type of size constraint not recognized!", ACRed);
            return;
        }

    }

}

double TracksAfter::invertInHisto (int planeID, double etaValue, std::string coordinate, std::string type)
{
    float eta;
    float pos;
    double posFound = 1000;
    double etaFound = 1000;
    std::stringstream ss;

    if (planeID != 9) return 55;
    if (planeID != 10) return 55; //to avoid inverting when histo is not defined (see similar statement in loadEtaFunc

    if (coordinate == "x")
    {
        if (type == "Size2")
        {
//            return 55;

            for (int i = 0; i < computedEtaFuncXSize2_[planeID]->GetNbinsX(); ++i)
            {
                eta = computedEtaFuncXSize2_[planeID]->GetBinContent(i);
                pos = computedEtaFuncXSize2_[planeID]->GetBinCenter(i);
                if (fabs(eta-etaValue) < fabs(etaFound-etaValue)) {
                    etaFound = eta;
                    posFound = pos;
                }
            }
            if (posFound != 1000) return posFound;
            else {
                ss << etaValue;
                STDLINE("Couldn't invert eta funtion (x) for eta value " + ss.str() + " in plane " + thePlaneMapping_->getPlaneName(planeID), ACRed);
                return 1000;
            }
        }

        else if (type == "SizeLE2")
        {
//            return 55;

            for (int i = 0; i < computedEtaFuncXSizeLE2_[planeID]->GetNbinsX(); ++i)
            {
                eta = computedEtaFuncXSizeLE2_[planeID]->GetBinContent(i);
                pos = computedEtaFuncXSizeLE2_[planeID]->GetBinCenter(i);
                if (fabs(eta-etaValue) < fabs(etaFound-etaValue)) {
                    etaFound = eta;
                    posFound = pos;
                }
            }
            if (posFound != 1000) return posFound;
            else {
                ss << etaValue;
                STDLINE("Couldn't invert eta funtion (x) for eta value " + ss.str() + " in plane " + thePlaneMapping_->getPlaneName(planeID), ACRed);
                return 1000;
            }
        }

        else {
            STDLINE("Type of size constraint not recognized!", ACRed);
            return 1000;
        }

    }
    else if (coordinate == "y")
    {
        if (type == "Size2")
        {
//            return 55;

            for (int i = 0; i < computedEtaFuncYSize2_[planeID]->GetEntries(); ++i)
            {
                eta = computedEtaFuncYSize2_[planeID]->GetBinContent(i);
                pos = computedEtaFuncYSize2_[planeID]->GetBinCenter(i);
                if (fabs(eta-etaValue) < fabs(etaFound - etaValue)) {
                    etaFound = eta;
                    posFound = pos;
                }
            }
            if (posFound != 1000) return posFound;
            else {
                ss << etaValue;
                STDLINE("Couldn't invert eta funtion (y) for eta value " + ss.str() + " in plane " + thePlaneMapping_->getPlaneName(planeID), ACRed);
                return 1000;
            }
        }

        else if (type == "SizeLE2")
        {
            for (int i = 0; i < computedEtaFuncYSizeLE2_[planeID]->GetEntries(); ++i)
            {
//                return 55;

                eta = computedEtaFuncYSizeLE2_[planeID]->GetBinContent(i);
                pos = computedEtaFuncYSizeLE2_[planeID]->GetBinCenter(i);
                if (fabs(eta-etaValue) < fabs(etaFound - etaValue)) {
                    etaFound = eta;
                    posFound = pos;
                }
            }
            if (posFound != 1000) return posFound;
            else {
                ss << etaValue;
                STDLINE("Couldn't invert eta funtion (y) for eta value " + ss.str() + " in plane " + thePlaneMapping_->getPlaneName(planeID), ACRed);
                return 1000;
            }
        }

        else {
            STDLINE("Type of size constraint not recognized!", ACRed);
            return 1000;
        }

    }
    else
    {
        STDLINE("Coordinate " + coordinate + " not recognized!", ACRed);
        return 0;
    }
}

double TracksAfter::invertInHistoSingle (double etaValue, TH1F * histo)
{
    float eta;
    float pos;
    double posFound = 1000;
    double etaFound = 1000;
    std::stringstream ss;

    for (int i = 0; i < histo->GetNbinsX(); ++i)
    {
        eta = histo->GetBinContent(i);
        pos = histo->GetBinCenter(i);
        if (fabs(eta-etaValue) < fabs(etaFound-etaValue)) {
            etaFound = eta;
            posFound = pos;
        }
    }
    if (posFound != 1000) return posFound;
    else {
        ss << histo->GetName() << " for eta value " << etaValue;
        STDLINE("Couldn't invert eta funtion in graph " +  ss.str(), ACRed);
        return 1000;
    }
}

double TracksAfter::calculateInHisto (double value, TH1F *histo)
{
    int bin = histo->FindBin(value);
    return histo->GetBinContent(bin);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TracksAfter::calculateXResiduals (const Data &data, int planeID, int threadNumber)
{
    if( !data.getIsInDetector( planeID ) || !data.getHasHit( planeID ))
        return;

    const Window* theWindow = theAnalysisManager_->getWindowsManager()->getWindow (planeID);
    int           row       = data.getRowPredicted                                (planeID);
    int           col       = data.getColPredicted                                (planeID);

    int   hitID             = -1;
    int   totalCharge       = 0 ;
    int   chargeLeft        = 0 ;
    int   chargeRight       = 0 ;
    float Asimmetry         = 0 ;

    if( !theWindow->checkWindow(col,row) ) {
        return;
    }

    if (data.getClusterSize(planeID) == 2)
    {
        for(int h=0; h<2; ++h)
        {
            if(    !theWindow->checkWindow(data.getClusterPixelCol(h,planeID),data.getClusterPixelRow(h,planeID)) //hits are in the window
                || !data.getIsPixelCalibrated(h,planeID)                                                          //pixels are calibrated
                ||  data.getClusterPixelRow  (h,planeID) != row )                                                 //hits are on the same row (sharing is along the row - x direction)
                return;
        }

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
        }

        float Xp = 0;

        if(data.getXPixelResidualLocal( planeID ) > 0)
            Xp = data.getXPitchLocal( planeID )/2 - data.getXPixelResidualLocal( planeID );
        else if(data.getXPixelResidualLocal( planeID ) <= 0)
            Xp = -(data.getXPixelResidualLocal( planeID ) + data.getXPitchLocal( planeID )/2);

        totalCharge = chargeLeft + chargeRight;
        Asimmetry   = (float)(chargeLeft - chargeRight)/totalCharge;
        if(Asimmetry >= -0.7 && Asimmetry <= 0.7 && totalCharge <= 30000) {
            float Xm = invertInHisto(planeID, Asimmetry, "x", "Size2");
            float XmLE2 = invertInHisto(planeID, Asimmetry, "x", "SizeLE2");
//            float Xp = data.getXPixelResidualLocal(planeID);
            THREADED(hXResidualsAsimmetry_[planeID])->Fill(Xm - Xp);
            THREADED(hXResidualsAsimmetryLE2_[planeID])->Fill(XmLE2 - Xp);
        }
    }

    else if (data.getClusterSize(planeID) == 1)
    {
        float Xm = (data.getColPredicted(planeID) + 0.5)*data.getXPitchLocal(planeID);
//        float Xp = data.getXPixelResidualLocal(planeID);
        float Xp = 0;
        if(data.getXPixelResidualLocal( planeID ) > 0)
            Xp = data.getXPitchLocal( planeID )/2 - data.getXPixelResidualLocal( planeID );
        else if(data.getXPixelResidualLocal( planeID ) <= 0)
            Xp = -(data.getXPixelResidualLocal( planeID ) + data.getXPitchLocal( planeID )/2);

        THREADED(hXResidualsAsimmetryLE2_[planeID])->Fill(Xm - Xp);
    }

    else return;
}

void TracksAfter::calculateYResiduals (const Data &data, int planeID, int threadNumber)
{
    if( !data.getIsInDetector( planeID ) || !data.getHasHit( planeID ) || data.getClusterSize( planeID ) != 2 )
        return;

    const Window* theWindow = theAnalysisManager_->getWindowsManager()->getWindow(planeID);
    int           row       = data.getRowPredicted                               (planeID);
    int           col       = data.getColPredicted                               (planeID);

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

    if (data.getClusterSize(planeID) == 2)
    {
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
        }

        float Xp = 0;

        if(data.getYPixelResidualLocal( planeID ) > 0)
            Xp = data.getYPitchLocal( planeID )/2 - data.getYPixelResidualLocal( planeID );
        else if(data.getYPixelResidualLocal( planeID ) <= 0)
            Xp = -(data.getYPixelResidualLocal( planeID ) + data.getYPitchLocal( planeID )/2);

        totalCharge = chargeLeft + chargeRight;
        Asimmetry   = (float)(chargeLeft - chargeRight)/totalCharge;
        if(Asimmetry >= -0.5 && Asimmetry <= 0.5 && totalCharge <= 30000) {
            float Xm = invertInHisto(planeID, Asimmetry, "y", "Size2");
            float XmLE2 = invertInHisto(planeID, Asimmetry, "y", "SizeLE2");
//        float Xp = data.getYPixelResidualLocal(planeID);
            THREADED(hYResidualsAsimmetry_[planeID])->Fill(Xm - Xp);
            THREADED(hYResidualsAsimmetryLE2_[planeID])->Fill(XmLE2 - Xp);
        }
    }

    else if (data.getClusterSize(planeID) == 1)
    {
        float Xm = (data.getRowPredicted(planeID) + 0.5)*data.getYPitchLocal(planeID);
//        float Xp = data.getXPixelResidualLocal(planeID);
        float Xp = 0;
        if(data.getYPixelResidualLocal( planeID ) > 0)
            Xp = data.getYPitchLocal( planeID )/2 - data.getYPixelResidualLocal( planeID );
        else if(data.getYPixelResidualLocal( planeID ) <= 0)
            Xp = -(data.getYPixelResidualLocal( planeID ) + data.getYPitchLocal( planeID )/2);

        THREADED(hYResidualsAsimmetryLE2_[planeID])->Fill(Xm - Xp);
    }

    else return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TracksAfter::profileChi2Par (double *par, int parNumber, double liminf, double limsup)
{
    std::stringstream ss;
    ss << "chi2ProfilesPar_" << parNumber;
    if ((int)chi2ProfilesPar_.size() == parNumber) chi2ProfilesPar_.push_back(new TH1F(ss.str().c_str(), ss.str().c_str(), 200, liminf, limsup));
    double parTemp = par[parNumber];
    par[parNumber] = liminf;
    double step = (limsup-liminf)/200;
    std::cout << hXAsimmetryLE2_[9]->GetName() << "!!!\n";
    for (int i = 1; i < chi2ProfilesPar_[parNumber]->GetXaxis()->GetNbins(); ++i)
    {
        chi2ProfilesPar_[parNumber]->SetBinContent(i, Utilities::chi2Eta(par, hXAsimmetryLE2_[9])); std::cout << i << ", " << par[parNumber] << ": " << Utilities::chi2Eta(par, hXAsimmetryLE2_[9]) << " !!!\n";
        par[parNumber] += step;
    }
    par[parNumber] = parTemp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TracksAfter::computeEtaDerivative (int planeID)
{
    double etaD, eta, pos;
//    double error, Neta, Npos;
//    int posHisto = 0;

    for (int i = 0; i < hXEtaDistribution_[planeID]->GetXaxis()->GetNbins()+1; ++i)
    {
        if (hXEtaDistribution_[planeID]->GetBinContent(i) > 0) etaD = 1/hXEtaDistribution_[planeID]->GetBinContent(i);
        else etaD = 0;
        eta = hXEtaDistribution_[planeID]->GetBinCenter(i);
        pos = invertInHisto(planeID, eta, "x", "SizeLE2");
        computedEtaDerivativeXFitted_[planeID]->Fill(pos, etaD);
        pos = invertInHistoSingle(eta, hXAsimmetryLE2_[planeID]);
        computedEtaDerivativeXData_[planeID]->Fill(pos, etaD);
        pos = invertInHisto(planeID, eta, "x", "Size2");
        computedEtaDerivativeXFittedSize2_[planeID]->Fill(pos, etaD);
        pos = invertInHistoSingle(eta, hXAsimmetry_[planeID]);
        computedEtaDerivativeXDataSize2_[planeID]->Fill(pos, etaD);
        pos = XAsimmetryFuncInv_[planeID]->Eval(eta);
        computedEtaDerivativeXFittedSize2Inv_[planeID]->Fill(pos, etaD);
        pos = calculateInHisto(eta, hXAsimmetryInv_[planeID]);
        computedEtaDerivativeXDataSize2Inv_[planeID]->Fill(pos, etaD);
        computedEtaDerivativeXDataSize2InvNorm_[planeID]->Fill(pos);
    }

    for (int i = 0; i < hYEtaDistribution_[planeID]->GetXaxis()->GetNbins()+1; ++i)
    {
        if (hYEtaDistribution_[planeID]->GetBinContent(i) > 0) etaD = 1/hYEtaDistribution_[planeID]->GetBinContent(i);
        else etaD = 0;
        eta = hYEtaDistribution_[planeID]->GetBinCenter(i);
        pos = invertInHisto(planeID, eta, "y", "SizeLE2");
        computedEtaDerivativeYFitted_[planeID]->Fill(pos, etaD);
        pos = invertInHistoSingle(eta, hYAsimmetryLE2_[planeID]);
        computedEtaDerivativeYData_[planeID]->Fill(pos, etaD);
        pos = invertInHisto(planeID, eta, "y", "Size2");
        computedEtaDerivativeYFittedSize2_[planeID]->Fill(pos, etaD);
        pos = invertInHistoSingle(eta, hYAsimmetry_[planeID]);
        computedEtaDerivativeYDataSize2_[planeID]->Fill(pos, etaD);
        pos = YAsimmetryFuncInv_[planeID]->Eval(eta);
        computedEtaDerivativeYFittedSize2Inv_[planeID]->Fill(pos, etaD);
        pos = calculateInHisto(eta, hYAsimmetryInv_[planeID]);
        computedEtaDerivativeYDataSize2Inv_[planeID]->Fill(pos, etaD);
        computedEtaDerivativeYDataSize2InvNorm_[planeID]->Fill(pos);
/*        Neta = calculateInHisto(eta, hYEtaDistribution_[planeID]);
        posHisto = projYSize2[planeID]->FindBin(pos);
        Npos = projYSize2[planeID]->GetBinContent(posHisto);
        error = etaD*etaD*sqrt((1/Neta) + (1/Npos));
        computedEtaDerivativeYDataSize2Inv_[planeID]->SetBinError(error); */
    }

/*    for (int i = 0; i < computedEtaDerivativeYDataSize2Inv_[planeID]->GetXaxis()->GetNbins()+1; ++i)
    {
        Neta = calculateInHisto(eta, hXEtaDistribution_[planeID]);
        posHisto = projXSize2[planeID]->FindBin(pos);
        Npos = projXSize2[planeID]->GetBinContent(posHisto);
        error = etaD*etaD*sqrt((1/Neta) + (1/Npos));
        computedEtaDerivativeXDataSize2Inv_[planeID]->SetBinError(i, error);
    } */

    computedEtaDerivativeXDataSize2Inv_[planeID]->Divide(computedEtaDerivativeXDataSize2InvNorm_[planeID]);
    computedEtaDerivativeYDataSize2Inv_[planeID]->Divide(computedEtaDerivativeYDataSize2InvNorm_[planeID]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TracksAfter::analyze(const Data& data, int threadNumber)//WARNING: You can't change this name (threadNumber) or the MACRO THREAD won't compile
{
//    bool cellTracksYCut = true;
//    if(cutsFormulas_.find("cell Tracks Y") != cutsFormulas_.end())
//        cellTracksYCut = cutsFormulas_["cell Tracks Y"][threadNumber]->EvalInstance();

    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {
        if(!passStandardCuts(p,data))
            continue;

        if(thePlaneMapping_->getPlaneName(p).find("Dut") != std::string::npos)
        {
            if(!passCalibrationsCut(p,data))
                return;
        }

        unfold (p, data.getClusterCharge(p), data.getXPixelResidualLocal(p), data.getYPixelResidualLocal(p), threadNumber);

        if (p == 9) {
            calculateXResiduals(data, p, threadNumber);
            calculateYResiduals(data, p, threadNumber);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TracksAfter::endJob(void)
{
//    std::stringstream ss;

    for(unsigned int i=0; i<thePlaneMapping_->getNumberOfPlanes(); i++)
    {
//        std::string planeName = thePlaneMapping_->getPlaneName(i);

        ADD_THREADED(XYReconstructed_[i]        );
        ADD_THREADED(XResidualsReconstructed_[i]);
        ADD_THREADED(YResidualsReconstructed_[i]);

        XYReconstructed_[i]->GetXaxis()->SetTitle("x coordinate (um)");
        XYReconstructed_[i]->GetYaxis()->SetTitle("y coordinate (um)");
        XResidualsReconstructed_[i]->GetXaxis()->SetTitle("x coordinate (um)");
        XResidualsReconstructed_[i]->GetYaxis()->SetTitle("residuals");
        YResidualsReconstructed_[i]->GetXaxis()->SetTitle("y coordinate (um)");
        YResidualsReconstructed_[i]->GetYaxis()->SetTitle("residuals");

        funcPredicted_->GetXaxis()->SetTitle("y coordinate (um)");
        funcPredicted_->GetYaxis()->SetTitle("eta value");
        funcPredicted2_->GetXaxis()->SetTitle("y coordinate (um)");
        funcPredicted2_->GetYaxis()->SetTitle("eta integrand value");
        funcPredicted3_->GetXaxis()->SetTitle("eta integrand value");
        funcPredicted3_->GetYaxis()->SetTitle("y coordinate (um)");

        ADD_THREADED(hXResidualsAsimmetry_[i]);
        hXResidualsAsimmetry_[i]->GetXaxis()->SetTitle("Residuals x (um)");
        ADD_THREADED(hYResidualsAsimmetry_[i]);
        hYResidualsAsimmetry_[i]->GetXaxis()->SetTitle("Residuals y (um)");
        ADD_THREADED(hXResidualsAsimmetryLE2_[i]);
        hXResidualsAsimmetryLE2_[i]->GetXaxis()->SetTitle("Residuals x (um)");
        ADD_THREADED(hYResidualsAsimmetryLE2_[i]);
        hYResidualsAsimmetryLE2_[i]->GetXaxis()->SetTitle("Residuals y (um)");

        computedEtaDerivativeXFitted_          [i]->GetXaxis()->SetTitle("Relative position on long pitch (um)");
        computedEtaDerivativeXFitted_          [i]->GetYaxis()->SetTitle("dEta/dx");
        computedEtaDerivativeYFitted_          [i]->GetXaxis()->SetTitle("Relative position on short pitch (um)");
        computedEtaDerivativeYFitted_          [i]->GetYaxis()->SetTitle("dEta/dy");
        computedEtaDerivativeXData_            [i]->GetXaxis()->SetTitle("Relative position on long pitch (um)");
        computedEtaDerivativeXData_            [i]->GetYaxis()->SetTitle("dEta/dx");
        computedEtaDerivativeYData_            [i]->GetXaxis()->SetTitle("Relative position on short pitch (um)");
        computedEtaDerivativeYData_            [i]->GetYaxis()->SetTitle("dEta/dy");
        computedEtaDerivativeXFittedSize2_     [i]->GetXaxis()->SetTitle("Relative position on long pitch (um)");
        computedEtaDerivativeXFittedSize2_     [i]->GetYaxis()->SetTitle("dEta/dx");
        computedEtaDerivativeYFittedSize2_     [i]->GetXaxis()->SetTitle("Relative position on short pitch (um)");
        computedEtaDerivativeYFittedSize2_     [i]->GetYaxis()->SetTitle("dEta/dy");
        computedEtaDerivativeXDataSize2_       [i]->GetXaxis()->SetTitle("Relative position on long pitch (um)");
        computedEtaDerivativeXDataSize2_       [i]->GetYaxis()->SetTitle("dEta/dx");
        computedEtaDerivativeYDataSize2_       [i]->GetXaxis()->SetTitle("Relative position on short pitch (um)");
        computedEtaDerivativeYDataSize2_       [i]->GetYaxis()->SetTitle("dEta/dy");
        computedEtaDerivativeXDataSize2Inv_    [i]->GetXaxis()->SetTitle("Relative position on long pitch (um)");
        computedEtaDerivativeXDataSize2Inv_    [i]->GetYaxis()->SetTitle("dEta/dx");
        computedEtaDerivativeYDataSize2Inv_    [i]->GetXaxis()->SetTitle("Relative position on short pitch (um)");
        computedEtaDerivativeYDataSize2Inv_    [i]->GetYaxis()->SetTitle("dEta/dy");
        computedEtaDerivativeXFittedSize2Inv_  [i]->GetXaxis()->SetTitle("Relative position on long pitch (um)");
        computedEtaDerivativeXFittedSize2Inv_  [i]->GetYaxis()->SetTitle("dEta/dx");
        computedEtaDerivativeYFittedSize2Inv_  [i]->GetXaxis()->SetTitle("Relative position on short pitch (um)");
        computedEtaDerivativeYFittedSize2Inv_  [i]->GetYaxis()->SetTitle("dEta/dy");
        computedEtaDerivativeXDataSize2InvNorm_[i]->GetXaxis()->SetTitle("Relative position on long pitch (um)");
        computedEtaDerivativeXDataSize2InvNorm_[i]->GetYaxis()->SetTitle("Number of events");
        computedEtaDerivativeYDataSize2InvNorm_[i]->GetXaxis()->SetTitle("Relative position on short pitch (um)");
        computedEtaDerivativeYDataSize2InvNorm_[i]->GetYaxis()->SetTitle("Number of events");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TracksAfter::preBook (void)
{
    std::string hName;
    std::string hTitle;
    std::string planeName;
    std::string toGet;

    int binSize = 5;
    int pixelLengthX = 150;
    int pixelLengthY = 100;
    int nBinsX = pixelLengthX/binSize;    //150um
    int nBinsY = pixelLengthY/binSize;    //100um originally, changed binning to 5x5um/bin

    theAnalysisManager_->cd("/");
    theAnalysisManager_->mkdir("TracksAfter");
    theAnalysisManager_->mkdir("Tests");

    funcPredicted_ = new TH1F("funcPredicted_", "test function for eta", 100, -50, 50);
    funcPredicted2_ = new TH1F("funcPredicted2_", "test function for eta integrand", 100, -50, 50);
    funcPredicted3_ = new TH1F("funcPredicted3_", "test function for eta inverse", 100, -1.1, 1.1);
    testPredictedFunc();

    theAnalysisManager_->cd("/");
    theAnalysisManager_->mkdir("TracksAfter");

    for(unsigned int i=0; i<thePlaneMapping_->getNumberOfPlanes(); i++) {
        planeName = thePlaneMapping_->getPlaneName(i);

        theAnalysisManager_->cd("TracksAfter");
        theAnalysisManager_->mkdir(planeName);
        theAnalysisManager_->mkdir("Probability");

        hName = "XProbabilityFunc_" + planeName;
        hTitle = "Combined Probability function for x";
        XProbabilityFunc_.push_back(new TH2F(hName.c_str(), hTitle.c_str(), nBinsX, -pixelLengthX/2, pixelLengthX/2, 500, 0, 80000));
        hName = "YProbabilityFunc_" + planeName;
        hTitle = "Combined Probability function for y";
        YProbabilityFunc_.push_back(new TH2F(hName.c_str(), hTitle.c_str(), nBinsY, -pixelLengthY/2, pixelLengthY/2, 500, 0, 80000));

        hName = "XAsimmetryFunc_" + planeName;
        XAsimmetryFunc_.push_back(new TF1(hName.c_str(), Utilities::etaFitFunc, -pixelLengthX/2, pixelLengthX/2, 6));
        hName = "YAsimmetryFunc_" + planeName;
        YAsimmetryFunc_.push_back(new TF1(hName.c_str(), Utilities::etaFitFunc, -pixelLengthY/2, pixelLengthY/2, 6));
        hName = "XAsimmetryFuncInv_" + planeName;
        XAsimmetryFuncInv_.push_back(new TF1(hName.c_str(), "[0] + [1]*x", -pixelLengthX/2, pixelLengthX/2));
        hName = "YAsimmetryFuncInv_" + planeName;
        YAsimmetryFuncInv_.push_back(new TF1(hName.c_str(), "[0] + [1]*x", -pixelLengthY/2, pixelLengthY/2));

//-----------------------------------Imported objects from other analyses-----------------------------------------------------------------
        theAnalysisManager_->cd("/TracksAfter/" + planeName);
        theAnalysisManager_->mkdir("Imported");

        toGet = "Tracks/" + planeName + "/Xdistribution1D_" + planeName;
        hXdistribution1D_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Tracks/" + planeName + "/" + "Xdistribution2D_" + planeName;
        hXdistribution2D_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Tracks/" + planeName + "/" + "XChargedistribution1D_" + planeName;
        hXChargedistribution1D_.push_back((TH2F*)inFile_->Get(toGet.c_str()));
        toGet = "Tracks/" + planeName + "/" + "XChargedistribution2D_" + planeName;
        hXChargedistribution2D_.push_back((TH2F*)inFile_->Get(toGet.c_str()));
        toGet = "Tracks/" + planeName + "/" + "Ydistribution1D_" + planeName;
        hYdistribution1D_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Tracks/" + planeName + "/" + "Ydistribution2D_" + planeName;
        hYdistribution2D_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Tracks/" + planeName + "/" + "YChargedistribution1D_" + planeName;
        hYChargedistribution1D_.push_back((TH2F*)inFile_->Get(toGet.c_str()));
        toGet = "Tracks/" + planeName + "/" + "YChargedistribution2D_" + planeName;
        hYChargedistribution2D_.push_back((TH2F*)inFile_->Get(toGet.c_str()));

        toGet = "Charge/" + planeName + "/Xasimmetry/h1DXcellChargeAsimmetry_" + planeName;
        hXAsimmetry_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Charge/" + planeName + "/Yasimmetry/h1DYcellChargeAsimmetry_" + planeName;
        hYAsimmetry_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Charge/" + planeName + "/Xasimmetry/h1DXcellChargeAsimmetryInv_" + planeName;
        hXAsimmetryInv_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Charge/" + planeName + "/Yasimmetry/h1DYcellChargeAsimmetryInv_" + planeName;
        hYAsimmetryInv_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Charge/" + planeName + "/Xasimmetry/h1DXcellChargeAsimmetrySizeLE2_" + planeName;
        hXAsimmetryLE2_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Charge/" + planeName + "/Yasimmetry/h1DYcellChargeAsimmetrySizeLE2_" + planeName;
        hYAsimmetryLE2_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Charge/" + planeName + "/Xasimmetry/h1DXEtaDistribution_" + planeName;
        hXEtaDistribution_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Charge/" + planeName + "/Yasimmetry/h1DYEtaDistribution_" + planeName;
        hYEtaDistribution_.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Charge/" + planeName + "/Xasimmetry/projX_" + planeName;
        projXSize2.push_back((TH1F*)inFile_->Get(toGet.c_str()));
        toGet = "Charge/" + planeName + "/Yasimmetry/projY_" + planeName;
        projYSize2.push_back((TH1F*)inFile_->Get(toGet.c_str()));

//-------------------------------Support objects-------------------------------------------------------------------------------------------
        theAnalysisManager_->cd("/TracksAfter/" + planeName);
        theAnalysisManager_->mkdir("Utils");

        hName = "computedEtaFuncXSize2_" + planeName;
        hTitle = "Eta function after fitting, x coordinate, size 2";
        computedEtaFuncXSize2_.push_back(new TH1F(hName.c_str(), hTitle.c_str(), 3000, -pixelLengthX/2, pixelLengthX/2));
        hName = "computedEtaFuncYSize2_" + planeName;
        hTitle = "Eta function after fitting, y coordinate, size 2";
        computedEtaFuncYSize2_.push_back(new TH1F(hName.c_str(), hTitle.c_str(), 3000, -pixelLengthY/2, pixelLengthY/2));
        hName = "computedEtaFuncXSizeLE2_" + planeName;
        hTitle = "Eta function after fitting, x coordinate, size <= 2";
        computedEtaFuncXSizeLE2_.push_back(new TH1F(hName.c_str(), hTitle.c_str(), 1000, -pixelLengthX/2, pixelLengthX/2));
        hName = "computedEtaFuncYSizeLE2_" + planeName;
        hTitle = "Eta function after fitting, y coordinate, size <= 2";
        computedEtaFuncYSizeLE2_.push_back(new TH1F(hName.c_str(), hTitle.c_str(), 1000, -pixelLengthY/2, pixelLengthY/2));

    }

    for (unsigned int i=0; i<thePlaneMapping_->getNumberOfPlanes(); i++)
    {
        positionUnfolded(i);
        fitEtaFunc(i, "Size2");
    }

    loadEtaFunc("Size2");

    for (unsigned int i=0; i<thePlaneMapping_->getNumberOfPlanes(); i++)
    {
        fitEtaFunc(i, "SizeLE2");
    }

    loadEtaFunc("SizeLE2");

    for (unsigned int i=0; i<thePlaneMapping_->getNumberOfPlanes(); i++)
    {
        fitEtaFunc(i, "SizeLE2Inv");
    }

}

void TracksAfter::book(void)
{
    destroy();
    preBook();

    std::string hName;
    std::string hTitle;
    std::string planeName;

    int binSize = 5;
    int pixelLengthX = 150;
    int pixelLengthY = 100;
    int nBinsX = pixelLengthX/binSize;    //150um
    int nBinsY = pixelLengthY/binSize;    //100um originally, changed binning to 5x5um/bin

    theAnalysisManager_->cd("/");
    theAnalysisManager_->cd("TracksAfter");

    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {
        planeName = thePlaneMapping_->getPlaneName(p);

        theAnalysisManager_->cd("/TracksAfter/" + planeName);
        theAnalysisManager_->mkdir("Unfolded");

        hName = "XYReconstructed_" + planeName;
        hTitle = "Distribution of points from unfolding";
        XYReconstructed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(), nBinsX, -pixelLengthX/2, pixelLengthX/2, nBinsY, -pixelLengthY/2, pixelLengthY/2)));
        hName = "XResidualsReconstructed_" + planeName;
        hTitle = "x residuals after reconstruction from unfolding";
        XResidualsReconstructed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), nBinsX, -pixelLengthX/2, pixelLengthX/2)));
        hName = "YResidualsReconstructed_" + planeName;
        hTitle = "y residuals after reconstruction from unfolding";
        YResidualsReconstructed_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), nBinsY, -pixelLengthY/2, pixelLengthY/2)));

        theAnalysisManager_->cd("/TracksAfter/" + planeName);
        theAnalysisManager_->mkdir("Reconstructed");

        hName  = "hXResidualsAsimmetry_"                      + planeName;
        hTitle = "X residuals calculated from asimmetry fit, size 2, " + planeName;
        hXResidualsAsimmetry_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 125, -500, 500)));
        hName  = "hYResidualsAsimmetry_"                      + planeName;
        hTitle = "Y residuals calculated from asimmetry fit, size 2, " + planeName;
        hYResidualsAsimmetry_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 125, -500, 500)));
        hName  = "hXResidualsAsimmetryLE2_"                      + planeName;
        hTitle = "X residuals calculated from asimmetry fit, size <= 2, " + planeName;
        hXResidualsAsimmetryLE2_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 125, -500, 500)));
        hName  = "hYResidualsAsimmetryLE2_"                      + planeName;
        hTitle = "Y residuals calculated from asimmetry fit, size <= 2, " + planeName;
        hYResidualsAsimmetryLE2_.push_back(NEW_THREADED(TH1F(hName.c_str(), hTitle.c_str(), 125, -500, 500)));

        theAnalysisManager_->cd("/TracksAfter/" + planeName);
        theAnalysisManager_->mkdir("X Eta derivative");

        hName = "computedDerivativeXFitted_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with fitted eta function, coordinate x, " + planeName;
        computedEtaDerivativeXFitted_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsX*5, -pixelLengthX/2, pixelLengthX/2));
        hName = "computedDerivativeXData_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with unfitted eta function, coordinate x, " + planeName;
        computedEtaDerivativeXData_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsX*5, -pixelLengthX/2, pixelLengthX/2));
        hName = "computedDerivativeXFittedSize2_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with fitted eta function of size 2, coordinate x, " + planeName;
        computedEtaDerivativeXFittedSize2_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsX*2, -pixelLengthX/2, pixelLengthX/2));
        hName = "computedDerivativeXDataSize2_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with unfitted eta function of size 2, coordinate x, " + planeName;
        computedEtaDerivativeXDataSize2_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsX*2, -pixelLengthX/2, pixelLengthX/2));
        hName = "computedDerivativeXFittedSize2Inv_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with fitted eta inverse function of size 2, coordinate x, " + planeName;
        computedEtaDerivativeXFittedSize2Inv_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsX*5, -pixelLengthX/2, pixelLengthX/2));
        hName = "computedDerivativeXDataSize2Inv_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with unfitted eta function of size 2 inverted, coordinate x, " + planeName;
        computedEtaDerivativeXDataSize2Inv_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsX*5, -pixelLengthX/2, pixelLengthX/2));
        hName = "computedDerivativeXDataSize2InvNorm_" + planeName;
        hTitle = "Number of events to normalize eta derivative from eta distribution, with unfitted eta function of size 2 inverted, coordinate x, " + planeName;
        computedEtaDerivativeXDataSize2InvNorm_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsX*5, -pixelLengthX/2, pixelLengthX/2));

        theAnalysisManager_->cd("/TracksAfter/" + planeName);
        theAnalysisManager_->mkdir("Y Eta derivative");

        hName = "computedDerivativeYFitted_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with fitted eta function, coordinate y, " + planeName;
        computedEtaDerivativeYFitted_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsY*5, -pixelLengthY/2, pixelLengthY/2));
        hName = "computedDerivativeYData_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with unfitted eta function, coordinate y, " + planeName;
        computedEtaDerivativeYData_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsY*5, -pixelLengthY/2, pixelLengthY/2));
        hName = "computedDerivativeYFittedSize2_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with fitted eta function of size 2, coordinate y, " + planeName;
        computedEtaDerivativeYFittedSize2_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsY*2, -pixelLengthY/2, pixelLengthY/2));
        hName = "computedDerivativeYDataSize2_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with unfitted eta function of size 2, coordinate y, " + planeName;
        computedEtaDerivativeYDataSize2_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsY*2, -pixelLengthY/2, pixelLengthY/2));
        hName = "computedDerivativeYFittedSize2Inv_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with fitted eta inverse function of size 2, coordinate y, " + planeName;
        computedEtaDerivativeYFittedSize2Inv_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsY*5, -pixelLengthY/2, pixelLengthY/2));
        hName = "computedDerivativeYDataSize2Inv_" + planeName;
        hTitle = "Computed eta derivative from eta distribution, with unfitted eta function of size 2 inverted, coordinate y, " + planeName;
        computedEtaDerivativeYDataSize2Inv_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsY*5, -pixelLengthY/2, pixelLengthY/2));
        hName = "computedDerivativeYDataSize2InvNorm_" + planeName;
        hTitle = "Number of events to normalize eta derivative from eta distribution, with unfitted eta function of size 2 inverted, coordinate y, " + planeName;
        computedEtaDerivativeYDataSize2InvNorm_.push_back(new TH1F (hName.c_str(), hTitle.c_str(), nBinsY*5, -pixelLengthY/2, pixelLengthY/2));

        computeEtaDerivative(p);

    }
/*
    double pars[7] = {500, 0.5, 70, 570, 0.002, 1, 0.0001};
    double limsInf[7] = {100, 0.05, 10, 510, 0.0005, 0.1, 0.00005};
    double limsSup[7] = {900, 2, 100, 600, 0.05, 10, 0.005};
    std::stringstream sss;

    for (int n = 0; n < 7; ++n)
    {
        sss.str("");
        sss << n;
        STDLINE("Tracing chi2 profile for parameter " + sss.str(), ACGreen);
        profileChi2Par(pars, n, limsInf[n], limsSup[n]);
    } */
}
