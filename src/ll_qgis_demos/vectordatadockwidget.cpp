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

void VectorDataDockWidget::on_pushButton_getFields_clicked()
{
    emit getFieldsSignal(ui->plainTextEdit);

}

void VectorDataDockWidget::on_pushButton_getFeatures_clicked()
{
    emit getFeaturesSignal(ui->plainTextEdit);
}

void VectorDataDockWidget::on_pushButton_selectFeatures_clicked()
{
    emit selectFeaturesSignal(ui->plainTextEdit);
}

void VectorDataDockWidget::on_pushButton_spatialIndex_clicked()
{
    emit spatialIndexSignal(ui->plainTextEdit);
}

void VectorDataDockWidget::on_pushButton_distanceArea_clicked()
{
    emit distanceAreaSignal(ui->plainTextEdit);
}

void VectorDataDockWidget::on_pushButton_vectorFileWriter_clicked()
{
    emit vectorFileWriterSignal(ui->plainTextEdit);
}
