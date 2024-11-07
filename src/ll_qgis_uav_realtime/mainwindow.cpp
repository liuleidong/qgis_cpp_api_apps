#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>
#include <QTimer>
#include <QRandomGenerator>

#include "qgsdockwidget.h"
#include "qgsproject.h"
#include "qgsmapcanvas.h"
#include "qgsmarkersymbollayer.h"

#include "ld_geometry.h"
//0. 模拟无人机/车 发送经纬度以及设备信息
//1. 根据经纬度，添加设备或者移动设备
//2. 设置参数：是否显示轨迹，设置设备图标，设备大小,设备是中心点，是否闪烁
//根据两点设置rotation
//setCenter flashFeatureIds

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
    ui->menuParams->addAction(mApp->layerTreeDock()->toggleViewAction());

    mParamDockWidget = new ParamDockWidget(this);
    this->addDockWidget(Qt::LeftDockWidgetArea,mParamDockWidget);
    ui->menuParams->addAction(mParamDockWidget->toggleViewAction());

//    QString url1 = QStringLiteral("https://webst01.is.autonavi.com/appmaptile?style=6&x={x}&y={y}&z={z}");
//    mApp->addWmsLayer(url1,"gaode satellite");
    statusBar()->showMessage(QString::fromLocal8Bit("正在加载图层......"));
    QString url = QStringLiteral("https://wprd01.is.autonavi.com/appmaptile?style=8&x={x}&y={y}&z={z}");
    mApp->addWmsLayer(url,"gaode roads");

    connect(mParamDockWidget,&ParamDockWidget::setParamsSignal,this,&MainWindow::setParamsSlot);

    QTimer::singleShot(1000*1,this,[=]{QgsPointXY pt(11804480,4660807);mApp->mapCanvas()->zoomByFactor(1/1600.0,&pt);startTimer();});
}

void MainWindow::setPointLayerSimpleMarker(QgsVectorLayer *layer, const SMarkerSymbolSimple &simpleMarker)
{
    if(layer == nullptr)
        return;
    layer->startEditing();

    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
    //使用Simple Marker
    QVariantMap mp;
    mp[QString("name")] = simpleMarker.name;
    mp[QString("color")] = simpleMarker.color;
    mp[QString("size")] = simpleMarker.size;
    mp[QString("angle")] = simpleMarker.angle;
    QgsMarkerSymbol *newsym = QgsMarkerSymbol::createSimple(mp);
    singleRenderer->setSymbol(newsym);
    layer->setRenderer(singleRenderer);

    layer->commitChanges();
}

void MainWindow::setPointLayerSvgMarker(QgsVectorLayer *layer, const SMarkerSymbolSvg &svgMarker)
{
    if(layer == nullptr)
        return;
    layer->startEditing();

    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
    //使用Simple Marker
    QVariantMap mp;
    mp[QString("name")] = svgMarker.name;
    mp[QString("color")] = svgMarker.color;
    mp[QString("size")] = svgMarker.size;
    mp[QString("angle")] = svgMarker.angle;

    QgsSvgMarkerSymbolLayer svglayer(svgMarker.name);
    QgsSymbolLayer* svgsymbol = svglayer.create(mp);
    auto newsym = QgsMarkerSymbol::createSimple(mp);
    newsym->changeSymbolLayer(0,svgsymbol);
    singleRenderer->setSymbol(newsym);
    layer->setRenderer(singleRenderer);

    layer->commitChanges();

}

void MainWindow::addOrMovePoint(const SGeometryInfo &geometryInfo, const QString &prefix)
{
    bool isFeatureExsit = false;
    QgsFeatureRequest r;
    QgsFeatureIterator it = mDevPointLayer->getFeatures(r);
    QgsFeature ff;
    while(it.nextFeature(ff))
    {
        int deviceId = ff.attribute("id").toInt();
        if(deviceId == geometryInfo.deviceId)
        {
            isFeatureExsit = true;
            break;
        }
    }
    QgsVectorDataProvider * dataProvider = mDevPointLayer->dataProvider();
    if(isFeatureExsit)
    {
        mDevPointLayer->startEditing();
        QgsGeometry geometry = QgsGeometry::fromPointXY(QgsPointXY(geometryInfo.longitude,geometryInfo.latitude));
        mDevPointLayer->changeGeometry(ff.id(),geometry);
        mDevPointLayer->commitChanges();
    }
    else
    {
        mDevPointLayer->startEditing();
        QgsFields fields;
        QgsField fId("id");
        QgsField fType("type");
        QgsField fName("name");
        fields.append(fId);
        fields.append(fType);
        fields.append(fName);
        QgsFeature f(fields);
        f.setGeometry(QgsGeometry::fromPointXY(QgsPointXY(geometryInfo.longitude,geometryInfo.latitude)));
        f.setAttribute("id",geometryInfo.deviceId);
        f.setAttribute("type",geometryInfo.deviceType);
        f.setAttribute("name",QString("%1%2").arg(prefix).arg(geometryInfo.deviceId));
        dataProvider->addFeature(f);
        mDevPointLayer->commitChanges();
    }
    if(mParams.centerShow)
        mApp->mapCanvas()->setCenter(QgsPointXY(geometryInfo.longitude,geometryInfo.latitude));
}

void MainWindow::addOrMoveLine(const SGeometryInfo &geometryInfo,int trajectoryLength)
{
    bool isFeatureExsit = false;
    QgsFeatureRequest r;
    QgsFeatureIterator it = mDevLineLayer->getFeatures(r);
    QgsFeature ff;
    while(it.nextFeature(ff))
    {
        int deviceId = ff.attribute("id").toInt();
        if(deviceId == geometryInfo.deviceId)
        {
            isFeatureExsit = true;
            break;
        }
    }

    QgsVectorDataProvider * dataProvider = mDevLineLayer->dataProvider();
    if(isFeatureExsit)
    {
        QgsGeometry geo = ff.geometry();
        QgsPolylineXY line = geo.asPolyline();
        QgsPolyline nLine ;
        //点数小于设置值，直接构建新线段。
        //点数大于等于，则将旧点抛弃
        if(line.size() < trajectoryLength)
        {
            for(int i = 0;i < line.size();++i)
            {
                QgsPointXY p = line.at(i);
                nLine << QgsPoint(p.x(),p.y());
            }
        }
        else
        {
            for(int i = line.size() - trajectoryLength;i < line.size();++i)
            {
                QgsPointXY p = line.at(i);
                nLine << QgsPoint(p.x(),p.y());
            }
        }

        nLine << QgsPoint(geometryInfo.longitude,geometryInfo.latitude);
        QgsGeometry geometry = QgsGeometry::fromPolyline(nLine);
        mDevLineLayer->startEditing();
        mDevLineLayer->changeGeometry(ff.id(),geometry);
        mDevLineLayer->commitChanges();
    }
    else
    {
        QgsFeature f;
        QgsPolyline line ;
        line << QgsPoint(geometryInfo.longitude,geometryInfo.latitude);
        QgsGeometry geometry = QgsGeometry::fromPolyline(line);
        f.setGeometry( geometry );
        dataProvider->addFeature(f);
    }
}

void MainWindow::startTimer()
{
    statusBar()->showMessage(QString::fromLocal8Bit("图层加载完成"));
    //创建以点为形式显示设备的图层
    mDevPointLayer = new QgsVectorLayer("MultiPoint?crs=epsg:4326"
                                        "&field=id:integer&field=type:integer&field=name:string(20)"
                                        "&index=yes","points","memory");
    QgsProject::instance()->addMapLayer(mDevPointLayer);
    //创建以线的形式显示设备的图层
    mDevLineLayer = new QgsVectorLayer("MultiLineString?crs=epsg:4326"
                                    "&field=id:integer&field=name:string(20)"
                                    "&index=yes","lines","memory");
    QgsProject::instance()->addMapLayer(mDevLineLayer);    
    //初始化参数
    mParams.mode = 1;
    mParams.svgPath = QStringLiteral("resources/plane.svg");
    mParams.color = "red";
    mParams.size = 10.0;
    mParams.showPath = true;
    mParams.pathLength = 20;
    mParams.centerShow = true;
    mParams.angle = 0;
    setParamsSlot(mParams);

    QTimer *timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::mockDevices);
    timer->start(1000);
}

QgsMarkerSymbol *MainWindow::getLayerSymbol(QgsVectorLayer *layer)
{
    QgsFeatureRenderer * layerRenderer= layer->renderer();
    QgsSingleSymbolRenderer *singleRenderer = QgsSingleSymbolRenderer::convertFromRenderer(layerRenderer);
    return (QgsMarkerSymbol *)singleRenderer->symbol();
}

//模拟接收外部经纬度信号
void MainWindow::mockDevices()
{
    SGeometryInfo info;
    info.deviceId = 0;
    info.deviceType = 0;
    info.longitude = mPt1X;
    info.latitude = mPt1Y;

    addOrMovePoint(info,"plane");
    if(mParams.showPath)
    {
        addOrMoveLine(info,mParams.pathLength);
    }    

    quint32 biasX = QRandomGenerator::global()->generate() % 100;
    quint32 biasY = QRandomGenerator::global()->generate() % 100;

    auto d = new QgsDistanceArea();
    double r = d->bearing(QgsPointXY(mPt1X,mPt1Y),QgsPointXY(mPt1X + mOffsetX + biasX,mPt1Y + mOffsetY + biasY));
    double angle = r * 180 / M_PI;
    if(angle != mParams.angle)
    {
        mParams.angle = angle;
        setParamsSlot(mParams);
    }

    mPt1X = mPt1X + mOffsetX + biasX;
    mPt1Y = mPt1Y + mOffsetY + biasY;
}

void MainWindow::setParamsSlot(SParams params)
{
    mParams = params;
    if(mParams.mode == 0)
    {
        SMarkerSymbolSimple simpleMarker;
        simpleMarker.name = mParams.shape;
        simpleMarker.color = mParams.color.name();
        simpleMarker.size = QString("%1").arg(mParams.size);
        setPointLayerSimpleMarker(mDevPointLayer,simpleMarker);
    }
    else if(mParams.mode == 1)
    {
        SMarkerSymbolSvg svgMarker;
        svgMarker.name = mParams.svgPath;
        svgMarker.color = mParams.color.name();
        svgMarker.size = QString("%1").arg(mParams.size);
        svgMarker.angle = QString("%1").arg(mParams.angle);
        setPointLayerSvgMarker(mDevPointLayer,svgMarker);
    }
}
