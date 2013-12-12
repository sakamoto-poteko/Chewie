#include "XmlConverter.h"

/////////////////////////////////////////////////////////////////////
XmlConverter::XmlConverter(QDomNode& node)
{
    thisNode_ = node;
}

/////////////////////////////////////////////////////////////////////
bool XmlConverter::isToLimit(void)
{
    if(thisNode_.toElement().attribute("IsToLimit") == "yes")
        return true;
    else
        return false;
}
