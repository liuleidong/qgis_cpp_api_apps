#include "vectordatadockwidget.h"
#include "ui_vectordatadockwidget.h"

VectorDataDockWidget::VectorDataDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::VectorDataDockWidget)
{
    ui->setupUi(this);
}

VectorDataDockWidget::~VectorDataDockWidget()
{
    delete ui;
}

QPlainTextEdit *VectorDataDockWidget::plainTextEdit()
{
    return ui->plainTextEdit;
}
