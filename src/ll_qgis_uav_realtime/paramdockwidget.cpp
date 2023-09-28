#include "paramdockwidget.h"
#include "ui_paramdockwidget.h"

ParamDockWidget::ParamDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ParamDockWidget)
{
    ui->setupUi(this);
}

ParamDockWidget::~ParamDockWidget()
{
    delete ui;
}
