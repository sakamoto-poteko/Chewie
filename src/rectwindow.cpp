#include <QSettings>

#include "rectwindow.h"
#include "MessageTools.h"

RectWindow::RectWindow()
{
    QSettings windowConfig("Config.ini", QSettings::IniFormat);
    windowConfig.beginGroup("Window");
    rectWindowEnabled   = windowConfig.value("WindowEnabled", false).toBool();
    windowRightResidual = windowConfig.value("LeftX",       0).toFloat();
    windowLeftResidual  = windowConfig.value("RightX",      0).toFloat();
    windowBottomResidual= windowConfig.value("BottomY",     0).toFloat();
    windowTopResidual   = windowConfig.value("TopY",        0).toFloat();
    windowConfig.endGroup();

    if (rectWindowEnabled) {
        STDLINE("Custom Window Enabled", ACRed);
        STDLINE(QString("\tBottom Right Column: %1, Bottom Right Row: %2")
                .arg(windowRightResidual).arg(windowBottomResidual).toStdString(), ACRed);
        STDLINE(QString("\tTop Left Column: %1, Top Left Row: %2")
                .arg(windowLeftResidual).arg(windowTopResidual).toStdString(), ACRed);
    }

}

bool RectWindow::checkRectWindow(float xRes, float yRes)
{
    if (!rectWindowEnabled)
        return false;

    if (xRes <= windowRightResidual && xRes >= windowLeftResidual
            && yRes <= windowTopResidual && yRes >= windowBottomResidual)
        return true;
    else
        return false;
}
