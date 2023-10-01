#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QTimer>

#include "qgsdockwidget.h"
#include "qgsproject.h"
#include "qgsmapcanvas.h"

#include "ld_geometry.h"
//0. 模拟无人机/车 发送经纬度以及设备信息
//1. 根据经纬度，添加设备或者移动设备
//2. 设置参数：是否显示轨迹，设置设备图标，设备大小,设备是中心点

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
    ui->menuParams->addAction(mApp->layerTreeDock()->toggleViewAction());

    mParamDockWidget = new ParamDockWidget(this);
    this->addDockWidget(Qt::LeftDockWidgetArea,mParamDockWidget);
    ui->menuParams->addAction(mParamDockWidget->toggleViewAction());

    QString url1 = QStringLiteral("https://webst01.is.autonavi.com/appmaptile?style=6&x={x}&y={y}&z={z}");
    mApp->addWmsLayer(url1,"gaode satellite");
    QString url = QStringLiteral("https://wprd01.is.autonavi.com/appmaptile?style=8&x={x}&y={y}&z={z}");
    mApp->addWmsLayer(url,"gaode roads");

    QTimer::singleShot(1000*5,this,[=]{QgsPointXY pt(11804480,4660807);mApp->mapCanvas()->zoomByFactor(1/1600.0,&pt);startTimer();});
}

void MainWindow::addOrMovePoint(const SGeometryInfo &geometryInfo, const QString &prefix)
{
    bool isFeatureExsit = false;
    QgsFeatureRequest r;
#if 0
    QgsFields existFields;
    QStringList l;
    l << "id";
    r.setSubsetOfAttributes(l,existFields);
#endif
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
//    mDevPointLayer->updateFields();
    //    mDevPointLayer->updateExtents();
}

void MainWindow::startTimer()
{
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
    QTimer *timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::mockDevices);
    timer->start(1000);
}

//模拟接收外部经纬度信号
void MainWindow::mockDevices()
{
    SGeometryInfo info;
    info.deviceId = 0;
    info.deviceType = 0;
    info.longitude = mPt1X;
    info.latitude = mPt1Y;
    mPt1X = mPt1X + mOffsetX;
    mPt1Y = mPt1Y + mOffsetY;

    addOrMovePoint(info,"plane");

}
