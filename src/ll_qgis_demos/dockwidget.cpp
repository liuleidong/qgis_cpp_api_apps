#include "dockwidget.h"
#include "ui_dockwidget.h"

DockWidget::DockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockWidget)
{
    ui->setupUi(this);    
}

DockWidget::~DockWidget()
{
    delete ui;
}

void DockWidget::on_pushButton_AddShp_clicked()
{
    emit addShpSignal();
}

void DockWidget::on_pushButton_AddGpx_clicked()
{
    emit addGpxSignal();
}

void DockWidget::on_pushButton_AddGpkg_clicked()
{
    emit addGpkgSignal();
}

void DockWidget::on_pushButton_AddJson_clicked()
{
    emit addGeoJsonSignal();
}

void DockWidget::on_pushButton_AddGml_clicked()
{
    emit addGmlSignal();
}

void DockWidget::on_pushButton_AddKml_clicked()
{
    emit addKmlSignal();
}

void DockWidget::on_pushButton_AddDxf_clicked()
{
    emit addDxfSignal();
}

void DockWidget::on_pushButton_AddCoverage_clicked()
{
    emit addCoverageSignal();
}

void DockWidget::on_pushButton_AddGpx1_clicked()
{
    emit addGpx1Signal();
}

void DockWidget::on_pushButton_AddCsv_clicked()
{
    emit addCsvSignal();
}

void DockWidget::on_pushButton_AddSpatiaLite_clicked()
{
    emit addSpatiaLite();
}

void DockWidget::on_pushButton_AddMemory_clicked()
{
    emit addMemorySignal();
}

void DockWidget::on_pushButton_AddWfs_clicked()
{
    emit addWfsSignal();
}

void DockWidget::on_pushButton_AddRaster_clicked()
{
    emit addRasterSignal();
}

void DockWidget::on_pushButton_AddGpkg1_clicked()
{
    emit addGpkg1Signal();
}

void DockWidget::on_pushButton_AddWms_clicked()
{
    emit addWmsSignal();
}

void DockWidget::on_pushButton_GdalOffline_clicked()
{
    emit addGdalOfflineSignal();
}
