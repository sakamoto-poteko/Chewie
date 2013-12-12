#include "XmlGeneral.h"

/////////////////////////////////////////////////////////////////////
XmlGeneral::XmlGeneral(QDomNode& node)
{
    thisNode_ = node;
    //this->setNumberOfThreads     (node.toElement().attribute("threads").toStdString());
    //this->setPreAnalysisMaxEvents(node.toElement().attribute("preAnalysisMaxEvents").toStdString());
}

/////////////////////////////////////////////////////////////////////
bool XmlGeneral::isToLimit(void)
{
    if(thisNode_.toElement().attribute("IsToLimit") == "yes")
        return true;
    else
        return false;
}
