#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>

#include "panelitem.h"

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
    addPanelItem(layout,"添加shapefile文件",row,++column);
    addPanelItem(layout,"添加gpx文件",row,++column);
    addPanelItem(layout,"添加gpkg文件",row,++column);
    addPanelItem(layout,"添加geojson文件",row,++column);
    ++row;column = -1;
    addPanelItem(layout,"添加gml文件",row,++column);
    addPanelItem(layout,"添加kml文件",row,++column);
    addPanelItem(layout,"添加dxf文件",row,++column);
    addPanelItem(layout,"添加coverage文件",row,++column);
    ++row;
    QLabel *label_gpx = new QLabel("GPX data provider(gpx)");
    layout->addWidget(label_gpx,row,0);
    ++row;column = -1;
    addPanelItem(layout,"添加gpx文件",row,++column);
}

void MainWindow::addPanelItem(QGridLayout *layout, const QString &title, int row, int column)
{
    auto p = new PanelItem;
    p->setTitleText(title);
    layout->addWidget(p,row,column);
}
