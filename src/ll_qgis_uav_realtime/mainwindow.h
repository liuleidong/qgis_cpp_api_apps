#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ll_qgis_base_lib.h"
#include "paramdockwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initialize();

    void addOrMovePoint(const SGeometryInfo &geometryInfo,const QString& prefix);
    void startTimer();

private slots:
    void mockDevices();

private:
    Ui::MainWindow *ui;
    double mPt1X = 11803081;
    double mPt1Y = 4651028;
    double mOffsetX = 100;
    double mOffsetY = 303;

    ll_qgis_base_lib *mApp = nullptr;
    ParamDockWidget *mParamDockWidget = nullptr;

    QgsVectorLayer *mDevPointLayer = nullptr;
    QgsVectorLayer *mDevLineLayer = nullptr;
};
#endif // MAINWINDOW_H
