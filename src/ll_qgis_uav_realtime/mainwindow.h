#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ll_qgis_base_lib.h"
#include "paramdockwidget.h"
#include "ld_symbol_property.h"

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

    void setPointLayerSimpleMarker(QgsVectorLayer *layer,const SMarkerSymbolSimple& simpleMarker);
    void setPointLayerSvgMarker(QgsVectorLayer *layer,const SMarkerSymbolSvg& svgMarker);
    void addOrMovePoint(const SGeometryInfo &geometryInfo,const QString& prefix);
    void addOrMoveLine(const SGeometryInfo &geometryInfo,int trajectoryLength=20);
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
    int mTrajectoryLength = 20;
};
#endif // MAINWINDOW_H
