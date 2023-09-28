#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "qgsdockwidget.h"

//0. 模拟无人机/车 发送经纬度以及设备信息
//1. 根据经纬度，添加设备或者移动设备
//2. 设置参数：是否显示轨迹，设置设备图标，设置设备大小等等

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
}
