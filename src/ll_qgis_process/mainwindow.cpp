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
    //ui->menuParams->addAction(mApp->layerTreeDock()->toggleViewAction());

    mParamDockWidget = new ParamDockWidget(this);
    this->addDockWidget(Qt::LeftDockWidgetArea,mParamDockWidget);
    //ui->menuParams->addAction(mParamDockWidget->toggleViewAction());

    connect(mParamDockWidget,&ParamDockWidget::setParamsSignal,this,&MainWindow::setParamsSlot);

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
    }
    else if(mParams.mode == 1)
    {
        SMarkerSymbolSvg svgMarker;
        svgMarker.name = mParams.svgPath;
        svgMarker.color = mParams.color.name();
        svgMarker.size = QString("%1").arg(mParams.size);
        svgMarker.angle = QString("%1").arg(mParams.angle);
    }
}
