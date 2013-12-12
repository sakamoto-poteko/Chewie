#ifndef XMLCONVERTER_H
#define XMLCONVERTER_H

#include <QWidget>
#include <QDomNode>

class XmlConverter : public QWidget
{
    Q_OBJECT
public:
    XmlConverter (QDomNode & node);
    ~XmlConverter(void){;}

    bool        isToLimit          (void);
    std::string getMaxEvents       (void){return thisNode_.toElement().attribute("MaxEvents").toStdString();}
    std::string getNumberOfThreads (void){return thisNode_.toElement().attribute("Threads").toStdString()  ;}
    QDomNode&   getNode            (void){return thisNode_                                                 ;}

private:
    QDomNode thisNode_;

signals:

public slots:

};

#endif // XMLCONVERTER_H
