#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ll_qgis_base_lib.h"
#include "exportdockwidget.h"

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

private:
    Ui::MainWindow *ui;

    ll_qgis_base_lib *mApp = nullptr;
    ExportDockWidget *mExportDockWidget = nullptr;
};
#endif // MAINWINDOW_H
