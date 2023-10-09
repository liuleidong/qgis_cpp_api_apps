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
    initMapTools();
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
    init_groupBox_vector_point_symbol();
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

    QLabel *label_mapTool = new QLabel(QString::fromLocal8Bit("Maptool的使用"));
    layout->addWidget(label_mapTool,labelrow,1);
    addPanelItem(layout,"mapToolPanSlot",QString::fromLocal8Bit("Maptool Pan"),":/res/images/mapToolPanSlot.png",row,++column);
    addPanelItem(layout,"mapToolZoomInSlot",QString::fromLocal8Bit("Maptool ZoomIn"),":/res/images/mapToolZoomInSlot.png",row,++column);
    addPanelItem(layout,"mapToolZoomOutSlot",QString::fromLocal8Bit("Maptool ZoomOut"),":/res/images/mapToolZoomOutSlot.png",row,++column);
}

void MainWindow::init_groupBox_vector_point_symbol()
{
    int row = 0,column = -1;
    QGridLayout *layout = (QGridLayout *)ui->groupBox_vector_point_symbol->layout();
    int labelrow = row;
    QLabel *label_SingleSymbolRenderer = new QLabel(QString::fromLocal8Bit("单一符号渲染"));
    layout->addWidget(label_SingleSymbolRenderer,row,0);
    ++row;
    addPanelItem(layout,"pointSimpleMarkerSlot",QString::fromLocal8Bit("点符号-简单标记"),":/res/images/pointSimpleMarkerSlot.png",row,++column);
    addPanelItem(layout,"pointSvgMarkerSlot",QString::fromLocal8Bit("点符号-svg标记"),":/res/images/pointSvgMarkerSlot.png",row,++column);
    addPanelItem(layout,"pointRasterMarkerSlot",QString::fromLocal8Bit("点符号-图片标记"),":/res/images/pointRasterMarkerSlot.png",row,++column);
    addPanelItem(layout,"pointAnimatedMarkerSlot",QString::fromLocal8Bit("点符号-动画标记"),":/res/images/pointAnimatedMarkerSlot.png",row,++column);
    addPanelItem(layout,"pointEllipseMarkerSlot",QString::fromLocal8Bit("点符号-椭圆形标记"),":/res/images/pointEllipseMarkerSlot.png",row,++column);
    ++row;column = -1;
    addPanelItem(layout,"pointFontMarkerSlot",QString::fromLocal8Bit("点符号-字符标记"),":/res/images/pointFontMarkerSlot.png",row,++column);
    addPanelItem(layout,"pointFilledMarkerSlot",QString::fromLocal8Bit("点符号-填充（圆形）标记"),":/res/images/pointFilledMarkerSlot.png",row,++column);
    addPanelItem(layout,"pointGeometryGeneratorMarkerSlot",QString::fromLocal8Bit("点符号-几何生成器"),":/res/images/pointGeometryGeneratorMarkerSlot.png",row,++column);
    addPanelItem(layout,"pointVectorFieldMarkerSlot",QString::fromLocal8Bit("点符号-矢量字段标记"),":/res/images/pointVectorFieldMarkerSlot.png",row,++column);
    addPanelItem(layout,"pointMaskSlot",QString::fromLocal8Bit("点符号-掩膜"),":/res/images/pointMaskSlot.png",row,++column);
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
        if(mVertexMarker)
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
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    mApp->addVectorLayer(filename,ff.baseName());

    mApp->mapCanvas()->setMapTool(mMapToolZoomIn);
    statusBar()->showMessage(QString::fromLocal8Bit("当前Map Tool是ZoomIn,1秒后切换为Pan"));
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

void MainWindow::pointSimpleMarkerSlot()
{
    //添加测试图层
    QString filename = QStringLiteral("maps/shapefile/places_33S.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //获取图层的渲染器renderer
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    //构造简单标记层(Simple Marker SymbolLayer)方法一
    //作为示例，用到了shape和size两个参数，其他参数使用类似
    auto markerSymbolLayer = new QgsSimpleMarkerSymbolLayer(Qgis::MarkerShape::Heart,4.0);
#else
    //构造简单标记层(Simple Marker SymbolLayer)方法二
    //作为示例，用到了shape和size两个参数，其他参数使用类似
    QVariantMap mp;
    mp[QString("name")] = "heart";
    mp[QString("color")] = "pink";
    mp[QString("size")] = "12.0";
    auto markerSymbolLayer = QgsSimpleMarkerSymbolLayer::create(mp);
#endif
    //QgsMarkerSymbol构造函数中需要传入QgsSymbolLayerList
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    //构造QgsMarkerSymbol并设置renderer
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    singleRenderer->setSymbol(markerSymbol);
    layer->setRenderer(singleRenderer);
}

void MainWindow::pointSvgMarkerSlot()
{
    //添加测试图层
    QString filename = QStringLiteral("maps/shapefile/places_33S.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //获取图层的渲染器renderer
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    //构造Svg标记层(Svg Marker SymbolLayer)方法一
    //作为示例，用到了path和size两个参数，其他参数使用类似
    QString path = QStringLiteral("resources/plane.svg");
    auto markerSymbolLayer = new QgsSvgMarkerSymbolLayer(path,8.0);
    markerSymbolLayer->setStrokeColor(QColor(255,0,0));
#else
    //构造Svg标记层(Svg Marker SymbolLayer)方法二
    //作为示例，用到了shape size color参数，其他参数使用类似
    QVariantMap mp;
    mp[QString("name")] = "resources/plane.svg";
    mp[QString("outline_color")] = "pink";
    mp[QString("size")] = "18.0";
    auto markerSymbolLayer = QgsSvgMarkerSymbolLayer::create(mp);
#endif
    //QgsMarkerSymbol构造函数中需要传入QgsSymbolLayerList
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    //构造QgsMarkerSymbol并设置renderer
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    singleRenderer->setSymbol(markerSymbol);
    layer->setRenderer(singleRenderer);

}

void MainWindow::pointRasterMarkerSlot()
{
    //添加一个点图层，用于设置图层符号
    QString filename = QStringLiteral("maps/shapefile/places_33S.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //获取图层的渲染器renderer
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    //构造Raster标记层(Raster Marker SymbolLayer)方法一
    //作为示例，用到了imageFile和size两个参数，其他参数使用类似
    QString imageFile = QStringLiteral("resources/cock.jpg");
    auto markerSymbolLayer = new QgsRasterMarkerSymbolLayer(imageFile,8.0);
#else
    //构造Raster标记层(Raster Marker SymbolLayer)方法二
    //作为示例，用到了imageFile size参数，其他参数使用类似
    QVariantMap mp;
    mp[QString("imageFile")] = "resources/cock.jpg";
    mp[QString("size")] = "18.0";
    auto markerSymbolLayer = QgsRasterMarkerSymbolLayer::create(mp);
#endif
    //QgsMarkerSymbol构造函数中需要传入QgsSymbolLayerList
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    //构造QgsMarkerSymbol并设置renderer
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    singleRenderer->setSymbol(markerSymbol);
    layer->setRenderer(singleRenderer);
}

void MainWindow::pointAnimatedMarkerSlot()
{
    //添加一个点图层，用于设置图层符号
    QString filename = QStringLiteral("maps/shapefile/places_33S.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //获取图层的渲染器renderer
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    //构造Animated标记层(Animated Marker SymbolLayer)方法一
    //作为示例，用到了path和size两个参数，其他参数使用类似
    QString path = QStringLiteral("resources/fire.gif");
    auto markerSymbolLayer = new QgsAnimatedMarkerSymbolLayer(path,8.0);
#else
    //构造Animated标记层(Animated Marker SymbolLayer)方法二
    //作为示例，用到了imageFile size参数，其他参数使用类似
    QVariantMap mp;
    mp[QString("imageFile")] = "resources/fire.gif";
    mp[QString("size")] = "18.0";
    auto markerSymbolLayer = QgsAnimatedMarkerSymbolLayer::create(mp);
#endif
    //QgsMarkerSymbol构造函数中需要传入QgsSymbolLayerList
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    //构造QgsMarkerSymbol并设置renderer
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    singleRenderer->setSymbol(markerSymbol);
    layer->setRenderer(singleRenderer);
}

void MainWindow::pointEllipseMarkerSlot()
{
    //添加一个点图层，用于设置图层符号
    QString filename = QStringLiteral("maps/shapefile/places_33S.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //获取图层的渲染器renderer
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    //构造Ellipse标记层(Ellipse Marker SymbolLayer)方法一
    auto markerSymbolLayer = new QgsEllipseSymbolLayer();
    markerSymbolLayer->setShape(QgsEllipseSymbolLayer::Circle);
    markerSymbolLayer->setFillColor(QColor("pink"));
    markerSymbolLayer->setSymbolWidth(6.0);
    markerSymbolLayer->setSymbolHeight(6.0);
#else
    //构造Ellipse标记层(Ellipse Marker SymbolLayer)方法二
    //作为示例，用到了imageFile size参数，其他参数使用类似
    QVariantMap mp;
    mp[QString("symbol_name")] = "circle";
    mp[QString("color")] = "pink";
    mp[QString("symbol_width")] = "16.0";
    mp[QString("symbol_height")] = "6.0";
    auto markerSymbolLayer = QgsEllipseSymbolLayer::create(mp);
#endif
    //QgsMarkerSymbol构造函数中需要传入QgsSymbolLayerList
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    //构造QgsMarkerSymbol并设置renderer
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    singleRenderer->setSymbol(markerSymbol);
    layer->setRenderer(singleRenderer);
}

void MainWindow::pointFontMarkerSlot()
{
    //添加一个点图层，用于设置图层符号
    QString filename = QStringLiteral("maps/shapefile/places_33S.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //获取图层的渲染器renderer
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    //构造Font标记层(Font Marker SymbolLayer)方法一
    auto markerSymbolLayer = new QgsFontMarkerSymbolLayer(QStringLiteral("Mukti"),QChar('@'),6.0,QColor("pink"));
#else
    //构造Font标记层(Font Marker SymbolLayer)方法二
    QVariantMap mp;
    mp[QString("font")] = "Mukti";
    mp[QString("chr")] = "@";
    mp[QString("size")] = "16.0";
    mp[QString("color")] = "pink";
    auto markerSymbolLayer = QgsFontMarkerSymbolLayer::create(mp);
#endif
    //QgsMarkerSymbol构造函数中需要传入QgsSymbolLayerList
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    //构造QgsMarkerSymbol并设置renderer
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    singleRenderer->setSymbol(markerSymbol);
    layer->setRenderer(singleRenderer);
}

void MainWindow::pointFilledMarkerSlot()
{
    //添加一个点图层，用于设置图层符号
    QString filename = QStringLiteral("maps/shapefile/places_33S.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //获取图层的渲染器renderer
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    //构造Fill标记层(Fill Marker SymbolLayer)方法一
    auto markerSymbolLayer = new QgsFilledMarkerSymbolLayer(Qgis::MarkerShape::Trapezoid,6.0);
    markerSymbolLayer->setColor(QColor("pink"));
    //上边两行已经能够实现功能，因为在QgsFilledMarkerSymbolLayer构造函数中，默认会创建QgsFillSymbol
    //这里是为了演示如何使用setSubSymbol和QgsFillSymbol
    auto fillSymbolLayer = new QgsSimpleFillSymbolLayer(QColor("pink"));
    QgsSymbolLayerList fillLayerList;
    fillLayerList << fillSymbolLayer;
    auto fillSymbol = new QgsFillSymbol(fillLayerList);
    markerSymbolLayer->setSubSymbol(fillSymbol);
#else
    //构造Fill标记层(Fill Marker SymbolLayer)方法二
    QVariantMap mp;
    mp[QString("name")] = "trapezoid";
    mp[QString("size")] = "16.0";
    mp[QString("color")] = "pink";
    auto markerSymbolLayer = QgsFilledMarkerSymbolLayer::create(mp);
#endif
    //QgsMarkerSymbol构造函数中需要传入QgsSymbolLayerList
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    //构造QgsMarkerSymbol并设置renderer
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    singleRenderer->setSymbol(markerSymbol);
    layer->setRenderer(singleRenderer);

}

void MainWindow::pointGeometryGeneratorMarkerSlot()
{
    //添加一个点图层，用于设置图层符号
    QString filename = QStringLiteral("maps/shapefile/places_33S.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //获取图层的渲染器renderer
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
    //构造标记层(Marker SymbolLayer)
    QVariantMap mp;
    mp[QString("geometryModifier")] = "buffer($geometry,100)";
    mp[QString("SymbolType")] = "Marker";
    auto markerSymbolLayer = QgsGeometryGeneratorSymbolLayer::create(mp);

    //QgsMarkerSymbol构造函数中需要传入QgsSymbolLayerList
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    //构造QgsMarkerSymbol并设置renderer
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    singleRenderer->setSymbol(markerSymbol);
    layer->setRenderer(singleRenderer);
}

void MainWindow::pointVectorFieldMarkerSlot()
{
    //添加一个点图层，用于设置图层符号
    QString filename = QStringLiteral("maps/shapefile/places_33S.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //获取图层的渲染器renderer
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto markerSymbolLayer = new QgsVectorFieldSymbolLayer();
    markerSymbolLayer->setXAttribute(QStringLiteral("IN_SWD"));
    markerSymbolLayer->setYAttribute(QStringLiteral("IS_TOWN"));
    markerSymbolLayer->setScale(10.0);
#else
    QVariantMap mp;
    mp[QString("x_attribute")] = QStringLiteral("IN_SWD");
    mp[QString("y_attribute")] = QStringLiteral("IS_TOWN");
    mp[QString("scale")] = QStringLiteral("10.0");
    auto markerSymbolLayer = QgsVectorFieldSymbolLayer::create(mp);
#endif
    //QgsMarkerSymbol构造函数中需要传入QgsSymbolLayerList
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    //构造QgsMarkerSymbol并设置renderer
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    singleRenderer->setSymbol(markerSymbol);
    layer->setRenderer(singleRenderer);
}

void MainWindow::pointMaskSlot()
{
    //添加两个图层，用于设置图层符号
    QString filename = QStringLiteral("maps/shapefile/myplaces.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    QString reffilename = QStringLiteral("maps/shapefile/farms_33S.shp");
    QFileInfo refff(reffilename);
    QgsVectorLayer* reflayer = (QgsVectorLayer*)mApp->addVectorLayer(reffilename,ff.baseName());

    //获取图层的渲染器renderer
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
    //构造QgsSymbolLayerReference
    QList< QgsSymbolLayerReference > maskedLayers;
    QgsSymbolLayerId slid("",0);
    QgsSymbolLayerReference slRef(reflayer->id(),slid);
    maskedLayers <<  slRef;
#if 0
    auto markerSymbolLayer = new QgsMaskMarkerSymbolLayer();
    markerSymbolLayer->setMasks(maskedLayers);
#else
    QVariantMap mp;
    mp[QString("mask_symbollayers")] = symbolLayerReferenceListToString(maskedLayers);
    auto markerSymbolLayer = QgsMaskMarkerSymbolLayer::create(mp);
#endif
    //QgsMarkerSymbol构造函数中需要传入QgsSymbolLayerList
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    //构造QgsMarkerSymbol并设置renderer
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    singleRenderer->setSymbol(markerSymbol);
    layer->setRenderer(singleRenderer);
}

void MainWindow::pointCategorizedSlot()
{
    //添加一个点图层，设置图层符号为分类渲染
    QString filename = QStringLiteral("maps/shapefile/places_33S.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //从图层获取分类渲染器并转换为分类渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsCategorizedSymbolRenderer *categorizedRenderer = QgsCategorizedSymbolRenderer::convertFromRenderer(layerRenderer);

    QString attrName = "IS_TOWN";
    categorizedRenderer->setClassAttribute(attrName);

    QList<QVariant> uniqueValues;
    const int idx = layer->fields().lookupField( attrName );
    uniqueValues = qgis::setToList( layer->uniqueValues( idx ) );
    QgsSymbol *symbol = QgsSymbol::defaultSymbol( layer->geometryType() );
    QgsCategoryList cats = QgsCategorizedSymbolRenderer::createCategories( uniqueValues, symbol, layer, attrName );
    for(int i = 0;i < cats.size();++i)
    {
        QgsRendererCategory cat = cats.at(i);
        categorizedRenderer->addCategory(cat);
    }
#if 0
    //使用渐变颜色指定类别颜色
    QgsGradientColorRamp *ramp = new QgsGradientColorRamp(QColor( 255, 255, 255 ), QColor( 255, 0, 0 ));
#else
    //使用随机颜色设定类别颜色
    QgsRandomColorRamp *ramp = new QgsRandomColorRamp();
#endif
    categorizedRenderer->updateColorRamp(ramp);
    //设置每个分类的图例Legend
    categorizedRenderer->updateCategoryLabel(0,"Not Town");
    categorizedRenderer->updateCategoryLabel(1,"Town");
    //设置每个分类的符号
#if 0
    QVariantMap mp;
    mp[QString("name")] = "resources/plane.svg";
    mp[QString("outline_color")] = "pink";
    mp[QString("size")] = "8.0";
    auto markerSymbolLayer = QgsSvgMarkerSymbolLayer::create(mp);
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    categorizedRenderer->updateRangeSymbol(0, markerSymbol);
#endif
    layer->setRenderer(categorizedRenderer);
}

void MainWindow::pointGraduatedSlot()
{
    //添加一个点图层，设置图层符号为分级渲染
    QString filename = QStringLiteral("maps/shapefile/myplaces.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //从图层获取分类渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsGraduatedSymbolRenderer *graduatedRenderer = QgsGraduatedSymbolRenderer::convertFromRenderer(layerRenderer);
    //从图层中选择一个属性进行分类
    //分类的方法有Quantile EqualInterval Jenks StdDev Pretty Logarithmic Fixed
    //图层有一个属性叫做RAINFALL，是指该点的降雨量
    QString attrName = "RAINFALL";
    int nclasses = 5;   //分为两类
    const QString methodId = "Quantile";    //设置分类方法
    QgsClassificationMethod *method = QgsApplication::classificationMethodRegistry()->method( methodId );
    Q_ASSERT( method );
    //首先设置一个默认符号
    QgsSymbol *symbol = QgsSymbol::defaultSymbol( layer->geometryType() );
    graduatedRenderer->addClass(symbol);
    //设置用于分类的属性名称
    graduatedRenderer->setClassAttribute(attrName);
    //然后设置符号表颜色
#if 1
    //渐变色
    QgsColorRamp *ramp = new QgsGradientColorRamp( QColor( 255, 255, 255 ), QColor( 255, 0, 0 ) );
#else
    //完全随机
    QgsColorRamp *ramp = new QgsRandomColorRamp();
#endif
    graduatedRenderer->setSourceColorRamp(ramp);
    //设置符号大小
    graduatedRenderer->setSymbolSizes( 1, 8 );
    //设置分类方法
    graduatedRenderer->setClassificationMethod( method );
    //进行分类
    graduatedRenderer->updateClasses( layer, nclasses );
    //设置每个分类的图例Legend
    graduatedRenderer->updateRangeLabel(0,"small");
    graduatedRenderer->updateRangeLabel(1,"medium");
    //设置每个分类的符号
#if 0
    QVariantMap mp;
    mp[QString("name")] = "resources/plane.svg";
    mp[QString("outline_color")] = "pink";
    mp[QString("size")] = "8.0";
    auto markerSymbolLayer = QgsSvgMarkerSymbolLayer::create(mp);
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    graduatedRenderer->updateRangeSymbol(0, markerSymbol);
#endif
    layer->setRenderer(graduatedRenderer);
}

void MainWindow::pointRuleBasedSlot()
{
    //添加一个点图层，设置图层符号为规则渲染
    QString filename = QStringLiteral("maps/shapefile/myplaces.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsRuleBasedRenderer *ruleBasedRenderer = QgsRuleBasedRenderer::convertFromRenderer(layerRenderer);
    //QgsRuleBasedRenderer会生成一条规则Rule，这里直接删除，后边直接添加两条新规则
    //也可以获取生成的规则，然后编辑
    ruleBasedRenderer->rootRule()->removeChildAt(0);

    //生成一条新规则
    QgsSymbol *s = QgsSymbol::defaultSymbol( layer->geometryType() );
    QgsRuleBasedRenderer::Rule *ifrule = new QgsRuleBasedRenderer::Rule( s );
    //设置表达式
    QString filter = QStringLiteral(" \"IS_TOWN\" = 1");
    ifrule->setFilterExpression( filter );
    //设置图例
    ifrule->setLabel( QStringLiteral("TOWN") );
    //使用Simple Marker生成符号，然后为新规则设置符号
    QVariantMap mp;
    mp[QString("name")] = QStringLiteral("circle");
    mp[QString("color")] = QStringLiteral("red");
    QgsMarkerSymbol *townSymbol = QgsMarkerSymbol::createSimple(mp);
    ifrule->setSymbol( townSymbol );
    //将规则添加到renderer中
    ruleBasedRenderer->rootRule()->appendChild(ifrule);

    //添加新规则
    QgsSymbol *s1 = QgsSymbol::defaultSymbol( layer->geometryType() );
    QgsRuleBasedRenderer::Rule *elserule = new QgsRuleBasedRenderer::Rule( s1 );
    //因为取值只有0 1,所以直接设置IsElse为true
    elserule->setIsElse(true);
    elserule->setLabel(QStringLiteral(" Not TOWN"));
    //为规则设置符号
    mp[QString("color")] = QStringLiteral("black");
    QgsMarkerSymbol *nottownSymbol = QgsMarkerSymbol::createSimple(mp);
    elserule->setSymbol(nottownSymbol);
    //将规则添加到renderer中
    ruleBasedRenderer->rootRule()->appendChild(elserule);
    //图层设置渲染器为Rule Based
    layer->setRenderer(ruleBasedRenderer);
}

void MainWindow::pointDisplacementSlot()
{
    //添加测试图层
    QString filename = QStringLiteral("maps/shapefile/myplaces.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsPointDisplacementRenderer *pointDisplacementRenderer = QgsPointDisplacementRenderer::convertFromRenderer(layerRenderer);

    //设置渲染器
    QVariantMap mp;
    mp[QString("name")] = QStringLiteral("circle");
    mp[QString("color")] = QStringLiteral("red");
    QgsMarkerSymbol *symbol = QgsMarkerSymbol::createSimple(mp);
    //中心点符号
    pointDisplacementRenderer->setCenterSymbol(symbol);
    //中心点的颜色
    pointDisplacementRenderer->setCircleColor(QColor("black"));
    //点要素围绕在中心符号的方式
    pointDisplacementRenderer->setPlacement(QgsPointDisplacementRenderer::Ring);
    //形成中心符号的距离阈值
    pointDisplacementRenderer->setTolerance(3.0);

    layer->setRenderer(pointDisplacementRenderer);
}

void MainWindow::pointClusterSlot()
{
    //添加测试图层
    QString filename = QStringLiteral("maps/shapefile/myplaces.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsPointClusterRenderer *pointClusterRenderer = QgsPointClusterRenderer::convertFromRenderer(layerRenderer);
    //设置clusterSymbol,其实以下代码都不用写，默认就是这个效果。
    //这里写出来只是演示
#if 0
    auto markerSymbol = new QgsMarkerSymbol();
    markerSymbol->setSize( 4 );
    markerSymbol->setColor( QColor( 245, 75, 80 ) );

    QgsFontMarkerSymbolLayer *fm = new QgsFontMarkerSymbolLayer();
    fm->setFontFamily( QFont().defaultFamily() );
    fm->setColor( QColor( 255, 255, 255 ) );
    fm->setSize( 3.2 );
    fm->setOffset( QPointF( 0, -0.4 ) );
    fm->setDataDefinedProperty( QgsSymbolLayer::PropertyCharacter, QgsProperty::fromExpression( QStringLiteral( "@cluster_size" ) ) );
    markerSymbol->insertSymbolLayer( 1, fm );
    pointClusterRenderer->setClusterSymbol(markerSymbol);
#else
    //设置渲染器
    QVariantMap mp;
    mp[QString("name")] = QStringLiteral("circle");
    mp[QString("color")] = QStringLiteral("red");
    mp[QString("size")] = QStringLiteral("6.0");
    auto simpleSymbolLayer = QgsSimpleMarkerSymbolLayer::create(mp);
    QVariantMap mp1;
    mp1[QString("font")] = QStringLiteral("Mukti");
    mp1[QString("color")] = QStringLiteral("white");
    mp1[QString("offset")] = QStringLiteral("0, -0.8");
    auto fontSymbolLayer = QgsFontMarkerSymbolLayer::create(mp1);
    fontSymbolLayer->setDataDefinedProperty( QgsSymbolLayer::PropertyCharacter, QgsProperty::fromExpression( QStringLiteral( "@cluster_size" ) ) );
    QgsSymbolLayerList layerList;
    layerList << simpleSymbolLayer << fontSymbolLayer;
    auto markerSymbol = new QgsMarkerSymbol(layerList);
    pointClusterRenderer->setClusterSymbol(markerSymbol);
#endif
    layer->setRenderer(pointClusterRenderer);
}

void MainWindow::pointHeatmapSlot()
{
    //添加测试图层
    QString filename = QStringLiteral("maps/shapefile/myplaces.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsHeatmapRenderer *heatmapRenderer = QgsHeatmapRenderer::convertFromRenderer(layerRenderer);

    heatmapRenderer->setColorRamp(new QgsGradientColorRamp(QColor(255, 255, 255),QColor(0, 0, 0)));
    heatmapRenderer->setRadius(10.0);
    heatmapRenderer->setRenderQuality(1);

    layer->setRenderer(heatmapRenderer);
}

