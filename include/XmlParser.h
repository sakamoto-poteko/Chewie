/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <sstream>
#include <map>
#include <string>
#include <QDomNode>

class QDomElement ;
class QFile       ;
class QDomDocument;

class XmlConverter;
class XmlGeneral  ;
class XmlAnalysis ;
class XmlPlane    ;
class XmlScan     ;

class XmlParser
{
public:
    XmlParser (void);
    ~XmlParser(void);

    void                               destroy       (void            );
    bool                               parseDocument (QString fileName);
    void                               save          (QString fileName);

    QDomDocument*                                        getDocument   (void            ){return document_        ;}
    XmlConverter*                                        getConverter  (void            ){return theConverter_    ;}
    XmlGeneral*                                          getGeneral    (void            ){return theGeneral_      ;}
    XmlScan*                                             getScan       (void            ){return theScan_         ;}
    unsigned int                                         getNPlanes    (void            ){return thePlanes_.size();}
    QDomNodeList&                                        getPlanesNode (void            ){return planes_         ;}
    std::map<std::string,XmlPlane*>                      getPlanes     (void            ){return thePlanes_       ;}
    std::map< int, std::pair<std::string,XmlAnalysis*> > getAnalyses   (void            ){return theAnalyses_     ;}

    XmlAnalysis *                                        getAnalysesFromString (std::string analysisName);

private:
    QDomDocument*                                       document_    ;
    QDomNode                                            rootNode_    ;
    QDomNodeList                                        planes_      ;

    XmlConverter*                                       theConverter_;
    XmlGeneral*                                         theGeneral_  ;
    XmlScan*                                            theScan_     ;
    std::map<std::string,XmlPlane*>                     thePlanes_   ;
    std::map<int, std::pair<std::string,XmlAnalysis*> > theAnalyses_ ;

    std::stringstream                  ss_          ;
};

#endif // XMLPARSER_H
