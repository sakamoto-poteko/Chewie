#ifndef WINDOWSMANAGER_H
#define WINDOWSMANAGER_H

#include "Analysis.h"

#include <string>
#include <vector>
#include <iostream>


class Window         ;
class PlanesMapping  ;
class AnalysisManager;
class XmlParser      ;

class WindowsManager : public Analysis
{
public:
    WindowsManager (AnalysisManager* analysisManager = 0, int nOfThreads = 1);
    ~WindowsManager(void                            );

    void beginJob            (void                              );
    void analyze             (const Data& data, int threadNumber);
    void endJob              (void                              );
    virtual bool passStandardCuts (int , const Data&            ){return true;}

    void getInFile           (TFile *                           ){}

    void init                (std::string type, int maxEntries = -1);
    void destroy             (void               );

    const Window* getWindow  (std::string plane  ) const;
    const Window* getWindow  (int plane          ) const;

private:

    std::vector<Window*> windows_           ;
    AnalysisManager*     theAnalysisManager_;
    PlanesMapping*       thePlanesMapping_  ;
    XmlParser*           theXmlParser_      ;
};

#endif // WINDOWSMANAGER_H
