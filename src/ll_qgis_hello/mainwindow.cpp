#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include "qgis.h" ///引入qgis头文件

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << Qgis::releaseName();
    ui->label->setText("QGis ReleaseName is:"+Qgis::releaseName());
}

MainWindow::~MainWindow()
{
    delete ui;
}

