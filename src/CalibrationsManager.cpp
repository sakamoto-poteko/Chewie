#include "CalibrationsManager.h"
#include "Geometry.h"
#include "MessageTools.h"
#include "PlanesMapping.h"
#include "Utilities.h"
#include "XmlParser.h"
#include "XmlPlane.h"
#include "XmlWindow.h"

#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"

#include <fstream>
#include <iostream>
#include <sstream>

#define ELECTRONS 421

///////////////////////////////////////////////////////////////////////////////////////////
CalibrationsManager::CalibrationsManager(EventManager* eventManager) :
    theEventManager_ (eventManager)
  , thePlanesMapping_(0 )
  , inFileName_      ("")
  , calibPlotsFile_  (0 )
  , dut_             ("")
{
    if(eventManager)
        theGeometry_ = eventManager->getGeometryPointer();
    else
        theGeometry_ = 0;

    thePlanesMapping_ = new PlanesMapping();

    h1Dresidual_   = 0;
    h2Dresidual_   = 0;
    hThreashold_   = 0;
    h2DThreashold_ = 0;

    for(int p=0; p<NPARS; ++p)
    {
        h1DparsPlots_[p] = 0;
        h2DparsPlots_[p] = 0;
    }

    for(int p=0; p<NPARS; ++p)
    {
        for(int pp=0; pp<NPARS; ++pp)
            hCorrelations_[p][pp] = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
CalibrationsManager::~CalibrationsManager(void)
{
    destroyCalibHistos();
    destroyParsPlots();
}

///////////////////////////////////////////////////////////////////////////////////////////
void CalibrationsManager:: destroyCalibHistos(void)
{
    if(h1Dresidual_)
    {
        delete h1Dresidual_;
        h1Dresidual_ = 0;
    }

    if(h2Dresidual_)
    {
        delete h2Dresidual_;
        h2Dresidual_ = 0;
    }

    if(hThreashold_)
    {
        delete hThreashold_;
        hThreashold_ = 0;
    }

    if(h2DThreashold_)
    {
        delete h2DThreashold_;
        h2DThreashold_ = 0;
    }

    for(std::map< int,std::map<int,TH1F*> >::iterator r=hPixelCalibration_.begin(); r!=hPixelCalibration_.end(); ++r)
    {
        for(std::map<int,TH1F*>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
        {
            delete c->second;
        }
    }

    hPixelCalibration_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////
void CalibrationsManager::destroyParsPlots(void)
{
    for(int p=0; p<NPARS; ++p)
    {
        if(h1DparsPlots_[p])
        {
            delete h1DparsPlots_[p];
            h1DparsPlots_[p] = 0;
        }
        if(h2DparsPlots_[p])
        {
            delete h2DparsPlots_[p];
            h2DparsPlots_[p] = 0;
        }
    }

    for(int p=0; p<NPARS; ++p)
    {
        for(int pp=0; pp<NPARS; ++pp)
        {
            if(hCorrelations_[p][pp])
            {
                delete hCorrelations_[p][pp];
                hCorrelations_[p][pp] = 0;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void CalibrationsManager::openCalibPlotsFile(std::string fileName)
{
    calibPlotsFile_ = TFile::Open(fileName.c_str());
    STDLINE("",ACWhite);

    if(!calibPlotsFile_)
    {
        STDLINE("Impossible to open file: " + fileName + "!!",ACRed);
        return;
    }
    else
    {
        STDLINE("File " + fileName + " succesfully opened!",ACYellow);
        copyHistograms();
    }

    STDLINE("",ACWhite);
}

///////////////////////////////////////////////////////////////////////////////////////////
void CalibrationsManager::copyHistograms(void)
{
    TDirectoryFile* dir;
    std::stringstream hName, ss;

    if((dir = (TDirectoryFile*)calibPlotsFile_->Get("Parameters_distr"))!=0)
    {
        for(int p=0; p<4; ++p)
        {
            hName.str(""); hName << "Par" << p << "/h2DParsPlot_" << p;
            h2DparsPlots_[p] = (TH2F*)dir->Get(hName.str().c_str());
        }
    }
    else
    {
        STDLINE("Directory Parameters_distr doesn't exist!",ACRed);
        STDLINE("Make the right histograms checking 'Parameters' in the Calibrations tab!",ACRed);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void CalibrationsManager::setWindow(void)
{
    XmlParser* theParser = theEventManager_->getXmlParser();

    firstRow_ = atoi( ((theParser->getPlanes())[dut_]->getWindow()->getLowerRow()).c_str()  );
    lastRow_  = atoi( ((theParser->getPlanes())[dut_]->getWindow()->getHigherRow()).c_str() );
    firstCol_ = atoi( ((theParser->getPlanes())[dut_]->getWindow()->getLowerCol()).c_str()  );
    lastCol_  = atoi( ((theParser->getPlanes())[dut_]->getWindow()->getHigherCol()).c_str() );
}

///////////////////////////////////////////////////////////////////////////////////////////
void CalibrationsManager::bookParsPlots(void)
{
    std::stringstream hName     ;
    std::stringstream hTitle    ;
    std::stringstream xAxisTitle;
    std::stringstream yAxisTitle;
    std::stringstream dirName   ;

    int    binsX = 0;
    int    binsY = 0;
    double Xmin  = 0;
    double Xmax  = 0;
    double Ymin  = 0;
    double Ymax  = 0;

    double plotsRangeAndBins[4][3];

    plotsRangeAndBins[0][2] =  1000 ;
    plotsRangeAndBins[0][1] =     0 ;
    plotsRangeAndBins[0][0] =   125 ;

    plotsRangeAndBins[1][2] =   500 ;
    plotsRangeAndBins[1][1] =     0 ;
    plotsRangeAndBins[1][0] =   125 ;

    plotsRangeAndBins[2][2] = 1E-04 ;
    plotsRangeAndBins[2][1] = 0     ;
    plotsRangeAndBins[2][0] = 125   ;

    plotsRangeAndBins[3][2] =     2 ;
    plotsRangeAndBins[3][1] =    -2 ;
    plotsRangeAndBins[3][0] =   125 ;

    theEventManager_->cd("/");
    theEventManager_->mkdir("Pars distributions");

    for(int p=0; p<NPARS; ++p)
    {
        theEventManager_->cd("Pars distributions");
        dirName.str(""); dirName << "Par" << p;
        theEventManager_->mkdir(dirName.str());

        hName.str("") ; hName  << "h1DParsPlot_Par" << p;
        hTitle.str(""); hTitle << "Parameter " << p << " distribution - " << dut_;

        binsX = plotsRangeAndBins[p][0];
        Xmin  = plotsRangeAndBins[p][1];
        Xmax  = plotsRangeAndBins[p][2];

        h1DparsPlots_[p] = new TH1F(hName.str().c_str(), hTitle.str().c_str(), binsX, Xmin, Xmax);
        hName.str("") ; hName  << "h2DParsPlot_" << p;
        h2DparsPlots_[p] = new TH2F(hName.str().c_str(), hTitle.str().c_str(), 52, 0, 52, 80, 0, 80);

        xAxisTitle.str(""); xAxisTitle << "Par" << p;
        h1DparsPlots_[p]->GetXaxis()->SetTitle(xAxisTitle.str().c_str());

        h2DparsPlots_[p]->GetXaxis()->SetTitle("columns");
        h2DparsPlots_[p]->GetYaxis()->SetTitle("rows");
    }

    theEventManager_->cd("/");
    theEventManager_->mkdir("Pars correlations");

    for(int p=0; p<NPARS; ++p)
    {
        //theEventManager_->cd("Parameters");
        theEventManager_->cd("Pars correlations");
        dirName.str(""); dirName << "Corr_Par" << p;
        theEventManager_->mkdir(dirName.str());

        for(int pp=0; pp<NPARS; ++pp)
        {
            if(pp!=p)
            {
                hName.str("") ; hName << "hCorrelation_Par" << p << "_vs_Par" << pp;
                hTitle.str(""); hTitle << "Correlation Par" << p << " vs Par" << pp << " - " << dut_;

                binsX = plotsRangeAndBins[pp][0];
                Xmin  = plotsRangeAndBins[pp][1];
                Xmax  = plotsRangeAndBins[pp][2];
                binsY = plotsRangeAndBins[p ][0];
                Ymin  = plotsRangeAndBins[p ][1];
                Ymax  = plotsRangeAndBins[p ][2];

                hCorrelations_[p][pp] = new TH2F(hName.str().c_str(), hTitle.str().c_str(), binsX, Xmin, Xmax, binsY, Ymin, Ymax);

                xAxisTitle.str(""); xAxisTitle << "Par" << pp;
                yAxisTitle.str(""); yAxisTitle << "Par" << p;

                hCorrelations_[p][pp]->GetXaxis()->SetTitle(xAxisTitle.str().c_str());
                hCorrelations_[p][pp]->GetYaxis()->SetTitle(yAxisTitle.str().c_str());
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void CalibrationsManager::bookCalibHistos(void)
{
    std::stringstream hName;
    std::stringstream hTitle;

    theEventManager_->cd("/");

    hThreashold_   = new TH1F("hThreashold_"  , "Threashold dispersion"   , 5000     ,  0, 10000);
    h2DThreashold_ = new TH2F("h2DThreashold_", "2D Threashold dispersion", 52, 0, 52, 80, 0, 80);
    hFirstBin_     = new TH2F("hFirstBin"     , "First bin distribution"  , 52, 0, 52, 80, 0, 80);
    hLastBin_      = new TH2F("hLastBin"      , "Last bin distribution"   , 52, 0, 52, 80, 0, 80);

    theEventManager_->mkdir("Pixel Calibrations");

    std::map<int,TH1F*> tmp;

    for(int r=firstRow_; r<=lastRow_; ++r)
    {
        for(int c=firstCol_; c<=lastCol_; ++c)
        {
            hName.str("") ; hName << "hPixelCalibration_r" << r << "_c" << c;
            hTitle.str(""); hTitle << "Calibration of pixel at row: " << r << ", col " << c << " - " << dut_;
            tmp[c] = new TH1F(hName.str().c_str(),hName.str().c_str(),255,0,255*ELECTRONS);
            tmp[c]->GetXaxis()->SetTitle("charge (electrons)");
            tmp[c]->GetYaxis()->SetTitle("adc"               );
        }
        hPixelCalibration_[r] = tmp;
    }

    calFunk_ = new TF1("calFunk",Utilities::calibrationFitFunk,0,255*ELECTRONS,4);
    calFunkInv_ = new TF1("calFunkInv",Utilities::calibrationFitFunkInv,600,1000,4);
    calFunk_->SetLineColor(kBlue);

    theEventManager_->cd("/");
    theEventManager_->mkdir("Residuals");

    h1Dresidual_ = new TH1F("h1Dresidual_", "1D residuals distribution", 200, -100,          100                 );
    h2Dresidual_ = new TH2F("h2Dresidual_", "Residuals vs charge"      , 255,    0, 255*ELECTRONS, 200, -100, 100);

    h1Dresidual_->GetXaxis()->SetTitle("Residual (adc)"    );
    h2Dresidual_->GetXaxis()->SetTitle("charge (electrons)");
    h2Dresidual_->GetYaxis()->SetTitle("Residual (adc)"    );
}

///////////////////////////////////////////////////////////////////////////////////////////
void CalibrationsManager::makeParsPlots(void)
{
    std::stringstream ss;
    ss.str(""); ss << "Making parameters plots for " << dut_ << "...";

    STDLINE("",ACWhite);
    STDLINE(ss.str(),ACCyan);
    STDLINE("",ACWhite);

    bookParsPlots();

    int dutPosition = thePlanesMapping_->getPlanePosition(dut_);
    int station     = thePlanesMapping_->getStation(dutPosition);
    int plaquette   = thePlanesMapping_->getPlaquette(dutPosition);

    double *pars;

    for(int r=firstRow_; r<=lastRow_; ++r)
    {
        for(int c=firstCol_; c<=lastCol_; ++c)
        {
            pars = theGeometry_->getDetector(station,plaquette)->getROC(0)->getCalibrationFunction(r,c);
            if(pars != NULL)
            {
                for(int p=0; p<NPARS; ++p)
                {
                    h1DparsPlots_[p]->Fill(    pars[p]);
                    h2DparsPlots_[p]->Fill(c,r,pars[p]);
                    for(int pp=0; pp<4; ++pp)
                    {
                        if(pp!=p)
                            hCorrelations_[p][pp]->Fill(pars[pp],pars[p]);
                    }
                }
            }
        }
    }

    STDLINE("",ACWhite);
    STDLINE("Done!",ACCyan);
    STDLINE("",ACWhite);
}

///////////////////////////////////////////////////////////////////////////////////////////
void CalibrationsManager::makeCalibHistos(void)
{
    bookCalibHistos();

    std::ifstream inputFile(inFileName_.c_str());

    if(!inputFile.is_open())
    {
        STDLINE("Impossible to open calibration file or no calibration file loaded!",ACRed);
        return;
    }

    std::stringstream ss;
    ss.str(""); ss << "Making pixel calibrations histograms for " << dut_ << "...";

    STDLINE("",ACWhite);
    STDLINE(ss.str(),ACCyan);
    STDLINE("",ACWhite);

    std::stringstream hName ;
    std::stringstream hTitle;
    std::map<int,TH1F*> tmpMap;
    std::map< int,std::map<int,TH1F*> > hTmpCalib;

    for(int r=firstRow_; r<=lastRow_; ++r)
    {
        for(int c=firstCol_; c<=lastCol_; ++c)
        {
            hName.str("") ; hName << "hTmpCalibration_r" << r << "_c" << c;
            hTitle.str(""); hTitle << "Temporary calibration histo of pixel at row: " << r << ", col " << c;
            tmpMap[c] = new TH1F(hName.str().c_str(),hName.str().c_str(),255,0,255*ELECTRONS);
        }
        hTmpCalib[r] = tmpMap;
    }

    std::map< int,std::map<int,TH1F*> >::iterator itR;
    std::map<int,TH1F*>::iterator itC;
    std::string line = "";
    std::string tmp  = "";
    int pos,posF,posL;
    int r,c,a,bin;
    int Vcal = 0;

    while(inputFile.good())
    {
        std::getline(inputFile,line);
        if(line.find("Iteration")==0)
        {
            pos  = line.find("=");
            tmp  = line.substr(pos+1,line.size());
            Vcal = atoi(tmp.c_str());
        }
        if(line.find("r ")==0)
        {
            posF = line.find("r");
            posL = line.find("c");
            tmp  = line.substr(posF+1,posL-2);
            r = atoi(tmp.c_str());

            posF = posL;
            posL = line.find("h");
            tmp = line.substr(posF+1,posL-2);
            c = atoi(tmp.c_str());

            posF = posL;
            posL = line.find("a");
            tmp = line.substr(posF+1,posL-2);
            tmp = line.substr(posL+1,line.size());
            a = atoi(tmp.c_str());

            itR = hTmpCalib.find(r);
            if( itR != hTmpCalib.end() )
            {
                itC = (*itR).second.find(c);
                if( itC != (*itR).second.end() )
                {
                    bin = hTmpCalib[(*itR).first][(*itC).first]->Fill(Vcal*ELECTRONS,a);
                    hTmpCalib[(*itR).first][(*itC).first]->SetBinError(bin,2.5);
                }
            }
        }
    }

    inputFile.close();

    int nBins;
    for(std::map< int,std::map<int,TH1F*> >::iterator r=hTmpCalib.begin(); r!=hTmpCalib.end(); ++r)
    {
        for(std::map<int,TH1F*>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
        {
            bin = 1;
            nBins = hTmpCalib[(*r).first][(*c).first]->GetNbinsX();
            while(bin <= nBins && hTmpCalib[(*r).first][(*c).first]->GetBinContent(bin) == 0)
                bin++;
            hFirstBin_->Fill((*c).first,(*r).first,bin);
            //hThreashold_->Fill(hTmpCalib[(*r).first][(*c).first]->GetBinCenter(bin));
        }
    }

    for(std::map< int,std::map<int,TH1F*> >::iterator r=hTmpCalib.begin(); r!=hTmpCalib.end(); ++r)
    {
        for(std::map<int,TH1F*>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
        {
            bin = hTmpCalib[(*r).first][(*c).first]->GetNbinsX();
            while(bin >= 1 && hTmpCalib[(*r).first][(*c).first]->GetBinContent(bin) == 0)
                bin--;
            hLastBin_->Fill((*c).first,(*r).first,bin);
        }
    }

    double precADC   ;
    double currentADC;
    int    lastBin   ;
    int    firstBin  ;
    for(std::map< int,std::map<int,TH1F*> >::iterator r=hTmpCalib.begin(); r!=hTmpCalib.end(); ++r)
    {
        for(std::map<int,TH1F*>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
        {
            lastBin  = hLastBin_->GetBinContent(hLastBin_->GetXaxis()->FindBin((*c).first),hLastBin_->GetYaxis()->FindBin((*r).first));
            firstBin = hFirstBin_->GetBinContent(hFirstBin_->GetXaxis()->FindBin((*c).first),hFirstBin_->GetYaxis()->FindBin((*r).first));
            precADC  = hTmpCalib[(*r).first][(*c).first]->GetBinContent(lastBin);
            for(int b=lastBin-1; b>=firstBin; b--)
            {
                currentADC = hTmpCalib[(*r).first][(*c).first]->GetBinContent(b);
                if(std::abs(precADC-currentADC)>30)
                    continue;
                else
                {
                    hPixelCalibration_[(*r).first][(*c).first]->SetBinContent(b,precADC);
                    hPixelCalibration_[(*r).first][(*c).first]->SetBinError(b,2.5);
                    precADC = currentADC;
                }
            }
        }
    }

    for(std::map< int,std::map<int,TH1F*> >::iterator r=hTmpCalib.begin(); r!=hTmpCalib.end(); ++r)
    {
        for(std::map<int,TH1F*>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
        {
            hTmpCalib[(*r).first][(*c).first]->Delete("0");
        }
    }

    STDLINE("",ACWhite);
    STDLINE("Done!",ACCyan);
    STDLINE("",ACWhite);

    drawFitFunction();
}

///////////////////////////////////////////////////////////////////////////////////////////
void CalibrationsManager::drawFitFunction(void)
{
    STDLINE("",ACWhite);
    STDLINE("Fitting pixel calibrations histograms....",ACCyan);
    STDLINE("",ACWhite);

    int dutPosition = thePlanesMapping_->getPlanePosition(dut_);
    int station     = thePlanesMapping_->getStation(dutPosition);
    int plaquette   = thePlanesMapping_->getPlaquette(dutPosition);

    double* pars;
    double  residual;
    std::stringstream ss;

    for(std::map< int,std::map<int,TH1F*> >::iterator r=hPixelCalibration_.begin(); r!=hPixelCalibration_.end(); ++r)
    {
        for(std::map<int,TH1F*>::iterator c=(*r).second.begin(); c!=(*r).second.end(); ++c)
        {
            if(hPixelCalibration_[(*r).first][(*c).first]->GetEntries() == 0) continue;
            pars = theGeometry_->getDetector(station,plaquette)->getROC(0)->getCalibrationFunction((*r).first,(*c).first);
            if(pars != NULL)
            {
                //calFunkInv_->SetParameters(pars);
                int firstBin = (int)hFirstBin_->GetBinContent(hFirstBin_->GetXaxis()->FindBin((*c).first),hFirstBin_->GetYaxis()->FindBin((*r).first));
                //double firstADC  = hPixelCalibration_[(*r).first][(*c).first]->GetBinContent(firstBin);
                //if(firstADC > 600 && firstADC < 1000)
                //{
                    //double firstVcal = calFunkInv_->Eval(firstADC);
                    double firstVcal = hPixelCalibration_[(*r).first][(*c).first]->GetBinCenter(firstBin);
                    hThreashold_->Fill(firstVcal);
                    h2DThreashold_->Fill((*c).first,(*r).first,firstVcal);
                //}

                calFunk_->SetRange(0,60000);
                calFunk_->SetParameters(pars);

                //for(int p=0; p<4; ++p)
                  //  calFunk_->FixParameter(p,pars[p]);

                hPixelCalibration_[(*r).first][(*c).first]->Fit(calFunk_,"+SQR",0,60000);

                for(int b=1; b<hPixelCalibration_[(*r).first][(*c).first]->GetNbinsX(); b++)
                {
                    if(hPixelCalibration_[(*r).first][(*c).first]->GetBinContent(b)!=0)
                    {
                        residual = calFunk_->Eval(hPixelCalibration_[(*r).first][(*c).first]->GetBinCenter(b)) - hPixelCalibration_[(*r).first][(*c).first]->GetBinContent(b);
                        h1Dresidual_->Fill(residual);
                        h2Dresidual_->Fill(hPixelCalibration_[(*r).first][(*c).first]->GetBinCenter(b),residual);
                    }
                    else
                        continue;
                }
            }
        }
    }

    STDLINE("",ACWhite);
    STDLINE("Done!",ACCyan);
    STDLINE("",ACWhite);
}
