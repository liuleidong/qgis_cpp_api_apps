#include "exportdockwidget.h"
#include "ui_exportdockwidget.h"

#include "qgsvectorfilewriter.h"

ExportDockWidget::ExportDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ExportDockWidget)
{
    ui->setupUi(this);
    const QList< QgsVectorFileWriter::DriverDetails > drivers = QgsVectorFileWriter::ogrDriverList();
    for ( const QgsVectorFileWriter::DriverDetails &driver : drivers )
    {
        qDebug() << driver.driverName;
        ui->comboBox_format->addItem( driver.longName, driver.driverName );
    }
    ui->comboBox_format->setCurrentIndex( ui->comboBox_format->findData( "DXF" ) );
}

ExportDockWidget::~ExportDockWidget()
{
    delete ui;
}

void ExportDockWidget::on_pushButton_export_clicked()
{
    SExportParams params;
    params.driverName = ui->comboBox_format->currentData().toString();
    params.fileName = ui->lineEdit->text();
    emit setExportParamsSignal(params);
}
