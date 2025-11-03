#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>
#include <QTimer>
#include <QRandomGenerator>
#include <QLibrary>

#include "qgsdockwidget.h"
#include "qgsproject.h"
#include "qgsmapcanvas.h"
#include "qgsmarkersymbollayer.h"
#include "qgsapplication.h"

#include "qgsproviderregistry.h"
#include "qgsprocessingregistry.h"
#include "qgsnativealgorithms.h"

#include "qgsjsonutils.h"

#include <nlohmann/json.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();
    mApp = ll_qgis_base_lib::Instance();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    mApp->initialize(this);
    auto gridLayout = new QGridLayout;
    gridLayout->addWidget((QWidget*)mApp->mapCanvas());
    centralWidget()->setLayout(gridLayout);

    addDockWidget(Qt::LeftDockWidgetArea,mApp->layerTreeDock());
    //ui->menuParams->addAction(mApp->layerTreeDock()->toggleViewAction());

    mParamDockWidget = new ParamDockWidget(this);
    this->addDockWidget(Qt::LeftDockWidgetArea,mParamDockWidget);
    //ui->menuParams->addAction(mParamDockWidget->toggleViewAction());

    connect(mParamDockWidget,&ParamDockWidget::setParamsSignal,this,&MainWindow::setParamsSlot);

    // core providers
    QgsApplication::processingRegistry()->addProvider( new QgsNativeAlgorithms( QgsApplication::processingRegistry() ) );

#ifdef WITH_BINDINGS
      // give Python plugins a chance to load providers
      mPythonUtils = loadPythonSupport();
      if ( !mPythonUtils )
      {
        //MessageBox
      }
#endif
    mFlags = Flag::UseJson;
    loadPlugins();

    listAlgorithms();
}


void MainWindow::setParamsSlot(SParams params)
{
    mParams = params;
    if(mParams.mode == 0)
    {
        SMarkerSymbolSimple simpleMarker;
        simpleMarker.name = mParams.shape;
        simpleMarker.color = mParams.color.name();
        simpleMarker.size = QString("%1").arg(mParams.size);
    }
    else if(mParams.mode == 1)
    {
        SMarkerSymbolSvg svgMarker;
        svgMarker.name = mParams.svgPath;
        svgMarker.color = mParams.color.name();
        svgMarker.size = QString("%1").arg(mParams.size);
        svgMarker.angle = QString("%1").arg(mParams.angle);
    }
}

void MainWindow::loadPlugins()
{
#ifdef WITH_BINDINGS
  if ( !mPythonUtils )
    return;

  QgsSettings settings;
  // load plugins
  const QStringList plugins = mPythonUtils->pluginList();
  for ( const QString &plugin : plugins )
  {
    if ( plugin == QLatin1String( "processing" ) || ( mPythonUtils->isPluginEnabled( plugin ) && mPythonUtils->pluginHasProcessingProvider( plugin ) ) )
    {
      if ( !mPythonUtils->loadPlugin( plugin ) )
      {
        std::cerr << "error loading plugin: " << plugin.toLocal8Bit().constData() << "\n\n";
      }
      else if ( !mPythonUtils->startProcessingPlugin( plugin ) )
      {
        std::cerr << "error starting plugin: " << plugin.toLocal8Bit().constData() << "\n\n";
      }
    }
  }

  if ( !mPythonUtils->finalizeProcessingStartup() )
  {
    std::cerr << "error finalizing Processing plugin startup\n\n";
  }

#endif

}

void MainWindow::listAlgorithms()
{
    QVariantMap json;
    if ( !( mFlags & Flag::UseJson ) )
    {
      std::cout << "Available algorithms\n\n";
    }
    else
    {
      addVersionInformation( json );
    }

    const QList<QgsProcessingProvider *> providers = QgsApplication::processingRegistry()->providers();
    QVariantMap jsonProviders;
    for ( QgsProcessingProvider *provider : providers )
    {
      QVariantMap providerJson;

      if ( !( mFlags & Flag::UseJson ) )
      {
        std::cout << provider->name().toLocal8Bit().constData() << "\n";
      }
      else
      {
        addProviderInformation( providerJson, provider );
      }
      QVariantMap algorithmsJson;
      const QList<const QgsProcessingAlgorithm *> algorithms = provider->algorithms();
      for ( const QgsProcessingAlgorithm *algorithm : algorithms )
      {
        if ( algorithm->flags() & Qgis::ProcessingAlgorithmFlag::NotAvailableInStandaloneTool )
          continue;

        if ( !( mFlags & Flag::UseJson ) )
        {
          if ( algorithm->flags() & Qgis::ProcessingAlgorithmFlag::Deprecated )
            continue;
          std::cout << "\t" << algorithm->id().toLocal8Bit().constData() << "\t" << algorithm->displayName().toLocal8Bit().constData() << "\n";
        }
        else
        {
          QVariantMap algorithmJson;
          addAlgorithmInformation( algorithmJson, algorithm );
          algorithmsJson.insert( algorithm->id(), algorithmJson );
        }
      }

      if ( !( mFlags & Flag::UseJson ) )
      {
        std::cout << "\n";
      }
      else
      {
        providerJson.insert( QStringLiteral( "algorithms" ), algorithmsJson );
        jsonProviders.insert( provider->id(), providerJson );
      }
    }

    if ( mFlags & Flag::UseJson )
    {
      json.insert( QStringLiteral( "providers" ), jsonProviders );
      std::cout << QgsJsonUtils::jsonFromVariant( json ).dump( 2 );
    }
}

void MainWindow::addVersionInformation(QVariantMap &json)
{
    json.insert( QStringLiteral( "qgis_version" ), Qgis::version() );
    if ( QString( Qgis::devVersion() ) != QLatin1String( "exported" ) )
    {
      json.insert( QStringLiteral( "qgis_code_revision" ), Qgis::devVersion() );
    }
    json.insert( QStringLiteral( "qt_version" ), qVersion() );
    json.insert( QStringLiteral( "python_version" ), PYTHON_VERSION );
//    json.insert( QStringLiteral( "gdal_version" ), GDALVersionInfo( "RELEASE_NAME" ) );
//    json.insert( QStringLiteral( "geos_version" ), GEOSversion() );

//    PJ_INFO info = proj_info();
//    json.insert( QStringLiteral( "proj_version" ), info.release );
}

void MainWindow::addAlgorithmInformation(QVariantMap &algorithmJson, const QgsProcessingAlgorithm *algorithm)
{
    algorithmJson.insert( QStringLiteral( "name" ), algorithm->displayName() );
    algorithmJson.insert( QStringLiteral( "short_description" ), algorithm->shortDescription() );
    algorithmJson.insert( QStringLiteral( "tags" ), algorithm->tags() );
    algorithmJson.insert( QStringLiteral( "help_url" ), algorithm->helpUrl() );
    algorithmJson.insert( QStringLiteral( "group" ), algorithm->group() );
    algorithmJson.insert( QStringLiteral( "can_cancel" ), bool( algorithm->flags() & Qgis::ProcessingAlgorithmFlag::CanCancel ) );
    algorithmJson.insert( QStringLiteral( "requires_matching_crs" ), bool( algorithm->flags() & Qgis::ProcessingAlgorithmFlag::RequiresMatchingCrs ) );
    algorithmJson.insert( QStringLiteral( "has_known_issues" ), bool( algorithm->flags() & Qgis::ProcessingAlgorithmFlag::KnownIssues ) );
    algorithmJson.insert( QStringLiteral( "deprecated" ), bool( algorithm->flags() & Qgis::ProcessingAlgorithmFlag::Deprecated ) );

}

void MainWindow::addProviderInformation(QVariantMap &providerJson, QgsProcessingProvider *provider)
{
    providerJson.insert( QStringLiteral( "name" ), provider->name() );
    providerJson.insert( QStringLiteral( "long_name" ), provider->longName() );
    providerJson.insert( QStringLiteral( "version" ), provider->versionInfo() );
    providerJson.insert( QStringLiteral( "can_be_activated" ), provider->canBeActivated() );
    if ( !provider->warningMessage().isEmpty() )
    {
      providerJson.insert( QStringLiteral( "warning" ), provider->warningMessage() );
    }
    providerJson.insert( QStringLiteral( "is_active" ), provider->isActive() );
    providerJson.insert( QStringLiteral( "supported_output_raster_extensions" ), provider->supportedOutputRasterLayerExtensions() );
    providerJson.insert( QStringLiteral( "supported_output_vector_extensions" ), provider->supportedOutputVectorLayerExtensions() );
    providerJson.insert( QStringLiteral( "supported_output_table_extensions" ), provider->supportedOutputTableExtensions() );
    providerJson.insert( QStringLiteral( "default_vector_file_extension" ), provider->defaultVectorFileExtension() );
    providerJson.insert( QStringLiteral( "default_raster_file_extension" ), provider->defaultRasterFileExtension() );
    providerJson.insert( QStringLiteral( "supports_non_file_based_output" ), provider->supportsNonFileBasedOutput() );
}

#ifdef WITH_BINDINGS
std::unique_ptr<QgsPythonUtils> MainWindow::loadPythonSupport()
{
    QString pythonlibName( QStringLiteral( "qgispython" ) );
    #if defined(Q_OS_UNIX) && !defined(Q_OS_ANDROID)
    pythonlibName.prepend( QgsApplication::libraryPath() );
    #endif
    #ifdef __MINGW32__
    pythonlibName.prepend( "lib" );
    #endif
    QString version = QStringLiteral( "%1.%2.%3" ).arg( Qgis::versionInt() / 10000 ).arg( Qgis::versionInt() / 100 % 100 ).arg( Qgis::versionInt() % 100 );
    QgsDebugMsgLevel( QStringLiteral( "load library %1 (%2)" ).arg( pythonlibName, version ), 1 );
    QLibrary pythonlib( pythonlibName, version );
    // It's necessary to set these two load hints, otherwise Python library won't work correctly
    // see http://lists.kde.org/?l=pykde&m=117190116820758&w=2
    pythonlib.setLoadHints( QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint );
    if ( !pythonlib.load() )
    {
      pythonlib.setFileName( pythonlibName );
      if ( !pythonlib.load() )
      {
        std::cerr << QStringLiteral( "Couldn't load Python support library: %1\n" ).arg( pythonlib.errorString() ).toLocal8Bit().constData();
        return nullptr;
      }
    }

    typedef QgsPythonUtils*( *inst )();
    inst pythonlib_inst = reinterpret_cast< inst >( cast_to_fptr( pythonlib.resolve( "instance" ) ) );
    if ( !pythonlib_inst )
    {
      //using stderr on purpose because we want end users to see this [TS]
      std::cerr << "Couldn't resolve Python support library's instance() symbol.\n";
      return nullptr;
    }

    std::unique_ptr< QgsPythonUtils > pythonUtils( pythonlib_inst() );
    if ( pythonUtils )
    {
      pythonUtils->initPython( nullptr, false );
    }

    return pythonUtils;

}
#endif
