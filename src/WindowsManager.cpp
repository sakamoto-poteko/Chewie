#include "WindowsManager.h"
#include "PlanesMapping.h"
#include "Window.h"
#include "AnalysisManager.h"
#include "HistogramWindow.h"
#include "Data.h"
#include "MessageTools.h"
#include "XmlParser.h"
#include "XmlPlane.h"
#include "XmlWindow.h"

#include <TTree.h>
#include <TThread.h>
#include <TROOT.h>

#include <sstream>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WindowsManager::WindowsManager(AnalysisManager* analysisManager, int nOfThreads) :
    Analysis            (analysisManager,
                         nOfThreads      )
  ,theAnalysisManager_ (analysisManager    )
  ,thePlanesMapping_   (new PlanesMapping())
  ,theXmlParser_       (0                  )
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WindowsManager::~WindowsManager()
{
    destroy();
    delete thePlanesMapping_;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowsManager::destroy(void)
{
    for(std::vector<Window*>::iterator it=windows_.begin(); it!=windows_.end(); it++)
    {
        (*it)->setDoNotDelete(Analysis::fDoNotDelete_);
        delete *it;
    }
    windows_.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowsManager::beginJob(void)
{
    theXmlParser_      = theAnalysisManager_->getXmlParser();
    destroy();
    theAnalysisManager_->mkdir("Windows");
    for(unsigned int w=0; w<thePlanesMapping_->getNumberOfPlanes(); w++)
    {
        int xChipsNumber = atoi((((theXmlParser_->getPlanes())[thePlanesMapping_->getPlaneName(w)]->getChipsNumber()).first).c_str());
        int yChipsNumber = atoi((((theXmlParser_->getPlanes())[thePlanesMapping_->getPlaneName(w)]->getChipsNumber()).second).c_str());
        int nBinsX = xChipsNumber*52;
        int nBinsY = yChipsNumber*80;
        windows_.push_back(new HistogramWindow(thePlanesMapping_->getPlaneName(w),nBinsX,nBinsY));
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowsManager::analyze(const Data& data, int )//threadNumber)//WARNING: You can't change this name (threadNumber) or the MACRO THREAD won't compile
{
    int lowerCol, higherCol, lowerRow, higherRow;

    for(unsigned int w=0; w<thePlanesMapping_->getNumberOfPlanes(); w++)
    {
        lowerCol  = atoi(((theXmlParser_->getPlanes())[thePlanesMapping_->getPlaneName(w)]->getWindow()->getLowerCol()).c_str());
        higherCol = atoi(((theXmlParser_->getPlanes())[thePlanesMapping_->getPlaneName(w)]->getWindow()->getHigherCol()).c_str());
        lowerRow  = atoi(((theXmlParser_->getPlanes())[thePlanesMapping_->getPlaneName(w)]->getWindow()->getLowerRow()).c_str());
        higherRow = atoi(((theXmlParser_->getPlanes())[thePlanesMapping_->getPlaneName(w)]->getWindow()->getHigherRow()).c_str());
        windows_[w]->calculateWindow(w,data,lowerCol,higherCol,lowerRow,higherRow);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowsManager::endJob(void)
{
    for(unsigned int w=0; w<thePlanesMapping_->getNumberOfPlanes(); w++)
    {
        theXmlParser_->getPlanes()[thePlanesMapping_->getPlaneName(w)]->getWindow()->setRemovedPixels();
        std::vector< std::map<std::string,int> > removedPixels = theXmlParser_->getPlanes()[thePlanesMapping_->getPlaneName(w)]->getWindow()->getRemovedPixels();
        for(unsigned int p=0; p<removedPixels.size(); p++)
        {
            windows_[w]->removePixel(removedPixels[p]["col"],removedPixels[p]["row"]);
        }
        windows_[w]->sumThreaded();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Window* WindowsManager::getWindow(std::string plane) const
{
    if(thePlanesMapping_->getPlanePosition(plane) >= 0)
        return windows_[thePlanesMapping_->getPlanePosition(plane)];
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const Window* WindowsManager::getWindow(int plane) const
{
    if(plane < (int)windows_.size())
        return windows_[plane];
    return 0;
}
