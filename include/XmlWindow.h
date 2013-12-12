#ifndef XMLWINDOW_H
#define XMLWINDOW_H

#include <QWidget>
#include <QDomNode>

#include <vector>
#include <map>
#include <string>

class XmlWindow : public QWidget
{
    Q_OBJECT
public:
    XmlWindow (QDomNode & node);
    ~XmlWindow(void){;}

    void        setRemovedPixels  (void);

    QDomNode&   getNode           (void){return thisNode_ ;}
    std::string getLowerCol       (void){return thisNode_.toElement().elementsByTagName("LowerCol").at(0).toElement().text().toStdString() ;}
    std::string getHigherCol      (void){return thisNode_.toElement().elementsByTagName("HigherCol").at(0).toElement().text().toStdString();}
    std::string getLowerRow       (void){return thisNode_.toElement().elementsByTagName("LowerRow").at(0).toElement().text().toStdString() ;}
    std::string getHigherRow      (void){return thisNode_.toElement().elementsByTagName("HigherRow").at(0).toElement().text().toStdString();}

    std::vector< std::map<std::string,int> >& getRemovedPixels(void){return removedPixelsVec_;}

private:
    QDomNode                                 thisNode_        ;
    std::vector< std::map<std::string,int> > removedPixelsVec_;

signals:

public slots:

};

#endif // XMLWINDOW_H
