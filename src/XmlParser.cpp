/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/
#include <assert.h>

#include <QTextStream>
#include <QFile>

#include "MessageTools.h"
#include "XmlConverter.h"
#include "XmlParser.h"
#include "XmlAnalysis.h"
#include "XmlGeneral.h"
#include "XmlPlane.h"
#include "XmlScan.h"

//================================================================================
XmlParser::XmlParser(void) :
    document_    (0)
  , theConverter_(0)
  , theGeneral_  (0)
  , theScan_     (0)
{
}

//================================================================================
XmlParser::~XmlParser()
{
    destroy();
}

//================================================================================
void XmlParser::destroy(void)
{
    for(std::map<int, std::pair<std::string,XmlAnalysis*> >::iterator it=theAnalyses_.begin(); it!=theAnalyses_.end(); it++)
    {
        delete it->second.second;
    }
    theAnalyses_.clear();

    for(std::map<std::string,XmlPlane*>::iterator it=thePlanes_.begin(); it != thePlanes_.end(); it++)
    {
        delete it->second;
    }
    thePlanes_.clear();

    if(theGeneral_)
    {
        delete theGeneral_;
        theGeneral_ = 0;
    }

    if(document_)
    {
        delete document_ ;
        document_ = 0;
    }

    if(theConverter_)
    {
        delete theConverter_;
        theConverter_ = 0;
    }

    if(theScan_)
    {
        delete theScan_;
        theScan_ = 0;
    }
}
//================================================================================
bool XmlParser::parseDocument(QString fileName)
{
    destroy();

    STDLINE("",ACWhite);
    document_ = new QDomDocument( "ConfigurationFile" );
    STDLINE("",ACWhite);
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        STDLINE(std::string("Could not open ")+file.fileName().toStdString(),ACRed);
        return false;
    }

    QString errMsg = "";
    int line;
    int col;
    if (!document_->setContent( &file, true , &errMsg, &line, &col))
    {
        STDLINE(std::string("Could not access ")+file.fileName().toStdString(),ACRed);
        ss_ << "Error: " << errMsg.toStdString() << " line: " << line << " col: " << col;
        STDLINE(ss_.str(),ACGreen);
        file.close();
        return false;
    }

    STDLINE(std::string("Parsing ")+file.fileName().toStdString(),ACGreen);
    STDLINE("",ACWhite);
    STDLINE("",ACWhite);

    rootNode_ = document_->elementsByTagName("ChewieConfiguration").at(0);

    QDomNode converter = document_->elementsByTagName("Converter").at(0);
    theConverter_ = new XmlConverter(converter);

    QDomNode general = document_->elementsByTagName("General").at(0);
    theGeneral_ = new XmlGeneral(general);

    planes_ = document_->elementsByTagName("Plane");

    for(int p=0; p<planes_.size(); ++p)
    {
        QDomNode planeNode    = planes_.at(p);
        std::string planeName = planeNode.toElement().attribute("Id").toStdString();
        thePlanes_[planeName] = new XmlPlane(planeNode);
    }

    QDomNodeList analyses = document_->elementsByTagName("Analysis");

    for(int a=0; a<analyses.size(); ++a)
    {
        QDomNode analysisNode = analyses.at(a);
        std::string type      = analysisNode.toElement().attribute("Type").toStdString();
        XmlAnalysis * newAnalysis = new XmlAnalysis(analysisNode);
        int priority              = newAnalysis->getPriority();
        theAnalyses_[priority]    = std::make_pair<std::string, XmlAnalysis*>(type, newAnalysis);
    }

    QDomNode scan = document_->elementsByTagName("Scan").at(0);
    theScan_ = new XmlScan(scan);
    file.close();
    return true;
}

XmlAnalysis * XmlParser::getAnalysesFromString (std::string analysisName)
{
    int pos = -1;
    for (int i = 0; i < (int)theAnalyses_.size(); ++i)
    {
        if (theAnalyses_[i].first == analysisName) pos = i;
    }
    if (pos >= 0) return theAnalyses_[pos].second;
    else {
        STDLINE("Cannot find " + analysisName + " analysis!", ACRed);
        return 0;
    }
}


//================================================================================
void XmlParser::save(QString fileName)
{
  QFile file(fileName);
  if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) )
  {
    QTextStream stream( &file );
    document_->save( stream, 2 ) ;
    file.close();
    ss_.str("") ;
    ss_  << ACCyan << ACBold
        << "File "
        << ACPlain << ACYellow << ACBold
        << fileName.toStdString()
        << ACPlain << ACCyan << ACBold
        << " saved"
        << ACPlain ;
    STDLINE(ss_.str(),ACWhite) ;
  }
}
