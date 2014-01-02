#include <QSettings>

#include "rectwindow.h"
#include "MessageTools.h"

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

    if (rectWindowEnabled) {
        STDLINE("Custom Window Enabled", ACRed);
        STDLINE(QString("\tBottom Right Column: %1, Bottom Right Row: %2")
                .arg(windowColBottomRight).arg(windowRowBottomRight).toStdString(), ACRed);
        STDLINE(QString("\tTop Left Column: %1, Top Left Row: %2")
                .arg(windowColTopLeft).arg(windowRowTopLeft).toStdString(), ACRed);
    }

}

bool RectWindow::checkRectWindow(int col, int row)
{
    if (!rectWindowEnabled)
        return false;

    if (col < windowColBottomRight && col > windowColTopLeft && row < windowRowTopLeft && row > windowRowBottomRight)
        return true;
    else
        return false;
}
