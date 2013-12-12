#include "AnalysisManager.h"
#include "Efficiency.h"
#include "Charge.h"
#include "Resolution.h"
#include "Tracks.h"
#include "TracksAfter.h"
#include "Data.h"
#include "WindowsManager.h"
#include "CalibrationsManager.h"
#include "HistogramWindow.h"
#include "MessageTools.h"
#include "ThreadUtilities.h"
#include "XmlGeneral.h"
#include "XmlPlane.h"
#include "XmlAnalysis.h"
#include "XmlParser.h"
#include "XmlScan.h"

#include <TTree.h>
#include <TTreeFormula.h>
#include <TFile.h>
#include <TROOT.h>

#include <QRegExp>

#include <iostream>
#include <sstream>
#include <algorithm>

struct Sorting {
    bool operator () (std::string A, std::string B) {AnalysisManager * a; return a->sorting(A,B); delete a;}
}sorter_;


///////////////////////////////////////////////////////////////////////////////////////////
AnalysisManager::AnalysisManager()://TTree* tree) :
    nOfThreads_           (0)
  , maxNOfThreads_        (0)
  , maxEvents_            (-1)
  , currentEntry_         (0)
  , outFileName_          ("")
  , outFile_              (0)
  , theXmlParser_         (0)
  , totalEvents_          (0)
  , totalEventsToProcess_ (0)
  , abort_                (false)
  , isFinished_           (true)
  , completionStatus_     (0)
  , completionStatusBegin_(0)
  , completionStatusEnd_  (100)
  , currentOperation_     ("")
{
    //if(tree)
    //    initializeTree();
}

///////////////////////////////////////////////////////////////////////////////////////////
AnalysisManager::~AnalysisManager(void)
{
    STDLINE("Destructor",ACRed);
    stopSequence();
    clearFilesList();
    resetAnalyses();
}

///////////////////////////////////////////////////////////////////////////////////////////
int AnalysisManager::initializeTree(std::string fileName)
{
    openInFile(fileName);
    inFilesList_[fileName]->cd();
    if(treeFilesList_[fileName] == 0)
    {
        treeFilesList_[fileName] = (TTree*)inFilesList_[fileName]->Get("CaptanTrack");
        if(!treeFilesList_[fileName])
        {
            std::string msg = "ERROR: Can't find tree CaptanTrack in file " + fileName;
            FATAL(msg,ACRed);
            exit(EXIT_FAILURE);
        }
        int threadNumber = fileNameToTreePos_[fileName].first;
        int treeNumber   = fileNameToTreePos_[fileName].second;
        threadedTrees_ [threadNumber][treeNumber] = treeFilesList_[fileName];
        if(threadedData_.find(threadNumber) == threadedData_.end()
                && threadedData_[threadNumber].find(treeNumber) == threadedData_[threadNumber].end())
            threadedData_  [threadNumber][treeNumber] = Data();

        threadedData_  [threadNumber][treeNumber].setBranchAddress(treeFilesList_[fileName]);
        threadedCurrentEntries_[threadNumber][treeNumber] = 0     ;
    }

    return treeFilesList_[fileName]->GetEntries();
}

///////////////////////////////////////////////////////////////////////////////////////////
int AnalysisManager::initializeTrees(void)
{
    int entries = 0;
    for(std::map<std::string,TFile*>::iterator fileIt=inFilesList_.begin(); fileIt!=inFilesList_.end(); fileIt++)
        entries += initializeTree(fileIt->first);

    return entries;
}

///////////////////////////////////////////////////////////////////////////////////////////
int AnalysisManager::calculateTotalEvents()
{
    totalEvents_ = initializeTrees();
    return totalEvents_;
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::setInFilesList(std::vector<std::string> fileList)
{
    clearFilesList();
    for(std::vector<std::string>::iterator itFileName=fileList.begin(); itFileName!=fileList.end(); itFileName++)
    {
        inFilesList_  [*itFileName] = 0;
        treeFilesList_[*itFileName] = 0;
    }
    setFileNameToTreePos();
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::addInFile(std::string fileName)
{
    if(inFilesList_.find(fileName) == inFilesList_.end())
    {
        inFilesList_  [fileName] = 0;
        treeFilesList_[fileName] = 0;
        setFileNameToTreePos();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::setFileNameToTreePos(void)
{
    if(maxNOfThreads_ < 1 || inFilesList_.size() <= maxNOfThreads_)
        nOfThreads_ = inFilesList_.size();
    else
        nOfThreads_ = maxNOfThreads_;

    fileNameToTreePos_.clear();
    int fNumber = 0;
    for(std::map<std::string,TFile*>::iterator it=inFilesList_.begin(); it != inFilesList_.end(); it++,fNumber++)
    {
        fileNameToTreePos_[it->first] = std::make_pair<int,int>(fNumber%nOfThreads_,fNumber/nOfThreads_);
    }
    currentTree_.clear();
    currentTree_.resize(nOfThreads_,0);
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::clearFilesList(void)
{
    closeInFiles();
    inFilesList_  .clear();
    treeFilesList_.clear();//Don't need to be deleted because it is taken care when the file closes
    totalEvents_ = 0;
    nOfThreads_  = 0;
    clearThreadedVariables();
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::clearThreadedVariables(void)
{
    for(std::map<std::string,TTree*>::iterator it=treeFilesList_.begin(); it!=treeFilesList_.end(); it++)
        it->second = 0;

    threadedTrees_         .clear();
    threadedData_          .clear();
    threadedCurrentEntries_.clear();
    threadedMaxEntries_    .clear();
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::resetThreadedEntries(void)
{
    for(std::map<int, std::map<int, int> >::iterator it=threadedCurrentEntries_.begin(); it!=threadedCurrentEntries_.end(); it++)
        for(std::map<int, int>::iterator itt=it->second.begin(); itt!=it->second.end(); itt++)
            itt->second = 0;
    for(std::vector<unsigned int>::iterator it=currentTree_.begin(); it!=currentTree_.end(); it++)
        *it = 0;

}
///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::openInFile(std::string fileName)
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
void AnalysisManager::openInFiles(void)
{
    for(std::map<std::string,TFile*>::iterator fileIt=inFilesList_.begin(); fileIt!=inFilesList_.end(); fileIt++)
        openInFile(fileIt->first);
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::closeInFile(std::string fileName)
{
    if(inFilesList_.find(fileName) != inFilesList_.end() && inFilesList_[fileName] != 0 && inFilesList_[fileName]->IsOpen())
    {
        delete inFilesList_[fileName];//Automatically closes the file
        inFilesList_  [fileName] = 0;
        treeFilesList_[fileName] = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::closeInFiles(void)
{
    for(std::map<std::string,TFile*>::iterator fileIt=inFilesList_.begin(); fileIt!=inFilesList_.end(); fileIt++)
        closeInFile(fileIt->first);
}

///////////////////////////////////////////////////////////////////////////////////////////
int AnalysisManager::openOutFile(std::string fileName, std::string mode)
{
    closeOutFile();

    if(fileName != "")
    {
        outFileName_ = fileName;
    }
    else if(outFileName_ == "")
    {
        STDLINE("ERROR: Don't have an output file name set!",ACRed);
        return 0;
    }
    outFile_  = TFile::Open(outFileName_.c_str(),mode.c_str());
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::closeOutFile(void)
{
    if(outFile_)
    {
        STDLINE("Closing out file", ACGreen);
        delete outFile_;
        outFile_ = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
bool AnalysisManager::isOutFileOpen(void)
{
    return (bool)outFile_;
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::writeOutFile(void)
{
    if(outFile_)
    {
        outFile_->cd();
        outFile_->Write("",TObject::kOverwrite);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::cd(std::string dirName)
{
    if(!outFile_)
        return;
    if(dirName == "/")
        outFile_->cd();
    else
        outFile_->cd(dirName.c_str());
}


///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::mkdir(std::string dirName)
{
    if(!outFile_)
        return;

    TDirectory* dir = 0;
    if((dir = (TDirectory*)gDirectory->FindObjectAny(dirName.c_str())) == 0)
        dir = gDirectory->mkdir(dirName.c_str());
    dir->cd();
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::setupCalibrations(int dut)
{
    if(!theCalibrationsManager_)
        theCalibrationsManager_ = new CalibrationsManager();

    std::stringstream whichDut;
    whichDut.str(""); whichDut << "Dut" << dut;
    std::string fileName = (theXmlParser_->getPlanes())[whichDut.str()]->getCalibPlotsFile();
    theCalibrationsManager_->openCalibPlotsFile(fileName);
}

///////////////////////////////////////////////////////////////////////////////////////////
Analysis* AnalysisManager::addAnalysis(int analysisPriority, std::string analysisName)
{
    if(analyses_.find(analysisPriority) == analyses_.end())
    {
        analyses_[analysisPriority].first = analysisName;

        if(analysisName == "Charge")
            analyses_[analysisPriority].second = new Charge      (this,nOfThreads_);
        else if(analysisName == "Efficiency")
            analyses_[analysisPriority].second = new Efficiency  (this,nOfThreads_);
        else if(analysisName == "Resolution")
            analyses_[analysisPriority].second = new Resolution  (this,nOfThreads_);
        else if(analysisName == "Tracks")
            analyses_[analysisPriority].second = new Tracks      (this,nOfThreads_);
        else if(analysisName == "TracksAfter")
            analyses_[analysisPriority].second = new TracksAfter (this,nOfThreads_);
        else if(analysisName == "Windows")
            analyses_[analysisPriority].second = new WindowsManager(this,nOfThreads_);
//            analysesOrder_.insert(analysesOrder_.begin(),analysisName);
        else
        {
            FATAL(std::string("Analysis name ") + analysisName + " not recognized",ACRed);
            exit(EXIT_FAILURE);
        }
//        if(analysisName != "Windows") {
//            if (analysisName )
            analysesOrder_.push_back(analysisName);
//        }
    }
    return analyses_[analysisPriority].second;
}
///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::removeAnalysis(std::string analysisName)
{
    int pos = findAnalysis(analysisName);
    if(pos >= 0)
    {
        delete analyses_[pos].second;
        analyses_.erase(pos);
        for(std::vector<std::string>::iterator it=analysesOrder_.begin(); it!=analysesOrder_.end(); it++)
            if(*it == analysisName)
            {
                analysesOrder_.erase(it);
                break;
            }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::resetAnalyses(void)
{
    for(std::map<int, std::pair<std::string,Analysis*> >::iterator it=analyses_.begin(); it != analyses_.end(); it++)
        delete it->second.second;

    analyses_     .clear();
    analysesOrder_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::setListOfRun (Analysis * analysis)
{
    QString runNumberString;
    int runNumber = 0;
    QRegExp regN ("Run(\\d+)\\w+");
    for (std::map<std::string, TFile*>::iterator it = inFilesList_.begin(); it != inFilesList_.end(); ++it)
    {
//        runNumberString = (it->first).substr(it->first.find_last_of('/')+4,it->first.size()-it->first.find_last_of('/')-26);
//        runNumber = atoi(runNumberString.c_str());
        runNumberString = (QString)(it->first.c_str());
        if (regN.indexIn(runNumberString) >= 0) runNumber = regN.cap(1).toInt();
//        std::cout << "RUN NUMBER: " << runNumber << std::endl;
        analysis->pushBackrunNumber(runNumber);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::startSequence(void)
{
    if(inFilesList_.size()==0 || (theXmlParser_->getDocument())==0)
        return;

    currentOperation_ = "Running analyses";
    STDLINE(currentOperation_,ACRed);
    abort_            = false;
    isFinished_       = false;
    currentEntry_     = 0;
    completionStatus_ = 0;
    currentEntry_     = 0;

    resetAnalyses();
    openOutFile();

    setMaxNumberOfThreads(atoi(theXmlParser_->getGeneral()->getNumberOfThreads().c_str()));
    setThreadEvents();

    if(abort_) return;

    if((theXmlParser_->getPlanes())["Dut0"]->useCalibrations())
    {
        STDLINE("Setting calibrations for Dut0...",ACRed);
        setupCalibrations(0);
        STDLINE("Done!",ACGreen);
    }
    else if((theXmlParser_->getPlanes())["Dut1"]->useCalibrations())
    {
        STDLINE("Setting calibrations for Dut1...",ACRed);
        setupCalibrations(1);
        STDLINE("Done!",ACGreen);
    }
    else
        STDLINE("Not using calibrations!",ACRed);

    std::map<int, std::pair<std::string,XmlAnalysis*> > analyses = theXmlParser_->getAnalyses();
    int totAnalysesToRun = 0;
    for(std::map<int, std::pair<std::string, XmlAnalysis*> >::iterator iy = analyses.begin(); iy != analyses.end(); ++iy)
    {
        if(iy->second.second->isToRun())
        {
            std::string analName = iy->second.first;
            int it = iy->second.second->getPriority();
            addAnalysis(it, analName);
            analyses[it].second->setCutsList();
            ++totAnalysesToRun;

            std::stringstream ss;
            ss << it;
            STDLINE("Adding " + analName + " analysis with priority " + ss.str(), ACWhite);
        }
    }

    std::sort(analysesOrder_.begin(), analysesOrder_.end(), sorter_);

    completionStatus_ = 1;
    if(abort_) return;

    //initializeTrees();//Done already in setThreadEvents()

    completionStatus_ = 2;
    if(abort_) return;

    currentOperation_ = "Begin jobs";
    STDLINE(currentOperation_, ACRed);

    for (int it = 0; it < totAnalysesToRun; ++it)
    {
        setCutsFormula(analyses[it].first,analyses[it].second->getCutsList());
    }
    completionStatus_ = 3;
    float completionRunning = 95/analysesOrder_.size();
    float n = 0;

    for (int it = 0; it < totAnalysesToRun; ++it)
    {
        analyses_[it].second->setDoNotDelete(true);//Because the out file is open and it will delete the objects when closed

        setListOfRun(analyses_[it].second);

        if (analyses[it].first.find("After") != std::string::npos)
        {
            writeOutFile();
            analyses_[it].second->getInFile(outFile_);
        }

        currentOperation_ = "Begin " + analyses_[it].first + "jobs";
        STDLINE(currentOperation_, ACRed);

        if (!outFile_) STDLINE("Where the hell is the output file??", ACRed);
        outFile_->cd();
        analyses_[it].second->beginJob();

        currentOperation_ += " done!";
        STDLINE(currentOperation_, ACGreen);

        if(abort_) return;
        currentOperation_ += " done!";
        STDLINE(currentOperation_, ACGreen);

        completionStatus_      = 3 + completionRunning*n + 1;
        if(abort_) return;
        completionStatusBegin_ = completionStatusEnd_ = completionStatus_;

        if(abort_) return;
        currentOperation_ = "Running " + analyses_[it].first + " analysis";
        STDLINE(currentOperation_, ACRed);

        currentEntry_ = 0;
        completionStatusBegin_ = completionStatusEnd_;
        completionStatusEnd_ += completionRunning -1;
        resetThreadedEntries();
        outFile_->cd();
        analyses_[it].second->runAnalysis();
        if(abort_) return;
        currentOperation_ = analyses_[it].first + " analysis done!";
        STDLINE(currentOperation_, ACGreen);

        if(abort_) return;
        currentOperation_ = "End " + analyses_[it].first + " jobs";
        STDLINE(currentOperation_, ACRed);

        outFile_->cd();
        analyses_[it].second->endJob();

        currentOperation_ += " done!";
        STDLINE(currentOperation_, ACGreen);

        n += 1;
        if(abort_) return;
    }
    closeInFiles();

    completionStatus_ = completionStatusEnd_;
    if(abort_) return;
    currentOperation_ += " done!";
    STDLINE(currentOperation_, ACGreen);

    if(abort_) return;
    currentOperation_ = "Writing file " + outFileName_;
    STDLINE(currentOperation_, ACRed);
    writeOutFile();
    //closeOutFile();
    resetAnalyses();
    completionStatus_ = 99;
    if(abort_) return;
    currentOperation_ += " done!";
    STDLINE(currentOperation_,ACGreen);

    isFinished_ = true;
    currentOperation_ = "Analyses completed!";
    STDLINE(currentOperation_,ACGreen);
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::stopSequence(void)
{
    //STDLINE("Aborting sequence!",ACRed);
    abort_ = true;
    if(!isFinished_)
        for(int it = 0; it < (int)analyses_.size(); ++it)
        {
            analyses_[it].second->stopThreads();
            outFile_->cd();
            analyses_[it].second->endJob();
        }

//    writeOutFile();
//    closeOutFile();
    resetAnalyses();
}

///////////////////////////////////////////////////////////////////////////////////////////
Data& AnalysisManager::getCurrentData(int* entry, unsigned int threadNumber)
{
    if(threadedCurrentEntries_[threadNumber][currentTree_[threadNumber]] >= threadedMaxEntries_[threadNumber][currentTree_[threadNumber]])
    {
        if(currentTree_[threadNumber] < threadedTrees_[threadNumber].size()-1)
            ++currentTree_[threadNumber];
        else
        {
           *entry = -1;
            return threadedData_[threadNumber][currentTree_[threadNumber]];//It is not a valid data!
        }
    }

    *entry = threadedCurrentEntries_[threadNumber][currentTree_[threadNumber]]++;
    threadedTrees_[threadNumber][currentTree_[threadNumber]]->GetEntry(*entry);
    return threadedData_[threadNumber][currentTree_[threadNumber]];
}

///////////////////////////////////////////////////////////////////////////////////////////
int AnalysisManager::getCurrentEntry(void)
{
    int entry = 0;
    for(std::map<int, std::map<int, int> >::iterator it=threadedCurrentEntries_.begin(); it!=threadedCurrentEntries_.end(); it++)
        for(std::map<int, int>::iterator itt=it->second.begin(); itt!=it->second.end(); itt++)
            entry += itt->second;
    currentEntry_ = entry;
    return currentEntry_;
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::setCutsFormula(std::string analysisName,std::map<std::string,std::string> cutsList)
{
    int pos = findAnalysis(analysisName);
    if(pos >= 0)
    {
        std::vector<TTree*> treesVector;
        for(std::map<std::string, TTree*>::iterator it=treeFilesList_.begin(); it!=treeFilesList_.end(); it++)
            treesVector.push_back(it->second);
        analyses_[pos].second->setCutsFormula(cutsList,treesVector);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::setMaxNumberOfThreads(int value)
{
    maxNOfThreads_ = value;
    clearThreadedVariables();
    setFileNameToTreePos();
}

///////////////////////////////////////////////////////////////////////////////////////////
void AnalysisManager::setThreadEvents(void)
{
    totalEventsToProcess_ = 0;
    if(inFilesList_.size() != 0)
    {
        initializeTrees();
        for(std::map<int, std::map<int, TTree*> >::iterator it=threadedTrees_.begin(); it!=threadedTrees_.end(); it++)
            for(std::map<int, TTree*>::iterator itt=it->second.begin(); itt!=it->second.end(); itt++)
            {
                if(maxEvents_ <= 0)
                    threadedMaxEntries_[it->first][itt->first] = itt->second->GetEntries();
                else
                {
                    threadedMaxEntries_[it->first][itt->first] = maxEvents_/inFilesList_.size();
                    if(it == threadedTrees_.begin() && itt == it->second.begin())
                        threadedMaxEntries_[it->first][itt->first] += maxEvents_%inFilesList_.size();
                    if(threadedMaxEntries_[it->first][itt->first] > itt->second->GetEntries())
                        threadedMaxEntries_[it->first][itt->first] = itt->second->GetEntries();
                }
                totalEventsToProcess_ += threadedMaxEntries_[it->first][itt->first];
            }

    }
}

///////////////////////////////////////////////////////////////////////////////////////////
int AnalysisManager::getCompletionStatus(void)
{
    if(getCurrentEntry() > 0)
    {
        completionStatus_ = completionStatusBegin_ + (completionStatusEnd_-completionStatusBegin_) * (float)currentEntry_/totalEventsToProcess_;
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
const WindowsManager* AnalysisManager::getWindowsManager(void)
{
    std::stringstream ss;
    ss << findAnalysis("Windows");
    if(analyses_.find(findAnalysis("Windows")) != analyses_.end())
        return (WindowsManager*)analyses_[findAnalysis("Windows")].second;   // Windows should be always Number 0
    FATAL("Windows Manager doesn't exist!",ACRed);
    return 0;
}

bool AnalysisManager::sorting (std::string a, std::string b)
{
    if (a == "Windows" && b != "Windows") return true;
    else if (b == "Windows" && a != "Windows") return false;
    else if (a != "Windows" && b != "Windows") {
        if (a.find("After") != std::string::npos && b.find("After") == std::string::npos) return false;
        else if (b.find("After") != std::string::npos && a.find("After") == std::string::npos) return true;
        else return true;
    }
    else return true;
}

int AnalysisManager::findAnalysis (std::string analysisName)
{
    int pos = -1;
    for (std::map<int, std::pair<std::string, Analysis*> >::iterator it = analyses_.begin(); it != analyses_.end(); ++it)
    {
        if (it->second.first == analysisName) pos = it->first;
    }
    if (pos == -1) STDLINE("Cannot find " + analysisName + " among analyses!", ACRed);
    return pos;
}

std::string AnalysisManager::getPriorityFromUi (std::string analysisName, int newPriority)
{
    int tmp = findAnalysis(analysisName);

    if (tmp == newPriority) return "";  // if nothing changed

    Analysis * temp = analyses_[newPriority].second;
    std::string replaced = analyses_[newPriority].first;
    analyses_[newPriority] = analyses_[tmp];
    analyses_[tmp].second = temp;
    analyses_[tmp].first = replaced;
    delete temp;

    return "Switch between " + analysisName + " and " + replaced + " done!";

}
