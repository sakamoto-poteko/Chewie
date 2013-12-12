#include "XmlAnalysis.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////
XmlAnalysis::XmlAnalysis(QDomNode& node)
{
    thisNode_ = node;
}

///////////////////////////////////////////////////////////////////////////////////////////
bool XmlAnalysis::isToRun(void)
{
    std::string isToRun = thisNode_.toElement().attribute("IsToRun").toStdString();

    if(isToRun=="yes")
        return true;
    else if(isToRun=="no")
        return false;
    else
        return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
bool XmlAnalysis::applyStandardCuts(void)
{
    std::string applyStandardCut = thisNode_.toElement().attribute("StandardCut").toStdString();

    if(applyStandardCut=="yes")
        return true;
    else if(applyStandardCut=="no")
        return false;
    else
        return false;
}

///////////////////////////////////////////////////////////////////////////////////////////
std::string XmlAnalysis::getCut(std::string cutType)
{
    QDomNodeList cutsList = thisNode_.toElement().elementsByTagName("Cut");
    for(int c=0; c<cutsList.size(); c++)
    {
        if(cutType == cutsList.at(c).toElement().attribute("CutType").toStdString())
            return (cutsList.at(c).toElement().attribute("CutString")).toStdString();
    }
    return "";
}

///////////////////////////////////////////////////////////////////////////////////////////
QDomNode& XmlAnalysis::getCutNode(std::string cutType)
{
    QDomNodeList cutsList = thisNode_.toElement().elementsByTagName("Cut");
    for(int c=0; c<cutsList.size(); c++)
    {
        cutNode_ = cutsList.at(c);
        if(cutType == cutsList.at(c).toElement().attribute("CutType").toStdString())
            return cutNode_;
    }
    return cutNode_;
}

///////////////////////////////////////////////////////////////////////////////////////////
void XmlAnalysis::setCutsList(void)
{
    QDomNodeList cutsList = thisNode_.toElement().elementsByTagName("Cut");

    if(cutsList.size()==0)
        return;

    for(int c=0; c<cutsList.size(); c++)
        cutsList_[cutsList.at(c).toElement().attribute("CutType").toStdString()] = cutsList.at(c).toElement().attribute("CutString").toStdString();
}

///////////////////////////////////////////////////////////////////////////////////////////
std::pair<bool,bool> XmlAnalysis::isParToLimit(int parID)
{
    QDomNode calibCut = thisNode_.toElement().elementsByTagName("CalibCut").at(0);
    QDomNode par = calibCut.toElement().elementsByTagName("Par").at(parID);
    QDomNode min = par.toElement().elementsByTagName("Min").at(0);
    QDomNode max = par.toElement().elementsByTagName("Max").at(0);

    bool limitMin = false;
    bool limitMax = false;

    if(min.toElement().text()=="X")
        limitMin = false;
    else
        limitMin = true;

    if(max.toElement().text()=="X")
        limitMax = false;
    else
        limitMax = true;

    return std::make_pair(limitMin,limitMax);
}

///////////////////////////////////////////////////////////////////////////////////////////
std::pair<std::string,std::string> XmlAnalysis::getParLimits(int parID)
{
    QDomNode calibCut = thisNode_.toElement().elementsByTagName("CalibCut").at(0);
    QDomNode par = calibCut.toElement().elementsByTagName("Par").at(parID);
    QDomNode min = par.toElement().elementsByTagName("Min").at(0);
    QDomNode max = par.toElement().elementsByTagName("Max").at(0);

    return std::make_pair(min.toElement().text().toStdString(),max.toElement().text().toStdString());
}

///////////////////////////////////////////////////////////////////////////////////////////
std::pair<double,double> XmlAnalysis::getParLimits(int parID, bool flag)
{
    if(!flag)
        return std::make_pair(0,0);

    QDomNode calibCut = thisNode_.toElement().elementsByTagName("CalibCut").at(0);
    QDomNode par = calibCut.toElement().elementsByTagName("Par").at(parID);
    QDomNode min = par.toElement().elementsByTagName("Min").at(0);
    QDomNode max = par.toElement().elementsByTagName("Max").at(0);

    return std::make_pair(min.toElement().text().toDouble(),max.toElement().text().toDouble());
}

///////////////////////////////////////////////////////////////////////////////////////////
std::string XmlAnalysis::getMinHits(void)
{
    return thisNode_.toElement().elementsByTagName("MinHits").at(0).toElement().text().toStdString();
}

///////////////////////////////////////////////////////////////////////////////////////////
int XmlAnalysis::getThreashold(void)
{
    return thisNode_.toElement().elementsByTagName("CellCharge").at(0).toElement().elementsByTagName("Threashold").at(0).toElement().text().toInt();
}

///////////////////////////////////////////////////////////////////////////////////////////
int XmlAnalysis::getMaxCharge(void)
{
    return thisNode_.toElement().elementsByTagName("CellCharge").at(0).toElement().elementsByTagName("MaxCharge").at(0).toElement().text().toInt();
}

///////////////////////////////////////////////////////////////////////////////////////////
int XmlAnalysis::getMinTotCharge(void)
{
    return thisNode_.toElement().elementsByTagName("TotalCharge").at(0).toElement().elementsByTagName("MinTotCharge").at(0).toElement().text().toInt();
}

///////////////////////////////////////////////////////////////////////////////////////////
int XmlAnalysis::getMaxTotCharge(void)
{
    return thisNode_.toElement().elementsByTagName("TotalCharge").at(0).toElement().elementsByTagName("MaxTotCharge").at(0).toElement().text().toInt();
}
//////////////////////////////////////////////////////////////////////////////////////////
int XmlAnalysis::getPriority(void)
{
    return thisNode_.toElement().attribute("Priority").toInt();
}
