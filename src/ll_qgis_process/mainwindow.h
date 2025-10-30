#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ll_qgis_base_lib.h"
#include "paramdockwidget.h"
#include "ld_symbol_property.h"

#ifdef WITH_BINDINGS
#include "qgspythonrunner.h"
#include "qgspythonutils.h"
#endif

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
    void loadPlugins();
#ifdef WITH_BINDINGS
    std::unique_ptr< QgsPythonUtils > mPythonUtils;
    std::unique_ptr<QgsPythonUtils> loadPythonSupport();
#endif

private:
    Ui::MainWindow *ui;

    ll_qgis_base_lib *mApp = nullptr;
    ParamDockWidget *mParamDockWidget = nullptr;
    SParams mParams;
};
#endif // MAINWINDOW_H
