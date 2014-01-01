#include <QSettings>

#include "rectwindow.h"

RectWindow::RectWindow()
{
    QSettings windowConfig("Config.ini", QSettings::IniFormat);
    windowConfig.beginGroup("Window");
    rectWindowEnabled = windowConfig.value("WindowEnabled", false).toBool();
    windowColBottomRight    = windowConfig.value("BottomRightColumn", 0).toInt();
    windowColTopLeft        = windowConfig.value("TopLeftColumn"    , 0).toInt();
    windowRowBottomRight    = windowConfig.value("BottomRightRow"   , 0).toInt();
    windowRowTopLeft        = windowConfig.value("TopLeftRow"       , 0).toInt();
    windowConfig.endGroup();
}

RectWindow::checkRectWindow(int col, int row)
{
    if (!rectWindowEnabled)
        return false;

    if (col < windowColBottomRight && col > windowColTopLeft && row < windowRowTopLeft && row > windowRowBottomRight)
        return true;
    else
        return false;
}
