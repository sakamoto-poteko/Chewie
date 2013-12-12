#ifndef __EventConverter_H__
#define __EventConverter_H__

#include "EventManager.h"
#include "Threader.h"
#include "PlanesMapping.h"
#include "Data.h"

#include <sstream>
#include <string>
#include <vector>

class Event;
class Data;
class TTree;
class Geometry;
class EventHeader;
class PlanesMapping;

//====================================================================
class EventConverter : public Threader
{
public:
    EventConverter  (EventManager* eventManager,
                     int nOfThreads);
    ~EventConverter (void          ){;}

    void        convert        (Event& event, int e );
    void        beginJob       (void                );
    void        endJob         (void                );
    int         runConverter   (void                );

    void        setRunNumber   (int r               ) {runNumber_ = r;}

private:
    int          execute       (int threadNumber);

    EventManager*       theEventManager_ ;
    EventHeader*        theHeader_       ;
    Geometry*           theGeometry_     ;
    TTree*              outTree_         ;
    Data                theData_         ;
    PlanesMapping       thePlanesMapping_;
    std::vector<Event>  eventVector_     ;
    int                 counter_         ;
    int                 runNumber_;
};

#endif
