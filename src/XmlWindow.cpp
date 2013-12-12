#include "XmlWindow.h"
#include "MessageTools.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
XmlWindow::XmlWindow(QDomNode& node)
{
    thisNode_ = node;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void XmlWindow::setRemovedPixels(void)
{
    QDomNodeList removedPixels = thisNode_.toElement().elementsByTagName("RemovedPixel");

    std::map<std::string,int> removedPixel;

    for(int p=0;p<removedPixels.size();p++)
    {
        removedPixel["row"] = atoi(removedPixels.at(p).toElement().attribute("Row").toStdString().c_str());
        removedPixel["col"] = atoi(removedPixels.at(p).toElement().attribute("Col").toStdString().c_str());
        removedPixelsVec_.push_back(removedPixel);
    }
}
