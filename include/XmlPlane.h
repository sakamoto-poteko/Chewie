#ifndef XMLPLANE_H
#define XMLPLANE_H

#include <string>

#include <QWidget>
#include <QDomNode>

class XmlWindow;

class XmlPlane : public QWidget
{
    Q_OBJECT
public:
    XmlPlane (QDomNode& node);
    ~XmlPlane(void          );

    QDomNode&                          getNode           (void){return thisNode_ ;}
    QDomNode&                          getCalibNode      (void){return calibNode_;}
    XmlWindow*                         getWindow         (void){return theWindow_;}
    std::pair<std::string,std::string> getChipsNumber    (void);
    std::string                        getZRotation      (void);
    std::string                        getZPosition      (void);
    std::string                        getCalibPlotsFile (void);
    std::string                        getName           (void);
    std::string                        getId             (void);
    std::string                        getCalibration    (void);
    bool                               useCalibrations   (void);

private:
    QDomNode   thisNode_ ;
    QDomNode   calibNode_;
    XmlWindow* theWindow_;

signals:

public slots:

};

#endif // XMLPLANE_H
