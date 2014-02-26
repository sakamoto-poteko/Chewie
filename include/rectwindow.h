#ifndef RECTWINDOW_H
#define RECTWINDOW_H

#include <QVector>

struct RoundWindowDef {
    float xResidual;
    float yResidual;
    float radius;
};

/**
 * @brief The RectWindow class
 *
 * If RectWindow is not enabled in the config file, the checkRectWindow will always return false.
 * The cost of constructing ths class is relative low. It's ok if you construct it in every Analysis class
 *
 */
class CustomPixelWindow
{
public:
    CustomPixelWindow();

    bool checkCustomWindow(float xRes, float yRes);
    bool windowEnabled() {return rectWindowEnabled;}

private:
    bool rectWindowEnabled  ;
    float windowLeftResidual;
    float windowTopResidual ;
    float windowRightResidual   ;
    float windowBottomResidual  ;
    QVector<RoundWindowDef> roundWindows;

    bool pointInCircle(float point_x, float point_y, float circle_x, float circle_y, float circle_radius);
};

#endif // RECTWINDOW_H
