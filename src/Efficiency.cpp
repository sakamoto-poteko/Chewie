#include "Efficiency.h"
#include "AnalysisManager.h"
#include "WindowsManager.h"
#include "Window.h"
#include "ThreadUtilities.h"
#include "PlanesMapping.h"
#include "MessageTools.h"
#include "XmlParser.h"
#include "XmlScan.h"
#include "XmlAnalysis.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TThread.h>
#include <TFile.h>
#include <TGraph.h>
#include <TTreeFormula.h>
#include <Rtypes.h>

#include <cmath>
#include <iostream>
#include <fstream>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Efficiency::Efficiency(AnalysisManager* analysisManager, int nOfThreads) :
    Analysis          (analysisManager, nOfThreads)
  , thePlaneMapping_  (0               )
  , theWindowsManager_(0               )
  , theXmlParser_     (0               )
{
    thePlaneMapping_ = new PlanesMapping();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Efficiency::~Efficiency(void)
{
    if(thePlaneMapping_)
        delete thePlaneMapping_;

    destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::destroy(void)
{
    if(Analysis::fDoNotDelete_) return;
    //STDLINE("Deleting histos", ACRed);

    std::vector<TH1F*>::iterator it1;
    std::vector<TH2F*>::iterator it2;
    for(it1=hEfficiency_                   .begin(); it1!=hEfficiency_                   .end(); it1++) delete *it1; hEfficiency_                   .clear();
    for(it1=hEfficiencyNorm_               .begin(); it1!=hEfficiencyNorm_               .end(); it1++) delete *it1; hEfficiencyNorm_               .clear();
    for(it2=h2DEfficiency_                 .begin(); it2!=h2DEfficiency_                 .end(); it2++) delete *it2; h2DEfficiency_                 .clear();
    for(it2=h2DEfficiencyNorm_             .begin(); it2!=h2DEfficiencyNorm_             .end(); it2++) delete *it2; h2DEfficiencyNorm_             .clear();
    for(it2=h2DInefficiency_               .begin(); it2!=h2DInefficiency_               .end(); it2++) delete *it2; h2DInefficiency_               .clear();
    for(it2=hCellEfficiency_               .begin(); it2!=hCellEfficiency_               .end(); it2++) delete *it2; hCellEfficiency_               .clear();
    for(it2=hCellEfficiencyEvenColumns_    .begin(); it2!=hCellEfficiencyEvenColumns_    .end(); it2++) delete *it2; hCellEfficiencyEvenColumns_    .clear();
    for(it2=hCellEfficiencyOddColumns_     .begin(); it2!=hCellEfficiencyOddColumns_     .end(); it2++) delete *it2; hCellEfficiencyOddColumns_     .clear();
    for(it2=hCellEfficiencyNorm_           .begin(); it2!=hCellEfficiencyNorm_           .end(); it2++) delete *it2; hCellEfficiencyNorm_           .clear();
    for(it2=hCellEfficiencyEvenColumnsNorm_.begin(); it2!=hCellEfficiencyEvenColumnsNorm_.end(); it2++) delete *it2; hCellEfficiencyEvenColumnsNorm_.clear();
    for(it2=hCellEfficiencyOddColumnsNorm_ .begin(); it2!=hCellEfficiencyOddColumnsNorm_ .end(); it2++) delete *it2; hCellEfficiencyOddColumnsNorm_ .clear();
    for(it2=hCellInefficiency_             .begin(); it2!=hCellInefficiency_             .end(); it2++) delete *it2; hCellInefficiency_             .clear();
    for(it1=h1DXcellEfficiencyFirstHit_    .begin(); it1!=h1DXcellEfficiencyFirstHit_    .end(); it1++) delete *it1; h1DXcellEfficiencyFirstHit_    .clear();
    for(it1=h1DXcellEfficiencySecondHit_   .begin(); it1!=h1DXcellEfficiencySecondHit_   .end(); it1++) delete *it1; h1DXcellEfficiencySecondHit_   .clear();
    for(it1=h1DXcellEfficiencyNorm_        .begin(); it1!=h1DXcellEfficiencyNorm_        .end(); it1++) delete *it1; h1DXcellEfficiencyNorm_        .clear();
    for(it1=h1DYcellEfficiencyFirstHit_    .begin(); it1!=h1DYcellEfficiencyFirstHit_    .end(); it1++) delete *it1; h1DYcellEfficiencyFirstHit_    .clear();
    for(it1=h1DYcellEfficiencySecondHit_   .begin(); it1!=h1DYcellEfficiencySecondHit_   .end(); it1++) delete *it1; h1DYcellEfficiencySecondHit_   .clear();
    for(it1=h1DYcellEfficiencyNorm_        .begin(); it1!=h1DYcellEfficiencyNorm_        .end(); it1++) delete *it1; h1DYcellEfficiencyNorm_        .clear();

    for(std::map< int,std::vector<TH1F*> >::iterator it=scanEfficiencyNorm_.begin(); it!=scanEfficiencyNorm_.end(); it++)
        for(std::vector<TH1F*>::iterator ii=(it->second).begin();ii!=(it->second).end();ii++)
            delete *ii;
    scanEfficiencyNorm_.clear();

    for(std::map< int,std::vector<TH1F*> >::iterator it=scanEfficiencyHistos_.begin(); it!=scanEfficiencyHistos_.end(); it++)
        for(std::vector<TH1F*>::iterator ii=(it->second).begin();ii!=(it->second).end();ii++)
            delete *ii;
    scanEfficiencyHistos_.clear();

    for(std::vector<TGraph*>::iterator it=scanGraph_.begin(); it!=scanGraph_.end(); it++) delete *it; scanGraph_.clear();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::beginJob(void)
{
    theWindowsManager_ = theAnalysisManager_->getWindowsManager();
    theXmlParser_      = theAnalysisManager_->getXmlParser();

    if(theXmlParser_->getScan()->getScanValues().size()==0)
        book();
    else
        scanBook();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::analyze(const Data& data, int threadNumber)//WARNING: You can't change this name (threadNumber) or the MACRO THREAD won't compile
{
    bool passMainCut =  true;
    if(cutsFormulas_.find("main cut")!=cutsFormulas_.end())
        passMainCut = cutsFormulas_["main cut"][threadNumber]->EvalInstance();

    if(!passMainCut)
        return;

    if(theXmlParser_->getScan()->getScanValues().size()==0)
    {
        bool passCellEfficiencyCut = true;
        if(cutsFormulas_.find("cell efficiency")!=cutsFormulas_.end())
            passCellEfficiencyCut = cutsFormulas_["cell efficiency"][threadNumber]->EvalInstance();

        bool passXCellEfficiencyCut = true;
        if(cutsFormulas_.find("cell efficiency X")!=cutsFormulas_.end())
            passXCellEfficiencyCut = cutsFormulas_["cell efficiency X"][threadNumber]->EvalInstance();

        bool passYCellEfficiencyCut = true;
        if(cutsFormulas_.find("cell efficiency Y")!=cutsFormulas_.end())
            passYCellEfficiencyCut = cutsFormulas_["cell efficiency Y"][threadNumber]->EvalInstance();

        for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
        {
            if(!passStandardCuts(p,data))
                continue;

            planeEfficiency (passMainCut           ,p,data,threadNumber);
            cellEfficiency  (passCellEfficiencyCut ,p,data,threadNumber);
            XcellEfficiency (passXCellEfficiencyCut,p,data,threadNumber);
            YcellEfficiency (passYCellEfficiencyCut,p,data,threadNumber);
        }
    }
    else
    {
        std::string inFileName = "Line493OfEfficiencycpp";//theAnalysisManager_->getInFileName();
        std::map<std::string,int> scanValues = theXmlParser_->getScan()->getScanValues();

        for(std::map<std::string,int>::iterator it=scanValues.begin(); it!=scanValues.end(); it++)
        {
            if(it->first == inFileName)
            {
                for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
                {
                    if(thePlaneMapping_->getPlaneName(p).find("Dut")!=std::string::npos)
                        scanEfficiency(p,it->second,data,threadNumber);
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::endJob(void)
{
    std::stringstream ss;
    std::ofstream outfile ("log.txt");
    double efficiency;
    double Ntrack;
    double error;

    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {
        if(theXmlParser_->getScan()->getScanValues().size()==0)
        {
            ADD_THREADED(hEfficiency_                   [p]);
            ADD_THREADED(hEfficiencyNorm_               [p]);
            ADD_THREADED(h2DEfficiency_                 [p]);
            ADD_THREADED(h2DEfficiencyNorm_             [p]);
            ADD_THREADED(h2DInefficiency_               [p]);
            ADD_THREADED(hCellEfficiencyNorm_           [p]);
            ADD_THREADED(hCellEfficiencyEvenColumnsNorm_[p]);
            ADD_THREADED(hCellEfficiencyOddColumnsNorm_ [p]);
            ADD_THREADED(hCellEfficiency_               [p]);
            ADD_THREADED(hCellEfficiencyEvenColumns_    [p]);
            ADD_THREADED(hCellEfficiencyOddColumns_     [p]);
            ADD_THREADED(h1DXcellEfficiencyNorm_        [p]);
            ADD_THREADED(h1DXcellEfficiencyFirstHit_    [p]);
            ADD_THREADED(h1DXcellEfficiencySecondHit_   [p]);
            ADD_THREADED(h1DYcellEfficiencyNorm_        [p]);
            ADD_THREADED(h1DYcellEfficiencyFirstHit_    [p]);
            ADD_THREADED(h1DYcellEfficiencySecondHit_   [p]);
            ADD_THREADED(hCellInefficiency_             [p]);

            hEfficiency_                [p]->Divide(hEfficiencyNorm_               [p]);
            h2DEfficiency_              [p]->Divide(h2DEfficiencyNorm_             [p]);
            h2DInefficiency_            [p]->Divide(h2DEfficiencyNorm_             [p]);
            hCellEfficiency_            [p]->Divide(hCellEfficiencyNorm_           [p]);
            hCellEfficiencyEvenColumns_ [p]->Divide(hCellEfficiencyEvenColumnsNorm_[p]);
            hCellEfficiencyOddColumns_  [p]->Divide(hCellEfficiencyOddColumnsNorm_ [p]);
            hCellInefficiency_          [p]->Divide(hCellEfficiencyNorm_           [p]);
            h1DXcellEfficiencyFirstHit_ [p]->Divide(h1DXcellEfficiencyNorm_        [p]);
            h1DXcellEfficiencySecondHit_[p]->Divide(h1DXcellEfficiencyNorm_        [p]);
            h1DYcellEfficiencyFirstHit_ [p]->Divide(h1DYcellEfficiencyNorm_        [p]);
            h1DYcellEfficiencySecondHit_[p]->Divide(h1DYcellEfficiencyNorm_        [p]);

            h2DEfficiency_              [p]->GetXaxis()->SetTitle("column");
            h2DEfficiency_              [p]->GetYaxis()->SetTitle("row"   );
            h2DInefficiency_            [p]->GetXaxis()->SetTitle("column");
            h2DInefficiency_            [p]->GetYaxis()->SetTitle("row"   );
            hCellEfficiency_            [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyEvenColumns_ [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyOddColumns_  [p]->GetXaxis()->SetTitle("x (um)");
            h1DXcellEfficiencyFirstHit_ [p]->GetXaxis()->SetTitle("x (um)");
            h1DXcellEfficiencySecondHit_[p]->GetXaxis()->SetTitle("x (um)");
            h1DYcellEfficiencyFirstHit_ [p]->GetXaxis()->SetTitle("y (um)");
            h1DYcellEfficiencySecondHit_[p]->GetXaxis()->SetTitle("y (um)");
            hCellEfficiency_            [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyEvenColumns_ [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyOddColumns_  [p]->GetYaxis()->SetTitle("y (um)");
            hCellInefficiency_          [p]->GetXaxis()->SetTitle("x (um)");
            hCellInefficiency_          [p]->GetYaxis()->SetTitle("y (um)");

            setErrorsBar(p);

            efficiency = hEfficiency_    [p]->GetBinContent(1);
            Ntrack     = hEfficiencyNorm_[p]->GetBinContent(1);

            error = sqrt(efficiency*(1-efficiency)/Ntrack);

            ss.str("");
            ss << "Detector: " << thePlaneMapping_->getPlaneName(p) << " efficiency: " << efficiency << " +- " << error;
            STDLINE(ss.str(),ACLightPurple);
        }
        else if(thePlaneMapping_->getPlaneName(p).find("Dut")!=std::string::npos)
        {
            int pointNumber = 0;
            for(std::map< int,std::vector<TH1F*> >::iterator it=scanEfficiencyHistos_.begin(); it!=scanEfficiencyHistos_.end(); ++it)
            {
                ADD_THREADED((it->second)[p-8]);
                ADD_THREADED((scanEfficiencyNorm_[it->first])[p-8]);
                (it->second)[p-8]->Divide((scanEfficiencyNorm_[it->first])[p-8]);
                scanGraph_[p-8]->SetPoint(pointNumber,it->first,(it->second)[p-8]->GetBinContent(1));
                pointNumber++;

                ss.str("");
                ss << "Detector: " << thePlaneMapping_->getPlaneName(p) << " - Efficiency: " << (it->second)[p-8]->GetBinContent(1) << " - Value: " << it->first;
                STDLINE(ss.str(),ACYellow);
                outfile << ss.str() << std::endl;
            }
            theAnalysisManager_->cd("Efficiency/"+thePlaneMapping_->getPlaneName(p));
            scanGraph_[p-8]->Write();
            STDLINE("",ACWhite);
            STDLINE("",ACWhite);
        }
    }
    outfile.close();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::book(void)
{
    destroy();

    const Window*     theWindow ;
    std::string       hName     ;
    std::stringstream value     ;
    std::string       hTitle    ;
    std::string       planeName ;
    int               nBinsX    ;
    int               nBinsY    ;
    float             resXRange = 150;
    float             resYRange = 100;

    theAnalysisManager_->cd("/");
    theAnalysisManager_->mkdir("Efficiency");

    for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
    {
        planeName = thePlaneMapping_->getPlaneName(p);
        theWindow = theWindowsManager_->getWindow(p);

        theAnalysisManager_->cd("Efficiency");
        theAnalysisManager_->mkdir(planeName);

        hName  = "Efficiency_"         + planeName;
        hTitle = "Overall efficiency " + planeName;
        hEfficiency_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),1,.5,1.5)));

        hName  = "EfficiencyNorm_"                   + planeName;
        hTitle = "Overall efficiency normalization " + planeName;
        hEfficiencyNorm_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),1,.5,1.5)));

        hName  =  "CellEfficiencyNorm_"            + planeName;
        hTitle =  "Cell efficiency normalization " + planeName;
        hCellEfficiencyNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  =  "CellEfficiencyEvenColumnsNorm_"            + planeName;
        hTitle =  "Cell efficiency normalization even columns" + planeName;
        hCellEfficiencyEvenColumnsNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  =  "CellEfficiencyOddColumnsNorm_"            + planeName;
        hTitle =  "Cell efficiency normalization odd columns" + planeName;
        hCellEfficiencyOddColumnsNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  = "CellEfficiency_"  + planeName;
        hTitle = "Cell efficiency " + planeName;
        hCellEfficiency_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  = "CellEfficiencyEvenColumns_"  + planeName;
        hTitle = "Cell efficiency even columns" + planeName;
        hCellEfficiencyEvenColumns_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  = "CellEfficiencyOddColumns_"  + planeName;
        hTitle = "Cell efficiency odd columns" + planeName;
        hCellEfficiencyOddColumns_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  = "CellInefficiency_"  + planeName;
        hTitle = "Cell inefficiency " + planeName;
        hCellInefficiency_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  = "h1DXcellEfficiencyFirstHit_"                  + planeName;
        hTitle = "1D cell Efficiency - X coordinate first hit " + planeName;
        h1DXcellEfficiencyFirstHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5)));
        //h1DXcellEfficiencyFirstHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellEfficiencySecondHit_"                  + planeName;
        hTitle = "1D cell Efficiency - X coordinate second hit " + planeName;
        h1DXcellEfficiencySecondHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5)));
        //h1DXcellEfficiencySecondHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DXcellEfficiencyNorm_"                          + planeName;
        hTitle = "1D cell Efficiency - X coordinate normalization " + planeName;
        h1DXcellEfficiencyNorm_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5)));
        //h1DXcellEfficiencyNorm_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resXRange/2,-(resXRange/2),resXRange/2)));

        hName  = "h1DYcellEfficiencyFirstHit_"                 + planeName;
        hTitle = "1D cell Efficiency - Y coordinate first hit " + planeName;
        h1DYcellEfficiencyFirstHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));
        //h1DYcellEfficiencyFirstHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellEfficiencySecondHit_"                  + planeName;
        hTitle = "1D cell Efficiency - Y coordinate second hit " + planeName;
        h1DYcellEfficiencySecondHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));
        // h1DYcellEfficiencySecondHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        hName  = "h1DYcellEfficiencyNorm_"                       + planeName;
        hTitle = "1D cell Efficiency - Y coordinate normalization " + planeName;
        h1DYcellEfficiencyNorm_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));
        //h1DYcellEfficiencyNorm_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resYRange/2 - 1,-(resYRange/2) + 1.,resYRange/2 - 1.)));

        nBinsX = theWindow->getNbins().first;
        nBinsY = theWindow->getNbins().second;

        hName  = "2DEfficiency_"               + planeName;
        hTitle = "2D efficiency distribution " + planeName;
        h2DEfficiency_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),nBinsX,0,nBinsX,nBinsY,0,nBinsY)));

        hName  = "2DEfficiencyNorm_"         + planeName;
        hTitle = "2D efficiency normalization " + planeName;
        h2DEfficiencyNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),nBinsX,0,nBinsX,nBinsY,0,nBinsY)));

        hName  = "2DInefficiency_"  + planeName;
        hTitle = "2D inefficiency " + planeName;
        h2DInefficiency_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),nBinsX,0,nBinsX,nBinsY,0,nBinsY)));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::scanBook(void)
{
    std::string       hName     ;
    std::stringstream value     ;
    std::string       hTitle    ;
    std::string       planeName ;

    std::map<std::string,int> scanValues = theXmlParser_->getScan()->getScanValues();

    std::vector<TH1F*> hEfficiencyValueDenom;
    std::vector<TH1F*> hEfficiencyValue     ;
    TGraph* tmp = 0;

    theAnalysisManager_->cd("/");
    theAnalysisManager_->mkdir("Efficiency");

    for(std::map<std::string,int>::iterator it=scanValues.begin(); it!=scanValues.end(); it++)
    {
        hEfficiencyValueDenom.clear();
        hEfficiencyValue.clear();

        for(unsigned int p=0; p<thePlaneMapping_->getNumberOfPlanes(); p++)
        {
            planeName = thePlaneMapping_->getPlaneName(p);
            value.str(""); value << it->second;

            if(planeName.find("Dut")!=std::string::npos)
            {
                theAnalysisManager_->cd("Efficiency");
                theAnalysisManager_->mkdir(planeName);

                hName  = "EfficiencyNorm_" + value.str() + "_" + planeName;
                hTitle = "EfficiencyNorm_" + value.str() + "_" + planeName;
                hEfficiencyValueDenom.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),1,.5,1.5)));

                hName  = "Efficiency_" + value.str() + "_" + planeName;
                hTitle = "Efficiency_" + value.str() + "_" + planeName;
                hEfficiencyValue.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),1,.5,1.5)));

                hName  = "EfficiencyScan_" + planeName;
                hTitle = "Efficiency scan " + planeName;
                tmp = new TGraph();
                tmp->SetName(hName.c_str());
                tmp->SetTitle(hTitle.c_str());
                tmp->SetMarkerStyle(23);
                tmp->SetMarkerColor(kRed);
                tmp->SetMarkerSize(1.5);
                tmp->SetLineWidth(2);
                tmp->SetLineColor(kBlue);
                scanGraph_.push_back(tmp);
                delete tmp;
            }
        }
        scanEfficiencyNorm_ [atoi(value.str().c_str())] = hEfficiencyValueDenom;
        scanEfficiencyHistos_[atoi(value.str().c_str())] = hEfficiencyValue     ;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::setErrorsBar(int planeID)
{
    double efficiency;
    double Ntrack;
    double error;
    int    nBins;

    nBins = h1DXcellEfficiencyFirstHit_[planeID]->GetNbinsX();
    for(int b=1; b<=nBins; ++b)
    {
        efficiency = h1DXcellEfficiencyFirstHit_[planeID]->GetBinContent(b);
        Ntrack     = h1DXcellEfficiencyNorm_[planeID]->GetBinContent(b);
        error      = sqrt(efficiency*(1-efficiency)/Ntrack);
        h1DXcellEfficiencyFirstHit_[planeID]->SetBinError(b,error);
        efficiency = h1DXcellEfficiencySecondHit_[planeID]->GetBinContent(b);
        Ntrack     = h1DXcellEfficiencyNorm_[planeID]->GetBinContent(b);
        error      = sqrt(efficiency*(1-efficiency)/Ntrack);
        h1DXcellEfficiencySecondHit_[planeID]->SetBinError(b,error);
    }

    nBins = h1DYcellEfficiencyFirstHit_[planeID]->GetNbinsX();
    for(int b=1; b<=nBins; ++b)
    {
        efficiency = h1DYcellEfficiencyFirstHit_[planeID]->GetBinContent(b);
        Ntrack     = h1DYcellEfficiencyNorm_[planeID]->GetBinContent(b);
        error      = sqrt(efficiency*(1-efficiency)/Ntrack);
        h1DYcellEfficiencyFirstHit_[planeID]->SetBinError(b,error);
        efficiency = h1DYcellEfficiencySecondHit_[planeID]->GetBinContent(b);
        Ntrack     = h1DYcellEfficiencyNorm_[planeID]->GetBinContent(b);
        error      = sqrt(efficiency*(1-efficiency)/Ntrack);
        h1DYcellEfficiencySecondHit_[planeID]->SetBinError(b,error);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::scanEfficiency(int planeID, int value, const Data& data, int threadNumber)
{
    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    float row  = data.getMeanRow(planeID);
    float col  = data.getMeanCol(planeID);
    int   hits = 0;

    for(unsigned int pp=0; pp<thePlaneMapping_->getNumberOfPlanes(); pp++)
    {
        if(data.getHasHit(pp) && thePlaneMapping_->getPlaneName(pp).find("Telescope") != std::string::npos) hits++;
    }

    if(theWindow->checkWindow(col,row) && hits>=8)
    {
        THREADED((scanEfficiencyNorm_[value])[planeID-8])->Fill(1);
        if(data.getHasHit(planeID))
            THREADED((scanEfficiencyHistos_[value])[planeID-8])->Fill(1);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::planeEfficiency(bool pass, int planeID, const Data& data, int threadNumber)
{
    if(!pass)
        return;

    const Window* theWindow = theWindowsManager_->getWindow(planeID);
    float         row       = data.getMeanRow(planeID);
    float         col       = data.getMeanCol(planeID);

    if(theWindow->checkWindow(col,row))
    {
        THREADED(hEfficiencyNorm_  [planeID])->Fill(1);
        THREADED(h2DEfficiencyNorm_[planeID])->Fill(col,row);
        if(data.getHasHit(planeID))
        {
            THREADED(hEfficiency_  [planeID])->Fill(1);
            THREADED(h2DEfficiency_[planeID])->Fill(col,row);
        }
        else
            THREADED(h2DInefficiency_[planeID])->Fill(col,row);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::cellEfficiency(bool pass, int planeID, const Data& data, int threadNumber)
{
    if(!pass || !data.getIsInDetector(planeID))
        return;

    const  Window* theWindow = theWindowsManager_->getWindow(planeID);
    float          xRes      = data.getXPixelResidualLocal(planeID)  ;
    float          yRes      = data.getYPixelResidualLocal(planeID)  ;
    float          row       = data.getRowPredicted(planeID)         ;
    float          col       = data.getColPredicted(planeID)         ;
    double         maxPitchX = 150                                   ;
    double         maxPitchY = 100                                   ;

    if(theWindow->checkWindow(col,row) && data.getXPitchLocal(planeID)<=maxPitchX && data.getYPitchLocal(planeID)<=maxPitchY)
    {
        THREADED(hCellEfficiencyNorm_[planeID])->Fill(xRes,yRes);
        if(((int)col)%2 == 0)
            THREADED(hCellEfficiencyEvenColumnsNorm_[planeID])->Fill(xRes,yRes);
        else
            THREADED(hCellEfficiencyOddColumnsNorm_[planeID])->Fill(xRes,yRes);
        if(data.getHasHit(planeID))
        {
            THREADED(hCellEfficiency_[planeID])->Fill(xRes,yRes);
            if(((int)col)%2 == 0)
                THREADED(hCellEfficiencyEvenColumns_[planeID])->Fill(xRes,yRes);
            else
                THREADED(hCellEfficiencyOddColumns_[planeID])->Fill(xRes,yRes);
        }
        else
            THREADED(hCellInefficiency_[planeID])->Fill(xRes,yRes);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::XcellEfficiency(bool pass, int planeID, const Data& data, int threadNumber)
{
    if(!pass || !data.getIsInDetector(planeID))
        return;

    float maxPitchX = 150;
    float xRes      = 0  ;

    if(data.getXPitchLocal(planeID)<=maxPitchX)
    {
        if(data.getXPixelResidualLocal(planeID)>0)
            xRes = data.getXPitchLocal(planeID)/2 - data.getXPixelResidualLocal(planeID);
        else if(data.getXPixelResidualLocal(planeID)<=0)
            xRes = -(data.getXPixelResidualLocal(planeID) + data.getXPitchLocal(planeID)/2);
    }
    else
        return;

    const Window* theWindow = theWindowsManager_->getWindow(planeID) ;
    int           row       = data.getRowPredicted(planeID)          ;
    int           col       = data.getColPredicted(planeID)          ;

    if(theWindow->checkWindow(col,row))
    {
        THREADED(h1DXcellEfficiencyNorm_[planeID])->Fill(xRes);
        if(data.getHasHit(planeID) /*&& data.getClusterSize(planeID)<=4*/)
        {
            if(data.getClusterSize(planeID)>4)
            {
                //std::cout << "ok!" << std::endl;
                THREADED(h1DXcellEfficiencyFirstHit_ [planeID])->Fill(xRes);
                THREADED(h1DXcellEfficiencySecondHit_[planeID])->Fill(xRes);
                return;
            }

            bool isOk = false;
            for(int h=0; h<data.getClusterSize(planeID); h++)
            {
                if(data.getClusterPixelCol(h,planeID)==col && data.getClusterPixelRow(h,planeID)==row)
                {
                    THREADED(h1DXcellEfficiencyFirstHit_ [planeID])->Fill(xRes);
                    THREADED(h1DXcellEfficiencySecondHit_[planeID])->Fill(xRes);
                    isOk = true;
                    break;
                }

            }
            if(isOk)
                return;
            else
            {
                isOk=false;
                for(int h=0; h<data.getClusterSize(planeID); h++)
                {
                    if(data.getClusterPixelRow(h,planeID)==row)
                    {
                        /*if((data.getXPixelResidualLocal(planeID)>0 && (col-data.getClusterPixelCol(h,planeID))==1) ||
                           (data.getXPixelResidualLocal(planeID)<0 && (col-data.getClusterPixelCol(h,planeID))==-1))*/
                        if( ( (col-data.getClusterPixelCol(h,planeID)) ==  1 ) ||
                                ( (col-data.getClusterPixelCol(h,planeID)) == -1 )  )
                        {
                            isOk=true;
                            break;
                        }
                    }
                }
                if(!isOk)
                    return;

                THREADED(h1DXcellEfficiencySecondHit_[planeID])->Fill(xRes);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::YcellEfficiency(bool pass, int planeID, const Data& data, int threadNumber)
{
    if(!pass || !data.getIsInDetector(planeID))
        return;

    float maxPitchY = 100;
    float yRes      = 0  ;

    if(data.getYPitchLocal(planeID)<=maxPitchY)
    {
        if(data.getYPixelResidualLocal(planeID)>0)
            yRes = data.getYPitchLocal(planeID)/2 - data.getYPixelResidualLocal(planeID);
        else if(data.getYPixelResidualLocal(planeID)<=0)
            yRes = -(data.getYPixelResidualLocal(planeID) + data.getYPitchLocal(planeID)/2);
    }
    else
        return;

    const Window* theWindow = theWindowsManager_->getWindow(planeID) ;
    int           row       = data.getRowPredicted(planeID)          ;
    int           col       = data.getColPredicted(planeID)          ;

    if(theWindow->checkWindow(col,row))
    {
        THREADED(h1DYcellEfficiencyNorm_[planeID])->Fill(yRes);
        if(data.getHasHit(planeID) /*&& data.getClusterSize(planeID)<=4*/)
        {
            if(data.getClusterSize(planeID)>4)
            {
                //std::cout << "ok!" << std::endl;
                THREADED(h1DYcellEfficiencyFirstHit_ [planeID])->Fill(yRes);
                THREADED(h1DYcellEfficiencySecondHit_[planeID])->Fill(yRes);
                return;
            }

            bool isOk = false;
            for(int h=0; h<data.getClusterSize(planeID); h++)
            {
                if(data.getClusterPixelCol(h,planeID)==col && data.getClusterPixelRow(h,planeID)==row)
                {
                    THREADED(h1DYcellEfficiencyFirstHit_[planeID])->Fill(yRes);
                    THREADED(h1DYcellEfficiencySecondHit_[planeID])->Fill(yRes);
                    isOk = true;
                    break;
                }
            }
            if(isOk)
                return;
            else
            {
                isOk=false;
                for(int h=0; h<data.getClusterSize(planeID); h++)
                {
                    if(data.getClusterPixelCol(h,planeID)==col)
                    {
                        /*if((data.getYPixelResidualLocal(planeID)>0 && (row-data.getClusterPixelRow(h,planeID))==1) ||
                           (data.getYPixelResidualLocal(planeID)<0 && (row-data.getClusterPixelRow(h,planeID))==-1))*/
                        if( ( (row-data.getClusterPixelRow(h,planeID)) ==  1  ) ||
                                ( (row-data.getClusterPixelRow(h,planeID)) == -1  )  )
                        {
                            isOk=true;
                            break;
                        }
                    }
                }
                if(!isOk)
                    return;

                THREADED(h1DYcellEfficiencySecondHit_[planeID])->Fill(yRes);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::setCutsFormula(std::map<std::string,std::string> cutsList, std::vector<TTree*> tree)
{
    std::vector<TTreeFormula*> formulasVector;

    for(std::map<std::string,std::string>::iterator it=cutsList.begin(); it!=cutsList.end(); it++)
    {
        if((it->first) == "main cut" && (it->second).size()==0)
            STDLINE("WARNING: no main cut set in efficiency analysis!! Default value = true!", ACRed);

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
bool Efficiency::passStandardCuts(int planeID, const Data &data)//Requires 8 telescope hits with cluster size <= 2
{
    if(theXmlParser_->getAnalysesFromString("Efficiency")->applyStandardCuts())
        return true;

    int minHits = 7;//To calculate efficiency on the telescope
    int excludeMe = 0;
    if(thePlaneMapping_->getPlaneName(planeID).find("Dut") != std::string::npos)//Dut case
        minHits = atoi(theXmlParser_->getAnalysesFromString("Efficiency")->getMinHits().c_str());
    else if(data.getHasHit(planeID) && data.getClusterSize(planeID)<=2)//Telescope case
        excludeMe = 1;

    std::cout << "Min hits in efficiency analysis: " << minHits << std::endl;

    int hitsontelescope = 0;
    bool HitsClusterLE2 = true;
    for (unsigned int i = 0; i < thePlaneMapping_->getNumberOfPlanes(); i++) {
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

