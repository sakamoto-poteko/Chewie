#ifndef XMLANALYSIS_H
#define XMLANALYSIS_H

#include <QWidget>
#include <QDomNode>

#include "MessageTools.h"

#include <string>
#include <map>

class XmlAnalysis : public QWidget
{
    Q_OBJECT
public:
    XmlAnalysis (QDomNode&  node);
    ~XmlAnalysis(void           ){;}

    QDomNode&                          getNode          (void                ){return thisNode_;}
    QDomNode&                          getCutNode       (std::string cutType );
    std::string                        getCut           (std::string cutType );
    std::map<std::string,std::string>  getCutsList      (void                ){return cutsList_;}
    std::string                        getMinHits       (void                );
    std::pair<bool,bool>               isParToLimit     (int         parID   );
    std::pair<std::string,std::string> getParLimits     (int         parID   );
    std::pair<double,double>           getParLimits     (int parID, bool flag);
    int                                getThreashold    (void                );
    int                                getMaxCharge     (void                );
    int                                getMinTotCharge  (void                );
    int                                getMaxTotCharge  (void                );
    int                                getPriority      (void                );
    bool                               isToRun          (void                );
    bool                               applyStandardCuts(void                );

    void                               setCutsList      (void                );

private:
    QDomNode                          thisNode_;
    QDomNode                          cutNode_;
    std::map<std::string,std::string> cutsList_;

signals:
    
public slots:
    
};

#endif // XMLANALYSIS_H
