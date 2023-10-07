#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>

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
}
