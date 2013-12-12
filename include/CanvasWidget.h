/****************************************************************************
** Authors: Dario Menasce, Stefano Terzo
**
** I.N.F.N. Milan-Bicocca
** Piazza  della Scienza 3, Edificio U2
** Milano, 20126
**
****************************************************************************/

#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <sstream>

#include <QWidget>
#include "TCanvas.h"

namespace Ui {
    class CanvasWidget;
}

class CanvasWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = 0);
            ~CanvasWidget();

    void     divide      (int nx, int ny    ) ;
    void     cd          (int pos           ) ;
    void     clear       (void              ) ;
    void     flush       (void              ) ;
    void     update      (void              ) ;
    void     setTitle    (std::string title ) ;
    void     setLogX     (bool set          ){logX_ = set;}
    void     setLogY     (bool set          ){logY_ = set;}
    void     setLogZ     (bool set          ){logZ_ = set;}

private:
    Ui::CanvasWidget  * ui;
    TCanvas           * theCanvas_ ;
    bool                logX_;
    bool                logY_;
    bool                logZ_;

    std::stringstream   ss_ ;

    void resizeEvent ( QResizeEvent * event ) ;

private slots:
} ;

#endif // CANVASWIDGET_H
