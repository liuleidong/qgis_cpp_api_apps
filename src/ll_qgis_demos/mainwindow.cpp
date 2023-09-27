#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "dockwidget.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QPlainTextEdit>

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
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    mApp->initialize(this);    
    ui->menuViews->addAction(mApp->layerTreeDock()->toggleViewAction());
    initDockWidget();
    initMapCanvasDockWidget();
    initSymbolDockWidget();
    initLineSymbolDockWidget();
    initPolygonSymbolDockWidget();
    initAnnotationDockWidget();
    initDiagramDockWidget();
    initRasterSymbolDockWidget();
    initVectorDataDockWidget();
    initProcessingDockWidget();
    initMapTools();
}

void MainWindow::initDockWidget()
{
    mDockWidget = new DockWidget(this);
    connect(mDockWidget,&DockWidget::addShpSignal,this,&MainWindow::addShpSlot);
    connect(mDockWidget,&DockWidget::addGpxSignal,this,&MainWindow::addGpxSlot);
    connect(mDockWidget,&DockWidget::addGpkgSignal,this,&MainWindow::addGpkgSlot);
    connect(mDockWidget,&DockWidget::addGeoJsonSignal,this,&MainWindow::addGeoJsonSlot);
    connect(mDockWidget,&DockWidget::addGmlSignal,this,&MainWindow::addGmlSlot);
    connect(mDockWidget,&DockWidget::addKmlSignal,this,&MainWindow::addKmlSlot);
    connect(mDockWidget,&DockWidget::addDxfSignal,this,&MainWindow::addDxfSlot);
    connect(mDockWidget,&DockWidget::addCoverageSignal,this,&MainWindow::addCoverageSlot);
    connect(mDockWidget,&DockWidget::addGpx1Signal,this,&MainWindow::addGpx1Slot);
    connect(mDockWidget,&DockWidget::addCsvSignal,this,&MainWindow::addCsvSlot);
    connect(mDockWidget,&DockWidget::addSpatiaLite,this,&MainWindow::addSpatiaLiteSlot);
    connect(mDockWidget,&DockWidget::addMemorySignal,this,&MainWindow::addMemorySlot);
    connect(mDockWidget,&DockWidget::addWfsSignal,this,&MainWindow::addWfsSlot);
    connect(mDockWidget,&DockWidget::addRasterSignal,this,&MainWindow::addRasterSlot);
    connect(mDockWidget,&DockWidget::addGpkg1Signal,this,&MainWindow::addGpkg1Slot);
    connect(mDockWidget,&DockWidget::addWmsSignal,this,&MainWindow::addWmsSlot);
    connect(mDockWidget,&DockWidget::addGdalOfflineSignal,this,&MainWindow::addGdalOfflineSlot);

    this->addDockWidget(Qt::LeftDockWidgetArea,mDockWidget);
    ui->menuViews->addAction(mDockWidget->toggleViewAction());
}

void MainWindow::initMapCanvasDockWidget()
{
    mMapCanvasDockWidget = new MapCanvasDockWidget(this);
    connect(mMapCanvasDockWidget,&MapCanvasDockWidget::canvasBrSignal,this,&MainWindow::canvasBrSlot);
    connect(mMapCanvasDockWidget,&MapCanvasDockWidget::canvasCenterSignal,this,&MainWindow::canvasCenterSlot);
    connect(mMapCanvasDockWidget,&MapCanvasDockWidget::canvasRotationSignal,this,&MainWindow::canvasRotationSlot);
    connect(mMapCanvasDockWidget,&MapCanvasDockWidget::rubberBandLineSignal,this,&MainWindow::rubberBandLineSlot);
    connect(mMapCanvasDockWidget,&MapCanvasDockWidget::rubberBandPolygonSignal,this,&MainWindow::rubberBandPolygonSlot);
    connect(mMapCanvasDockWidget,&MapCanvasDockWidget::vertexMarkerSignal,this,&MainWindow::vertexMarkerSlot);
    connect(mMapCanvasDockWidget,&MapCanvasDockWidget::mapToolPanSignal,this,&MainWindow::mapToolPanSlot);
    connect(mMapCanvasDockWidget,&MapCanvasDockWidget::mapToolZoomOutSignal,this,&MainWindow::mapToolZoomOutSlot);
    connect(mMapCanvasDockWidget,&MapCanvasDockWidget::mapToolZoomInSignal,this,&MainWindow::mapToolZoomInSlot);

    this->addDockWidget(Qt::LeftDockWidgetArea,mMapCanvasDockWidget);
    ui->menuViews->addAction(mMapCanvasDockWidget->toggleViewAction());
}

void MainWindow::initSymbolDockWidget()
{
    mSymbolDockWidget = new SymbolDockWidget(this);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointSimpleMarkerSignal,this,&MainWindow::pointSimpleMarkerSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointSvgMarkerSignal,this,&MainWindow::pointSvgMarkerSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointRasterMarkerSignal,this,&MainWindow::pointRasterMarkerSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointAnimatedMarkerSignal,this,&MainWindow::pointAnimatedMarkerSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointEllipseMarkerSignal,this,&MainWindow::pointEllipseMarkerSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointFontMarkerSignal,this,&MainWindow::pointFontMarkerSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointFilledMarkerSignal,this,&MainWindow::pointFilledMarkerSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointGeometryGeneratorMarkerSignal,this,&MainWindow::pointGeometryGeneratorMarkerSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointVectorFieldMarkerSignal,this,&MainWindow::pointVectorFieldMarkerSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointMaskSignal,this,&MainWindow::pointMaskSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointCategorizedSignal,this,&MainWindow::pointCategorizedSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointGraduatedSignal,this,&MainWindow::pointGraduatedSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointRuleBasedSignal,this,&MainWindow::pointRuleBasedSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointDisplacementSignal,this,&MainWindow::pointDisplacementSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointClusterSignal,this,&MainWindow::pointClusterSlot);
    connect(mSymbolDockWidget,&SymbolDockWidget::pointHeatmapSignal,this,&MainWindow::pointHeatmapSlot);

    this->addDockWidget(Qt::LeftDockWidgetArea,mSymbolDockWidget);
    ui->menuViews->addAction(mSymbolDockWidget->toggleViewAction());
}

void MainWindow::initLineSymbolDockWidget()
{
    mLineSymbolDockWidget = new LineSymbolDockWidget(this);

    connect(mLineSymbolDockWidget,&LineSymbolDockWidget::lineSimplelineSignal,this,&MainWindow::lineSimplelineSlot);
    connect(mLineSymbolDockWidget,&LineSymbolDockWidget::lineArrowSignal,this,&MainWindow::lineArrowSlot);
    connect(mLineSymbolDockWidget,&LineSymbolDockWidget::lineInterpolatedSignal,this,&MainWindow::lineInterpolatedSlot);
    connect(mLineSymbolDockWidget,&LineSymbolDockWidget::lineHashedSignal,this,&MainWindow::lineHashedSlot);
    connect(mLineSymbolDockWidget,&LineSymbolDockWidget::lineMarkerSignal,this,&MainWindow::lineMarkerSlot);
    connect(mLineSymbolDockWidget,&LineSymbolDockWidget::lineRasterSignal,this,&MainWindow::lineRasterSlot);
    connect(mLineSymbolDockWidget,&LineSymbolDockWidget::lineBurstSignal,this,&MainWindow::lineBurstSlot);
    this->addDockWidget(Qt::RightDockWidgetArea,mLineSymbolDockWidget);
    ui->menuViews->addAction(mLineSymbolDockWidget->toggleViewAction());
}

void MainWindow::initPolygonSymbolDockWidget()
{
    mPolygonDockWidget = new PolygonDockWidget(this);

    connect(mPolygonDockWidget,&PolygonDockWidget::polygonSimplefillSignal,this,&MainWindow::polygonSimplefillSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonCentroidSignal,this,&MainWindow::polygonCentroidSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonGradientSignal,this,&MainWindow::polygonGradientSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonLinePatternSignal,this,&MainWindow::polygonLinePatternSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonPointPatternSignal,this,&MainWindow::polygonPointPatternSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonRasterImageSignal,this,&MainWindow::polygonRasterImageSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonSvgSignal,this,&MainWindow::polygonSvgSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonRandomMarkerSignal,this,&MainWindow::polygonRandomMarkerSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonShapeburstSignal,this,&MainWindow::polygonShapeburstSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonOutlineSimpleSignal,this,&MainWindow::polygonOutlineSimpleSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonOutlineArrowSignal,this,&MainWindow::polygonOutlineArrowSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonOutlineHashedSignal,this,&MainWindow::polygonOutlineHashedSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonOutlineInterpolatedSignal,this,&MainWindow::polygonOutlineInterpolatedSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonOutlineLineburstSignal,this,&MainWindow::polygonOutlineLineburstSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonOutlineMarkerSignal,this,&MainWindow::polygonOutlineMarkerSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonOutlineRasterSignal,this,&MainWindow::polygonOutlineRasterSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygonInvertedRendererSignal,this,&MainWindow::polygonInvertedRendererSlot);
    connect(mPolygonDockWidget,&PolygonDockWidget::polygon25DSignal,this,&MainWindow::polygon25DSlot);
    this->addDockWidget(Qt::RightDockWidgetArea,mPolygonDockWidget);
    ui->menuViews->addAction(mPolygonDockWidget->toggleViewAction());
}

void MainWindow::initAnnotationDockWidget()
{
    mAnnotationDockWidget = new AnnotationDockWidget(this);

    connect(mAnnotationDockWidget,&AnnotationDockWidget::labelSignal,this,&MainWindow::labelSlot);
    connect(mAnnotationDockWidget,&AnnotationDockWidget::annotationTextSignal,this,&MainWindow::annotationTextSlot);
    connect(mAnnotationDockWidget,&AnnotationDockWidget::annotationPointSignal,this,&MainWindow::annotationPointSlot);
    connect(mAnnotationDockWidget,&AnnotationDockWidget::annotationLineSignal,this,&MainWindow::annotationLineSlot);
    connect(mAnnotationDockWidget,&AnnotationDockWidget::annotationPolygonSignal,this,&MainWindow::annotationPolygonSlot);

    this->addDockWidget(Qt::RightDockWidgetArea,mAnnotationDockWidget);
    ui->menuViews->addAction(mAnnotationDockWidget->toggleViewAction());
}

void MainWindow::initDiagramDockWidget()
{
    mDiagramDockWidget = new DiagramDockWidget(this);

    connect(mDiagramDockWidget,&DiagramDockWidget::diagramPieSignal,this,&MainWindow::diagramPieSlot);
    connect(mDiagramDockWidget,&DiagramDockWidget::diagramTextSignal,this,&MainWindow::diagramTextSlot);
    connect(mDiagramDockWidget,&DiagramDockWidget::diagramHistogramSignal,this,&MainWindow::diagramHistogramSlot);
    connect(mDiagramDockWidget,&DiagramDockWidget::diagramStackedBarSignal,this,&MainWindow::diagramStackedBarSlot);

    this->addDockWidget(Qt::RightDockWidgetArea,mDiagramDockWidget);
    ui->menuViews->addAction(mDiagramDockWidget->toggleViewAction());
}

void MainWindow::initRasterSymbolDockWidget()
{
    mRasterSymbolDockWidget = new RasterSymbolDockWidget(this);

    connect(mRasterSymbolDockWidget,&RasterSymbolDockWidget::rasterMultibandSignal,this,&MainWindow::rasterMultibandSlot);
    connect(mRasterSymbolDockWidget,&RasterSymbolDockWidget::rasterPalettedSignal,this,&MainWindow::rasterPalettedSlot);
    connect(mRasterSymbolDockWidget,&RasterSymbolDockWidget::rasterSinglebandGraySignal,this,&MainWindow::rasterSinglebandGraySlot);
    connect(mRasterSymbolDockWidget,&RasterSymbolDockWidget::rasterSinglebandPseudoSignal,this,&MainWindow::rasterSinglebandPseudoSlot);
    connect(mRasterSymbolDockWidget,&RasterSymbolDockWidget::rasterHillshadeSignal,this,&MainWindow::rasterHillshadeSlot);
    connect(mRasterSymbolDockWidget,&RasterSymbolDockWidget::rasterContoursSignal,this,&MainWindow::rasterContoursSlot);

    this->addDockWidget(Qt::RightDockWidgetArea,mRasterSymbolDockWidget);
    ui->menuViews->addAction(mRasterSymbolDockWidget->toggleViewAction());
}

void MainWindow::initVectorDataDockWidget()
{
    mVectorDataDockWidget = new VectorDataDockWidget(this);

    connect(mVectorDataDockWidget,&VectorDataDockWidget::getFieldsSignal,this,&MainWindow::getFieldsSlot);
    connect(mVectorDataDockWidget,&VectorDataDockWidget::getFeaturesSignal,this,&MainWindow::getFeaturesSlot);
    connect(mVectorDataDockWidget,&VectorDataDockWidget::selectFeaturesSignal,this,&MainWindow::selectFeaturesSlot);
    connect(mVectorDataDockWidget,&VectorDataDockWidget::spatialIndexSignal,this,&MainWindow::spatialIndexSlot);
    connect(mVectorDataDockWidget,&VectorDataDockWidget::distanceAreaSignal,this,&MainWindow::distanceAreaSlot);
    connect(mVectorDataDockWidget,&VectorDataDockWidget::vectorFileWriterSignal,this,&MainWindow::vectorFileWriterSlot);

    this->addDockWidget(Qt::RightDockWidgetArea,mVectorDataDockWidget);
    ui->menuViews->addAction(mVectorDataDockWidget->toggleViewAction());
}

void MainWindow::initProcessingDockWidget()
{
    mProcessingDockWidget = new ProcessingDockWidget(this);

    connect(mProcessingDockWidget,&ProcessingDockWidget::processingClipSignal,this,&MainWindow::processingClipSlot);
    connect(mProcessingDockWidget,&ProcessingDockWidget::processingRandomPointsSignal,this,&MainWindow::processingRandomPointsSlot);
    connect(mProcessingDockWidget,&ProcessingDockWidget::processingBufferSignal,this,&MainWindow::processingBufferSlot);

    this->addDockWidget(Qt::RightDockWidgetArea,mProcessingDockWidget);
    ui->menuViews->addAction(mProcessingDockWidget->toggleViewAction());
}

void MainWindow::initMapTools()
{
    mMapToolPan = new QgsMapToolPan(mApp->mapCanvas());
    mMapToolZoomOut = new QgsMapToolZoom(mApp->mapCanvas(),false);
    mMapToolZoomIn = new QgsMapToolZoom(mApp->mapCanvas(),true);
}

void MainWindow::addShpSlot()
{
    QString filename = QFileDialog::getOpenFileName();
    if(filename.isNull())//如果未选择文件则返回
        return;

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
    QgsVectorLayer* points_gpkgLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=Layer1"),"Layer1","ogr");
    QgsVectorLayer* points_smallLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=Layer2"),"Layer2","ogr");
    QList<QgsMapLayer *> mapLayers;
    mapLayers << points_gpkgLayer << points_smallLayer;
    QgsProject::instance()->addMapLayers(mapLayers);
}

void MainWindow::addDxfSlot()
{
    QString filename = QStringLiteral("maps/points_lines_3d.dxf");
    QFileInfo ff(filename);
    //创建图层
    QgsVectorLayer* points_gpkgLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=entities|geometrytype=LineString"),"entities","ogr");
    QgsVectorLayer* points_smallLayer = new QgsVectorLayer(QString("%1%2").arg(filename).arg("|layername=entities|geometrytype=Point"),"entities","ogr");
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
    QString uri = QString("file://%1/%2%3").arg(QCoreApplication::applicationDirPath()).arg(filename).arg("?type=csv&xField=longitude&yField=latitude&crs=EPSG:4326");
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
    QString url = QStringLiteral("https://webst01.is.autonavi.com/appmaptile?style=6&x={x}&y={y}&z={z}");
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
    QString filename = QStringLiteral("maps/gaode20230630/tms.xml");
    QFileInfo ff(filename);
    QgsRasterLayer* layer = new QgsRasterLayer(filename,"gaodeoffline","gdal");
    QgsProject::instance()->addMapLayer(layer);
}

void MainWindow::canvasBrSlot()
{
    mApp->mapCanvas()->setCanvasColor(QColor::fromHsl(rand()%360,rand()%256,rand()%200));
}

void MainWindow::canvasCenterSlot()
{
    mApp->mapCanvas()->setCenter(QgsPointXY(131.194,43.265));
}

void MainWindow::canvasRotationSlot()
{
    mApp->mapCanvas()->setRotation(45.0);
}

void MainWindow::rubberBandLineSlot()
{
    //添加shapefile
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* vecLayer = new QgsVectorLayer(filename,ff.baseName(),"ogr");
    QgsProject::instance()->addMapLayer(vecLayer);
    //定义点
    QgsPointXY startPoint(20.33989,-33.86805);
    QgsPointXY endPoint(20.47760,-33.86676);
    //新建QgsRubberBand，注意类型是LineGeometry
    QgsRubberBand *rubberBand = new QgsRubberBand(mApp->mapCanvas(),QgsWkbTypes::LineGeometry);
    //将点添加到rubberband中
    rubberBand->addPoint(startPoint);
    rubberBand->addPoint(endPoint);
    //更改线宽，颜色等属性
    rubberBand->setWidth(4);
    rubberBand->setColor(QColor(222,155,67));
    //定义一个点类型的RubberBand
    QgsRubberBand *rubberBandPoints = new QgsRubberBand(mApp->mapCanvas(),QgsWkbTypes::PointGeometry);
    rubberBandPoints->addPoint(startPoint);
    rubberBandPoints->addPoint(endPoint);
    rubberBandPoints->setWidth(6);
    rubberBandPoints->setColor(QColor(222,155,67));
    //显示出来
    rubberBand->show();
    rubberBandPoints->show();
}

void MainWindow::rubberBandPolygonSlot()
{
    //添加shapefile
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* vecLayer = new QgsVectorLayer(filename,ff.baseName(),"ogr");
    QgsProject::instance()->addMapLayer(vecLayer);
    //定义三个点
    QgsPointXY point1(20.33989,-33.86805);
    QgsPointXY point2(20.47760,-33.86676);
    QgsPointXY point3(20.39973,-33.80499);
    //新建PolygonGeometry类型的RubberBand
    QgsRubberBand *rubberBand = new QgsRubberBand(mApp->mapCanvas(),QgsWkbTypes::PolygonGeometry);
    //添加三个点
    rubberBand->addPoint(point1);
    rubberBand->addPoint(point2);
    rubberBand->addPoint(point3);
    //设置线宽颜色等属性
    rubberBand->setWidth(4);
    rubberBand->setColor(QColor(222,155,67));
    QgsRubberBand *rubberBandPoints = new QgsRubberBand(mApp->mapCanvas(),QgsWkbTypes::PointGeometry);
    rubberBandPoints->addPoint(point1);
    rubberBandPoints->addPoint(point2);
    rubberBandPoints->addPoint(point3);
    rubberBandPoints->setWidth(6);
    rubberBandPoints->setColor(QColor(222,155,67));
    rubberBand->show();
    rubberBandPoints->show();
}

void MainWindow::vertexMarkerSlot()
{
    //添加shapefile
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    QgsVectorLayer* vecLayer = new QgsVectorLayer(filename,ff.baseName(),"ogr");
    QgsProject::instance()->addMapLayer(vecLayer);
    //构造QgsVertexMarker并设置属性
    QgsVertexMarker *vm = new QgsVertexMarker( mApp->mapCanvas() );
    vm->setCenter( QgsPointXY(20.33989,-33.86805) );
    vm->setIconType( QgsVertexMarker::ICON_DOUBLE_TRIANGLE );
    vm->setPenWidth( 2 );
    vm->setColor( Qt::green );
    vm->setZValue( vm->zValue() + 1 );
}

void MainWindow::mapToolPanSlot()
{
    mApp->mapCanvas()->setMapTool(mMapToolPan);
}

void MainWindow::mapToolZoomInSlot()
{
    mApp->mapCanvas()->setMapTool(mMapToolZoomIn);
}

void MainWindow::mapToolZoomOutSlot()
{
    mApp->mapCanvas()->setMapTool(mMapToolZoomOut);
}

void MainWindow::pointSimpleMarkerSlot()
{
    //添加测试图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/places_33S.shp"));
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
    mp[QString("size")] = "4.0";
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/places_33S.shp"));
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
    mp[QString("size")] = "8.0";
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/places_33S.shp"));
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
    mp[QString("size")] = "8.0";
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/places_33S.shp"));
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
    mp[QString("size")] = "8.0";
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/places_33S.shp"));
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
    mp[QString("symbol_width")] = "6.0";
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/places_33S.shp"));
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
    mp[QString("size")] = "6.0";
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/places_33S.shp"));
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
    mp[QString("size")] = "6.0";
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/places_33S.shp"));
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/places_33S.shp"));
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/myplaces.shp"));
    QgsVectorLayer* reflayer = addTestShape(QStringLiteral("maps/shapefile/farms_33S.shp"));

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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/places_33S.shp"));
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/myplaces.shp"));
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/myplaces.shp"));
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/myplaces.shp"));
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/myplaces.shp"));
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
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/myplaces.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsHeatmapRenderer *heatmapRenderer = QgsHeatmapRenderer::convertFromRenderer(layerRenderer);

    heatmapRenderer->setColorRamp(new QgsGradientColorRamp(QColor(255, 255, 255),QColor(0, 0, 0)));
    heatmapRenderer->setRadius(10.0);
    heatmapRenderer->setRenderQuality(1);

    layer->setRenderer(heatmapRenderer);
}

QgsRasterLayer *MainWindow::addTestRaster(const QString &rasterFile)
{
    QString filename = QString(rasterFile);
    QFileInfo ff(filename);
    QgsRasterLayer* rlayer = new QgsRasterLayer(filename,ff.baseName(),"gdal");
    QgsProject::instance()->addMapLayer(rlayer);
    return rlayer;
}

QgsVectorLayer* MainWindow::addTestShape(const QString &shapeFile)
{
    QString filename = QString(shapeFile);
    QFileInfo ff(filename);
    QgsVectorLayer* layer = new QgsVectorLayer(filename,ff.baseName(),"ogr");
    QgsProject::instance()->addMapLayer(layer);
    return layer;
}

QgsAnnotationLayer *MainWindow::addTestAnnotationLayer()
{
    QString name = QStringLiteral("Annotation");
    QgsAnnotationLayer::LayerOptions options( QgsProject::instance()->transformContext() );
    QgsAnnotationLayer *layer = new QgsAnnotationLayer( name, options );
    layer->setCrs( QgsProject::instance()->crs() );
    QgsProject::instance()->addMapLayer(layer);
    return layer;
}

bool MainWindow::minMaxValuesForBand(int band, QgsRasterDataProvider *provider, double &minValue, double &maxValue) const
{
    if ( !provider )
    {
      return false;
    }

    minValue = 0;
    maxValue = 0;

    //qgis的配置文件
    //LLD_TODO 二次开发是否有配置文件呢?
    const QgsSettings s;
    if ( s.value( QStringLiteral( "/Raster/useStandardDeviation" ), false ).toBool() )
    {
      const QgsRasterBandStats stats = provider->bandStatistics( band, QgsRasterBandStats::Mean | QgsRasterBandStats::StdDev );

      const double stdDevFactor = s.value( QStringLiteral( "/Raster/defaultStandardDeviation" ), 2.0 ).toDouble();
      const double diff = stdDevFactor * stats.stdDev;
      minValue = stats.mean - diff;
      maxValue = stats.mean + diff;
    }
    else
    {
      const QgsRasterBandStats stats = provider->bandStatistics( band, QgsRasterBandStats::Min | QgsRasterBandStats::Max );
      minValue = stats.minimumValue;
      maxValue = stats.maximumValue;
    }
    return true;
}

void MainWindow::lineSimplelineSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/rivers.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);

    //构建QgsSimpleLineSymbolLayer
#if 0
    auto simpleLineSymbolLayer = new QgsSimpleLineSymbolLayer(QColor("black"));
#else
    QVariantMap mp;
    mp["line_color"] = QStringLiteral("black");
    auto simpleLineSymbolLayer = QgsSimpleLineSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << simpleLineSymbolLayer;
    //构造QgsLineSymbol并设置renderer
    auto lineSymbol = new QgsLineSymbol(layerList);
    singleSymbolRenderer->setSymbol(lineSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::lineArrowSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/rivers.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto arrowSymbolLayer  = new QgsArrowSymbolLayer();
#else
    QVariantMap mp;
    mp["head_type"] = QStringLiteral("0");
    mp["arrow_type"] = QStringLiteral("0");
    auto arrowSymbolLayer  = QgsArrowSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << arrowSymbolLayer;
    //构造QgsLineSymbol并设置renderer
    auto lineSymbol = new QgsLineSymbol(layerList);
    singleSymbolRenderer->setSymbol(lineSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::lineInterpolatedSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/rivers.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto interpolatedSymbolLayer = new QgsInterpolatedLineSymbolLayer();
#else
    QVariantMap mp;
    mp["single_color"] = QStringLiteral("black");
    auto interpolatedSymbolLayer = QgsInterpolatedLineSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << interpolatedSymbolLayer;
    //构造QgsLineSymbol并设置renderer
    auto lineSymbol = new QgsLineSymbol(layerList);
    singleSymbolRenderer->setSymbol(lineSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::lineHashedSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/rivers.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto hashedSymbolLayer = new QgsHashedLineSymbolLayer(true,3);
#else
    QVariantMap mp;
    mp["rotate"] = QStringLiteral("1");
    mp["interval"] = QStringLiteral("3");
    auto hashedSymbolLayer = QgsHashedLineSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << hashedSymbolLayer;
    //构造QgsLineSymbol并设置renderer
    auto lineSymbol = new QgsLineSymbol(layerList);
    singleSymbolRenderer->setSymbol(lineSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::lineMarkerSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/rivers.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 1
    auto markerSymbolLayer = new QgsMarkerLineSymbolLayer(true,3);
#else
    QVariantMap mp;
    mp["rotate"] = QStringLiteral("1");
    mp["interval"] = QStringLiteral("3");
    auto markerSymbolLayer = QgsMarkerLineSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    //构造QgsLineSymbol并设置renderer
    auto lineSymbol = new QgsLineSymbol(layerList);
    singleSymbolRenderer->setSymbol(lineSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::lineRasterSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/rivers.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto rasterSymbolLayer = new QgsRasterLineSymbolLayer(QStringLiteral("resources/cock.jpg"));
    rasterSymbolLayer->setWidth(2.0);
#else
    QVariantMap mp;
    mp["imageFile"] = QStringLiteral("resources/cock.jpg");
    mp["line_width"] = QStringLiteral("2.0");
    auto rasterSymbolLayer = QgsRasterLineSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << rasterSymbolLayer;
    //构造QgsLineSymbol并设置renderer
    auto lineSymbol = new QgsLineSymbol(layerList);
    singleSymbolRenderer->setSymbol(lineSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::lineBurstSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/rivers.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
//    auto lineBurstSymbolLayer = new QgsLineburstSymbolLayer(QColor(35,35,35),Qt::white);
//    lineBurstSymbolLayer->setGradientColorType(Qgis::GradientColorSource::SimpleTwoColor);
//    lineBurstSymbolLayer->setWidth(2.0);
    auto lineBurstSymbolLayer = new QgsLineburstSymbolLayer();
    lineBurstSymbolLayer->setGradientColorType(Qgis::GradientColorSource::ColorRamp);
    QgsGradientColorRamp *ramp = new QgsGradientColorRamp(QColor( 0, 255, 0 ), QColor( 0, 20, 0 ));
    lineBurstSymbolLayer->setColorRamp(ramp);
    lineBurstSymbolLayer->setWidth(2.0);

#else
    QVariantMap mp;
//    mp["color_type"] = QStringLiteral("0");
//    mp["color"] = QStringLiteral("35,35,35");
//    mp["gradient_color2"] = QStringLiteral("250,250,250");

    mp["color_type"] = QStringLiteral("1");
    mp["color1"] = QStringLiteral("0, 255, 0");
    mp["color2"] = QStringLiteral("0, 20, 0");
    mp["line_width"] = QStringLiteral("2.0");
    auto lineBurstSymbolLayer = QgsLineburstSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << lineBurstSymbolLayer;
    //构造QgsLineSymbol并设置renderer
    auto lineSymbol = new QgsLineSymbol(layerList);
    singleSymbolRenderer->setSymbol(lineSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonSimplefillSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/water.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 1
    auto simplefillSymbolLayer = new QgsSimpleFillSymbolLayer();
    simplefillSymbolLayer->setFillColor(QColor("red"));
#else
    QVariantMap mp;
    mp["color"] = QStringLiteral("red");
    auto simplefillSymbolLayer = QgsSimpleFillSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << simplefillSymbolLayer;
    //构造QgsFillSymbol并设置renderer
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonCentroidSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/water.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto centroidFillSymbolLayer = new QgsCentroidFillSymbolLayer();
    centroidFillSymbolLayer->setPointOnSurface(false);
    centroidFillSymbolLayer->setPointOnAllParts(true);
    centroidFillSymbolLayer->setClipPoints(false);
    centroidFillSymbolLayer->setClipOnCurrentPartOnly(false);
#else
    QVariantMap mp;
    mp["point_on_surface"] = QStringLiteral("0");
    mp["point_on_all_parts"] = QStringLiteral("1");
    mp["clip_points"] = QStringLiteral("0");
    mp["clip_on_current_part_only"] = QStringLiteral("0");
    auto centroidFillSymbolLayer = QgsCentroidFillSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << centroidFillSymbolLayer;
    //构造QgsFillSymbol并设置renderer
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);

}

void MainWindow::polygonGradientSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/water.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto gradientFillSymbolLayer = new QgsGradientFillSymbolLayer();
    //SimpleTwoColor
//    gradientFillSymbolLayer->setGradientColorType(Qgis::GradientColorSource::SimpleTwoColor);
//    gradientFillSymbolLayer->setColor(QColor("blue"));
//    gradientFillSymbolLayer->setColor2(QColor("white"));
    //Color ramp
    gradientFillSymbolLayer->setGradientColorType(Qgis::GradientColorSource::ColorRamp);
    auto colorRamp = new QgsGradientColorRamp(QColor("blue"),QColor("white"));
    gradientFillSymbolLayer->setColorRamp(colorRamp);
#else
    QVariantMap mp;
    //SimpleTwoColor
//    mp["color_type"] = QStringLiteral("0");
//    mp["color"] = QStringLiteral("0,0,255");
//    mp["gradient_color2"] = QStringLiteral("255,255,255");
    //Color ramp
    mp["color_type"] = QStringLiteral("1");
    mp["color1"] = QStringLiteral("0,0,255");
    mp["color2"] = QStringLiteral("255,255,255");
    auto gradientFillSymbolLayer = QgsGradientFillSymbolLayer ::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << gradientFillSymbolLayer;
    //构造QgsFillSymbol并设置renderer
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);

}

void MainWindow::polygonLinePatternSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto linePatternFillSymbolLayer = new QgsLinePatternFillSymbolLayer();
    linePatternFillSymbolLayer->setLineAngle(0.0);
    linePatternFillSymbolLayer->setColor(QColor(0,0,255));
#else
    QVariantMap mp;
    mp["lineangle"] = QStringLiteral("0.0");
    mp["line_color"] = QStringLiteral("0,0,255");
    auto linePatternFillSymbolLayer = QgsLinePatternFillSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << linePatternFillSymbolLayer;
    //构造QgsFillSymbol并设置renderer
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonPointPatternSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto pointPatternFillSymbolLayer = new QgsPointPatternFillSymbolLayer();
    pointPatternFillSymbolLayer->setAngle(45.0);
    pointPatternFillSymbolLayer->setColor(QColor(0,0,255));
#else
    QVariantMap mp;
    mp["angle"] = QStringLiteral("45.0");
    mp["line_color"] = QStringLiteral("0,0,255");
    auto pointPatternFillSymbolLayer = QgsPointPatternFillSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << pointPatternFillSymbolLayer;
    //构造QgsFillSymbol并设置renderer
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonRasterImageSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    QString path = QStringLiteral("resources/eye.png");
    auto rasterFillSymbolLayer = new QgsRasterFillSymbolLayer(path);
    rasterFillSymbolLayer->setAngle(0.0);
#else
    QVariantMap mp;
    mp["imageFile"] = QStringLiteral("resources/eye.png");
    mp["angle"] = QStringLiteral("0.0");
    auto rasterFillSymbolLayer = QgsRasterFillSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << rasterFillSymbolLayer;
    //构造QgsFillSymbol并设置renderer
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonSvgSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    QString path = QStringLiteral("resources/plane.svg");
    auto svgFillSymbolLayer = new QgsSVGFillSymbolLayer(path);
    svgFillSymbolLayer->setColor(QColor(213,180,60));
    svgFillSymbolLayer->setPatternWidth(20.0);
#else
    QVariantMap mp;
    mp["svgFile"] = QStringLiteral("resources/plane.svg");
    mp["color"] = QStringLiteral("213,180,60");
    mp["width"] = QStringLiteral("20.0");
    auto svgFillSymbolLayer = QgsSVGFillSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << svgFillSymbolLayer;
    //构造QgsFillSymbol并设置renderer
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonRandomMarkerSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 1
    int pointCount = 100;
    auto randomMarkerFillSymbolLayer = new QgsRandomMarkerFillSymbolLayer(100);
    randomMarkerFillSymbolLayer->setDensityArea(250.0);
    randomMarkerFillSymbolLayer->setCountMethod(Qgis::PointCountMethod::Absolute);
#else
    QVariantMap mp;
    mp["point_count"] = QStringLiteral("100");
    mp["density_area"] = QStringLiteral("250.0");
    mp["count_method"] = QStringLiteral("0");
    auto randomMarkerFillSymbolLayer = QgsRandomMarkerFillSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << randomMarkerFillSymbolLayer;
    //构造QgsFillSymbol并设置renderer
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonShapeburstSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto shapeburstFillSymbolLayer = new QgsShapeburstFillSymbolLayer();
    //SimpleTwoColor
//    shapeburstFillSymbolLayer->setColorType(Qgis::GradientColorSource::SimpleTwoColor);
//    shapeburstFillSymbolLayer->setColor(QColor("green"));
//    shapeburstFillSymbolLayer->setColor2(QColor("white"));
    //Color ramp
    shapeburstFillSymbolLayer->setColorType(Qgis::GradientColorSource::ColorRamp);
    auto colorRamp = new QgsGradientColorRamp(QColor("green"),QColor("white"));
    shapeburstFillSymbolLayer->setColorRamp(colorRamp);
#else
    QVariantMap mp;
    //SimpleTwoColor
//    mp["color_type"] = QStringLiteral("0");
//    mp["color"] = QStringLiteral("0,235,0");
//    mp["gradient_color2"] = QStringLiteral("255,255,255");
    //Color ramp
    mp["color_type"] = QStringLiteral("1");
    mp["color1"] = QStringLiteral("0,235,0");
    mp["color2"] = QStringLiteral("255,255,255");
    auto shapeburstFillSymbolLayer = QgsShapeburstFillSymbolLayer ::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << shapeburstFillSymbolLayer;
    //构造QgsFillSymbol并设置renderer
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonOutlineSimpleSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
    //构建QgsSimpleLineSymbolLayer
#if 0
    auto simpleLineSymbolLayer = new QgsSimpleLineSymbolLayer(QColor("black"));
#else
    QVariantMap mp;
    mp["line_color"] = QStringLiteral("black");
    auto simpleLineSymbolLayer = QgsSimpleLineSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << simpleLineSymbolLayer;
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonOutlineArrowSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto arrowSymbolLayer  = new QgsArrowSymbolLayer();
#else
    QVariantMap mp;
    mp["head_type"] = QStringLiteral("0");
    mp["arrow_type"] = QStringLiteral("0");
    auto arrowSymbolLayer  = QgsArrowSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << arrowSymbolLayer;
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonOutlineHashedSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto hashedSymbolLayer = new QgsHashedLineSymbolLayer(true,3);
#else
    QVariantMap mp;
    mp["rotate"] = QStringLiteral("1");
    mp["interval"] = QStringLiteral("3");
    auto hashedSymbolLayer = QgsHashedLineSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << hashedSymbolLayer;
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);

}

void MainWindow::polygonOutlineInterpolatedSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto interpolatedSymbolLayer = new QgsInterpolatedLineSymbolLayer();
#else
    QVariantMap mp;
    mp["single_color"] = QStringLiteral("black");
    auto interpolatedSymbolLayer = QgsInterpolatedLineSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << interpolatedSymbolLayer;
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonOutlineLineburstSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
//    auto lineBurstSymbolLayer = new QgsLineburstSymbolLayer(QColor(35,35,35),Qt::white);
//    lineBurstSymbolLayer->setGradientColorType(Qgis::GradientColorSource::SimpleTwoColor);
//    lineBurstSymbolLayer->setWidth(2.0);
    auto lineBurstSymbolLayer = new QgsLineburstSymbolLayer();
    lineBurstSymbolLayer->setGradientColorType(Qgis::GradientColorSource::ColorRamp);
    QgsGradientColorRamp *ramp = new QgsGradientColorRamp(QColor( 0, 255, 0 ), QColor( 0, 20, 0 ));
    lineBurstSymbolLayer->setColorRamp(ramp);
    lineBurstSymbolLayer->setWidth(2.0);

#else
    QVariantMap mp;
//    mp["color_type"] = QStringLiteral("0");
//    mp["color"] = QStringLiteral("35,35,35");
//    mp["gradient_color2"] = QStringLiteral("250,250,250");

    mp["color_type"] = QStringLiteral("1");
    mp["color1"] = QStringLiteral("0, 255, 0");
    mp["color2"] = QStringLiteral("0, 20, 0");
    mp["line_width"] = QStringLiteral("2.0");
    auto lineBurstSymbolLayer = QgsLineburstSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << lineBurstSymbolLayer;
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);
}

void MainWindow::polygonOutlineMarkerSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 1
    auto markerSymbolLayer = new QgsMarkerLineSymbolLayer(true,3);
#else
    QVariantMap mp;
    mp["rotate"] = QStringLiteral("1");
    mp["interval"] = QStringLiteral("3");
    auto markerSymbolLayer = QgsMarkerLineSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << markerSymbolLayer;
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);

}

void MainWindow::polygonOutlineRasterSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
#if 0
    auto rasterSymbolLayer = new QgsRasterLineSymbolLayer(QStringLiteral("resources/cock.jpg"));
    rasterSymbolLayer->setWidth(2.0);
#else
    QVariantMap mp;
    mp["imageFile"] = QStringLiteral("resources/cock.jpg");
    mp["line_width"] = QStringLiteral("2.0");
    auto rasterSymbolLayer = QgsRasterLineSymbolLayer::create(mp);
#endif
    //多个Symbol Layer构成一个Symbol
    QgsSymbolLayerList layerList;
    layerList << rasterSymbolLayer;
    auto fillSymbol = new QgsFillSymbol(layerList);
    singleSymbolRenderer->setSymbol(fillSymbol);
    layer->setRenderer(singleSymbolRenderer);

}

void MainWindow::polygonInvertedRendererSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //分别获取反转面和单一符号渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsInvertedPolygonRenderer *invertedRenderer = QgsInvertedPolygonRenderer::convertFromRenderer(layerRenderer);
    QgsSingleSymbolRenderer * singleSymbolRenderer = QgsSingleSymbolRenderer::convertFromRenderer( layerRenderer);
    //创建fill类型的符号
#if 1
    auto simplefillSymbolLayer = new QgsSimpleFillSymbolLayer();
    simplefillSymbolLayer->setFillColor(QColor("red"));
#else
    QVariantMap mp;
    mp["color"] = QStringLiteral("red");
    auto simplefillSymbolLayer = QgsSimpleFillSymbolLayer::create(mp);
#endif
    QgsSymbolLayerList layerList;
    layerList << simplefillSymbolLayer;
    auto fillSymbol = new QgsFillSymbol(layerList);
    //单一符号渲染器设置其符号为fillSymbol
    singleSymbolRenderer->setSymbol(fillSymbol);
    //反转面渲染器调用该函数，将单一符号渲染器作为参数传进去
    invertedRenderer->setEmbeddedRenderer(singleSymbolRenderer);
    layer->setRenderer(invertedRenderer);
}

void MainWindow::polygon25DSlot()
{
    //添加一个线图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/houses.shp"));
    //从图层获取渲染器
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    Qgs25DRenderer *D25DRenderer = Qgs25DRenderer::convertFromRenderer(layerRenderer);

    //实现2.5D效果的关键点在这里
    QgsExpressionContextUtils::setLayerVariable( layer, QStringLiteral( "qgis_25d_height" ), 10 );
    QgsExpressionContextUtils::setLayerVariable( layer, QStringLiteral( "qgis_25d_angle" ), 70 );

    layer->setRenderer(D25DRenderer);
}

void MainWindow::labelSlot()
{
    //添加一个图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    //
    //QgsTextFormat --> QgsTextBackgroundSettings
    //              --> QgsTextBufferSettings
    //              --> QgsTextMaskSettings
    //              --> QgsTextShadowSettings
    //QgsCallout
    //QgsLabelPlacementSettings
    //QgsPalLayerSettings包含文本设置
    QgsPalLayerSettings settings;
    settings.drawLabels = true;
    settings.fieldName = "name";
    //文字设置
    QgsTextFormat textFormat;
    QgsTextBackgroundSettings backgroundSettings;
    QgsTextBufferSettings bufferSettings;
    QgsTextMaskSettings maskSettings;
    QgsTextShadowSettings shadowSettings;
    //开启背景并设置为白色
    backgroundSettings.setEnabled(true);
    backgroundSettings.setFillColor(QColor("white"));
    textFormat.setBackground(backgroundSettings);
    //开启buffer并设置颜色为黑色
    bufferSettings.setEnabled(true);
    bufferSettings.setColor(QColor("black"));
    textFormat.setBuffer(bufferSettings);
    //开启mask并设置size
    maskSettings.setEnabled(true);
    maskSettings.setSize(1.0);
    textFormat.setMask(maskSettings);
    //开启shadow
    shadowSettings.setEnabled(true);
    shadowSettings.setColor(QColor("red"));
    textFormat.setShadow(shadowSettings);
    settings.setFormat(textFormat);
    //设置callout
    auto callout = new QgsBalloonCallout();
    settings.setCallout(callout);
    //设置位置
    QgsLabelPlacementSettings placementSettings;
    placementSettings.setAllowDegradedPlacement(true);
    settings.setPlacementSettings(placementSettings);

    QgsVectorLayerSimpleLabeling* simpleLabeling = new QgsVectorLayerSimpleLabeling(settings);
    layer->setLabelsEnabled(true);
    layer->setLabeling(simpleLabeling);
}

void MainWindow::annotationTextSlot()
{
    //添加注释图层
    QgsAnnotationLayer *annotationLayer = addTestAnnotationLayer();
    //点即Text注释位置
    QgsPoint p1(20.33989,-33.86805);
    //构造并添加
    auto pointText = new QgsAnnotationPointTextItem(QStringLiteral("Test"),p1);
    annotationLayer->addItem(pointText);
}

void MainWindow::annotationPointSlot()
{
    //添加注释图层
    QgsAnnotationLayer *annotationLayer = addTestAnnotationLayer();
    //点即Text注释位置
    QgsPoint p2(20.47760,-33.86676);
    //构造并添加
    auto markerItem = new QgsAnnotationMarkerItem(p2);
    annotationLayer->addItem(markerItem);
}

void MainWindow::annotationLineSlot()
{
    //添加注释图层
    QgsAnnotationLayer *annotationLayer = addTestAnnotationLayer();
    //定义两个端点
    QgsPoint p1(20.33989,-33.86805);
    QgsPoint p2(20.47760,-33.86676);
    QVector<QgsPoint> points;
    points << p1 << p2;
    auto lineString = new QgsLineString(points);
    //构造并添加
    auto lineItem = new QgsAnnotationLineItem(lineString);
    annotationLayer->addItem(lineItem);
}

void MainWindow::annotationPolygonSlot()
{
    //添加注释图层
    QgsAnnotationLayer *annotationLayer = addTestAnnotationLayer();
    //定义3个点
    QgsPoint p1(20.33989,-33.86805);
    QgsPoint p2(20.47760,-33.86676);
    QgsPoint p3(20.39973,-33.80499);
    //3个点组成两条线
    QgsLineString *line1 = new QgsLineString(p1,p2);
    QgsLineString *line2 = new QgsLineString(p2,p3);
    //两条线添加到QgsCompoundCurve
    auto compoundCurve = new QgsCompoundCurve();
    compoundCurve->addCurve(line1);
    compoundCurve->addCurve(line2);
    compoundCurve->close();
    //创建多边形
    auto curvePolygon = new QgsCurvePolygon();
    curvePolygon->setExteriorRing(compoundCurve);
    //添加多边形item
    auto annotationPolygonItem = new QgsAnnotationPolygonItem(curvePolygon);
    annotationLayer->addItem(annotationPolygonItem);
}

void MainWindow::diagramPieSlot()
{
    //添加一个图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));

    QgsDiagramSettings ds;
    ds.enabled = true;
    //Attributes选项卡
    QColor col1 = Qt::red;
    QColor col2 = Qt::yellow;
    QColor col3 = Qt::blue;
    ds.categoryColors = QList<QColor>() << col1 << col2 << col3;
    ds.categoryAttributes = QList<QString>() << QStringLiteral( "prec_2020" ) << QStringLiteral( "prec_2021" ) << QStringLiteral( "prec_2022" );
    ds.categoryLabels = QList<QString>() << QStringLiteral( "prec_2020" ) << QStringLiteral( "prec_2021" ) << QStringLiteral( "prec_2022" );
    //Rendering选项卡
    ds.opacity = 1.0;
    //Size选项卡
    ds.size = QSizeF( 15, 15 );
#if 1
    QgsLinearlyInterpolatedDiagramRenderer *dr = new QgsLinearlyInterpolatedDiagramRenderer();
    dr->setLowerValue( 0.0 );
    dr->setLowerSize( QSizeF( 0.0, 0.0 ) );
    dr->setUpperValue( 100 );
    dr->setUpperSize( QSizeF( 40, 40 ) );
    dr->setClassificationField( QStringLiteral( "prec_2020" ) );
#else
    QgsSingleCategoryDiagramRenderer *dr = new QgsSingleCategoryDiagramRenderer();
#endif
    dr->setDiagram(new QgsPieDiagram());
    dr->setDiagramSettings(ds);

    QgsDiagramLayerSettings dls = QgsDiagramLayerSettings();
    dls.setPlacement( QgsDiagramLayerSettings::OverPoint );
    dls.setShowAllDiagrams( true );
    layer->setDiagramLayerSettings(dls);

    layer->setDiagramRenderer( dr );
}

void MainWindow::diagramTextSlot()
{
    //添加一个图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));

    QgsDiagramSettings ds;
    ds.enabled = true;
    //Attributes选项卡
    QColor col1 = Qt::red;
    QColor col2 = Qt::yellow;
    QColor col3 = Qt::blue;
    ds.categoryColors = QList<QColor>() << col1 << col2 << col3;
    ds.categoryAttributes = QList<QString>() << QStringLiteral( "prec_2020" ) << QStringLiteral( "prec_2021" ) << QStringLiteral( "prec_2022" );
    ds.categoryLabels = QList<QString>() << QStringLiteral( "prec_2020" ) << QStringLiteral( "prec_2021" ) << QStringLiteral( "prec_2022" );
    //Rendering选项卡
    ds.opacity = 1.0;
    //Size选项卡
    ds.size = QSizeF( 15, 15 );
#if 1
    QgsLinearlyInterpolatedDiagramRenderer *dr = new QgsLinearlyInterpolatedDiagramRenderer();
    dr->setLowerValue( 0.0 );
    dr->setLowerSize( QSizeF( 0.0, 0.0 ) );
    dr->setUpperValue( 100 );
    dr->setUpperSize( QSizeF( 40, 40 ) );
    dr->setClassificationField( QStringLiteral( "prec_2020" ) );
#else
    QgsSingleCategoryDiagramRenderer *dr = new QgsSingleCategoryDiagramRenderer();
#endif
    dr->setDiagram( new QgsTextDiagram() );
    dr->setDiagramSettings( ds );

    QgsDiagramLayerSettings dls = QgsDiagramLayerSettings();
    dls.setPlacement( QgsDiagramLayerSettings::OverPoint );
    dls.setShowAllDiagrams( true );

    layer->setDiagramLayerSettings(dls);
    layer->setDiagramRenderer( dr );
}

void MainWindow::diagramHistogramSlot()
{
    //添加一个图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));

    QgsDiagramSettings ds;
    ds.enabled = true;
    //Attributes选项卡
    QColor col1 = Qt::red;
    QColor col2 = Qt::yellow;
    QColor col3 = Qt::blue;
    ds.categoryColors = QList<QColor>() << col1 << col2 << col3;
    ds.categoryAttributes = QList<QString>() << QStringLiteral( "prec_2020" ) << QStringLiteral( "prec_2021" ) << QStringLiteral( "prec_2022" );
    ds.categoryLabels = QList<QString>() << QStringLiteral( "prec_2020" ) << QStringLiteral( "prec_2021" ) << QStringLiteral( "prec_2022" );
    //Rendering选项卡
    ds.opacity = 1.0;
    //Size选项卡
    ds.size = QSizeF( 15, 15 );

    QgsLinearlyInterpolatedDiagramRenderer *dr = new QgsLinearlyInterpolatedDiagramRenderer();
    dr->setLowerValue( 0.0 );
    dr->setLowerSize( QSizeF( 0.0, 0.0 ) );
    dr->setUpperValue( 100 );
    dr->setUpperSize( QSizeF( 40, 40 ) );
    dr->setClassificationField( QStringLiteral( "prec_2020" ) );

    dr->setDiagram(new QgsHistogramDiagram());
    dr->setDiagramSettings(ds);

    QgsDiagramLayerSettings dls = QgsDiagramLayerSettings();
    dls.setPlacement( QgsDiagramLayerSettings::OverPoint );
    dls.setShowAllDiagrams( true );

    layer->setDiagramLayerSettings(dls);
    layer->setDiagramRenderer( dr );

}

void MainWindow::diagramStackedBarSlot()
{
    //添加一个图层
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));

    QgsDiagramSettings ds;
    ds.enabled = true;
    //Attributes选项卡
    QColor col1 = Qt::red;
    QColor col2 = Qt::yellow;
    QColor col3 = Qt::blue;
    ds.categoryColors = QList<QColor>() << col1 << col2 << col3;
    ds.categoryAttributes = QList<QString>() << QStringLiteral( "prec_2020" ) << QStringLiteral( "prec_2021" ) << QStringLiteral( "prec_2022" );
    ds.categoryLabels = QList<QString>() << QStringLiteral( "prec_2020" ) << QStringLiteral( "prec_2021" ) << QStringLiteral( "prec_2022" );
    //Rendering选项卡
    ds.opacity = 1.0;
    //Size选项卡
    ds.size = QSizeF( 15, 15 );
#if 1
    QgsLinearlyInterpolatedDiagramRenderer *dr = new QgsLinearlyInterpolatedDiagramRenderer();
    dr->setLowerValue( 0.0 );
    dr->setLowerSize( QSizeF( 0.0, 0.0 ) );
    dr->setUpperValue( 100 );
    dr->setUpperSize( QSizeF( 40, 40 ) );
    dr->setClassificationField( QStringLiteral( "prec_2020" ) );
#else
    QgsSingleCategoryDiagramRenderer *dr = new QgsSingleCategoryDiagramRenderer();
#endif
    dr->setDiagram(new QgsStackedBarDiagram());
    dr->setDiagramSettings(ds);

    QgsDiagramLayerSettings dls = QgsDiagramLayerSettings();
    dls.setPlacement( QgsDiagramLayerSettings::OverPoint );
    dls.setShowAllDiagrams( true );

    layer->setDiagramLayerSettings(dls);
    layer->setDiagramRenderer( dr );
}

void MainWindow::rasterMultibandSlot()
{
    //添加测试图层
    QgsRasterLayer *layer = addTestRaster("maps/raster/3420C_2010_327_RGB_LATLNG.tif");

    double minValue = 0;
    double maxValue = 0;
    minMaxValuesForBand( 1, layer->dataProvider(), minValue, maxValue );
    QgsContrastEnhancement *rce = new QgsContrastEnhancement( ( Qgis::DataType )(layer->dataProvider()->dataType( 1 ) ) );
    rce->setContrastEnhancementAlgorithm(QgsContrastEnhancement::StretchToMinimumMaximum);
    rce->setMinimumValue(minValue);
    rce->setMaximumValue(maxValue);
    minMaxValuesForBand( 2, layer->dataProvider(), minValue, maxValue );
    QgsContrastEnhancement *gce = new QgsContrastEnhancement( ( Qgis::DataType )(layer->dataProvider()->dataType( 2 ) ) );
    gce->setContrastEnhancementAlgorithm(QgsContrastEnhancement::StretchToMinimumMaximum);
    gce->setMinimumValue(minValue);
    gce->setMaximumValue(maxValue);
    minMaxValuesForBand( 3, layer->dataProvider(), minValue, maxValue );
    QgsContrastEnhancement *bce = new QgsContrastEnhancement( ( Qgis::DataType )(layer->dataProvider()->dataType( 3 ) ) );
    bce->setContrastEnhancementAlgorithm(QgsContrastEnhancement::StretchToMinimumMaximum);
    bce->setMinimumValue(minValue);
    bce->setMaximumValue(maxValue);

    QgsMultiBandColorRenderer *renderer = new QgsMultiBandColorRenderer(layer->dataProvider(),1,2,3,rce,gce,bce);

    //设置layer的属性
    layer->setBlendMode(QPainter::CompositionMode_SourceOver);
    layer->setRenderer(renderer);
}

void MainWindow::rasterPalettedSlot()
{
    //添加测试图层
    QgsRasterLayer *layer = addTestRaster("maps/raster/with_color_table.tif");

    const int grayBand = 1;
    const QgsPalettedRasterRenderer::ClassData classes = QgsPalettedRasterRenderer::colorTableToClassData( layer->dataProvider()->colorTable( grayBand ) );
    auto renderer = new QgsPalettedRasterRenderer( layer->dataProvider(),grayBand,classes );
    layer->setRenderer(renderer);
}

void MainWindow::rasterSinglebandGraySlot()
{
    //添加测试图层
    QgsRasterLayer *layer = addTestRaster("maps/raster/3420C_2010_327_RGB_LATLNG.tif");
    //
    const int grayBand = 1;
    QgsSingleBandGrayRenderer *renderer = new QgsSingleBandGrayRenderer( layer->dataProvider(), grayBand );
    renderer->setGradient(QgsSingleBandGrayRenderer::BlackToWhite);//
    QgsContrastEnhancement *ce = new QgsContrastEnhancement( ( Qgis::DataType )(layer->dataProvider()->dataType( grayBand ) ) );
    ce->setContrastEnhancementAlgorithm(QgsContrastEnhancement::StretchToMinimumMaximum);
    ce->setMinimumValue(-1);
    ce->setMaximumValue(2630);
    renderer->setContrastEnhancement(ce);
    QgsRasterMinMaxOrigin mmOrigin = renderer->minMaxOrigin();
    renderer->setMinMaxOrigin(mmOrigin);
    //QgsRasterLayer中有Layer Rendering和Resampling中的相应设置

    layer->setRenderer(renderer);
}

void MainWindow::rasterSinglebandPseudoSlot()
{
    //添加测试图层
    QgsRasterLayer *layer = addTestRaster("maps/raster/3420C_2010_327_RGB_LATLNG.tif");

    const int bandNo = 1;
    double minValue = 0;
    double maxValue = 0;
    minMaxValuesForBand( bandNo, layer->dataProvider(), minValue, maxValue );
    QgsSingleBandPseudoColorRenderer *renderer =
            new QgsSingleBandPseudoColorRenderer( layer->dataProvider(), bandNo, nullptr );
    QgsGradientColorRamp *ramp = new QgsGradientColorRamp(QColor(20,0,0),QColor(255,0,0));
    renderer->setClassificationMin(minValue);
    renderer->setClassificationMax(maxValue);
    renderer->createShader(ramp,QgsColorRampShader::Interpolated,QgsColorRampShader::Continuous,255);
    layer->setRenderer(renderer);
}

void MainWindow::rasterHillshadeSlot()
{
    //添加测试图层
    QgsRasterLayer *layer = addTestRaster("maps/raster/3420C_2010_327_RGB_LATLNG.tif");

    QgsHillshadeRenderer *renderer = new QgsHillshadeRenderer(layer->dataProvider(),1,45,315);
    renderer->setZFactor(10);
    layer->setRenderer(renderer);
}

void MainWindow::rasterContoursSlot()
{
    //添加测试图层
    QgsRasterLayer *layer = addTestRaster("maps/raster/3420C_2010_327_RGB_LATLNG.tif");

    QgsRasterContourRenderer *renderer = new QgsRasterContourRenderer(layer->dataProvider());
    renderer->setInputBand(1);
    renderer->setContourInterval(100.00);
    //这里可以设置QgsLineSymbol
//    renderer->setContourSymbol()
    renderer->setContourIndexInterval(500.00);
    //这里可以设置QgsLineSymbol
//    renderer->setContourIndexSymbol()

    renderer->setDownscale(4.00);
    layer->setRenderer(renderer);
}

void MainWindow::getFieldsSlot(QPlainTextEdit *plainTextEdit)
{
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/myplaces.shp"));
    plainTextEdit->clear();
    for(auto field : layer->fields())
    {
        plainTextEdit->appendPlainText(QString("%1:%2").arg(field.name()).arg(field.typeName()));
    }
}

void MainWindow::getFeaturesSlot(QPlainTextEdit *plainTextEdit)
{
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/myplaces.shp"));
    plainTextEdit->clear();
#if 0
    QgsFeatureIterator it = layer->getFeatures();
    QgsFeature f;
    while(it.nextFeature(f))
    {
        QString str;
        str.append(QString("%1").arg(f.id()));
        str.append(" ");
        QgsAttributes attrs = f.attributes();
        for(int i = 0;i < attrs.size();++i)
        {
            str.append(attrs[i].toString());
            str.append(" ");
        }
        plainTextEdit->appendPlainText(str);
    }
#else
    //带feature的expression
    QString str = QString("\"RAINFALL\" > 200.0");
    auto request = QgsFeatureRequest().setFilterExpression(str);
    QgsFeatureIterator it = layer->getFeatures(request);
    QgsFeature f;
    while(it.nextFeature(f))
    {
        QString str;
        str.append(QString("%1").arg(f.id()));
        str.append(" ");
        QgsAttributes attrs = f.attributes();
        for(int i = 0;i < attrs.size();++i)
        {
            str.append(attrs[i].toString());
            str.append(" ");
        }
        plainTextEdit->appendPlainText(str);
    }
#endif
}

void MainWindow::selectFeaturesSlot(QPlainTextEdit *plainTextEdit)
{
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/myplaces.shp"));
    plainTextEdit->clear();
    QString expression = QString("\"RAINFALL\" > 200.0");
    //选择所有
//    layer->selectAll();
    //按照id选择
//    QgsFeatureId id = 0;
//    layer->select(id);
    //按照多个id选择
//    QgsFeatureIds ids;
//    ids << 0 << 1 << 2 << 3;
//    layer->select(ids);
    //按照表达式选择
//    layer->selectByExpression(expression);
    //按照区域选择,并且使用rubberband显示区域
    QgsRectangle rect(1006585,6222254,1010253,6219118);
    layer->selectByRect(rect);
    QgsPointXY point1(1006585,6222254);
    QgsPointXY point2(1010253,6222254);
    QgsPointXY point3(1006585,6219118);
    QgsPointXY point4(1010253,6219118);
    QgsRubberBand *rubberBand = new QgsRubberBand(mApp->mapCanvas(),QgsWkbTypes::PolygonGeometry);
    rubberBand->addPoint(point1);
    rubberBand->addPoint(point2);
    rubberBand->addPoint(point4);
    rubberBand->addPoint(point3);
    rubberBand->show();
    //选择feature并且闪烁
//    QgsMapCanvasUtils::flashMatchingFeatures(mApp->mapCanvas(),layer,expression);
    //选择Feature并zoom
//    QgsMapCanvasUtils::zoomToMatchingFeatures(mApp->mapCanvas(),layer,expression);
}

void MainWindow::spatialIndexSlot(QPlainTextEdit *plainTextEdit)
{
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/myplaces.shp"));
    QgsSpatialIndex *spatialIndex = new QgsSpatialIndex(layer->getFeatures());
    QgsPointXY pt(1010253,6222254);
    QList<QgsFeatureId> ids = spatialIndex->nearestNeighbor(pt,3);
    for(int i = 0;i < ids.size();++i)
    {
        layer->select(ids[i]);
    }
}

void MainWindow::distanceAreaSlot(QPlainTextEdit *plainTextEdit)
{
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/protected_areas.shp"));
    auto feature = layer->getFeature(0);

    QgsGeometry geom = feature.geometry();
    QString name = feature.attribute("name").toString();
    double area = geom.area();
    double length = geom.length();
    plainTextEdit->appendPlainText(QString("%1:length is %2,area is %3").arg(name).arg(length).arg(area));

    auto d = QgsDistanceArea();
    d.setEllipsoid("WGS84");
    double a = d.measureArea(geom);
    double l = d.measurePerimeter(geom);
    plainTextEdit->appendPlainText(QString("%1:length is %2,area is %3").arg(name).arg(l).arg(a));
}

void MainWindow::vectorFileWriterSlot(QPlainTextEdit *plainTextEdit)
{
    //virtual layer的用法
    //QgsVirtualLayerDefinition
    QgsVirtualLayerDefinition def;
    //add embedded layers
    def.addSource("prec","maps/virtuallayer/2017.xlsx|layername=Sheet1","ogr");
    def.addSource("jilin_dist","maps/virtuallayer/jilin_dist.shp","ogr");
    //查询语句
    def.setQuery("SELECT * FROM jilin_dist left outer join prec on jilin_dist.NAME = prec.city;");
    //地理信息 无/auto detect/manual
    //def.setGeometryWkbType( QgsWkbTypes::NoGeometry );
    /*
     *     const QgsWkbTypes::Type t = mGeometryType->currentIndex() > -1 ? static_cast<QgsWkbTypes::Type>( mGeometryType->currentData().toLongLong() ) : QgsWkbTypes::NoGeometry;
            def.setGeometryWkbType( t );
            def.setGeometryField( mGeometryField->text() );
            def.setGeometrySrid( mSrid );
    */
    if ( ! def.toString().isEmpty() )
    {
        const QgsVectorLayer::LayerOptions options { QgsProject::instance()->transformContext() };
        QgsVectorLayer *layer = new QgsVectorLayer( def.toString(), QStringLiteral( "virtual_layer" ), QStringLiteral( "virtual" ), options );
        QgsProject::instance()->addMapLayer(layer);

        QgsVectorFileWriter::SaveVectorOptions opt;
        QgsVectorFileWriter::writeAsVectorFormatV3(layer,"maps/virtuallayer/myvurtual.shp",QgsProject::instance()->transformContext(),opt);

        //从虚拟图层中读出数据
        plainTextEdit->clear();
        QgsFeatureIterator it = layer->getFeatures();
        QgsFeature f;
        while(it.nextFeature(f))
        {
            QString str;
            str.append(QString("%1").arg(f.id()));
            str.append(" ");
            QgsAttributes attrs = f.attributes();
            for(int i = 0;i < attrs.size();++i)
            {
                str.append(attrs[i].toString());
                str.append(" ");
            }
            plainTextEdit->appendPlainText(str);
        }
    }
}

void MainWindow::algExecuted(bool successful, const QVariantMap &results)
{
    QgsMapLayer *layer = mContext->getMapLayer(results["OUTPUT"].toString());
    if(layer)
    {
        QgsProject::instance()->addMapLayer(layer);
    }
}

void MainWindow::processingClipSlot()
{
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/processing/dongbei_roads.shp"));
    const QString id = "native:clip";
    QVariantMap conf;
//    conf.insert(QStringLiteral("INPUT"),"maps/processing/dongbei_roads.shp");//直接用路径
    conf.insert(QStringLiteral("INPUT"),layer->id());//或者使用layer id
    conf.insert(QStringLiteral("OVERLAY"),"maps/processing/jilin_dist.shp");
    QgsProcessingOutputLayerDefinition value( "TEMPORARY_OUTPUT" );
    conf.insert(QStringLiteral("OUTPUT"),value);
    auto algorithm = QgsApplication::processingRegistry()->createAlgorithmById(id,conf);
    QgsProcessingContext *context = new QgsProcessingContext;
    context->setProject(QgsProject::instance());
    QgsProcessingFeedback *feedback = new QgsProcessingFeedback(false);
#if 0
    //这里采用的是运行在主线程的方法
    algorithm->prepare(conf,*context,feedback);
    QVariantMap runResults = algorithm->run(conf,*context,feedback);
    QgsMapLayer *tempLayer = context->getMapLayer(runResults["OUTPUT"].toString());
    if(layer)
    {
        QgsProject::instance()->addMapLayer(tempLayer);
    }
#else
    //在线程中运行
    mContext = context;
    QgsProcessingAlgRunnerTask *algTask = new QgsProcessingAlgRunnerTask(algorithm,conf,*mContext,feedback);
    connect(algTask,&QgsProcessingAlgRunnerTask::executed,this,&MainWindow::algExecuted);
    QgsApplication::taskManager()->addTask(algTask);
#endif
}

void MainWindow::processingRandomPointsSlot()
{
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/processing/jilin_dist.shp"));
    const QString id = "native:randompointsinpolygons";
    QVariantMap conf;
    conf.insert(QStringLiteral("INPUT"),layer->id());//使用layer id或者使用物理路径均可
    conf.insert(QStringLiteral("POINTS_NUMBER"),"10");//
    QgsProcessingOutputLayerDefinition value( "TEMPORARY_OUTPUT" );
    conf.insert(QStringLiteral("OUTPUT"),value);
    QgsProcessingAlgorithm *algorithm = QgsApplication::processingRegistry()->createAlgorithmById(id,conf);
    QgsProcessingContext *context = new QgsProcessingContext;
    context->setProject(QgsProject::instance());
    QgsProcessingFeedback *feedback = new QgsProcessingFeedback(false);
#if 0
    //这里采用的是运行在主线程的方法
    algorithm->prepare(conf,*context,feedback);
    QVariantMap runResults = algorithm->run(conf,*context,feedback);
    QgsMapLayer *tempLayer = context->getMapLayer(runResults["OUTPUT"].toString());
    if(layer)
    {
        QgsProject::instance()->addMapLayer(tempLayer);
    }
#else
    //在线程中运行
    mContext = context;
    QgsProcessingAlgRunnerTask *algTask = new QgsProcessingAlgRunnerTask(algorithm,conf,*mContext,feedback);
    connect(algTask,&QgsProcessingAlgRunnerTask::executed,this,&MainWindow::algExecuted);
    QgsApplication::taskManager()->addTask(algTask);
#endif
}

void MainWindow::processingBufferSlot()
{
    QgsVectorLayer* layer = addTestShape(QStringLiteral("maps/shapefile/myplaces.shp"));
    const QString id = "native:buffer";
    QVariantMap conf;
    conf.insert(QStringLiteral("INPUT"),layer->id());//使用layer id或者使用物理路径均可
    conf.insert(QStringLiteral("DISTANCE"),"100");
    QgsProcessingOutputLayerDefinition value( "TEMPORARY_OUTPUT" );
    conf.insert(QStringLiteral("OUTPUT"),value);
    auto algorithm = QgsApplication::processingRegistry()->createAlgorithmById(id,conf);
    QgsProcessingContext *context = new QgsProcessingContext;
    context->setProject(QgsProject::instance());
    QgsProcessingFeedback *feedback = new QgsProcessingFeedback(false);
#if 0
    //这里采用的是运行在主线程的方法
    algorithm->prepare(conf,*context,feedback);
    QVariantMap runResults = algorithm->run(conf,*context,feedback);
    QgsMapLayer *tempLayer = context->getMapLayer(runResults["OUTPUT"].toString());
    if(layer)
    {
        QgsProject::instance()->addMapLayer(tempLayer);
    }
#else
    //在线程中运行
    mContext = context;
    QgsProcessingAlgRunnerTask *algTask = new QgsProcessingAlgRunnerTask(algorithm,conf,*mContext,feedback);
    connect(algTask,&QgsProcessingAlgRunnerTask::executed,this,&MainWindow::algExecuted);
    QgsApplication::taskManager()->addTask(algTask);
#endif
}
