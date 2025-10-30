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

private slots:
    void setParamsSlot(SParams params);

private:
    Ui::MainWindow *ui;

    ll_qgis_base_lib *mApp = nullptr;
    ParamDockWidget *mParamDockWidget = nullptr;
    SParams mParams;
};
#endif // MAINWINDOW_H
