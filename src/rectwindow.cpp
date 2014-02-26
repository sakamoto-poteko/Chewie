#include <cmath>
#include <QSettings>

#include "rectwindow.h"
#include "MessageTools.h"

CustomPixelWindow::CustomPixelWindow()
{
    QSettings windowConfig("Config.ini", QSettings::IniFormat);
    windowConfig.beginGroup("CustomWindow");
    rectWindowEnabled   = windowConfig.value("WindowEnabled", false).toBool();
    windowRightResidual = windowConfig.value("RightX",      0).toFloat();
    windowLeftResidual  = windowConfig.value("LeftX",       0).toFloat();
    windowBottomResidual= windowConfig.value("BottomY",     0).toFloat();
    windowTopResidual   = windowConfig.value("TopY",        0).toFloat();

    for (int i = 0; ;++i) {
        if (windowConfig.contains(QString("RoundWindow%1_x").arg(i))
                && windowConfig.contains(QString("RoundWindow%1_y").arg(i))
                && windowConfig.contains(QString("RoundWindow%1_radius").arg(i))) {
            RoundWindowDef window;
            window.xResidual = windowConfig.value(QString("RoundWindow%1_x").arg(i)).toFloat();
            window.yResidual = windowConfig.value(QString("RoundWindow%1_y").arg(i)).toFloat();
            window.radius = windowConfig.value(QString("RoundWindow%1_radius").arg(i)).toFloat();
            roundWindows.append(window);
        } else {
            break; // Leave for loop
        }
    }


    windowConfig.endGroup();

    if (rectWindowEnabled) {
        STDLINE("Custom Window Enabled", ACRed);
        STDLINE(QString("\tRight X: %1, Left X: %2")
                .arg(windowRightResidual).arg(windowLeftResidual).toStdString(), ACRed);
        STDLINE(QString("\tBottom Y: %1, Top Y: %2")
                .arg(windowBottomResidual).arg(windowTopResidual).toStdString(), ACRed);

        for (int i = 0; i < roundWindows.size(); ++i) {
            STDLINE(QString("\tRound Window %1: X: %2, Y: %3, Radius: %4")
                    .arg(i).arg(roundWindows.at(i).xResidual).arg(roundWindows.at(i).yResidual)
                    .arg(roundWindows.at(i).radius), ACRed);
        }
    }

}

bool CustomPixelWindow::checkCustomWindow(float xRes, float yRes)
{
    if (!rectWindowEnabled)
        return false;

    if (xRes > windowRightResidual || xRes < windowLeftResidual)
        return false;
    if (yRes > windowTopResidual || yRes < windowBottomResidual)
        return false;

    foreach (RoundWindowDef window, roundWindows) {
        if (!pointInCircle(xRes, yRes, window.xResidual, window.yResidual, window.radius))
            return false;
    }


    return true;

}

inline bool CustomPixelWindow::pointInCircle(float point_x, float point_y, float circle_x, float circle_y, float circle_radius)
{

    float delta_x = circle_x - point_x;
    float delta_y = circle_y - point_y;
    return (float)sqrt(delta_x*delta_x+delta_y*delta_y) <= circle_radius;
}
