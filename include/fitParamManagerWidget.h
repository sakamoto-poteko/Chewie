#ifndef FITPARAMMANAGERWIDGET_H
#define FITPARAMMANAGERWIDGET_H

#include <sstream>
#include <string.h>

#include <QWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QCheckBox>

namespace Ui {
    class FitParamManagerWidget;
}

class FitParamManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FitParamManagerWidget (QWidget *parent = 0);
            ~FitParamManagerWidget();

    void setParName   (std::string paramName) {parName  ->setText((QString)paramName.c_str());}
    void setParValue  (double p             ) {parValue ->setValue(p)                        ;}
    void setParLimInf (double p             ) {parLimInf->setValue(p)                        ;}
    void setParLimSup (double p             ) {parLimSup->setValue(p)                        ;}
    void setParFixed  (bool   f             );

    void setParPrecision (int decimals          );
    void setParStep      (double step           );
    void setParLimits    (double min, double max);

    std::string getParName   () {return parName  ->text() .toStdString();}
    double      getParValue  () {return parValue ->value()              ;}
    double      getParLimInf () {return parLimInf->value()              ;}
    double      getParLimSup () {return parLimSup->value()              ;}
    bool        isParFixed   () {return fixPar->isChecked()             ;}

    QCheckBox * getParFixed  () {return fixPar                          ;}

private:
    Ui::FitParamManagerWidget  * ui;
    QLineEdit                  * parName;
    QDoubleSpinBox             * parValue;
    QDoubleSpinBox             * parLimInf;
    QDoubleSpinBox             * parLimSup;
    QLabel                     * parSetValue;
    QLabel                     * parSetLimits;
    QCheckBox                  * fixPar;

    std::stringstream   ss_ ;

private slots:
        void on_fixPar_clicked(void);
} ;

#endif // FITPARAMMANAGERWIDGET_H
