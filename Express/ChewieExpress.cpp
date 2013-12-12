#include "MessageTools.h"
#include "ANSIColors.h"
#include "XmlParser.h"
#include "XmlConverter.h"
#include "EventManager.h"
#include "AnalysisManager.h"
//#include "trackFinder.h"
//#include "HManager.h"
#include <TApplication.h>
#include <QApplication>

#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <QDomDocument>
//#include <QDomImplementation>
//#include <QDomDocumentType>
#include <QFile>
#include <QString>
#include <QDomNode>

using namespace std;

class XmlDefaults;
class XmlFiles;
class XmlFile;

class ExpressXmlParser
{
public:
    ExpressXmlParser (void);
    ~ExpressXmlParser(void);

    void parseDocument(std::string fileName);

    XmlDefaults*           getDefaults (void){return theDefaults_ ;}
    std::vector<XmlFiles*> getFilesList(void){return theFilesList_;}

private:
    QDomDocument*  document_;
    QDomNode       rootNode_;

    XmlDefaults*           theDefaults_;
    std::vector<XmlFiles*> theFilesList_;
    stringstream ss_;
};

class XmlDefaults
{
public:
    XmlDefaults (QDomNode& node);
    ~XmlDefaults(void){;}
    QDomNode&   getNode(void){return thisNode_;}
    std::string filesPath_;
    bool        convert_;
    bool        runAnalysis_;
    int         numberOfEvents_;     

private:
    QDomNode    thisNode_;
};

class XmlFiles
{
public:
    XmlFiles (QDomNode& node);
    ~XmlFiles(void){;}
    QDomNode&   getNode(void){return thisNode_;}
    vector<XmlFile*> fileNames_;
    std::string configurationName_;
    std::string outFileName_;

private:
    QDomNode    thisNode_;
};

class XmlFile
{
public:
    XmlFile (QDomNode& node);
    ~XmlFile(void){;}
    QDomNode&   getNode(void){return thisNode_;}
    std::string fileName_;

private:
    QDomNode    thisNode_;
};

int main(int argc, char** argv)
{
  stringstream ss;
  TApplication tApp("App",&argc,argv);
  QApplication app(argc, argv);
  ExpressXmlParser theExpressXmlParser;
  std::string configFileName = "./xml/ExpressConfiguration.xml";
//  std::cout << argc << " " << argv[1] << std::endl;
//  return 1;
  if(argc == 2)
    configFileName = std::string("./xml/") + argv[1];
  else if(argc > 2)
  {
    ss.str("");
    ss << "Usage: ./ChewieExpress optional(configuration file)";
    STDLINE(ss.str(),ACRed);
    exit(EXIT_SUCCESS);
  }
  ss.str("");
  ss << "Using: " << configFileName << " configuration.";
  STDLINE(ss.str(),ACGreen);
  
  theExpressXmlParser.parseDocument(configFileName.c_str());

  const string filesPath = theExpressXmlParser.getDefaults()->filesPath_     ;
  bool   convert         = theExpressXmlParser.getDefaults()->convert_       ;
  bool   runAnalysis     = theExpressXmlParser.getDefaults()->runAnalysis_   ;
  int    numberOfEvents  = theExpressXmlParser.getDefaults()->numberOfEvents_;
  XmlParser* theChewieXmlParser     = new XmlParser();    
  string chewieXmlDir    = getenv("CHEWIEXMLDIR");
  string chewieDataDir   = getenv("CHEWIEDATADIR");
  string chewieOutputDir = getenv("CHEWIEOUTPUTDIR");
  string chewieInputDir  = string(getenv("CHEWIEINPUTDIR")) + "/";

  for(unsigned int fs=0; fs<theExpressXmlParser.getFilesList().size(); fs++)
  {
    string configurationName = chewieXmlDir + "/" + theExpressXmlParser.getFilesList()[fs]->configurationName_;
    string outFileName       = chewieOutputDir + "/" + theExpressXmlParser.getFilesList()[fs]->outFileName_;
    theChewieXmlParser->parseDocument(QString(configurationName.c_str()));
    EventManager*    theEventManager    = new EventManager();
    AnalysisManager* theAnalysisManager = new AnalysisManager;
    theEventManager   ->setConfiguration(theChewieXmlParser);
    theAnalysisManager->setConfiguration(theChewieXmlParser);
    
    //Setting the number of events
    if(numberOfEvents != -1)
    {
      QDomAttr maxEvents = theChewieXmlParser->getDocument()->createAttribute("maxEvents");
      std::stringstream ss; ss.str("");
      ss << numberOfEvents;
      maxEvents.setValue(ss.str().c_str());
      theChewieXmlParser->getConverter()->getNode().toElement().setAttributeNode(maxEvents);
      //theEventManager->setEventsLimit();it is done in startConverter
    }
    
    //////////////////////////////////////////
    //Open Monicelli File
    std::vector<std::string> monicelliFileList;
    std::vector<std::string> convertedFileList;
    QStringList convertedFileNames;
    for(unsigned int f=0;f<theExpressXmlParser.getFilesList()[fs]->fileNames_.size();f++)
    {
      std::string fileToAnalyze = theExpressXmlParser.getFilesList()[fs]->fileNames_[f]->fileName_;
      std::string fileName = filesPath + fileToAnalyze;
      STDLINE(fileName,ACRed);
      monicelliFileList.push_back(fileName);
      convertedFileList.push_back(chewieInputDir + fileToAnalyze.substr(0,fileToAnalyze.length()-5) + "_Converted.root");
      cout << chewieInputDir + fileToAnalyze.substr(0,fileToAnalyze.length()-5) + "_Converted.root" << endl;
    }
    theEventManager   ->setInFilesList(monicelliFileList);
    theAnalysisManager->setInFilesList(convertedFileList);
    
    if(convert)
      theEventManager->startConverter();

    if(runAnalysis)
    {
      theAnalysisManager->setOutputFileName(outFileName);
      theAnalysisManager->startSequence();
    }

    delete theAnalysisManager;
    delete theEventManager   ;
  }
  delete theChewieXmlParser;
  return EXIT_SUCCESS;
}

//================================================================================
//================================================================================
//================================================================================
ExpressXmlParser::ExpressXmlParser(void) : document_(0)
{
}

//================================================================================
ExpressXmlParser::~ExpressXmlParser()
{
    if(document_)
        delete document_ ;
}

//================================================================================
void ExpressXmlParser::parseDocument(std::string xmlFileName)
{
    if(document_)
        delete document_;

//    QDomImplementation implementation;
//    QDomDocumentType type = implementation.createDocumentType("ConfigurationFile","MonicelliExpressConfiguration","/home/uplegger/Programming/Monicelli/Express/xml/dtd/ExpressConfiguration.dtd");
    document_ = new QDomDocument( "ConfigurationFile" );
    QFile xmlFile(xmlFileName.c_str());
    if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text ))
    {
        STDLINE(std::string("Could not open ") + xmlFile.fileName().toStdString(),ACRed);
        return;
    }

    QString errMsg = "";
    int line;
    int col;
    if (!document_->setContent( &xmlFile, true , &errMsg, &line, &col))
    {
        STDLINE(std::string("Could not access ") + xmlFile.fileName().toStdString(),ACRed);
        ss_ << "Error: " << errMsg.toStdString() << " line: " << line << " col: " << col;
        STDLINE(ss_.str(),ACGreen);
        xmlFile.close();
        return;
    }

    STDLINE(std::string("Parsing ") + xmlFile.fileName().toStdString(),ACGreen);

    rootNode_ = document_->elementsByTagName("ChewieExpressConfiguration").at(0);

    QDomNode defaults = document_->elementsByTagName("Defaults").at(0);
    theDefaults_ = new XmlDefaults(defaults);

    QDomNodeList filesList = document_->elementsByTagName("Files");

    for(int fs=0; fs<filesList.size(); ++fs)
    {
      QDomNode filesNode = filesList.at(fs);
      theFilesList_.push_back(new XmlFiles(filesNode));
    }

    xmlFile.close();
}

//================================================================================
//================================================================================
//================================================================================
XmlDefaults::XmlDefaults(QDomNode& node)
{
    thisNode_       	   = node;
    filesPath_      	   = node.toElement().attribute("FilesPath")		.toStdString();
    convert_ = true;
    if(node.toElement().attribute("Convert") == "no" || node.toElement().attribute("Convert") == "No" || node.toElement().attribute("Convert") == "NO")
      convert_ = false;
    runAnalysis_ = true;
    if(node.toElement().attribute("RunAnalysis") == "no" || node.toElement().attribute("RunAnalysis") == "No" || node.toElement().attribute("RunAnalysis") == "NO")
      runAnalysis_ = false;
    numberOfEvents_        = node.toElement().attribute("NumberOfEvents") 	.toInt();

}

//================================================================================
//================================================================================
//================================================================================
XmlFiles::XmlFiles(QDomNode& node)
{
    thisNode_           = node;
    configurationName_  = node.toElement().attribute("Configuration").toStdString();
    outFileName_        = node.toElement().attribute("OutFileName").toStdString();
    QDomNodeList fileList = node.childNodes();
    for(int f=0; f<fileList.size(); ++f)
    {
      QDomNode fileNode = fileList.at(f);
      if(!fileNode.isComment())
        fileNames_.push_back(new XmlFile(fileNode));
    }
}

//================================================================================
//================================================================================
//================================================================================
XmlFile::XmlFile(QDomNode& node)
{
    thisNode_       = node;
    fileName_       = node.toElement().attribute("Name").toStdString();
    STDLINE(fileName_,ACGreen);
}
