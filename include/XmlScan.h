#ifndef XMLSCAN_H
#define XMLSCAN_H

#include <QWidget>
#include <QDomNode>

#include <map>
#include <string>

class XmlScan : public QWidget
{
    Q_OBJECT
public:
    XmlScan (QDomNode & node);
    ~XmlScan(void           ){;}

    QDomNode&                 getNode       (void){return thisNode_  ;}
    std::map<std::string,int> getScanValues (void){return scanValues_;}

    void      addScanValues (void);

private:
    QDomNode thisNode_;
    std::map<std::string,int> scanValues_;

signals:

public slots:

};
#endif // XMLSCAN_H
