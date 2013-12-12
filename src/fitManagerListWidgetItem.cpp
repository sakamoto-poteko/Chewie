#include "fitManagerListWidgetItem.h"

#include "MessageTools.h"

#include <iostream>

//===========================================================================
FitManagerListWidgetItem::FitManagerListWidgetItem(QListWidgetItem *parent) :
    QListWidgetItem(*parent)
//  ,ui(new Ui::FitParamManagerWidget)

{
//    this->setGeometry(5, 5, 206, 101);

    theFitManager_ = new FitParamManagerWidget;
    theFitManager_->setGeometry(5, 5, 206, 101);
    theFitManager_->show();

//    ui->setupUi(this);
}

//===========================================================================
FitManagerListWidgetItem::~FitManagerListWidgetItem()
{
    //  STDLINE("Dtor",ACRed) ;
    //  delete ui;

    delete theFitManager_;
}
