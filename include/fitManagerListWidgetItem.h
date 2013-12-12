#ifndef FITMANAGERLISTWIDGETITEM_H
#define FITMANAGERLISTWIDGETITEM_H

#include <sstream>

#include <QWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include "TCanvas.h"

#include "fitParamManagerWidget.h"

namespace Ui {
    class FitManagerListWidgetItem;
}

class FitManagerListWidgetItem : public QListWidgetItem
{
    Q_OBJECT

public:
    explicit FitManagerListWidgetItem (QListWidgetItem *parent = 0);
            ~FitManagerListWidgetItem();


private:
    Ui::FitManagerListWidgetItem  * ui;

    FitParamManagerWidget * theFitManager_;

    std::stringstream   ss_ ;

private slots:
} ;

#endif // FITMANAGERLISTWIDGETITEM_H
