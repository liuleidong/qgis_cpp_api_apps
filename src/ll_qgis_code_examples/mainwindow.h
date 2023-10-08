#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ll_qgis_base_lib.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QGridLayout;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initialize();
    void initToolbar();
    void initStackwidgetPageCanvas();
    void initPanels();
    void initGroupboxInPanel();
    void init_groupBox_maps();

public slots:
    void actionToPanelsSlot();
    void actionToCanvasSlot();
    void panelImageButtonClickedSlot(QString slotName);
    void stackWidgetCurentChangedSlot(int index);
    //
    void addShpSlot();
    void addGpxSlot();
    void addGpkgSlot();
    void addGeoJsonSlot();
    void addGmlSlot();
    void addKmlSlot();
    void addDxfSlot();
    void addCoverageSlot();
    void addGpx1Slot();
    void addCsvSlot();
    void addSpatiaLiteSlot();
    void addMemorySlot();
    void addWfsSlot();
    void addRasterSlot();
    void addGpkg1Slot();
    void addWmsSlot();
    void addGdalOfflineSlot();
private:
    void addPanelItem(QGridLayout *layout,const QString &objectName,const QString &title,const QString &url,int row,int column);

private:
    ll_qgis_base_lib *mApp = nullptr;
    Ui::MainWindow *ui;


};
#endif // MAINWINDOW_H
