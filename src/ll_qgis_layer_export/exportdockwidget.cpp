#include "exportdockwidget.h"
#include "ui_exportdockwidget.h"

#include "qgsproject.h"
#include "qgsvectorlayer.h"
#include "qgsvectorfilewriter.h"

ExportDockWidget::ExportDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ExportDockWidget)
{
    ui->setupUi(this);
    const QList< QgsVectorFileWriter::DriverDetails > drivers = QgsVectorFileWriter::ogrDriverList();
    for ( const QgsVectorFileWriter::DriverDetails &driver : drivers )
    {
        ui->comboBox_format->addItem( driver.longName, driver.driverName );
    }
    ui->comboBox_format->setCurrentIndex( ui->comboBox_format->findData( "DXF" ) );
}

ExportDockWidget::~ExportDockWidget()
{
    delete ui;
}

void ExportDockWidget::on_pushButton_browse_clicked()
{

}

void ExportDockWidget::on_pushButton_export_clicked()
{
    SExportParams params;
    params.driverName = ui->comboBox_format->currentData().toString();
    emit setExportParamsSignal(params);
}
