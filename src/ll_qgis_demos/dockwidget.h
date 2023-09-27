#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class DockWidget;
}

class DockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit DockWidget(QWidget *parent = nullptr);
    ~DockWidget();

public slots:
    void on_pushButton_AddShp_clicked();
    void on_pushButton_AddGpx_clicked();
    void on_pushButton_AddGpkg_clicked();
    void on_pushButton_AddJson_clicked();
    void on_pushButton_AddGml_clicked();
    void on_pushButton_AddKml_clicked();
    void on_pushButton_AddDxf_clicked();
    void on_pushButton_AddCoverage_clicked();
    void on_pushButton_AddGpx1_clicked();
    void on_pushButton_AddCsv_clicked();
    void on_pushButton_AddSpatiaLite_clicked();
    void on_pushButton_AddMemory_clicked();
    void on_pushButton_AddWfs_clicked();
    void on_pushButton_AddRaster_clicked();
    void on_pushButton_AddGpkg1_clicked();
    void on_pushButton_AddWms_clicked();
    void on_pushButton_GdalOffline_clicked();

signals:
    void addShpSignal();
    void addGpxSignal();
    void addGpkgSignal();
    void addGeoJsonSignal();
    void addGmlSignal();
    void addKmlSignal();
    void addDxfSignal();
    void addCoverageSignal();
    void addGpx1Signal();
    void addCsvSignal();
    void addSpatiaLite();
    void addMemorySignal();
    void addWfsSignal();
    void addRasterSignal();
    void addGpkg1Signal();
    void addWmsSignal();
    void addGdalOfflineSignal();

private:
    Ui::DockWidget *ui;
};

#endif // DOCKWIDGET_H
