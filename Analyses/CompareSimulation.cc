#include <iostream>
#include <TF1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <sstream>
#include <string>
#include "TMath.h"

using namespace std;

int langausFit(TH1* histo, double *fitParameters);
double langaus(double *x, double *par);

int main()
{
  TFile* file = new TFile("Chewie_Runs262_269.root");
  //file->cd("Charge/Dut1");
  //file->ls();
  TH2F* hCellChargeCoarse = (TH2F*)file->Get("Charge/Dut1/2DCellCharge/hCellChargeCoarse_Dut1");
  //float posX = 56.25;
  //float posY = 25;
  float posX = 10;
  float posY = 5;
  TFile* outFile = new TFile("Landau.root","RECREATE");
  TH1F* hCellChargeCoarseLandauSum;
  for(int i=0; i<4; i++)
  {
    if(i==1) posX = -posX;
    if(i==2) posY = -posY;
    if(i==3){ posX = -posX; posY = -posY;}
    float binCenterX = hCellChargeCoarse->GetXaxis()->GetBinCenter(hCellChargeCoarse->GetXaxis()->FindBin(posX));
    float binCenterY = hCellChargeCoarse->GetYaxis()->GetBinCenter(hCellChargeCoarse->GetYaxis()->FindBin(posY));
    stringstream sName;
    sName.str("");
    sName << "Charge/Dut1/2DCellChargeLandau/hCellChargeCoarseLandau_" << binCenterX << "," << binCenterY << "Dut1";
    cout << binCenterX << " " << binCenterY << endl;
    TH1F* hCellChargeCoarseLandau = (TH1F*)file->Get(sName.str().c_str());
    if(i==0)
    {
      outFile->cd();
      hCellChargeCoarseLandauSum = (TH1F*)hCellChargeCoarseLandau->Clone();
      hCellChargeCoarseLandauSum->SetName("hCellChargeCoarseLandauSum");
    }
    else
    {
      hCellChargeCoarseLandauSum->Add(hCellChargeCoarseLandau);
    }
    cout << hCellChargeCoarseLandau << endl;
  }

  //cout << hCellChargeCoarse->GetBinContent(binX,binY)<< endl;

  file->Close();
  double fitParameters[4];
  langausFit(hCellChargeCoarseLandauSum,fitParameters);
  hCellChargeCoarseLandauSum->Write();
  outFile->Close();
  return 0;
}


int langausFit(TH1* histo, double *fitParameters)
{
    TF1*   langaus_        ;
    langaus_ = new TF1("langaus",langaus,0,60000,4);
    langaus_->SetParNames("Width","MPV","Area","GSigma");
    langaus_->SetLineColor(kBlue);
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

    fitRange[0]=0.4*(histo->GetMean());
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

////////////////////////////////////////////////////////////////////////////////////
double langaus(double *x, double *par)
{
     double invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
     double mpshift  = -0.22278298;       // Landau maximum location

     // Control constants
     double np = 100.0;      // number of convolution steps
     double sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

     // Variables
     double xx;
     double mpc;
     double fland;
     double sum = 0.0;
     double xlow,xupp;
     double step;
     double i;


     // MP shift correction
     mpc = par[1] - mpshift * par[0];

     // Range of convolution integral
     xlow = x[0] - sc * par[3];
     xupp = x[0] + sc * par[3];

     step = (xupp-xlow) / np;

     // Convolution integral of Landau and Gaussian by sum
     for(i=1.0; i<=np/2; i++) {
        xx = xlow + (i-.5) * step;
        fland = TMath::Landau(xx,mpc,par[0]) / par[0];
        sum += fland * TMath::Gaus(x[0],xx,par[3]);

        xx = xupp - (i-.5) * step;
        fland = TMath::Landau(xx,mpc,par[0]) / par[0];
        sum += fland * TMath::Gaus(x[0],xx,par[3]);
     }

     return (par[2] * step * sum * invsq2pi / par[3]);
}
