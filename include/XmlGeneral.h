#ifndef XMLGENERAL_H
#define XMLGENERAL_H

#include <QWidget>
#include <QDomNode>

class XmlGeneral : public QWidget
{
    Q_OBJECT
public:
    XmlGeneral (QDomNode & node);
    ~XmlGeneral(void           ){;}

    bool        isToLimit               (void);
    std::string getAnalysesMaxEvents    (void){return thisNode_.toElement().attribute("AnalysesMaxEvents").toStdString()   ;}
    std::string getNumberOfThreads      (void){return thisNode_.toElement().attribute("Threads").toStdString()             ;}
    std::string getPreAnalysisMaxEvents (void){return thisNode_.toElement().attribute("PreAnalysisMaxEvents").toStdString();}
    QDomNode&   getNode                 (void){return thisNode_                                                            ;}

private:
    QDomNode thisNode_;

signals:
    
public slots:
    
};

#endif // XMLGENERAL_H
