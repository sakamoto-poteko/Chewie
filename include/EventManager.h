#ifndef _EventManager_h_
#define _EventManager_h_

#include "TTree.h"

#include <string>
#include <vector>
#include <map>

class TTree;
class TFile;
class CalibrationsManager;
class EventConverter;
class Geometry;
class TBranch;
class Event;
class EventHeader;
class XmlParser;

class EventManager
{
public:
    EventManager  (void);
    ~EventManager (void);

    void         setInFilesList          (std::vector<std::string> fileNames);
    void         addInFile               (std::string fileName      );
    void         openInFiles             (void                      );
    void         openInFile              (std::string fileName      );
    void         openOutFile             (std::string fileName      );
    bool         openGeoFile             (std::string fileName      );
    void         closeInFiles            (void                      );
    void         closeInFile             (std::string fileName      );
    void         closeGeoFile            (void                      );
    void         writeOutFile            (void                      );
    void         closeOutFile            (void                      );
    void         closeFilesList          (void                      );

    void         openCalibPlotsFile      (std::string fileName = "" );
    void         closeCalibPlotsFile     (void                      );
    void         writeCalibPlotsFile     (void                      );
    void         addCalibAnalysis        (std::string calibAnalysis );
    void         removeCalibAnalysis     (std::string calibAnalysis );
    void         startCalibrationManager (void                      );
    void         setCalibrationsFileName (std::string fileName      );
    void         setXmlParser            (XmlParser*  parser        ){theXmlParser_ = parser;}
    void         whichDut                (std::string dutName       );
    void         mkdir                   (std::string dirName       );
    void         cd                      (std::string dirName       );

    void         startConverter          (void                      );
    void         stopConverter           (void                      );

    void         setConfiguration        (XmlParser*  xmlParser     ){theXmlParser_ = xmlParser          ;}
    void         setNumberOfThreads      (int         nOfThreads    ){nOfThreads_ = nOfThreads           ;}
    void         setMaxEvents            (int         maxEvents     ){maxEvents_  = maxEvents            ;}

    XmlParser*   getXmlParser            (void                      ){return theXmlParser_               ;}
    std::string  getInRootFileName       (void                      ){return inRootFileName_             ;}
    std::string  getInGeoFileName        (void                      ){return inGeoFileName_              ;}
    std::string  getOutFileName          (void                      ){return outFileName_                ;}
    Event*       getEventPointer         (void                      ){return theEvent_                   ;}
    EventHeader* getEventHeaderPointer   (void                      ){return theEventHeader_             ;}
    Geometry*    getGeometryPointer      (void                      ){return theGeometry_                ;}
    std::string  getCurrentOperation     (void                      ){return currentOperation_           ;}
    int          getCurrentEntry         (void                      ){return currentEntry_               ;}
    int          getCompletionStatus     (void                      );
    int          getProcessedEvents      (void                      ){return totalProcessedEvents_       ;}
    int          getMaxEvents            (void                      ){return maxEvents_                  ;}
    int          getNumberOfThreads      (void                      ){return nOfThreads_                 ;}
    int          calculateTotalEvents    (void                      );
    int          getEntries              (void                      ){return theEventsTree_->GetEntries();}
    bool         isFinished              (void                      ){return isFinished_                 ;}
    int          getCurrentEvent         (Event&      event         );

    bool         readEventsTree          (std::string fileName      );
    bool         readGeometry            (void                      );

private:
    std::map<std::string,TFile*> inFilesList_          ;
    std::map<std::string,int>    entriesList_          ;
    std::map<std::string,int>    eventsToProcessList_  ;
    TFile*                       geoFile_              ;
    TFile*                       outFile_              ;
    TFile*                       calibPlotsFile_       ;
    TTree*                       theEventsTree_        ;
    TTree*                       inputGeometryTree_    ;
    TTree*                       inputEventHeader_     ;
    TBranch*                     theEventBranch_       ;
    Event*                       theEvent_             ;
    EventHeader*                 theEventHeader_       ;
    EventConverter*              theConverter_         ;
    CalibrationsManager*         theCalibrationManager_;
    Geometry*                    theGeometry_          ;
    std::string                  inRootFileName_       ;
    std::string                  inGeoFileName_        ;
    std::string                  outFileName_          ;
    std::string                  calibPlotsFileName_   ;
    std::vector<std::string>     calibAnalysis_        ;
    int                          totalEvents_          ;
    int                          totalEventsToProcess_ ;
    int                          totalProcessedEvents_ ;
    int                          eventsToProcess_      ;
    int                          maxEvents_            ;
    int                          nOfThreads_           ;
    XmlParser*                   theXmlParser_         ;
    std::string                  currentOperation_     ;
    bool                         abort_                ;
    bool                         isFinished_           ;
    int                          currentEntry_         ;
    int                          completionStatus_     ;
    int                          completionStatusBegin_;
    int                          completionStatusEnd_  ;
};

#endif
