#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>

#include "qgsdockwidget.h"
#include "qgsproject.h"
#include "qgsmapcanvas.h"

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
//    ui->menuParams->addAction(mApp->layerTreeDock()->toggleViewAction());
}
