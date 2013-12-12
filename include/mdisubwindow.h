#ifndef MDISUBWINDOW_H
#define MDISUBWINDOW_H

#include <QMdiSubWindow>

class QMdiSubWindow;
class QResizeEvent ;

class mdiSubWindow : public QMdiSubWindow
{
  Q_OBJECT

  public:
          mdiSubWindow   (void    )  ;
         ~mdiSubWindow   (void    ) {;}
  private:
         void resizeEvent(QResizeEvent * event      ) ;

};

#endif // MDISUBWINDOW_H
