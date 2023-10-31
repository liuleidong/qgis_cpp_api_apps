#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ll_qgis_base_lib.h"
#include "exportdockwidget.h"
#include "qgsstatusbarscalewidget.h"
#include "qgsstatusbarcoordinateswidget.h"
#include "ui_qgsvectorlayersaveasdialogbase.h"

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

public slots:
    void setExportParamsSlot(SExportParams eparam);
    void showScaleSlot( double scale );
    void updateMouseCoordinatePrecisionSlot();
//    QString saveAsVectorFileGeneral( QgsVectorLayer *vlayer = nullptr, bool symbologyOption = true, bool onlySelected = false, bool defaultToAddToMap = true );

//    QString saveAsVectorFileGeneral( QgsVectorLayer *vlayer, bool symbologyOption, bool onlySelected, bool defaultToAddToMap,
//                                     const std::function< void ( const QString &newFilename,
//                                         bool addToCanvas,
//                                         const QString &layerName,
//                                         const QString &encoding,
//                                         const QString &vectorFileName )> &onSuccess, const std::function< void ( int error, const QString &errorMessage ) > &onFailure,
//                                     QgsVectorLayerSaveAsDialog::Options dialogOptions = QgsVectorLayerSaveAsDialog::AllOptions,
//                                     const QString &dialogTitle = QString() );

private:
    Ui::MainWindow *ui;

    ll_qgis_base_lib *mApp = nullptr;
    ExportDockWidget *mExportDockWidget = nullptr;

    QgsStatusBarScaleWidget *mScaleWidget = nullptr;
    QgsStatusBarCoordinatesWidget *mCoordsEdit = nullptr;
};
#endif // MAINWINDOW_H
