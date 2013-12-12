#include "fitParamManagerWidget.h"

#include "MessageTools.h"

#include <iostream>

//===========================================================================
FitParamManagerWidget::FitParamManagerWidget(QWidget *parent) :
    QWidget(parent)
//  ,ui(new Ui::FitParamManagerWidget)

{
//    this->setGeometry(5, 5, 206, 101);

    parName = new QLineEdit(this);
    parName->setGeometry(10, 5, 181, 16);
    parName->show();

    parValue = new QDoubleSpinBox(this);
    parValue->setGeometry(65, 30, 85, 16);
    parValue->setRange(0, 1000);
    parValue->show();

    parLimInf = new QDoubleSpinBox(this);
    parLimInf->setGeometry(25, 75, 85, 16);
    parLimInf->setRange(0, 1000);
    parLimInf->show();

    parLimSup = new QDoubleSpinBox(this);
    parLimSup->setGeometry(110, 74, 85, 16);
    parLimSup->setRange(0, 1000);
    parLimSup->show();

    parSetValue = new QLabel(this);
    parSetValue->setGeometry(10, 30, 62, 16);
    parSetValue->setText("Set Value");
    parSetValue->show();

    parSetLimits = new QLabel(this);
    parSetLimits->setGeometry(10, 50, 61, 16);
    parSetLimits->setText("Set Limits");
    parSetLimits->show();

    fixPar = new QCheckBox(this);
    fixPar->setGeometry(155, 30, 50, 16);
    fixPar->setText("Fix");

//    ui->setupUi(this);
}

//===========================================================================
FitParamManagerWidget::~FitParamManagerWidget()
{
    //  STDLINE("Dtor",ACRed) ;
    //  delete ui;
    delete parName;
    delete parValue;
    delete parLimInf;
    delete parLimSup;
    delete parSetValue;
    delete parSetLimits;

}

//============================================================================

void FitParamManagerWidget::setParFixed (bool f)
{
    fixPar->setChecked (f);
    if (f == true)
    {
        parValue ->setEnabled(false);
        parLimInf->setEnabled(false);
        parLimSup->setEnabled(false);
    }
}

void FitParamManagerWidget::setParPrecision (int decimals)
{
    parValue ->setDecimals(decimals);
    parLimInf->setDecimals(decimals);
    parLimSup->setDecimals(decimals);
}

void FitParamManagerWidget::setParStep (double step)
{
    parValue ->setSingleStep (step);
    parLimInf->setSingleStep (step);
    parLimSup->setSingleStep (step);
}

void FitParamManagerWidget::setParLimits (double min, double max)
{
    parValue ->setRange (min,max);
    parLimInf->setRange (min,max);
    parLimSup->setRange (min,max);
}

void FitParamManagerWidget::on_fixPar_clicked(void)
{
    bool checked = fixPar->isChecked();
    if (checked == true)
    {
        parValue ->setEnabled(false);
        parLimInf->setEnabled(false);
        parLimSup->setEnabled(false);
    }
    else {
        parValue ->setEnabled(true);
        parLimInf->setEnabled(true);
        parLimSup->setEnabled(true);
    }
}
