#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>

#include "qgsdockwidget.h"
#include "qgsproject.h"
#include "qgsmapcanvas.h"
#include "qgsvectorfilewriter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();
    mApp = ll_qgis_base_lib::Instance();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    mApp->initialize(this);
    auto gridLayout = new QGridLayout;
    gridLayout->addWidget((QWidget*)mApp->mapCanvas());
    centralWidget()->setLayout(gridLayout);

    addDockWidget(Qt::LeftDockWidgetArea,mApp->layerTreeDock());
//    ui->menuParams->addAction(mApp->layerTreeDock()->toggleViewAction());

    mExportDockWidget = new ExportDockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea,mExportDockWidget);
    connect(mExportDockWidget,&ExportDockWidget::setExportParamsSignal,this,&MainWindow::setExportParamsSlot);
    //    ui->menuParams->addAction(mExportDockWidget->toggleViewAction());

    //add test layer
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());

//    QString filename = QStringLiteral("maps/points_lines_3d.dxf");
//    QFileInfo ff(filename);
//    //创建图层
//    QgsVectorLayer* points_gpkgLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=entities|geometrytype=LineString"),"entities1","ogr");
//    QgsVectorLayer* points_smallLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=entities|geometrytype=Point"),"entities2","ogr");
//    QList<QgsMapLayer *> mapLayers;
//    mapLayers << points_gpkgLayer << points_smallLayer;
//    QgsProject::instance()->addMapLayers(mapLayers);
}

void MainWindow::setExportParamsSlot(SExportParams eparam)
{
    QgsVectorFileWriter::SaveVectorOptions options;
    options.driverName = eparam.driverName;
    options.fileEncoding = "UTF-8";
    options.skipAttributeCreation = true;
    auto layer = QgsProject::instance()->layers<QgsVectorLayer*>().first();
    QgsVectorFileWriter::writeAsVectorFormatV3(layer,"maps/testExport/test_revert",QgsProject::instance()->transformContext(),options);

}
