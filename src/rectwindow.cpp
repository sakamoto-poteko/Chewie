#include <QSettings>

#include "rectwindow.h"
#include "MessageTools.h"

CustomPixelWindow::CustomPixelWindow()
{
    QSettings windowConfig("Config.ini", QSettings::IniFormat);
    windowConfig.beginGroup("Window");
    rectWindowEnabled   = windowConfig.value("WindowEnabled", false).toBool();
    windowRightResidual = windowConfig.value("RightX",      0).toFloat();
    windowLeftResidual  = windowConfig.value("LeftX",       0).toFloat();
    windowBottomResidual= windowConfig.value("BottomY",     0).toFloat();
    windowTopResidual   = windowConfig.value("TopY",        0).toFloat();
    windowConfig.endGroup();

    if (rectWindowEnabled) {
        STDLINE("Custom Window Enabled", ACRed);
        STDLINE(QString("\tRight X: %1, Left X: %2")
                .arg(windowRightResidual).arg(windowLeftResidual).toStdString(), ACRed);
        STDLINE(QString("\tBottom Y: %1, Top Y: %2")
                .arg(windowBottomResidual).arg(windowTopResidual).toStdString(), ACRed);
    }

}

bool CustomPixelWindow::checkCustomWindow(float xRes, float yRes)
{
    if (!rectWindowEnabled)
        return false;

    if (xRes <= windowRightResidual && xRes >= windowLeftResidual
            && yRes <= windowTopResidual && yRes >= windowBottomResidual)
        return true;
    else
        return false;
}
