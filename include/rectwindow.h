#ifndef RECTWINDOW_H
#define RECTWINDOW_H

/**
 * @brief The RectWindow class
 *
 * If RectWindow is not enabled in the config file, the checkRectWindow will always return false.
 * The cost of constructing ths class is relative low. It's ok if you construct it in every Analysis class
 *
 */
class RectWindow
{
public:
    RectWindow();

    bool checkRectWindow(float xRes, float yRes);
    bool windowEnabled() {return rectWindowEnabled;}
private:
    bool rectWindowEnabled  ;
    float windowLeftResidual;
    float windowTopResidual ;
    float windowRightResidual   ;
    float windowBottomResidual  ;
};

#endif // RECTWINDOW_H
