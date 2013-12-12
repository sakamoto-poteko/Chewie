#include "Analysis.h"
#include "AnalysisManager.h"
#include "Data.h"
#include "MessageTools.h"

#include <TThread.h>

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////
Analysis::Analysis(AnalysisManager* analysisManager, int nOfThreads) : Threader(nOfThreads)
  , theAnalysisManager_(analysisManager)
  , fDoNotDelete_(false)
{
}

///////////////////////////////////////////////////////////////////////////////////////////
Analysis::~Analysis()
{
    stopThreads();
}

///////////////////////////////////////////////////////////////////////////////////////////
int Analysis::execute(int threadNumber)
{
    int entry;
    Data& data = theAnalysisManager_->getCurrentData(&entry,threadNumber);
    if(entry >= 0)
        analyze(data,threadNumber);

    return entry;
}

///////////////////////////////////////////////////////////////////////////////////////////
int Analysis::runAnalysis()
{
    startThreads();
//    STDLINE("Waiting for threads to be done...",ACPurple);
    join();
//    STDLINE("Done with threads, stopping them...",ACPurple);
    stopThreads();
    return 0;
}
