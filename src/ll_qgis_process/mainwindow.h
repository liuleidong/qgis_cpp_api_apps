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
#include "qgsprocessingalgorithm.h"
#include "qgsprocessingprovider.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum class Flag
    {
      UseJson = 1 << 0,
      SkipPython = 1 << 1,
      SkipLoadingPlugins = 1 << 2,
    };
    Q_DECLARE_FLAGS( Flags, Flag )

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initialize();


private:
    void loadPlugins();
    void listAlgorithms();
    int showAlgorithmHelp( const QString &id );

    void addVersionInformation( QVariantMap &json );
    void addAlgorithmInformation( QVariantMap &json, const QgsProcessingAlgorithm *algorithm );
    void addProviderInformation( QVariantMap &json, QgsProcessingProvider *provider );

#ifdef WITH_BINDINGS
    std::unique_ptr< QgsPythonUtils > mPythonUtils;
    std::unique_ptr<QgsPythonUtils> loadPythonSupport();
#endif

private:
    Ui::MainWindow *ui;
    Flags mFlags;
    ll_qgis_base_lib *mApp = nullptr;
    ParamDockWidget *mParamDockWidget = nullptr;
};
#endif // MAINWINDOW_H
