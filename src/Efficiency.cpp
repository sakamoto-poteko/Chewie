/**
  * File: Efficiency.cpp
  *
  * Currently maintined by Afa.L Cheng <alpha@tomatoeskit.org>
 **/

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

    // START 4 Cell Efficiency .. destroy();
    for(it2=hCellEfficiencyOddColumnsOddRowsNorm_   .begin(); it2!=hCellEfficiencyOddColumnsOddRowsNorm_    .end(); it2++) delete *it2; hCellEfficiencyOddColumnsOddRowsNorm_   .clear();
    for(it2=hCellEfficiencyEvenColumnsOddRowsNorm_  .begin(); it2!=hCellEfficiencyEvenColumnsOddRowsNorm_   .end(); it2++) delete *it2; hCellEfficiencyEvenColumnsOddRowsNorm_  .clear();
    for(it2=hCellEfficiencyOddColumnsOddRows_       .begin(); it2!=hCellEfficiencyOddColumnsOddRows_        .end(); it2++) delete *it2; hCellEfficiencyOddColumnsOddRows_       .clear();
    for(it2=hCellEfficiencyEvenColumnsOddRows_      .begin(); it2!=hCellEfficiencyEvenColumnsOddRows_       .end(); it2++) delete *it2; hCellEfficiencyEvenColumnsOddRows_      .clear();
    for(it2=hCellEfficiencyOddColumnsEvenRowsNorm_  .begin(); it2!=hCellEfficiencyOddColumnsEvenRowsNorm_   .end(); it2++) delete *it2; hCellEfficiencyOddColumnsEvenRowsNorm_  .clear();
    for(it2=hCellEfficiencyEvenColumnsEvenRowsNorm_ .begin(); it2!=hCellEfficiencyEvenColumnsEvenRowsNorm_  .end(); it2++) delete *it2; hCellEfficiencyEvenColumnsEvenRowsNorm_ .clear();
    for(it2=hCellEfficiencyOddColumnsEvenRows_      .begin(); it2!=hCellEfficiencyOddColumnsEvenRows_       .end(); it2++) delete *it2; hCellEfficiencyOddColumnsEvenRows_      .clear();
    for(it2=hCellEfficiencyEvenColumnsEvenRows_     .begin(); it2!=hCellEfficiencyEvenColumnsEvenRows_      .end(); it2++) delete *it2; hCellEfficiencyEvenColumnsEvenRows_     .clear();
    for(it2=h4CellEfficiency_                       .begin(); it2!=h4CellEfficiency_                        .end(); it2++) delete *it2; h4CellEfficiency_                       .clear();
    // END 4 Cell Efficiency

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


    // START Window

    for(it1=hEfficiencyWindowed_                   .begin(); it1!=hEfficiencyWindowed_                   .end(); it1++) delete *it1; hEfficiencyWindowed_                   .clear();
    for(it1=hEfficiencyNormWindowed_               .begin(); it1!=hEfficiencyNormWindowed_               .end(); it1++) delete *it1; hEfficiencyNormWindowed_               .clear();
    for(it2=h2DEfficiencyWindowed_                 .begin(); it2!=h2DEfficiencyWindowed_                 .end(); it2++) delete *it2; h2DEfficiencyWindowed_                 .clear();
    for(it2=h2DEfficiencyNormWindowed_             .begin(); it2!=h2DEfficiencyNormWindowed_             .end(); it2++) delete *it2; h2DEfficiencyNormWindowed_             .clear();
    for(it2=h2DInefficiencyWindowed_               .begin(); it2!=h2DInefficiencyWindowed_               .end(); it2++) delete *it2; h2DInefficiencyWindowed_               .clear();
    for(it2=hCellEfficiencyWindowed_               .begin(); it2!=hCellEfficiencyWindowed_               .end(); it2++) delete *it2; hCellEfficiencyWindowed_               .clear();

    // START 4 Cell Efficiency .. destroy();
    for(it2=hCellEfficiencyOddColumnsOddRowsNormWindowed_   .begin(); it2!=hCellEfficiencyOddColumnsOddRowsNormWindowed_    .end(); it2++) delete *it2; hCellEfficiencyOddColumnsOddRowsNormWindowed_   .clear();
    for(it2=hCellEfficiencyEvenColumnsOddRowsNormWindowed_  .begin(); it2!=hCellEfficiencyEvenColumnsOddRowsNormWindowed_   .end(); it2++) delete *it2; hCellEfficiencyEvenColumnsOddRowsNormWindowed_  .clear();
    for(it2=hCellEfficiencyOddColumnsOddRowsWindowed_       .begin(); it2!=hCellEfficiencyOddColumnsOddRowsWindowed_        .end(); it2++) delete *it2; hCellEfficiencyOddColumnsOddRowsWindowed_       .clear();
    for(it2=hCellEfficiencyEvenColumnsOddRowsWindowed_      .begin(); it2!=hCellEfficiencyEvenColumnsOddRowsWindowed_       .end(); it2++) delete *it2; hCellEfficiencyEvenColumnsOddRowsWindowed_      .clear();
    for(it2=hCellEfficiencyOddColumnsEvenRowsNormWindowed_  .begin(); it2!=hCellEfficiencyOddColumnsEvenRowsNormWindowed_   .end(); it2++) delete *it2; hCellEfficiencyOddColumnsEvenRowsNormWindowed_  .clear();
    for(it2=hCellEfficiencyEvenColumnsEvenRowsNormWindowed_ .begin(); it2!=hCellEfficiencyEvenColumnsEvenRowsNormWindowed_  .end(); it2++) delete *it2; hCellEfficiencyEvenColumnsEvenRowsNormWindowed_ .clear();
    for(it2=hCellEfficiencyOddColumnsEvenRowsWindowed_      .begin(); it2!=hCellEfficiencyOddColumnsEvenRowsWindowed_       .end(); it2++) delete *it2; hCellEfficiencyOddColumnsEvenRowsWindowed_      .clear();
    for(it2=hCellEfficiencyEvenColumnsEvenRowsWindowed_     .begin(); it2!=hCellEfficiencyEvenColumnsEvenRowsWindowed_      .end(); it2++) delete *it2; hCellEfficiencyEvenColumnsEvenRowsWindowed_     .clear();
    for(it2=h4CellEfficiencyWindowed_                       .begin(); it2!=h4CellEfficiencyWindowed_                        .end(); it2++) delete *it2; h4CellEfficiencyWindowed_                       .clear();
    // END 4 Cell Efficiency

    for(it2=hCellEfficiencyEvenColumnsWindowed_    .begin(); it2!=hCellEfficiencyEvenColumnsWindowed_    .end(); it2++) delete *it2; hCellEfficiencyEvenColumnsWindowed_    .clear();
    for(it2=hCellEfficiencyOddColumnsWindowed_     .begin(); it2!=hCellEfficiencyOddColumnsWindowed_     .end(); it2++) delete *it2; hCellEfficiencyOddColumnsWindowed_     .clear();
    for(it2=hCellEfficiencyNormWindowed_           .begin(); it2!=hCellEfficiencyNormWindowed_           .end(); it2++) delete *it2; hCellEfficiencyNormWindowed_           .clear();
    for(it2=hCellEfficiencyEvenColumnsNormWindowed_.begin(); it2!=hCellEfficiencyEvenColumnsNormWindowed_.end(); it2++) delete *it2; hCellEfficiencyEvenColumnsNormWindowed_.clear();
    for(it2=hCellEfficiencyOddColumnsNormWindowed_ .begin(); it2!=hCellEfficiencyOddColumnsNormWindowed_ .end(); it2++) delete *it2; hCellEfficiencyOddColumnsNormWindowed_ .clear();
    for(it2=hCellInefficiencyWindowed_             .begin(); it2!=hCellInefficiencyWindowed_             .end(); it2++) delete *it2; hCellInefficiencyWindowed_             .clear();

    // END Window
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
            ADD_THREADED(hEfficiency_                   [p]);
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
            // START 4 Cell Efficiency .. endJob()
            ADD_THREADED(hCellEfficiencyOddColumnsOddRowsNorm_  [p]);
            ADD_THREADED(hCellEfficiencyEvenColumnsOddRowsNorm_ [p]);
            ADD_THREADED(hCellEfficiencyOddColumnsEvenRowsNorm_ [p]);
            ADD_THREADED(hCellEfficiencyEvenColumnsEvenRowsNorm_[p]);
            ADD_THREADED(hCellEfficiencyOddColumnsOddRows_      [p]);
            ADD_THREADED(hCellEfficiencyEvenColumnsOddRows_     [p]);
            ADD_THREADED(hCellEfficiencyOddColumnsEvenRows_     [p]);
            ADD_THREADED(hCellEfficiencyEvenColumnsEvenRows_    [p]);
            ADD_THREADED(h4CellEfficiency_                      [p]);
            // END 4 Cell Efficiency

            // START Windowed

            ADD_THREADED(hEfficiencyWindowed_                   [p]);
            ADD_THREADED(hEfficiencyNormWindowed_               [p]);
            ADD_THREADED(h2DEfficiencyWindowed_                 [p]);
            ADD_THREADED(h2DEfficiencyNormWindowed_             [p]);
            ADD_THREADED(h2DInefficiencyWindowed_               [p]);
            ADD_THREADED(hCellEfficiencyNormWindowed_           [p]);
            ADD_THREADED(hCellEfficiencyEvenColumnsNormWindowed_[p]);
            ADD_THREADED(hCellEfficiencyOddColumnsNormWindowed_ [p]);
            ADD_THREADED(hCellEfficiencyWindowed_               [p]);
            ADD_THREADED(hCellEfficiencyEvenColumnsWindowed_    [p]);
            ADD_THREADED(hCellEfficiencyOddColumnsWindowed_     [p]);
            ADD_THREADED(hCellInefficiencyWindowed_             [p]);
            // START 4 Cell Efficiency .. endJob()
            ADD_THREADED(hCellEfficiencyOddColumnsOddRowsNormWindowed_  [p]);
            ADD_THREADED(hCellEfficiencyEvenColumnsOddRowsNormWindowed_ [p]);
            ADD_THREADED(hCellEfficiencyOddColumnsEvenRowsNormWindowed_ [p]);
            ADD_THREADED(hCellEfficiencyEvenColumnsEvenRowsNormWindowed_[p]);
            ADD_THREADED(hCellEfficiencyOddColumnsOddRowsWindowed_      [p]);
            ADD_THREADED(hCellEfficiencyEvenColumnsOddRowsWindowed_     [p]);
            ADD_THREADED(hCellEfficiencyOddColumnsEvenRowsWindowed_     [p]);
            ADD_THREADED(hCellEfficiencyEvenColumnsEvenRowsWindowed_    [p]);
            ADD_THREADED(h4CellEfficiencyWindowed_                      [p]);
            // END 4 Cell Efficiency


            // END Windowed

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
            // START 4 Cell Efficiency .. endJob() Calc
            hCellEfficiencyOddColumnsOddRows_      [p]->Divide(hCellEfficiencyOddColumnsOddRowsNorm_    [p]);
            hCellEfficiencyEvenColumnsOddRows_     [p]->Divide(hCellEfficiencyEvenColumnsOddRowsNorm_   [p]);
            hCellEfficiencyOddColumnsEvenRows_     [p]->Divide(hCellEfficiencyOddColumnsEvenRowsNorm_   [p]);
            hCellEfficiencyEvenColumnsEvenRows_    [p]->Divide(hCellEfficiencyEvenColumnsEvenRowsNorm_  [p]);
            // Generate 4 Cell Histogram
            int _cell_xnbins = hCellEfficiencyOddColumnsOddRows_[p]->GetNbinsX();
            int _cell_ynbins = hCellEfficiencyOddColumnsOddRows_[p]->GetNbinsY();

            // O Col O Row  -x, +y
            for (int i = 1; i <= _cell_xnbins; ++i) {
                for (int j = 1; j <= _cell_ynbins; ++j) {
                    h4CellEfficiency_[p]->SetBinContent(i, j + _cell_ynbins,
                                                        hCellEfficiencyOddColumnsOddRows_[p]->GetBinContent(i, j));
                }
            }
            // O Col E Row  -x, -y
            for (int i = 1; i <= _cell_xnbins; ++i) {
                for (int j = 1; j <= _cell_ynbins; ++j) {
                    h4CellEfficiency_[p]->SetBinContent(i, j,
                                                        hCellEfficiencyOddColumnsEvenRows_[p]->GetBinContent(i, j));
                }
            }
            // E Col O Row  +x, +y
            for (int i = 1; i <= _cell_xnbins; ++i) {
                for (int j = 1; j <= _cell_ynbins; ++j) {
                    h4CellEfficiency_[p]->SetBinContent(i + _cell_xnbins, j + _cell_ynbins,
                                                        hCellEfficiencyEvenColumnsOddRows_[p]->GetBinContent(i, j));
                }
            }
            // E Col E Row  +x, -y
            for (int i = 1; i <= _cell_xnbins; ++i) {
                for (int j = 1; j <= _cell_ynbins; ++j) {
                    h4CellEfficiency_[p]->SetBinContent(i + _cell_xnbins, j,
                                                        hCellEfficiencyEvenColumnsEvenRows_[p]->GetBinContent(i, j));
                }
            }

            // End 4 Cell Efficiency

            std::cout<<"DEBUG EndJob"<<std::endl;

            // START Windowed

            hEfficiencyWindowed_                [p]->Divide(hEfficiencyNormWindowed_               [p]);
            h2DEfficiencyWindowed_              [p]->Divide(h2DEfficiencyNormWindowed_             [p]);
            h2DInefficiencyWindowed_            [p]->Divide(h2DEfficiencyNormWindowed_             [p]);
            hCellEfficiencyWindowed_            [p]->Divide(hCellEfficiencyNormWindowed_           [p]);
            hCellEfficiencyEvenColumnsWindowed_ [p]->Divide(hCellEfficiencyEvenColumnsNormWindowed_[p]);
            hCellEfficiencyOddColumnsWindowed_  [p]->Divide(hCellEfficiencyOddColumnsNormWindowed_ [p]);
            hCellInefficiencyWindowed_          [p]->Divide(hCellEfficiencyNormWindowed_           [p]);
            // START 4 Cell Efficiency .. endJob() Calc
            hCellEfficiencyOddColumnsOddRowsWindowed_      [p]->Divide(hCellEfficiencyOddColumnsOddRowsNormWindowed_    [p]);
            hCellEfficiencyEvenColumnsOddRowsWindowed_     [p]->Divide(hCellEfficiencyEvenColumnsOddRowsNormWindowed_   [p]);
            hCellEfficiencyOddColumnsEvenRowsWindowed_     [p]->Divide(hCellEfficiencyOddColumnsEvenRowsNormWindowed_   [p]);
            hCellEfficiencyEvenColumnsEvenRowsWindowed_    [p]->Divide(hCellEfficiencyEvenColumnsEvenRowsNormWindowed_  [p]);
            std::cout<<"DEBUG End Divide"<<std::endl;
            // Generate 4 Cell Histogram
            int Windowed_cellWindowed_xnbins = hCellEfficiencyOddColumnsOddRowsWindowed_[p]->GetNbinsX();
            int Windowed_cellWindowed_ynbins = hCellEfficiencyOddColumnsOddRowsWindowed_[p]->GetNbinsY();

            // O Col O Row  -x, +y
            for (int i = 1; i <= Windowed_cellWindowed_xnbins; ++i) {
                for (int j = 1; j <= Windowed_cellWindowed_ynbins; ++j) {
                    h4CellEfficiencyWindowed_[p]->SetBinContent(i, j + Windowed_cellWindowed_ynbins,
                                                        hCellEfficiencyOddColumnsOddRowsWindowed_[p]->GetBinContent(i, j));
                }
            }
            // O Col E Row  -x, -y
            for (int i = 1; i <= Windowed_cellWindowed_xnbins; ++i) {
                for (int j = 1; j <= Windowed_cellWindowed_ynbins; ++j) {
                    h4CellEfficiencyWindowed_[p]->SetBinContent(i, j,
                                                        hCellEfficiencyOddColumnsEvenRowsWindowed_[p]->GetBinContent(i, j));
                }
            }
            // E Col O Row  +x, +y
            for (int i = 1; i <= Windowed_cellWindowed_xnbins; ++i) {
                for (int j = 1; j <= Windowed_cellWindowed_ynbins; ++j) {
                    h4CellEfficiencyWindowed_[p]->SetBinContent(i + Windowed_cellWindowed_xnbins, j + Windowed_cellWindowed_ynbins,
                                                        hCellEfficiencyEvenColumnsOddRowsWindowed_[p]->GetBinContent(i, j));
                }
            }
            // E Col E Row  +x, -y
            for (int i = 1; i <= Windowed_cellWindowed_xnbins; ++i) {
                for (int j = 1; j <= Windowed_cellWindowed_ynbins; ++j) {
                    h4CellEfficiencyWindowed_[p]->SetBinContent(i + Windowed_cellWindowed_xnbins, j,
                                                        hCellEfficiencyEvenColumnsEvenRowsWindowed_[p]->GetBinContent(i, j));
                }
            }

            // End 4 Cell Efficiency
std::cout<<"DEBUG End Combine"<<std::endl;
            // END Windowed

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
            // START 4 Cell Efficiency .. endJob() Axis
            hCellEfficiencyOddColumnsOddRowsNorm_  [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyOddColumnsOddRowsNorm_  [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyEvenColumnsOddRowsNorm_ [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyEvenColumnsOddRowsNorm_ [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyOddColumnsEvenRowsNorm_ [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyOddColumnsEvenRowsNorm_ [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyEvenColumnsEvenRowsNorm_[p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyEvenColumnsEvenRowsNorm_[p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyOddColumnsOddRows_      [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyOddColumnsOddRows_      [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyEvenColumnsOddRows_     [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyEvenColumnsOddRows_     [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyOddColumnsEvenRows_     [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyOddColumnsEvenRows_     [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyEvenColumnsEvenRows_    [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyEvenColumnsEvenRows_    [p]->GetYaxis()->SetTitle("y (um)");
            h4CellEfficiency_                      [p]->GetXaxis()->SetTitle("x (um)");
            h4CellEfficiency_                      [p]->GetYaxis()->SetTitle("y (um)");
            // END 4 Cell Efficiency

            // START Windowed

            h2DEfficiencyWindowed_              [p]->GetXaxis()->SetTitle("column");
            h2DEfficiencyWindowed_              [p]->GetYaxis()->SetTitle("row"   );
            h2DInefficiencyWindowed_            [p]->GetXaxis()->SetTitle("column");
            h2DInefficiencyWindowed_            [p]->GetYaxis()->SetTitle("row"   );
            hCellEfficiencyWindowed_            [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyEvenColumnsWindowed_ [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyOddColumnsWindowed_  [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyWindowed_            [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyEvenColumnsWindowed_ [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyOddColumnsWindowed_  [p]->GetYaxis()->SetTitle("y (um)");
            hCellInefficiencyWindowed_          [p]->GetXaxis()->SetTitle("x (um)");
            hCellInefficiencyWindowed_          [p]->GetYaxis()->SetTitle("y (um)");
            // START 4 Cell Efficiency .. endJob() Axis
            hCellEfficiencyOddColumnsOddRowsNormWindowed_  [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyOddColumnsOddRowsNormWindowed_  [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyEvenColumnsOddRowsNormWindowed_ [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyEvenColumnsOddRowsNormWindowed_ [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyOddColumnsEvenRowsNormWindowed_ [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyOddColumnsEvenRowsNormWindowed_ [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyEvenColumnsEvenRowsNormWindowed_[p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyEvenColumnsEvenRowsNormWindowed_[p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyOddColumnsOddRowsWindowed_      [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyOddColumnsOddRowsWindowed_      [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyEvenColumnsOddRowsWindowed_     [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyEvenColumnsOddRowsWindowed_     [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyOddColumnsEvenRowsWindowed_     [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyOddColumnsEvenRowsWindowed_     [p]->GetYaxis()->SetTitle("y (um)");
            hCellEfficiencyEvenColumnsEvenRowsWindowed_    [p]->GetXaxis()->SetTitle("x (um)");
            hCellEfficiencyEvenColumnsEvenRowsWindowed_    [p]->GetYaxis()->SetTitle("y (um)");
            h4CellEfficiencyWindowed_                      [p]->GetXaxis()->SetTitle("x (um)");
            h4CellEfficiencyWindowed_                      [p]->GetYaxis()->SetTitle("y (um)");
            // END 4 Cell Efficiency
std::cout<<"DEBUG End Set Axis"<<std::endl;
            // END Windowed

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

        // START 4 Cell Efficiency .. book() New TH2F
        // Odd Rows
        hName  =  "hCellEfficiencyOddColumnsOddRowsNorm_"               + planeName;
        hTitle =  "Cell efficiency odd columns odd rows normalization " + planeName;
        hCellEfficiencyOddColumnsOddRowsNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyEvenColumnsOddRowsNorm_"              + planeName;
        hTitle =  "Cell efficiency even columns odd rows normalization " + planeName;
        hCellEfficiencyEvenColumnsOddRowsNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyOddColumnsOddRows_"                   + planeName;
        hTitle =  "Cell efficiency odd columns odd rows " + planeName;
        hCellEfficiencyOddColumnsOddRows_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyEvenColumnsOddRows_"                  + planeName;
        hTitle =  "Cell efficiency even columns odd rows " + planeName;
        hCellEfficiencyEvenColumnsOddRows_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        // Even Rows
        hName  =  "hCellEfficiencyOddColumnsEvenRowsNorm_"              + planeName;
        hTitle =  "Cell efficiency odd columns even rows normalization " + planeName;
        hCellEfficiencyOddColumnsEvenRowsNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyEvenColumnsEvenRowsNorm_"             + planeName;
        hTitle =  "Cell efficiency even columns even rows normalization " + planeName;
        hCellEfficiencyEvenColumnsEvenRowsNorm_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyOddColumnsEvenRows_"                  + planeName;
        hTitle =  "Cell efficiency odd columns even rows " + planeName;
        hCellEfficiencyOddColumnsEvenRows_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyEvenColumnsEvenRows_"                 + planeName;
        hTitle =  "Cell efficiency even columns even rows " + planeName;
        hCellEfficiencyEvenColumnsEvenRows_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        // 4 Cells
        hName  =  "h4CellEfficiency_"                 + planeName;
        hTitle =  "4 Cell efficiency " + planeName;
        h4CellEfficiency_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(),  ((int)2 * resXRange/5), -resXRange, resXRange, ((int)2 * resYRange/5), -resYRange, resYRange)));

        // END 4 Cell Efficiency

        hName  = "CellInefficiency_"  + planeName;
        hTitle = "Cell inefficiency " + planeName;
        hCellInefficiency_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  = "h1DXcellEfficiencyFirstHit_"                  + planeName;
        hTitle = "1D cell Efficiency - X coordinate first hit " + planeName;
        h1DXcellEfficiencyFirstHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5)));

        hName  = "h1DXcellEfficiencySecondHit_"                  + planeName;
        hTitle = "1D cell Efficiency - X coordinate second hit " + planeName;
        h1DXcellEfficiencySecondHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5)));

        hName  = "h1DXcellEfficiencyNorm_"                          + planeName;
        hTitle = "1D cell Efficiency - X coordinate normalization " + planeName;
        h1DXcellEfficiencyNorm_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5)));

        hName  = "h1DYcellEfficiencyFirstHit_"                 + planeName;
        hTitle = "1D cell Efficiency - Y coordinate first hit " + planeName;
        h1DYcellEfficiencyFirstHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  = "h1DYcellEfficiencySecondHit_"                  + planeName;
        hTitle = "1D cell Efficiency - Y coordinate second hit " + planeName;
        h1DYcellEfficiencySecondHit_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  = "h1DYcellEfficiencyNorm_"                       + planeName;
        hTitle = "1D cell Efficiency - Y coordinate normalization " + planeName;
        h1DYcellEfficiencyNorm_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

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

        // START Window

        theAnalysisManager_->cd("Efficiency");
        theAnalysisManager_->mkdir("Windowed" + planeName);

        hName  = "EfficiencyWindowed_"         + planeName;
        hTitle = "Overall efficiency " + planeName;
        hEfficiencyWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),1,.5,1.5)));

        hName  = "EfficiencyNormWindowed_"                   + planeName;
        hTitle = "Overall efficiency normalization " + planeName;
        hEfficiencyNormWindowed_.push_back(NEW_THREADED(TH1F(hName.c_str(),hTitle.c_str(),1,.5,1.5)));

        hName  =  "CellEfficiencyNormWindowed_"            + planeName;
        hTitle =  "Cell efficiency normalization " + planeName;
        hCellEfficiencyNormWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  =  "CellEfficiencyEvenColumnsNormWindowed_"            + planeName;
        hTitle =  "Cell efficiency normalization even columns" + planeName;
        hCellEfficiencyEvenColumnsNormWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  =  "CellEfficiencyOddColumnsNormWindowed_"            + planeName;
        hTitle =  "Cell efficiency normalization odd columns" + planeName;
        hCellEfficiencyOddColumnsNormWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  = "CellEfficiencyWindowed_"  + planeName;
        hTitle = "Cell efficiency " + planeName;
        hCellEfficiencyWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  = "CellEfficiencyEvenColumnsWindowed_"  + planeName;
        hTitle = "Cell efficiency even columns" + planeName;
        hCellEfficiencyEvenColumnsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        hName  = "CellEfficiencyOddColumnsWindowed_"  + planeName;
        hTitle = "Cell efficiency odd columns" + planeName;
        hCellEfficiencyOddColumnsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        // START 4 Cell Efficiency .. book() New TH2F
        // Odd Rows
        hName  =  "hCellEfficiencyOddColumnsOddRowsNormWindowed_"               + planeName;
        hTitle =  "Cell efficiency odd columns odd rows normalization " + planeName;
        hCellEfficiencyOddColumnsOddRowsNormWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyEvenColumnsOddRowsNormWindowed_"              + planeName;
        hTitle =  "Cell efficiency even columns odd rows normalization " + planeName;
        hCellEfficiencyEvenColumnsOddRowsNormWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyOddColumnsOddRowsWindowed_"                   + planeName;
        hTitle =  "Cell efficiency odd columns odd rows " + planeName;
        hCellEfficiencyOddColumnsOddRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyEvenColumnsOddRowsWindowed_"                  + planeName;
        hTitle =  "Cell efficiency even columns odd rows " + planeName;
        hCellEfficiencyEvenColumnsOddRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        // Even Rows
        hName  =  "hCellEfficiencyOddColumnsEvenRowsNormWindowed_"              + planeName;
        hTitle =  "Cell efficiency odd columns even rows normalization " + planeName;
        hCellEfficiencyOddColumnsEvenRowsNormWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyEvenColumnsEvenRowsNormWindowed_"             + planeName;
        hTitle =  "Cell efficiency even columns even rows normalization " + planeName;
        hCellEfficiencyEvenColumnsEvenRowsNormWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyOddColumnsEvenRowsWindowed_"                  + planeName;
        hTitle =  "Cell efficiency odd columns even rows " + planeName;
        hCellEfficiencyOddColumnsEvenRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        hName  =  "hCellEfficiencyEvenColumnsEvenRowsWindowed_"                 + planeName;
        hTitle =  "Cell efficiency even columns even rows " + planeName;
        hCellEfficiencyEvenColumnsEvenRowsWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5,-(resXRange/2),resXRange/2,(int)resYRange/5,-(resYRange/2),resYRange/2)));

        // 4 Cells
        hName  =  "h4CellEfficiencyWindowed_"                 + planeName;
        hTitle =  "4 Cell efficiency " + planeName;
        h4CellEfficiencyWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(), hTitle.c_str(),  ((int)2 * resXRange/5), -resXRange, resXRange, ((int)2 * resYRange/5), -resYRange, resYRange)));

        // END 4 Cell Efficiency

        hName  = "CellInefficiencyWindowed_"  + planeName;
        hTitle = "Cell inefficiency " + planeName;
        hCellInefficiencyWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),(int)resXRange/5 - 1,-(resXRange/2) + 2.5,resXRange/2 - 2.5,(int)resYRange/5 - 1,-(resYRange/2) + 2.5,resYRange/2 - 2.5)));

        nBinsX = theWindow->getNbins().first;
        nBinsY = theWindow->getNbins().second;

        hName  = "2DEfficiencyWindowed_"               + planeName;
        hTitle = "2D efficiency distribution " + planeName;
        h2DEfficiencyWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),nBinsX,0,nBinsX,nBinsY,0,nBinsY)));

        hName  = "2DEfficiencyNormWindowed_"         + planeName;
        hTitle = "2D efficiency normalization " + planeName;
        h2DEfficiencyNormWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),nBinsX,0,nBinsX,nBinsY,0,nBinsY)));

        hName  = "2DInefficiencyWindowed_"  + planeName;
        hTitle = "2D inefficiency " + planeName;
        h2DInefficiencyWindowed_.push_back(NEW_THREADED(TH2F(hName.c_str(),hTitle.c_str(),nBinsX,0,nBinsX,nBinsY,0,nBinsY)));

        // END Window
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

    // START Window

    if (rectWindow.checkRectWindow(col, row)) {
        THREADED(hEfficiencyNormWindowed_  [planeID])->Fill(1);
        THREADED(h2DEfficiencyNormWindowed_[planeID])->Fill(col,row);
        if(data.getHasHit(planeID))
        {
            THREADED(hEfficiencyWindowed_  [planeID])->Fill(1);
            THREADED(h2DEfficiencyWindowed_[planeID])->Fill(col,row);
        }
        else
            THREADED(h2DInefficiencyWindowed_[planeID])->Fill(col,row);
    }

    // END Window
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Efficiency::cellEfficiency(bool pass, int planeID, const Data& data, int threadNumber)
{
    if(!pass || !data.getIsInDetector(planeID))
        return;

    const  Window* theWindow = theWindowsManager_->getWindow(planeID);
    float          xRes      = data.getXPixelResidualLocal(planeID)  ;
    float          yRes      = data.getYPixelResidualLocal(planeID)  ;
    int            row       = data.getRowPredicted(planeID)         ;
    int            col       = data.getColPredicted(planeID)         ;
    double         maxPitchX = 150                                   ;
    double         maxPitchY = 100                                   ;

    if(theWindow->checkWindow(col,row) && data.getXPitchLocal(planeID)<=maxPitchX && data.getYPitchLocal(planeID)<=maxPitchY)
    {
        THREADED(hCellEfficiencyNorm_[planeID])->Fill(xRes,yRes);
        if(col%2 == 0)
            THREADED(hCellEfficiencyEvenColumnsNorm_[planeID])->Fill(xRes,yRes);
        else
            THREADED(hCellEfficiencyOddColumnsNorm_[planeID])->Fill(xRes,yRes);
        // START 4 Cell Efficiency .. cellEfficiency() Norm
        if (row % 2 == 0) {   // Even row
            if (col % 2 == 0)
                THREADED(hCellEfficiencyEvenColumnsEvenRowsNorm_[planeID])->Fill(xRes, yRes);
            else
                THREADED(hCellEfficiencyOddColumnsEvenRowsNorm_[planeID])->Fill(xRes, yRes);
        } else {    // Odd row
            if (col % 2 == 0)
                THREADED(hCellEfficiencyEvenColumnsOddRowsNorm_[planeID])->Fill(xRes, yRes);
            else
                THREADED(hCellEfficiencyOddColumnsOddRowsNorm_[planeID])->Fill(xRes, yRes);
        }
        // End 4 Cell Efficiency

        if(data.getHasHit(planeID))
        {
            THREADED(hCellEfficiency_[planeID])->Fill(xRes,yRes);
            if(col%2 == 0)
                THREADED(hCellEfficiencyEvenColumns_[planeID])->Fill(xRes,yRes);
            else
                THREADED(hCellEfficiencyOddColumns_[planeID])->Fill(xRes,yRes);
            // START 4 Cell Efficiency .. cellEfficiency() not norm
            if (row % 2 == 0) {   // Even row
                if (col % 2 == 0)
                    THREADED(hCellEfficiencyEvenColumnsEvenRows_[planeID])->Fill(xRes, yRes);
                else
                    THREADED(hCellEfficiencyOddColumnsEvenRows_[planeID])->Fill(xRes, yRes);
            } else {    // Odd row
                if (col % 2 == 0)
                    THREADED(hCellEfficiencyEvenColumnsOddRows_[planeID])->Fill(xRes, yRes);
                else
                    THREADED(hCellEfficiencyOddColumnsOddRows_[planeID])->Fill(xRes, yRes);
            }
            // END 4 Cell Efficiency
        }
        else
            THREADED(hCellInefficiency_[planeID])->Fill(xRes,yRes);
    }

    // START Window

    if(rectWindow.checkRectWindow(col,row) && data.getXPitchLocal(planeID)<=maxPitchX && data.getYPitchLocal(planeID)<=maxPitchY)
    {
        THREADED(hCellEfficiencyNormWindowed_[planeID])->Fill(xRes,yRes);
        if(col%2 == 0)
            THREADED(hCellEfficiencyEvenColumnsNormWindowed_[planeID])->Fill(xRes,yRes);
        else
            THREADED(hCellEfficiencyOddColumnsNormWindowed_[planeID])->Fill(xRes,yRes);
        // START 4 Cell Efficiency .. cellEfficiency() Norm
        if (row % 2 == 0) {   // Even row
            if (col % 2 == 0)
                THREADED(hCellEfficiencyEvenColumnsEvenRowsNormWindowed_[planeID])->Fill(xRes, yRes);
            else
                THREADED(hCellEfficiencyOddColumnsEvenRowsNormWindowed_[planeID])->Fill(xRes, yRes);
        } else {    // Odd row
            if (col % 2 == 0)
                THREADED(hCellEfficiencyEvenColumnsOddRowsNormWindowed_[planeID])->Fill(xRes, yRes);
            else
                THREADED(hCellEfficiencyOddColumnsOddRowsNormWindowed_[planeID])->Fill(xRes, yRes);
        }
        // End 4 Cell Efficiency

        if(data.getHasHit(planeID))
        {
            THREADED(hCellEfficiencyWindowed_[planeID])->Fill(xRes,yRes);
            if(col%2 == 0)
                THREADED(hCellEfficiencyEvenColumnsWindowed_[planeID])->Fill(xRes,yRes);
            else
                THREADED(hCellEfficiencyOddColumnsWindowed_[planeID])->Fill(xRes,yRes);
            // START 4 Cell Efficiency .. cellEfficiency() not norm
            if (row % 2 == 0) {   // Even row
                if (col % 2 == 0)
                    THREADED(hCellEfficiencyEvenColumnsEvenRowsWindowed_[planeID])->Fill(xRes, yRes);
                else
                    THREADED(hCellEfficiencyOddColumnsEvenRowsWindowed_[planeID])->Fill(xRes, yRes);
            } else {    // Odd row
                if (col % 2 == 0)
                    THREADED(hCellEfficiencyEvenColumnsOddRowsWindowed_[planeID])->Fill(xRes, yRes);
                else
                    THREADED(hCellEfficiencyOddColumnsOddRowsWindowed_[planeID])->Fill(xRes, yRes);
            }
            // END 4 Cell Efficiency
        }
        else
            THREADED(hCellInefficiencyWindowed_[planeID])->Fill(xRes,yRes);
    }

    // END Window
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

