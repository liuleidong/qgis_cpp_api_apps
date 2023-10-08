#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>

#include "panelimagebutton.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mApp = ll_qgis_base_lib::Instance();
    mApp->initialize(this);
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    auto gridLayout = new QGridLayout;
    gridLayout->addWidget((QWidget*)mApp->mapCanvas());
    ui->page_canvas->setLayout(gridLayout);
    ui->stackedWidget->setCurrentIndex(0);
    initPanels();
}

void MainWindow::initPanels()
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
    addPanelItem(layout,"添加shapefile文件",":/res/big.png",row,++column);
    addPanelItem(layout,"添加gpx文件",":/res/l_tiledMapLayer4326.png",row,++column);
    addPanelItem(layout,"添加gpkg文件",":/res/l_tiledMapLayer4326.png",row,++column);
    addPanelItem(layout,"添加geojson文件",":/res/l_tiledMapLayer4326.png",row,++column);
    ++row;column = -1;
    addPanelItem(layout,"添加gml文件",":/res/l_tiledMapLayer4326.png",row,++column);
    addPanelItem(layout,"添加kml文件",":/res/l_tiledMapLayer4326.png",row,++column);
    addPanelItem(layout,"添加dxf文件",":/res/l_tiledMapLayer4326.png",row,++column);
    addPanelItem(layout,"添加coverage文件",":/res/l_tiledMapLayer4326.png",row,++column);
    ++row;
    QLabel *label_gpx = new QLabel("GPX data provider(gpx)");
    layout->addWidget(label_gpx,row,0);
    ++row;column = -1;
    addPanelItem(layout,"添加gpx文件",":/res/l_tiledMapLayer4326.png",row,++column);
}

void MainWindow::addPanelItem(QGridLayout *layout, const QString &title,const QString &url, int row, int column)
{
    QString objectName = QString("widget_%1_%2").arg(row).arg(column);
    auto p = new PanelImageButton;
    p->setObjectName(objectName);
    p->setTitleText(title);
    p->setImageUrl(url);
    layout->addWidget(p,row,column);
}
