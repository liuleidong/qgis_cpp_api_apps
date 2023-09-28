#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QTimer>

#include "qgsdockwidget.h"
#include "qgsproject.h"
#include "qgsmapcanvas.h"

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

    QTimer *timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&MainWindow::mockDevices);
    timer->start(1000);

    QString url1 = QStringLiteral("https://webst01.is.autonavi.com/appmaptile?style=6&x={x}&y={y}&z={z}");
    mApp->addWmsLayer(url1,"gaode satellite");
    QString url = QStringLiteral("https://wprd01.is.autonavi.com/appmaptile?style=8&x={x}&y={y}&z={z}");
    mApp->addWmsLayer(url,"gaode roads");
}

void MainWindow::mockDevices()
{

}
