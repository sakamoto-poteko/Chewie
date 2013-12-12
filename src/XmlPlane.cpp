#include "XmlPlane.h"
#include "XmlWindow.h"

//////////////////////////////////////////////////////////////////////////////////////
XmlPlane::XmlPlane(QDomNode& node)
{
    thisNode_ = node;
    QDomNode window = node.toElement().elementsByTagName("Window").at(0);
    theWindow_ = new XmlWindow(window);
    calibNode_ = thisNode_.toElement().elementsByTagName("Calibrations").at(0);
}

//////////////////////////////////////////////////////////////////////////////////////
XmlPlane::~XmlPlane()
{
    if(theWindow_)
        delete theWindow_;
}

//////////////////////////////////////////////////////////////////////////////////////
std::pair<std::string,std::string> XmlPlane::getChipsNumber(void)
{
    std::string xChipsNumber = thisNode_.toElement().attribute("XRocs").toStdString();
    std::string yChipsNumber = thisNode_.toElement().attribute("YRocs").toStdString();

    return std::make_pair<std::string,std::string>(xChipsNumber,yChipsNumber);
}

//////////////////////////////////////////////////////////////////////////////////////
std::string XmlPlane::getZRotation(void)
{
    return thisNode_.toElement().attribute("ZRotation").toStdString();
}

//////////////////////////////////////////////////////////////////////////////////////
std::string XmlPlane::getZPosition(void)
{
    return thisNode_.toElement().attribute("ZPosition").toStdString();
}

//////////////////////////////////////////////////////////////////////////////////////
std::string XmlPlane::getName(void)
{
    return thisNode_.toElement().attribute("Name").toStdString();
}

//////////////////////////////////////////////////////////////////////////////////////
std::string XmlPlane::getId(void)
{
    return thisNode_.toElement().attribute("Id").toStdString();
}

//////////////////////////////////////////////////////////////////////////////////////
std::string XmlPlane::getCalibration(void)
{
    return thisNode_.toElement().attribute("Calibration").toStdString();
}

//////////////////////////////////////////////////////////////////////////////////////
std::string XmlPlane::getCalibPlotsFile (void)
{
    return getCalibNode().toElement().elementsByTagName("File").at(0).toElement().text().toStdString();
}

//////////////////////////////////////////////////////////////////////////////////////
bool XmlPlane::useCalibrations(void)
{
    std::string use = getCalibNode().toElement().attribute("Use").toStdString();
    if(use == "yes")
        return true;
    else if (use == "no")
        return false;
    else
        return false;
}
