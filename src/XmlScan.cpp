#include "XmlScan.h"

/////////////////////////////////////////////////////////////////////
XmlScan::XmlScan(QDomNode& node)
{
    thisNode_ = node;
}

/////////////////////////////////////////////////////////////////////
void XmlScan::addScanValues(void)
{
    QDomNodeList filesList = thisNode_.toElement().elementsByTagName("File");

    for(int f=0; f<filesList.size(); ++f)
    {
        QString fileName = filesList.at(f).toElement().attribute("Name");
        QString value    = filesList.at(f).toElement().attribute("Value");
        if(!(scanValues_.find(fileName.toStdString()) != scanValues_.end()))
        {
            scanValues_[fileName.toStdString()] = atoi(value.toStdString().c_str());
        }
    }
}
