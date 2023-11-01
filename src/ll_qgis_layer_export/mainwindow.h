#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_qgsvectorlayersaveasdialogbase.h"

#include "ll_qgis_base_lib.h"
#include "exportdockwidget.h"
#include "vectorlayersaveasdialog.h"

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
    QString saveAsVectorFileGeneral( QgsVectorLayer *vlayer = nullptr, bool symbologyOption = true, bool onlySelected = false, bool defaultToAddToMap = true );

    QString saveAsVectorFileGeneral( QgsVectorLayer *vlayer, bool symbologyOption, bool onlySelected, bool defaultToAddToMap,
                                     const std::function< void ( const QString &newFilename,
                                         bool addToCanvas,
                                         const QString &layerName,
                                         const QString &encoding,
                                         const QString &vectorFileName )> &onSuccess, const std::function< void ( int error, const QString &errorMessage ) > &onFailure,
                                     VectorLayerSaveAsDialog::Options dialogOptions = VectorLayerSaveAsDialog::AllOptions,
                                     const QString &dialogTitle = QString() );
public slots:
    void setExportParamsSlot(SExportParams eparam);
    void showScaleSlot( double scale );
    void updateMouseCoordinatePrecisionSlot();

    void on_action_open_triggered();
    void on_action_export_triggered();


private:
    Ui::MainWindow *ui;

    ll_qgis_base_lib *mApp = nullptr;
    ExportDockWidget *mExportDockWidget = nullptr;

};
#endif // MAINWINDOW_H
