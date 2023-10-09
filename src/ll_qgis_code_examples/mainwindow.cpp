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
    mApp->layerTreeDock()->hide();
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

void MainWindow::initMapTools()
{
    mMapToolPan = new QgsMapToolPan(mApp->mapCanvas());
    mMapToolZoomOut = new QgsMapToolZoom(mApp->mapCanvas(),false);
    mMapToolZoomIn = new QgsMapToolZoom(mApp->mapCanvas(),true);
}

void MainWindow::initGroupboxInPanel()
{
    init_groupBox_maps();
    init_groupBox_canvas();
}

void MainWindow::init_groupBox_maps()
{
    int row = 0,column = -1;
    QGridLayout *layout = (QGridLayout *)ui->groupBox_maps->layout();

    QLabel *label_ogr = new QLabel("OGR data provider(ogr)");
    layout->addWidget(label_ogr,row,0);
    ++row;
    addPanelItem(layout,"addShpSlot",QString::fromLocal8Bit("添加shapefile文件"),":/res/images/addShpSlot.png",row,++column);
    addPanelItem(layout,"addGpxSlot",QString::fromLocal8Bit("添加gpx文件"),":/res/images/addGpxSlot.png",row,++column);
    addPanelItem(layout,"addGpkgSlot",QString::fromLocal8Bit("添加gpkg文件"),":/res/images/addGpkgSlot.png",row,++column);
    addPanelItem(layout,"addGeoJsonSlot",QString::fromLocal8Bit("添加geojson文件"),":/res/images/addGeoJsonSlot.png",row,++column);
    addPanelItem(layout,"addGmlSlot",QString::fromLocal8Bit("添加gml文件"),":/res/images/addGmlSlot.png",row,++column);
    ++row;column = -1;
    addPanelItem(layout,"addKmlSlot",QString::fromLocal8Bit("添加kml文件"),":/res/images/addKmlSlot.png",row,++column);
    addPanelItem(layout,"addDxfSlot",QString::fromLocal8Bit("添加dxf文件"),":/res/images/addDxfSlot.png",row,++column);
    addPanelItem(layout,"addCoverageSlot",QString::fromLocal8Bit("添加coverage文件"),":/res/images/addCoverageSlot.png",row,++column);

    int labelRow = ++row;
    QLabel *label_gpx = new QLabel("GPX data provider(gpx)");
    layout->addWidget(label_gpx,labelRow,0);
    ++row;column = -1;
    addPanelItem(layout,"addGpx1Slot",QString::fromLocal8Bit("添加gpx文件"),":/res/images/addGpx1Slot.png",row,++column);
    QLabel *label_delimitedtext = new QLabel("Delimited text file provider(delimitedtext)");
    layout->addWidget(label_delimitedtext,labelRow,1);
    addPanelItem(layout,"addCsvSlot",QString::fromLocal8Bit("添加csv文件"),":/res/images/addGpx1Slot.png",row,++column);
    QLabel *label_spatiaLite = new QLabel("SpatiaLite data provider(spatialite)");
    layout->addWidget(label_spatiaLite,labelRow,2);
    addPanelItem(layout,"addSpatiaLiteSlot",QString::fromLocal8Bit("spatialite db添加图层"),":/res/images/addSpatiaLiteSlot.png",row,++column);
    QLabel *label_memory = new QLabel("Memory data provider(memory)");
    layout->addWidget(label_memory,labelRow,3);
    addPanelItem(layout,"addMemorySlot",QString::fromLocal8Bit("添加memory类型图层"),":/res/images/addMemorySlot.png",row,++column);
    QLabel *label_wfs = new QLabel("WFS(web feature service) data provider(wfs)");
    layout->addWidget(label_wfs,labelRow,4);
    addPanelItem(layout,"addWfsSlot",QString::fromLocal8Bit("添加wfs类型图层"),":/res/images/addWfsSlot.png",row,++column);
    labelRow = ++row;
    QLabel *label_gdal = new QLabel("GDAL data provider(gdal)");
    layout->addWidget(label_gdal,row,0);
    ++row;column = -1;
    addPanelItem(layout,"addRasterSlot",QString::fromLocal8Bit("添加tiff文件"),":/res/images/addRasterSlot.png",row,++column);
    addPanelItem(layout,"addGpkg1Slot",QString::fromLocal8Bit("添加多图层tiff文件"),":/res/images/addGpkg1Slot.png",row,++column);

    QLabel *label_wms = new QLabel("WMS data provider(wms)");
    layout->addWidget(label_wms,labelRow,2);
    addPanelItem(layout,"addWmsSlot",QString::fromLocal8Bit("添加在线高德路网"),":/res/images/addWmsSlot.png",row,++column);
    addPanelItem(layout,"addGdalOfflineSlot",QString::fromLocal8Bit("添加离线高德影像"),":/res/images/addGpkg1Slot.png",row,++column);
}

void MainWindow::init_groupBox_canvas()
{
    int row = 0,column = -1;
    QGridLayout *layout = (QGridLayout *)ui->groupBox_canvas->layout();
    int labelrow = row;
    QLabel *label_canvasColor = new QLabel(QString::fromLocal8Bit("Map Canvas相关"));
    layout->addWidget(label_canvasColor,row,0);
    ++row;
    addPanelItem(layout,"canvasBrSlot",QString::fromLocal8Bit("map canvas背景色"),":/res/images/canvasBrSlot.png",row,++column);
    addPanelItem(layout,"canvasCenterSlot",QString::fromLocal8Bit("map canvas中心点"),":/res/images/canvasCenterSlot.png",row,++column);
    addPanelItem(layout,"canvasRotationSlot",QString::fromLocal8Bit("map canvas角度"),":/res/images/canvasRotationSlot.png",row,++column);
    QLabel *label_rubberBand = new QLabel(QString::fromLocal8Bit("RubberBand使用"));
    layout->addWidget(label_rubberBand,labelrow,3);
    addPanelItem(layout,"rubberBandLineSlot",QString::fromLocal8Bit("RubberBand画线"),":/res/images/rubberBandLineSlot.png",row,++column);
    addPanelItem(layout,"rubberBandPolygonSlot",QString::fromLocal8Bit("RubberBand画多边形"),":/res/images/rubberBandPolygonSlot.png",row,++column);
    labelrow = ++row;
    QLabel *label_vertexMarker = new QLabel(QString::fromLocal8Bit("VertexMarker使用"));
    layout->addWidget(label_vertexMarker,labelrow,0);
    ++row;column = -1;
    addPanelItem(layout,"vertexMarkerSlot",QString::fromLocal8Bit("VertexMarker画点"),":/res/images/vertexMarkerSlot.png",row,++column);

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
        mApp->mapCanvas()->setMapTool(mMapToolPan);
        mApp->mapCanvas()->setCanvasColor(QColor("white"));
        mApp->mapCanvas()->setRotation(0);
        if(mRubberBandPoint)
        {
            mRubberBandPoint->reset(QgsWkbTypes::PointGeometry);
            mRubberBandPoint = nullptr;
        }
        //QgsRubberBand和QgsVertexMarker都是QGraphicsItem的子类
        //QgsMapCanvas是QGraphicsView的子类
        //从map canvas删除item用到过两种方式：
        //一种删除QGraphicsItem的方式
        if(mRubberBandLine)
        {
            mRubberBandLine->reset(QgsWkbTypes::LineGeometry);
            mRubberBandLine = nullptr;
        }
        if(mRubberBandPolygon)
        {
            mRubberBandPolygon->reset(QgsWkbTypes::PolygonGeometry);
            mRubberBandPolygon = nullptr;
        }
        //另一种删除QGraphicsItem的方式
        mApp->mapCanvas()->scene()->removeItem(mVertexMarker);
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsVectorLayer*>();
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
    zoomToFirstLayer<QgsRasterLayer*>();
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
    zoomToFirstLayer<QgsRasterLayer*>();
}

void MainWindow::addWmsSlot()
{
    QString url = QStringLiteral("https://wprd01.is.autonavi.com/appmaptile?style=8&x={x}&y={y}&z={z}");
    mApp->addWmsLayer(url,"gaode roads");
    QTimer::singleShot(1000*3,this,[=]{QgsPointXY pt(11804480,4660807);mApp->mapCanvas()->zoomByFactor(1/1600.0,&pt);});
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
    zoomToFirstLayer<QgsRasterLayer*>();
}

void MainWindow::canvasBrSlot()
{
    statusBar()->showMessage(tr("ready to change map canvas color"));
    QTimer::singleShot(1000*1,this,[=]
    {mApp->mapCanvas()->setCanvasColor(QColor::fromHsl(rand()%360,rand()%256,rand()%200));
     statusBar()->showMessage(tr("map canvas color change done"));
    });
}

void MainWindow::canvasCenterSlot()
{
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    mApp->addVectorLayer(filename,ff.baseName());
    QTimer::singleShot(1000*1,this,[=]
    {
        statusBar()->showMessage(QString::fromLocal8Bit("修改map canvas中心点"));
        mApp->mapCanvas()->setCenter(QgsPointXY(20.23960,-33.98442));
    });
}

void MainWindow::canvasRotationSlot()
{
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    mApp->addVectorLayer(filename,ff.baseName());
    QTimer::singleShot(1000*1,this,[=]
    {
       statusBar()->showMessage(QString::fromLocal8Bit("修改map canvas角度"));
       mApp->mapCanvas()->setRotation(45.0);
    });

}

void MainWindow::rubberBandLineSlot()
{
    //添加shapefile
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    mApp->addVectorLayer(filename,ff.baseName());
    //定义点                            
    QgsPointXY startPoint(20.34013,-33.90453);
    QgsPointXY endPoint(20.49744,-33.91126);
    //新建QgsRubberBand，注意类型是LineGeometry
    mRubberBandLine = new QgsRubberBand(mApp->mapCanvas(),QgsWkbTypes::LineGeometry);
    //将点添加到rubberband中
    mRubberBandLine->addPoint(startPoint);
    mRubberBandLine->addPoint(endPoint);
    //更改线宽，颜色等属性
    mRubberBandLine->setWidth(4);
    mRubberBandLine->setColor(QColor(222,155,67));
    //定义一个点类型的RubberBand
    mRubberBandPoint = new QgsRubberBand(mApp->mapCanvas(),QgsWkbTypes::PointGeometry);
    mRubberBandPoint->addPoint(startPoint);
    mRubberBandPoint->addPoint(endPoint);
    mRubberBandPoint->setWidth(6);
    mRubberBandPoint->setColor(QColor(222,155,67));
    //显示出来
    mRubberBandLine->show();
    mRubberBandPoint->show();
}

void MainWindow::rubberBandPolygonSlot()
{
    //添加shapefile
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    mApp->addVectorLayer(filename,ff.baseName());
    //定义三个点
    QgsPointXY point1(20.34013,-33.90453);
    QgsPointXY point2(20.49744,-33.91126);
    QgsPointXY point3(20.41396,-33.93079);
    //新建PolygonGeometry类型的RubberBand
    mRubberBandPolygon = new QgsRubberBand(mApp->mapCanvas(),QgsWkbTypes::PolygonGeometry);
    //添加三个点
    mRubberBandPolygon->addPoint(point1);
    mRubberBandPolygon->addPoint(point2);
    mRubberBandPolygon->addPoint(point3);
    //设置线宽颜色等属性
    mRubberBandPolygon->setWidth(4);
    mRubberBandPolygon->setColor(QColor(222,155,67));
    mRubberBandPoint = new QgsRubberBand(mApp->mapCanvas(),QgsWkbTypes::PointGeometry);
    mRubberBandPoint->addPoint(point1);
    mRubberBandPoint->addPoint(point2);
    mRubberBandPoint->addPoint(point3);
    mRubberBandPoint->setWidth(6);
    mRubberBandPoint->setColor(QColor(222,155,67));
    mRubberBandPolygon->show();
    mRubberBandPoint->show();
}

void MainWindow::vertexMarkerSlot()
{
    //添加shapefile
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    mApp->addVectorLayer(filename,ff.baseName());
    //构造QgsVertexMarker并设置属性
    mVertexMarker = new QgsVertexMarker( mApp->mapCanvas() );
    mVertexMarker->setCenter( QgsPointXY(20.33474,-33.91104) );
    mVertexMarker->setIconType( QgsVertexMarker::ICON_DOUBLE_TRIANGLE );
    mVertexMarker->setPenWidth( 2 );
    mVertexMarker->setIconSize(24);
    mVertexMarker->setColor( Qt::green );
    mVertexMarker->setZValue( mVertexMarker->zValue() + 1 );
}
void MainWindow::mapToolPanSlot()
{
    statusBar()->showMessage(QString::fromLocal8Bit("当前Map Tool是ZoomIn,1秒后切换为Pan"));
    mApp->mapCanvas()->setMapTool(mMapToolZoomIn);
    QTimer::singleShot(1000*1,this,[=]
   {
       statusBar()->showMessage(QString::fromLocal8Bit("Map Tool改为Pan"));
       mApp->mapCanvas()->setMapTool(mMapToolPan);
   });
}

void MainWindow::mapToolZoomInSlot()
{
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    mApp->addVectorLayer(filename,ff.baseName());
    statusBar()->showMessage(QString::fromLocal8Bit("当前Map Tool是Pan,1秒后切换为ZoomIn"));
    QTimer::singleShot(1000*1,this,[=]
   {
       statusBar()->showMessage(QString::fromLocal8Bit("Map Tool改为ZoomIn"));
       mApp->mapCanvas()->setMapTool(mMapToolZoomIn);
   });
}

void MainWindow::mapToolZoomOutSlot()
{    
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    mApp->addVectorLayer(filename,ff.baseName());
    statusBar()->showMessage(QString::fromLocal8Bit("当前Map Tool是Pan,1秒后切换为ZoomOut"));
    QTimer::singleShot(1000*1,this,[=]
   {
       statusBar()->showMessage(QString::fromLocal8Bit("Map Tool改为ZoomOut"));
       mApp->mapCanvas()->setMapTool(mMapToolZoomOut);
   });
}
