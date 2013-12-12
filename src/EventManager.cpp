#include "EventManager.h"
#include "EventConverter.h"
#include "CalibrationsManager.h"
#include "Event.h"
#include "EventHeader.h"
#include "Geometry.h"
#include "XmlParser.h"
#include "XmlConverter.h"
#include "analyzerdlg.h"

#include <TFile.h>
#include <TTree.h>

#include <iostream>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////////////////
EventManager::EventManager(void) :
   geoFile_              (0 )
  ,outFile_              (0 )
  ,calibPlotsFile_       (0 )
  ,theEventsTree_        (0 )
  ,inputGeometryTree_    (0 )
  ,inputEventHeader_     (0 )
  ,theEventBranch_       (0 )
  ,theEvent_             (0 )
  ,theEventHeader_       (0 )
  ,theConverter_         (0 )
  ,theCalibrationManager_(0 )
  ,theGeometry_          (0 )
  ,inRootFileName_       ("")
  ,inGeoFileName_        ("")
  ,outFileName_          ("")
  ,calibPlotsFileName_   ("")
  ,totalEvents_          (0 )
  ,totalEventsToProcess_ (0 )
  ,totalProcessedEvents_ (0 )
  ,eventsToProcess_      (0 )
  ,maxEvents_            (-1)
  ,nOfThreads_           (0 )
  ,theXmlParser_         (0 )
  ,currentOperation_     ("")
  ,abort_                (false)
  ,isFinished_           (true)
  ,currentEntry_         (0)
  ,completionStatus_     (0)
  ,completionStatusBegin_(0)
  ,completionStatusEnd_  (0)
{
}

///////////////////////////////////////////////////////////////////////////////////////////
EventManager::~EventManager(void)
{
    if(!isFinished_)
        stopConverter();
    writeOutFile();
    closeOutFile();
    closeInFiles();
    closeGeoFile();
    closeCalibPlotsFile();

    if(theConverter_)
    {
        delete theConverter_;
        theConverter_ = 0;
    }

    if(theCalibrationManager_)
    {
        delete theCalibrationManager_;
        theCalibrationManager_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::setInFilesList(std::vector<std::string> fileNames)
{
    closeInFiles();
    inFilesList_.clear();
    for(std::vector<std::string>::iterator it=fileNames.begin(); it!=fileNames.end(); it++)
        inFilesList_[*it] = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::addInFile(std::string fileName)
{
    if(!(inFilesList_.find(fileName) != inFilesList_.end()))
        inFilesList_[fileName] = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::closeInFiles(void)
{
    for(std::map<std::string,TFile*>::iterator it=inFilesList_.begin(); it != inFilesList_.end(); it++)
        closeInFile(it->first);
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::closeInFile(std::string fileName)
{
    if(inFilesList_.find(fileName) != inFilesList_.end() && inFilesList_[fileName] != 0)
    {
        inFilesList_[fileName]->Close();
        delete inFilesList_[fileName];
        inFilesList_[fileName] = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::openInFiles(void)
{
    for(std::map<std::string,TFile*>::iterator it=inFilesList_.begin(); it != inFilesList_.end(); it++)
        openInFile(it->first);
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::openInFile(std::string fileName)
{
    if(inFilesList_.find(fileName) == inFilesList_.end())
    {
        FATAL("ERROR: File " + fileName + " is not in the list of input files! Aborting...", ACRed);
        exit(EXIT_FAILURE);
    }
    if(inFilesList_[fileName] == 0)
        inFilesList_[fileName] = TFile::Open(fileName.c_str(),"READ");

    if(!inFilesList_[fileName]->IsOpen())
    {
        FATAL("ERROR: Can't open file " + fileName + "! Aborting...", ACRed);
        exit(EXIT_FAILURE);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
bool EventManager::openGeoFile(std::string fileName)
{
    closeGeoFile();

    if(fileName.find(".root") != std::string::npos)
        inGeoFileName_ = fileName.replace(fileName.find(".root"),5,".geo");
    else
        inGeoFileName_ = fileName;

    geoFile_ = TFile::Open( inGeoFileName_.c_str(), "READ" );

    if( !geoFile_->IsOpen() )
    {
        STDLINE(std::string("Could not open file ") + inGeoFileName_, ACRed) ;
        exit(EXIT_FAILURE) ;
    }

    STDLINE("File " + inGeoFileName_ + " successfully opened", ACYellow) ;

    return readGeometry();
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::closeGeoFile(void)
{
    if(geoFile_)
    {
        geoFile_->Close();
        delete geoFile_;
        geoFile_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::openOutFile(std::string fileName)
{
    fileName.replace(fileName.find(".root"),5,"_Converted.root");

    outFileName_ = fileName;
    closeOutFile();
    outFile_ = TFile::Open(outFileName_.c_str(),"RECREATE");
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::writeOutFile(void)
{
    if(outFile_)
    {
        outFile_->cd();
        outFile_->Write();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::closeOutFile(void)
{
    if(outFile_)
    {
        outFile_->Close();
        delete outFile_;
        outFile_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::startConverter(void)
{
    if(inFilesList_.size()==0 || (theXmlParser_->getDocument())==0)
        return;
    currentOperation_ = "Starting conversion";
    STDLINE(currentOperation_,ACRed);
    abort_            = false;
    isFinished_       = false;
    completionStatus_ = 0;
    currentEntry_     = 0;

    if(abort_) return;
    setNumberOfThreads(atoi(theXmlParser_->getConverter()->getNumberOfThreads().c_str()));
    if(theConverter_ == 0 || theConverter_->getNumberOfThreads() != nOfThreads_)
    {
        if(theConverter_ != 0)
            delete theConverter_;
        theConverter_ = new EventConverter(this,nOfThreads_);
    }
    completionStatus_ = 1;
    calculateTotalEvents();
    completionStatus_ = 2;

    if(abort_) return;
    completionStatusBegin_ = completionStatusEnd_ = completionStatus_;
    const float percentToProcess = 100-completionStatus_;
    float analysisPercent;
    totalProcessedEvents_ = 0;
    for(std::map<std::string,TFile*>::iterator it=inFilesList_.begin(); it!=inFilesList_.end(); it++)
    {
        if(abort_) return;
        currentOperation_ = "Converting " + (it->first).substr(it->first.find_last_of('/')+1,it->first.size()-it->first.find_last_of('/')-1);
        std::string runNumberString = (it->first).substr(it->first.find_last_of('/')+4,it->first.size()-it->first.find_last_of('/')-16);
        int runNumber = atoi(runNumberString.c_str());
        std::string dataDir = AnalyzerDlg::getEnvPath("CHEWIEINPUTDIR").toStdString();
        dataDir = dataDir.substr(0, dataDir.length()-1);
        STDLINE(currentOperation_,ACRed);
        currentEntry_          = 0;
        eventsToProcess_       = eventsToProcessList_[it->first];
        analysisPercent        = percentToProcess*((float)eventsToProcess_/totalEventsToProcess_);
        completionStatusBegin_ = completionStatusEnd_;
        completionStatusEnd_  += analysisPercent;

        openInFile    (it->first);
        readEventsTree(it->first);
        openGeoFile   (it->first);
        openOutFile   (dataDir + "/" + (it->first).substr(it->first.find_last_of('/')+1,it->first.size()-it->first.find_last_of('/')-6) + ".root");
        theConverter_->setRunNumber(runNumber);
        theConverter_->beginJob();
        theConverter_->runConverter();
        theConverter_->endJob();
        writeOutFile();
        closeOutFile();
        closeGeoFile();
        closeInFile (it->first);
        if(currentEntry_ >= 0)
            totalProcessedEvents_ += currentEntry_;
        else
            totalProcessedEvents_ += eventsToProcessList_[it->first];
        if(abort_) return;
        currentOperation_ += " done";
        STDLINE(currentOperation_,ACRed);
    }

    isFinished_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////
int EventManager::getCurrentEvent(Event& event)
{
    if(currentEntry_ >= 0 && currentEntry_ < eventsToProcess_)
    {
        theEventsTree_->GetEntry(currentEntry_++);
        event = *theEvent_;
    }
    else
        currentEntry_ = -1;

    return currentEntry_;
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::stopConverter(void)
{
    abort_ = true;
    theConverter_->stopThreads();
}

///////////////////////////////////////////////////////////////////////////////////////////
bool EventManager::readEventsTree(std::string fileName)
{
    std::stringstream ss;
    std::stringstream eventsTreeName;
    std::stringstream eventsHeaderName;

    std::string treeName = fileName.substr(fileName.rfind("/")+1,fileName.size());

    eventsTreeName   << treeName << "Events";
    eventsHeaderName << treeName << "Header";

    bool atLeastOneBranchFound = false ;

    if ( (theEventsTree_ = (TTree*)inFilesList_[fileName]->Get(eventsTreeName.str().c_str())) )
    {
        theEventsTree_    ->SetBranchAddress("EventBranch",    &theEvent_      );
        entriesList_[fileName] = theEventsTree_->GetEntries();
        //ss.str("");
        //ss << "Found " << entriesList_[fileName] << " events on file";
        //STDLINE(ss.str(),ACGreen);

        inputEventHeader_ = (TTree*)inFilesList_[fileName]->Get(eventsHeaderName.str().c_str()) ;

        if( inputEventHeader_ )
        {
            inputEventHeader_->SetBranchAddress("EventHeader",    &theEventHeader_);
            inputEventHeader_->GetEntry(0);
        }

        atLeastOneBranchFound = true ;
    }

    if( !atLeastOneBranchFound )
    {
        STDLINE("FATAL: no branches found in file. Aborting",ACRed) ;
        exit(0) ;
    }

    return atLeastOneBranchFound;
}

///////////////////////////////////////////////////////////////////////////////////////////
bool EventManager::readGeometry()
{
    std::stringstream ss;
    std::stringstream geometryTreeName;
    geometryTreeName << "Geometry";

    if(theGeometry_) delete theGeometry_;

    theGeometry_    = new Geometry() ;

    bool atLeastOneBranchFound = false ;

    if ( (TTree*)geoFile_->Get(geometryTreeName.str().c_str()) )
    {
        inputGeometryTree_ = (TTree*)geoFile_->Get(geometryTreeName.str().c_str());
        inputGeometryTree_->SetBranchAddress("GeometryBranch", &theGeometry_   );
        inputGeometryTree_->GetEntry(0);

        ss.str("");
        ss << "Found " << inputGeometryTree_->GetEntries() << " geometry entries";
        STDLINE(ss.str(),ACGreen) ;

        atLeastOneBranchFound = true ;
    }

    if( !atLeastOneBranchFound )
    {
        STDLINE("FATAL: no branches found in file. Aborting",ACRed) ;
        exit(0);
    }

    return atLeastOneBranchFound;
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::mkdir(std::string dirName)
{
    if(!calibPlotsFile_)
        return;

    TDirectory* dir = 0;
    if((dir = (TDirectory*)gDirectory->FindObjectAny(dirName.c_str())) == 0)
        dir = gDirectory->mkdir(dirName.c_str());
    dir->cd();
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::cd(std::string dirName)
{
    if(!calibPlotsFile_)
        return;
    if(dirName == "/")
        calibPlotsFile_->cd();
    else
        calibPlotsFile_->cd(dirName.c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::openCalibPlotsFile(std::string fileName)
{
    closeCalibPlotsFile();

    if(!theCalibrationManager_)
        theCalibrationManager_ = new CalibrationsManager(this);

    if(fileName != "")
    {
        calibPlotsFileName_ = fileName;
    }
    else if(calibPlotsFileName_ == "")
    {
        STDLINE("ERROR: Don't have an output file name set!",ACRed);
        return;
    }
    STDLINE("",ACWhite);
    STDLINE("Opening output file: " + fileName,ACYellow);
    calibPlotsFile_  = new TFile(calibPlotsFileName_.c_str(),"RECREATE");
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::closeCalibPlotsFile(void)
{
    if(calibPlotsFile_)
    {
        STDLINE("Closing out file", ACGreen);
        STDLINE("",ACWhite);
        calibPlotsFile_->Close();
        delete calibPlotsFile_;
        calibPlotsFile_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::writeCalibPlotsFile(void)
{
    if(calibPlotsFile_)
    {
        calibPlotsFile_->cd();
        calibPlotsFile_->Write();
    }

    closeCalibPlotsFile();
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::setCalibrationsFileName (std::string fileName)
{
    if(!theCalibrationManager_)
        theCalibrationManager_ = new CalibrationsManager(this);

    theCalibrationManager_->setInFileName(fileName);
}
///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::addCalibAnalysis(std::string calibAnalysis)
{
    for(unsigned int c=0; c<calibAnalysis_.size(); c++)
    {
        if(calibAnalysis_[c].find(calibAnalysis) != std::string::npos)
            return;
    }

    calibAnalysis_.push_back(calibAnalysis);
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::removeCalibAnalysis(std::string calibAnalysis)
{
    for(unsigned int c=0; c<calibAnalysis_.size(); c++)
    {
        if(calibAnalysis_[c].find(calibAnalysis) != std::string::npos)
        {
            calibAnalysis_.erase(calibAnalysis_.begin()+c);
            return;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::whichDut(std::string dutName)
{
    theCalibrationManager_->setDut(dutName);
}

///////////////////////////////////////////////////////////////////////////////////////////
void EventManager::startCalibrationManager(void)
{
    if(!theGeometry_)
    {
        STDLINE("Load the geometry file first!!", ACRed);
        return;
    }

    theCalibrationManager_->setWindow();

    for(unsigned int c=0; c<calibAnalysis_.size(); c++)
    {
        if(calibAnalysis_[c].find("parameters plots") != std::string::npos)
            theCalibrationManager_->makeParsPlots();
        else if(calibAnalysis_[c].find("dispersion plots") != std::string::npos)
            theCalibrationManager_->makeCalibHistos();
    }

    writeCalibPlotsFile();
}

///////////////////////////////////////////////////////////////////////////////////////////
int EventManager::getCompletionStatus(void)
{
    if(currentEntry_ > 0)
    {
        completionStatus_ = completionStatusBegin_ + (completionStatusEnd_-completionStatusBegin_) * (float)currentEntry_/eventsToProcess_;
/*
        std::stringstream ss;
        ss << "S: " << completionStatus_
           << " B: " << completionStatusBegin_
           << " B-E: " << (completionStatusEnd_-completionStatusBegin_)
           << " CE : " << currentEntry_
           << " TE: " << totalEventsToProcess_
           << " D: " << (float)currentEntry_/totalEventsToProcess_;

        STDLINE(ss.str(),ACGreen);
*/
    }
    return completionStatus_;
}

///////////////////////////////////////////////////////////////////////////////////////////
int EventManager::calculateTotalEvents(void)
{
    totalEvents_          = 0;
    totalEventsToProcess_ = 0;
    for(std::map<std::string,TFile*>::iterator it=inFilesList_.begin(); it!=inFilesList_.end(); it++)
    {
        openInFile    (it->first);
        readEventsTree(it->first);
        closeInFile   (it->first);
        totalEvents_ += entriesList_[it->first];
        eventsToProcessList_[it->first] = entriesList_[it->first];
        if(maxEvents_ > 0 && maxEvents_ < entriesList_[it->first])
            eventsToProcessList_[it->first] = maxEvents_;
        totalEventsToProcess_ += eventsToProcessList_[it->first];
    }
    return totalEvents_;
}
