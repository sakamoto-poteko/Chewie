#ifndef _Analysis_h_
#define _Analysis_h_ 

#include "Threader.h"

#include <vector>
#include <string>
#include <map>

class AnalysisManager;
class TFile;
class TTree;
class Data;

class Analysis : public Threader
{
public:
    Analysis (AnalysisManager* analysisManager,
              int nOfThreads = 1               );
    ~Analysis(void                             );

    virtual void beginJob         (void                                                 ){}
    virtual void analyze          (const Data&, int                                     ){}
    virtual void endJob           (void                                                 ){}
    virtual int  runAnalysis      (void                                                 );

    virtual void setCutsFormula   (std::map<std::string,std::string>,std::vector<TTree*>){}
    virtual bool passStandardCuts (int , const Data&                                    ) = 0;
            void setDoNotDelete   (bool flag                                            ){fDoNotDelete_ = flag;}
    virtual void getInFile        (TFile *                                              ){}

            void pushBackrunNumber (int runNumber) {fRunNumbers_.push_back(runNumber);}

protected:
    AnalysisManager* theAnalysisManager_;
    bool             fDoNotDelete_;
    std::vector<int> fRunNumbers_;

private:
    int execute(int threadNumber);
};

#endif
