#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QLabel>
#include <QToolBar>

#include "panelimagebutton.h"

#include "qgsapplication.h"
#include "qgsproject.h"
#include "qgsrasterlayer.h"
#include "qgsdockwidget.h"
#include "qgsmapcanvas.h"
#include "qgsrubberband.h"
#include "qgsvertexmarker.h"
#include "qgsmaptoolpan.h"
#include "qgsmaptoolzoom.h"
#include "qgsmarkersymbollayer.h"
#include "qgsellipsesymbollayer.h"
#include "qgsgeometrygeneratorsymbollayer.h"
#include "qgsvectorfieldsymbollayer.h"
#include "qgsmasksymbollayer.h"
#include "qgsfillsymbollayer.h"
#include "qgsfillsymbol.h"
#include "qgscategorizedsymbolrenderer.h"
#include "qgsexpressioncontextutils.h"
#include "qgsgraduatedsymbolrenderer.h"
#include "qgsclassificationmethod.h"
#include "qgsclassificationmethodregistry.h"
#include "qgscolorrampimpl.h"
#include "qgsrulebasedrenderer.h"
#include "qgspointdisplacementrenderer.h"
#include "qgspointclusterrenderer.h"
#include "qgsheatmaprenderer.h"
#include "qgslinesymbollayer.h"
#include "qgslinesymbol.h"
#include "qgsinterpolatedlinerenderer.h"
#include "qgsinvertedpolygonrenderer.h"
#include "qgs25drenderer.h"
#include "qgscallout.h"
#include "qgsannotationlayer.h"
#include "qgsannotationpolygonitem.h"
#include "qgsannotationlineitem.h"
#include "qgsannotationmarkeritem.h"
#include "qgsannotationpointtextitem.h"
#include "qgsdiagramrenderer.h"
#include "qgspiediagram.h"
#include "qgstextdiagram.h"
#include "qgshistogramdiagram.h"
#include "qgsstackedbardiagram.h"
#include "qgsmultibandcolorrenderer.h"
#include "qgsbrightnesscontrastfilter.h"
#include "qgshuesaturationfilter.h"
#include "qgsrasterresamplefilter.h"
#include "qgsrasterresampler.h"
#include "qgscubicrasterresampler.h"
#include "qgspalettedrasterrenderer.h"
#include "qgssinglebandgrayrenderer.h"
#include "qgssinglebandpseudocolorrenderer.h"
#include "qgshillshaderenderer.h"
#include "qgsrastercontourrenderer.h"
#include "qgsrastershader.h"
#include "qgsmapcanvasutils.h"
#include "qgsvirtuallayerdefinition.h"
#include "qgsvectorfilewriter.h"
#include "qgsprocessingregistry.h"
#include "qgsprocessingalgrunnertask.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mApp = ll_qgis_base_lib::Instance();
    mApp->initialize(this);
    connect(ui->stackedWidget,&QStackedWidget::currentChanged,this,&MainWindow::stackWidgetCurentChangedSlot);
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    initToolbar();
    initStackwidgetPageCanvas();
    initPanels();
    initGroupboxInPanel();
}

void MainWindow::initToolbar()
{
    QToolBar *toolbar = addToolBar("Tool Bar");

    QAction *toPanels = new QAction("To Panels");
    toPanels->setToolTip("To Panels");
    toPanels->setIcon(QIcon(":/res/icons/mActionBackward.png"));
    toolbar->addAction(toPanels);
    connect(toPanels,&QAction::triggered,this,&MainWindow::actionToPanelsSlot);

    QAction *toCanvas = new QAction("To Canvas");
    toCanvas->setToolTip("To Canvas");
    toCanvas->setIcon(QIcon(":/res/icons/mActionForward.png"));
    toolbar->addAction(toCanvas);
    connect(toCanvas,&QAction::triggered,this,&MainWindow::actionToCanvasSlot);
}

void MainWindow::actionToPanelsSlot()
{
    QgsProject::instance()->removeAllMapLayers();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::actionToCanvasSlot()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::initStackwidgetPageCanvas()
{
    auto gridLayout = new QGridLayout;
    gridLayout->addWidget((QWidget*)mApp->mapCanvas());
    ui->page_canvas->setLayout(gridLayout);
    addDockWidget(Qt::LeftDockWidgetArea,mApp->layerTreeDock());
}

void MainWindow::initPanels()
{    
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::initGroupboxInPanel()
{
    init_groupBox_maps();
}

void MainWindow::init_groupBox_maps()
{
    int row = 0,column = -1;
    QGridLayout *layout = (QGridLayout *)ui->groupBox_maps->layout();
    QLabel *label_ogr = new QLabel("OGR data provider(ogr)");
    layout->addWidget(label_ogr,row,0);
    ++row;
    addPanelItem(layout,"addShpSlot","添加shapefile文件",":/res/images/addShpSlot.png",row,++column);
    addPanelItem(layout,"addGpxSlot","添加gpx文件",":/res/images/addGpxSlot.png",row,++column);
    addPanelItem(layout,"addGpkgSlot","添加gpkg文件",":/res/images/addGpkgSlot.png",row,++column);
    addPanelItem(layout,"addGeoJsonSlot","添加geojson文件",":/res/images/addGeoJsonSlot.png",row,++column);
    addPanelItem(layout,"addGmlSlot","添加gml文件",":/res/images/addGmlSlot.png",row,++column);
    ++row;column = -1;
    addPanelItem(layout,"addKmlSlot","添加kml文件",":/res/images/addKmlSlot.png",row,++column);
    addPanelItem(layout,"addDxfSlot","添加dxf文件",":/res/images/addDxfSlot.png",row,++column);
    addPanelItem(layout,"addCoverageSlot","添加coverage文件",":/res/images/addCoverageSlot.png",row,++column);
    ++row;
    QLabel *label_gpx = new QLabel("GPX data provider(gpx)");
    layout->addWidget(label_gpx,row,0);
    ++row;column = -1;
    addPanelItem(layout,"addGpx1Slot","添加gpx文件",":/res/images/addGpx1Slot.png",row,++column);
    ++row;
    QLabel *label_delimitedtext = new QLabel("Delimited text file provider(delimitedtext)");
    layout->addWidget(label_delimitedtext,row,0);
    ++row;column = -1;
    addPanelItem(layout,"addCsvSlot","添加csv文件",":/res/images/addGpx1Slot.png",row,++column);
    ++row;
    QLabel *label_spatiaLite = new QLabel("SpatiaLite data provider(spatialite)");
    layout->addWidget(label_spatiaLite,row,0);
    ++row;column = -1;
    addPanelItem(layout,"addSpatiaLiteSlot","spatialite db添加图层",":/res/images/addSpatiaLiteSlot.png",row,++column);
    ++row;
    QLabel *label_memory = new QLabel("Memory data provider(memory)");
    layout->addWidget(label_memory,row,0);
    ++row;column = -1;
    addPanelItem(layout,"addMemorySlot","添加memory类型图层",":/res/images/addMemorySlot.png",row,++column);
    ++row;
    QLabel *label_wfs = new QLabel("WFS(web feature service) data provider(wfs)");
    layout->addWidget(label_wfs,row,0);
    ++row;column = -1;
    addPanelItem(layout,"addWfsSlot","添加wfs类型图层",":/res/images/addWfsSlot.png",row,++column);
    ++row;
    QLabel *label_gdal = new QLabel("GDAL data provider(gdal)");
    layout->addWidget(label_gdal,row,0);
    ++row;column = -1;
    addPanelItem(layout,"addRasterSlot","添加tiff文件",":/res/images/addRasterSlot.png",row,++column);
    addPanelItem(layout,"addGpkg1Slot","添加多图层tiff文件",":/res/images/addGpkg1Slot.png",row,++column);
    ++row;
    QLabel *label_wms = new QLabel("WMS data provider(wms)");
    layout->addWidget(label_wms,row,0);
    ++row;column = -1;
    addPanelItem(layout,"addWmsSlot","添加在线高德影像",":/res/images/addRasterSlot.png",row,++column);
    addPanelItem(layout,"addGdalOfflineSlot","添加离线高德影像",":/res/images/addGpkg1Slot.png",row,++column);
}


void MainWindow::panelImageButtonClickedSlot(QString slotName)
{
    QMetaObject::invokeMethod(this,slotName.toStdString().c_str());
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::stackWidgetCurentChangedSlot(int index)
{
    if(index == 0)
    {
        mApp->layerTreeDock()->hide();
    }
    else
    {
        mApp->layerTreeDock()->show();
    }
}

void MainWindow::addPanelItem(QGridLayout *layout,const QString &objectName, const QString &title,const QString &url, int row, int column)
{
    auto p = new PanelImageButton;
    connect(p,&PanelImageButton::panelImageButtonClicked,this,&MainWindow::panelImageButtonClickedSlot);
    p->setObjectName(objectName);
    p->setTitleText(title);
    p->setImageUrl(url);
    layout->addWidget(p,row,column);
}

void MainWindow::addShpSlot()
{
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    //创建图层
    QgsVectorLayer* vecLayer = new QgsVectorLayer(filename,ff.baseName(),"ogr");
    if(!vecLayer->isValid())
    {
        QMessageBox::critical(this,tr("error"),tr("invalid layer"));
        return;
    }
    QgsProject::instance()->addMapLayer(vecLayer);
}
void MainWindow::addGpxSlot()
{
    QString filename = QStringLiteral("maps/route.gpx");
    QFileInfo ff(filename);
    //创建图层
    QgsVectorLayer* route_pointsLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=route_points"),"route_points","ogr");
    QgsVectorLayer* routesLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=routes"),"routes","ogr");
    QgsVectorLayer* tracks_pointsLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=track_points"),"track_points","ogr");
    QgsVectorLayer* tracksLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=tracks"),"tracks","ogr");
    QgsVectorLayer* waypointsLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=waypoints"),"waypoints","ogr");
    QList<QgsMapLayer *> mapLayers;
    mapLayers << route_pointsLayer << routesLayer << tracks_pointsLayer << tracksLayer << waypointsLayer;
    QgsProject::instance()->addMapLayers(mapLayers);
}
void MainWindow::addGpkgSlot()
{
    QString filename = QStringLiteral("maps/points_gpkg.gpkg");
    QFileInfo ff(filename);
    //创建图层
    QgsVectorLayer* points_gpkgLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=points_gpkg"),"points_gpkg","ogr");
    QgsVectorLayer* points_smallLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=points_small"),"points_small","ogr");
    QList<QgsMapLayer *> mapLayers;
    mapLayers << points_gpkgLayer << points_smallLayer;
    QgsProject::instance()->addMapLayers(mapLayers);
}

void MainWindow::addGeoJsonSlot()
{
    QString filename = QStringLiteral("maps/grid_4326.geojson");
    QFileInfo ff(filename);
    QgsVectorLayer* vecLayer = new QgsVectorLayer(filename,ff.baseName(),"ogr");
    if(!vecLayer->isValid())
    {
        QMessageBox::critical(this,tr("error"),tr("invalid layer"));
        return;
    }
    QgsProject::instance()->addMapLayer(vecLayer);
}

void MainWindow::addGmlSlot()
{
    QString filename = QStringLiteral("maps/linedensity.gml");
    QFileInfo ff(filename);
    QgsVectorLayer* vecLayer = new QgsVectorLayer(filename,ff.baseName(),"ogr");
    if(!vecLayer->isValid())
    {
        QMessageBox::critical(this,tr("error"),tr("invalid layer"));
        return;
    }
    QgsProject::instance()->addMapLayer(vecLayer);
}

void MainWindow::addKmlSlot()
{
    QString filename = QStringLiteral("maps/multilayer.kml");
    QFileInfo ff(filename);
    //创建图层
    QgsVectorLayer* layer1 = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=Layer1"),"Layer1","ogr");
    QgsVectorLayer* layer2 = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=Layer2"),"Layer2","ogr");
    QgsVectorLayer* layer3 = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=Layer3"),"Layer3","ogr");
    QList<QgsMapLayer *> mapLayers;
    mapLayers << layer1 << layer2 << layer3;
    QgsProject::instance()->addMapLayers(mapLayers);
}

void MainWindow::addDxfSlot()
{
    QString filename = QStringLiteral("maps/points_lines_3d.dxf");
    QFileInfo ff(filename);
    //创建图层
    QgsVectorLayer* points_gpkgLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=entities|geometrytype=LineString"),"entities1","ogr");
    QgsVectorLayer* points_smallLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=entities|geometrytype=Point"),"entities2","ogr");
    QList<QgsMapLayer *> mapLayers;
    mapLayers << points_gpkgLayer << points_smallLayer;
    QgsProject::instance()->addMapLayers(mapLayers);
}

void MainWindow::addCoverageSlot()
{
    QString filename = QStringLiteral("maps/CoverageDir/testvector");
    QFileInfo ff(filename);
    QgsVectorLayer* aaLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=aa"),"aa","ogr");
    QgsVectorLayer* ARCLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=ARC"),"ARC","ogr");
    QgsVectorLayer* CNTLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=CNT"),"CNT","ogr");
    QgsVectorLayer* PALLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=PAL"),"PAC","ogr");
    QList<QgsMapLayer *> mapLayers;
    mapLayers << aaLayer << ARCLayer << CNTLayer << PALLayer;
    QgsProject::instance()->addMapLayers(mapLayers);
}

void MainWindow::addGpx1Slot()
{
    QString filename = QStringLiteral("maps/route.gpx");
    QFileInfo ff(filename);
    QgsVectorLayer* routesLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("?type=route"),"route","gpx");
    QgsVectorLayer* tracksLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("?type=track"),"track","gpx");
    QgsVectorLayer* waypointsLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("?type=waypoint"),"waypoint","gpx");
    QList<QgsMapLayer *> mapLayers;
    mapLayers << routesLayer << tracksLayer << waypointsLayer;
    QgsProject::instance()->addMapLayers(mapLayers);
}

void MainWindow::addCsvSlot()
{
    QString filename = QStringLiteral("maps/jilin_maincity.csv");
    QFileInfo ff(filename);
    QString uri = QString("file:///%1/%2%3").arg(QCoreApplication::applicationDirPath()).arg(filename).arg("?type=csv&xField=longitude&yField=latitude&crs=EPSG:4326");
    QgsVectorLayer* vecLayer = new QgsVectorLayer(uri,ff.baseName(),"delimitedtext");
    if(!vecLayer->isValid())
    {
        QMessageBox::critical(this,tr("error"),tr("invalid layer"));
        return;
    }
    QgsProject::instance()->addMapLayer(vecLayer);
}

void MainWindow::addSpatiaLiteSlot()
{
    QString filename = QStringLiteral("maps/landuse.sqlite");
    QgsDataSourceUri uri;
    uri.setDatabase(filename);
    uri.setDataSource("","landuse","Geometry");
    QFileInfo ff(filename);
    QgsVectorLayer* vecLayer = new QgsVectorLayer(uri.uri(),ff.baseName(),"spatialite");
    if(!vecLayer->isValid())
    {
        QMessageBox::critical(this,tr("error"),tr("invalid layer"));
        return;
    }
    QgsProject::instance()->addMapLayer(vecLayer);
}

void MainWindow::addMemorySlot()
{
    QString uri = QStringLiteral("Point?crs=epsg:4326&field=name:string(20)");
    QgsVectorLayer *layer = new QgsVectorLayer(uri,"New stratch layer","memory");
    QgsProject::instance()->addMapLayer(layer);
    if(layer)
    {
        //开启编辑
        layer->startEditing();

        //设置Feature的符号
        QgsFeatureRenderer * layerRenderer= layer->renderer();
        QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
        //使用Simple Marker
        QVariantMap mp;
        mp[QString("name")] = QStringLiteral("circle");
        mp[QString("color")] = QStringLiteral("red");
        mp[QString("size")] = QStringLiteral("2.0");
        QgsMarkerSymbol *newsym = QgsMarkerSymbol::createSimple(mp);
        singleRenderer->setSymbol(newsym);
        layer->setRenderer(singleRenderer);

        //添加Feature
        QgsVectorDataProvider * dataProvider = layer->dataProvider();
        QgsFields fields;
        QgsField fName("test");
        fields.append(fName);
        QgsFeature f(fields);
        f.setGeometry(QgsGeometry::fromPointXY(QgsPointXY(-34.0146,20.3046)));
        f.setAttribute("test",QString("string1"));
        dataProvider->addFeature(f);
        //提交改动
        layer->commitChanges();
    }
}

void MainWindow::addWfsSlot()
{
    QString uri;
#if 0
    uri = QStringLiteral("https://demo.mapserver.org/cgi-bin/wfs?service=WFS&version=2.0.0&request=GetFeature&typename=ms:cities");
#else
    QgsDataSourceUri dataSourceUri;
    dataSourceUri.setParam("url","https://demo.mapserver.org/cgi-bin/wfs");
    dataSourceUri.setParam("typename","ms:cities");
    dataSourceUri.setParam("version","2.0");
    dataSourceUri.setParam("srsname","EPSG:4326");
    uri = dataSourceUri.uri();
#endif
    auto layer = new QgsVectorLayer(uri,"mapserver","WFS");
    QgsProject::instance()->addMapLayer(layer);
}

void MainWindow::addRasterSlot()
{
    QString filename = QStringLiteral("maps/SRTM.tif");
    QFileInfo ff(filename);
    QgsRasterLayer* rlayer = new QgsRasterLayer(filename,ff.baseName(),"gdal");
    if(!rlayer->isValid())
    {
        QMessageBox::critical(this,tr("error"),tr("invalid layer"));
        return;
    }
    QgsProject::instance()->addMapLayer(rlayer);
}

void MainWindow::addGpkg1Slot()
{
    QString filename = QStringLiteral("maps/two_raster_layers.gpkg");
    QFileInfo ff(filename);
    QString uri1 = QString("GPKG:%1/%2:layer01").arg(QCoreApplication::applicationDirPath()).arg(filename);
    QString uri2 = QString("GPKG:%1/%2:layer02").arg(QCoreApplication::applicationDirPath()).arg(filename);
    QgsRasterLayer* rlayer1 = new QgsRasterLayer(uri1,"layer01","gdal");
    QgsRasterLayer* rlayer2 = new QgsRasterLayer(uri2,"layer02","gdal");
    QgsProject::instance()->addMapLayer(rlayer1);
    QgsProject::instance()->addMapLayer(rlayer2);
}

void MainWindow::addWmsSlot()
{
    QString urlWithParams;
//    QString url = QStringLiteral("https://webst01.is.autonavi.com/appmaptile?style=6&x={x}&y={y}&z={z}");
    QString url = QStringLiteral("https://wprd01.is.autonavi.com/appmaptile?style=8&x={x}&y={y}&z={z}");
    QString type = QStringLiteral("xyz");
    int zMin = 0;
    int zMax = 18;
#if 1
    QgsDataSourceUri uri;
    uri.setParam( QStringLiteral( "url" ), url );
    uri.setParam( QStringLiteral( "type" ), type );
    uri.setParam( QStringLiteral( "zmin" ), QString::number( zMin ) );
    uri.setParam( QStringLiteral( "zmax" ), QString::number( zMax ) );
    urlWithParams = uri.encodedUri();
#else
    QString urlEncode = QUrl::toPercentEncoding(url);
    urlWithParams = QString("type=xyz&url=%1&zmax=18&zmin=0").arg(urlEncode);
#endif
    QgsRasterLayer *rlayer = new QgsRasterLayer(urlWithParams,"gaode","wms");
    QgsProject::instance()->addMapLayer(rlayer);
}

void MainWindow::addGdalOfflineSlot()
{
#ifdef Q_OS_WINDOWS
    QString filename = QStringLiteral("maps/gaode20230630/tms_win.xml");
#else
    QString filename = QStringLiteral("maps/gaode20230630/tms.xml");
#endif
    QFileInfo ff(filename);
    QgsRasterLayer* layer = new QgsRasterLayer(filename,"gaodeoffline","gdal");
    QgsProject::instance()->addMapLayer(layer);
}
