#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>
#include <QTimer>
#include <QRandomGenerator>
#include <QLibrary>
#include <QJsonArray>
#include <QTextBrowser>
#include <QMessageBox>
#include <QDesktopServices>

#include "qgsdockwidget.h"
#include "qgsproject.h"
#include "qgsmapcanvas.h"
#include "qgsmarkersymbollayer.h"
#include "qgsapplication.h"

#include "qgsproviderregistry.h"
#include "qgsprocessingregistry.h"
#include "qgsnativealgorithms.h"
#include "qgsprocessingparametertype.h"
#include "qgsprocessingmodelalgorithm.h"

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
    ui->menuParams->addAction(mApp->layerTreeDock()->toggleViewAction());

    mParamDockWidget = new ParamDockWidget(this);
    this->addDockWidget(Qt::LeftDockWidgetArea,mParamDockWidget);
    ui->menuParams->addAction(mParamDockWidget->toggleViewAction());

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

    connect(mParamDockWidget,&ParamDockWidget::showAlgHelp,this,&MainWindow::showAlgHelp);
    connect(mParamDockWidget,&ParamDockWidget::algRun,this,&MainWindow::algRun);
}

void MainWindow::showAlgHelp(const QString &id)
{
    showAlgorithmHelp(id);
}


void MainWindow::algRun(const QString &id)
{
    if(id.compare("qgis:basicstatisticsforfields") == 0)
    {
        //添加测试图层
        QString filename = QStringLiteral("maps/shapefile/myplaces.shp");
        QVariantMap conf;
        conf.insert(QStringLiteral("INPUT_LAYER"),filename);//直接用路径
        conf.insert(QStringLiteral("FIELD_NAME"),QStringLiteral("name"));
        QgsProcessingOutputLayerDefinition value( "TEMPORARY_OUTPUT" );
        conf.insert(QStringLiteral("OUTPUT_HTML_FILE"),value);
        auto algorithm = QgsApplication::processingRegistry()->createAlgorithmById(id,conf);
        QgsProcessingContext *context = new QgsProcessingContext;
        context->setProject(QgsProject::instance());
        QgsProcessingFeedback *feedback = new QgsProcessingFeedback(false);
        QVariantMap runResults = algorithm->run(conf,*context,feedback);
        QString htmlFilePath = runResults["OUTPUT_HTML_FILE"].toString();
        statusBar()->showMessage(QStringLiteral("统计结果文件路径: %1").arg(htmlFilePath), 10000);
        QDesktopServices::openUrl(QUrl::fromLocalFile(htmlFilePath));
    }
    else if(id.compare("native:buffer") == 0)
    {
        //添加测试图层
        QString filename = QStringLiteral("maps/shapefile/myplaces.shp");
        QFileInfo ff(filename);
        QgsVectorLayer* layer = (QgsVectorLayer*)mApp->addVectorLayer(filename,ff.baseName());
        QVariantMap conf;
        conf.insert(QStringLiteral("INPUT"),layer->id());//使用layer id或者使用物理路径均可
        conf.insert(QStringLiteral("DISTANCE"),"100");
        QgsProcessingOutputLayerDefinition value( "TEMPORARY_OUTPUT" );
        conf.insert(QStringLiteral("OUTPUT"),value);
        auto algorithm = QgsApplication::processingRegistry()->createAlgorithmById(id,conf);
        QgsProcessingContext *context = new QgsProcessingContext;
        context->setProject(QgsProject::instance());
        QgsProcessingFeedback *feedback = new QgsProcessingFeedback(false);
        QVariantMap runResults = algorithm->run(conf,*context,feedback);
        QgsMapLayer *tempLayer = context->getMapLayer(runResults["OUTPUT"].toString());
        if(layer)
        {
            QgsProject::instance()->addMapLayer(tempLayer);
        }
    }
    else if(id.compare("gdal:aspect") == 0)
    {
        QString filename = QStringLiteral("maps/raster/3420C_2010_327_RGB_LATLNG.tif");
        QFileInfo ff(filename);
        QgsRasterLayer* layer = (QgsRasterLayer*)mApp->addRasterLayer(filename,ff.baseName());

        QVariantMap conf;
        conf.insert(QStringLiteral("INPUT"), layer->id());
        conf.insert(QStringLiteral("BAND"), 1);

        QgsProcessingOutputLayerDefinition value( "TEMPORARY_OUTPUT" );
        conf.insert(QStringLiteral("OUTPUT"), value);

        auto algorithm = QgsApplication::processingRegistry()->createAlgorithmById(id,conf);
        QgsProcessingContext *context = new QgsProcessingContext;
        context->setProject(QgsProject::instance());
        QgsProcessingFeedback *feedback = new QgsProcessingFeedback(false);

        QVariantMap runResults = algorithm->run(conf,*context,feedback);

        QString tempFilename = runResults["OUTPUT"].toString();
        QFileInfo t(tempFilename);
        QgsRasterLayer* tempLayer = (QgsRasterLayer*)mApp->addRasterLayer(tempFilename,t.baseName());
    }
    else
    {
        QString message = QStringLiteral("当前尝试运行的算法 ID: **") + id + QStringLiteral("** 暂不支持。\n\n");
        message += QStringLiteral("目前支持的算法包括：\n");
        message += QStringLiteral("1. native:buffer (C++ 原生算法，用于创建缓冲区)\n");
        message += QStringLiteral("2. qgis:basicstatisticsforfields (Python 算法，用于计算字段统计信息)\n");
        message += QStringLiteral("3. gdal:aspect (GDAL/Python 算法，用于计算栅格坡向)\n\n");
        message += QStringLiteral("欢迎补充更多算法的使用方法！https://gitee.com/richie12/qgis_cpp_api_apps");

        QMessageBox::information(nullptr,
                                 QStringLiteral("算法演示不支持"),
                                 message,
                                 QMessageBox::Ok);
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

    // 用于GUI显示的数据结构
    QJsonObject algorithmsData;

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

      // 构建GUI需要的数据结构（无论是否使用JSON都构建）
      QJsonObject providerData;
      providerData["name"] = provider->name();
      providerData["long_name"] = provider->name(); // 如果没有长名称，使用短名称
      providerData["is_active"] = provider->isActive();
      providerData["can_be_activated"] = true; // 假设都可以激活
      providerData["default_raster_file_extension"] = provider->defaultRasterFileExtension();
      providerData["default_vector_file_extension"] = provider->defaultVectorFileExtension();
      providerData["supports_non_file_based_output"] = true; // 假设支持
      providerData["version"] = provider->versionInfo();

      // 支持的扩展名列表（简化处理）
      QStringList rasterExtensions, vectorExtensions, tableExtensions;
      // 这里可以根据需要添加实际的扩展名支持
      rasterExtensions << "tif" << "img" << "vrt";
      vectorExtensions << "shp" << "gpkg" << "geojson";
      tableExtensions << "csv" << "dbf" << "xlsx";

      providerData["supported_output_raster_extensions"] = QJsonArray::fromStringList(rasterExtensions);
      providerData["supported_output_vector_extensions"] = QJsonArray::fromStringList(vectorExtensions);
      providerData["supported_output_table_extensions"] = QJsonArray::fromStringList(tableExtensions);

      // 构建算法数据
      QJsonObject algorithmsDataForProvider;
      for ( const QgsProcessingAlgorithm *algorithm : algorithms )
      {
        if ( algorithm->flags() & Qgis::ProcessingAlgorithmFlag::NotAvailableInStandaloneTool )
          continue;

        QJsonObject algorithmData;
        algorithmData["name"] = algorithm->displayName();
        algorithmData["id"] = algorithm->id();
        algorithmData["group"] = algorithm->group();
        algorithmData["short_description"] = algorithm->shortDescription();
        algorithmData["help_url"] = algorithm->helpUrl();
        algorithmData["can_cancel"] = true; // 假设可以取消
        bool isDeprecated = (algorithm->flags() & Qgis::ProcessingAlgorithmFlag::Deprecated);
        algorithmData["deprecated"] = isDeprecated;
        algorithmData["has_known_issues"] = false; // 需要根据实际情况设置
        algorithmData["requires_matching_crs"] = false; // 需要根据实际情况设置

        // 标签
        QStringList tags;
        tags << algorithm->groupId(); // 使用算法组ID作为标签
        if (algorithm->tags().size() > 0) {
          tags << algorithm->tags();
        }
        algorithmData["tags"] = QJsonArray::fromStringList(tags);

        algorithmsDataForProvider[algorithm->id()] = algorithmData;
      }

      providerData["algorithms"] = algorithmsDataForProvider;
      algorithmsData[provider->id()] = providerData;
    }

    // 创建并显示算法面板（GUI模式）
    if ((mFlags & Flag::UseJson))
    {
      // 转换QVariantMap为QJsonObject
      QJsonObject guiAlgorithmsData;
      for (auto it = algorithmsData.begin(); it != algorithmsData.end(); ++it) {
        guiAlgorithmsData[it.key()] = it.value();
      }

      mParamDockWidget->setAlgorithmsData(guiAlgorithmsData);
    }
}

int MainWindow::showAlgorithmHelp(const QString &inputId)
{
    QString id = inputId;

    std::unique_ptr< QgsProcessingModelAlgorithm > model;
    const QgsProcessingAlgorithm *alg = nullptr;
    if ( QFile::exists( id ) && QFileInfo( id ).suffix() == QLatin1String( "model3" ) )
    {
      model = std::make_unique< QgsProcessingModelAlgorithm >();
      if ( !model->fromFile( id ) )
      {
        std::cerr << QStringLiteral( "File %1 is not a valid Processing model!\n" ).arg( id ).toLocal8Bit().constData();
        return 1;
      }

      alg = model.get();
    }
  #ifdef WITH_BINDINGS
    else if ( mPythonUtils && QFile::exists( id ) && QFileInfo( id ).suffix() == QLatin1String( "py" ) )
    {
      QString res;
      if ( !mPythonUtils->evalString( QStringLiteral( "qgis.utils.import_script_algorithm(\"%1\")" ).arg( id ), res ) || res.isEmpty() )
      {
        std::cerr << QStringLiteral( "File %1 is not a valid Processing script!\n" ).arg( id ).toLocal8Bit().constData();
        return 1;
      }

      id = res;
    }
  #endif

    if ( !alg )
    {
      alg = QgsApplication::processingRegistry()->algorithmById( id );
      if ( ! alg )
      {
        std::cerr << QStringLiteral( "Algorithm %1 not found!\n" ).arg( id ).toLocal8Bit().constData();
        return 1;
      }
    }

    if ( alg->flags() & Qgis::ProcessingAlgorithmFlag::NotAvailableInStandaloneTool )
    {
      std::cerr << QStringLiteral( "The \"%1\" algorithm is not available for use outside of the QGIS desktop application\n" ).arg( id ).toLocal8Bit().constData();
      return 1;
    }

    QVariantMap json;
    if ( !( mFlags & Flag::UseJson ) )
    {
      std::cout << QStringLiteral( "%1 (%2)\n" ).arg( alg->displayName(), alg->id() ).toLocal8Bit().constData();

      std::cout << "\n----------------\n";
      std::cout << "Description\n";
      std::cout << "----------------\n";

      if ( const QgsProcessingModelAlgorithm *model = dynamic_cast< const QgsProcessingModelAlgorithm * >( alg ) )
      {
        // show finer help content for models
        const QVariantMap help = model->helpContent();
        std::cout << help.value( QStringLiteral( "ALG_DESC" ) ).toString().toLocal8Bit().constData() << '\n';

        if ( !help.value( QStringLiteral( "ALG_CREATOR" ) ).toString().isEmpty() ||
             !help.value( QStringLiteral( "ALG_VERSION" ) ).toString().isEmpty() )
          std::cout << '\n';

        if ( !help.value( QStringLiteral( "ALG_CREATOR" ) ).toString().isEmpty() )
          std::cout << "Algorithm author:\t" << help.value( QStringLiteral( "ALG_CREATOR" ) ).toString().toLocal8Bit().constData() << '\n';
        if ( !help.value( QStringLiteral( "ALG_VERSION" ) ).toString().isEmpty() )
          std::cout << "Algorithm version:\t" << help.value( QStringLiteral( "ALG_VERSION" ) ).toString().toLocal8Bit().constData() << '\n';

        if ( !help.value( QStringLiteral( "EXAMPLES" ) ).toString().isEmpty() )
        {
          std::cout << "\n----------------\n";
          std::cout << "Examples\n";
          std::cout << "----------------\n";
          std::cout << help.value( QStringLiteral( "EXAMPLES" ) ).toString().toLocal8Bit().constData() << '\n';
        }
      }
      else
      {
        if ( !alg->shortDescription().isEmpty() )
          std::cout << alg->shortDescription().toLocal8Bit().constData() << '\n';
        if ( !alg->shortHelpString().isEmpty() && alg->shortHelpString() != alg->shortDescription() )
          std::cout << alg->shortHelpString().toLocal8Bit().constData() << '\n';
      }

      std::cout << "\n----------------\n";
      std::cout << "Arguments\n";
      std::cout << "----------------\n\n";
    }
    else
    {
      addVersionInformation( json );

      QVariantMap algorithmDetails;
      algorithmDetails.insert( QStringLiteral( "id" ), alg->id() );
      addAlgorithmInformation( algorithmDetails, alg );
      json.insert( QStringLiteral( "algorithm_details" ), algorithmDetails );
      QVariantMap providerJson;
      if ( alg->provider() )
        addProviderInformation( providerJson, alg->provider() );
      json.insert( QStringLiteral( "provider_details" ), providerJson );
    }

    QgsProcessingContext context;
    QVariantMap parametersJson;
    const QgsProcessingParameterDefinitions defs = alg->parameterDefinitions();
    for ( const QgsProcessingParameterDefinition *p : defs )
    {
      if ( p->flags() & Qgis::ProcessingParameterFlag::Hidden )
        continue;

      QVariantMap parameterJson;

      if ( !( mFlags & Flag::UseJson ) )
      {
        QString line = QStringLiteral( "%1: %2" ).arg( p->name(), p->description() );
        if ( p->flags() & Qgis::ProcessingParameterFlag::Optional )
          line += QLatin1String( " (optional)" );
        std::cout << QStringLiteral( "%1\n" ).arg( line ).toLocal8Bit().constData();

        if ( p->defaultValue().isValid() )
        {
          bool ok = false;
          std::cout << QStringLiteral( "\tDefault value:\t%1\n" ).arg( p->valueAsString( p->defaultValue(), context, ok ) ).toLocal8Bit().constData();
        }
      }
      else
      {
        parameterJson.insert( QStringLiteral( "name" ), p->name() );
        parameterJson.insert( QStringLiteral( "description" ), p->description() );


        if ( const QgsProcessingParameterType *type = QgsApplication::processingRegistry()->parameterType( p->type() ) )
        {
          QVariantMap typeDetails;
          typeDetails.insert( QStringLiteral( "id" ), type->id() );
          typeDetails.insert( QStringLiteral( "name" ), type->name() );
          typeDetails.insert( QStringLiteral( "description" ), type->description() );
          typeDetails.insert( QStringLiteral( "metadata" ), type->metadata() );
          typeDetails.insert( QStringLiteral( "acceptable_values" ), type->acceptedStringValues() );

          parameterJson.insert( QStringLiteral( "type" ), typeDetails );
        }
        else
        {
          parameterJson.insert( QStringLiteral( "type" ), p->type() );
        }

        parameterJson.insert( QStringLiteral( "is_destination" ), p->isDestination() );
        parameterJson.insert( QStringLiteral( "default_value" ), p->defaultValue() );
        parameterJson.insert( QStringLiteral( "optional" ), bool( p->flags() & Qgis::ProcessingParameterFlag::Optional ) );
        parameterJson.insert( QStringLiteral( "is_advanced" ), bool( p->flags() & Qgis::ProcessingParameterFlag::Advanced ) );

        parameterJson.insert( QStringLiteral( "raw_definition" ), p->toVariantMap() );
      }

      if ( ! p->help().isEmpty() )
      {
        if ( !( mFlags & Flag::UseJson ) )
          std::cout << QStringLiteral( "\t%1\n" ).arg( p->help() ).toLocal8Bit().constData();
        else
          parameterJson.insert( QStringLiteral( "help" ), p->help() );
      }
      if ( !( mFlags & Flag::UseJson ) )
        std::cout << QStringLiteral( "\tArgument type:\t%1\n" ).arg( p->type() ).toLocal8Bit().constData();

      if ( p->type() == QgsProcessingParameterEnum::typeName() )
      {
        const QgsProcessingParameterEnum *enumParam = static_cast< const QgsProcessingParameterEnum * >( p );
        QStringList options;
        QVariantMap jsonOptions;
        for ( int i = 0; i < enumParam->options().count(); ++i )
        {
          options << QStringLiteral( "\t\t- %1: %2" ).arg( i ).arg( enumParam->options().at( i ) );
          jsonOptions.insert( QString::number( i ), enumParam->options().at( i ) );
        }

        if ( !( mFlags & Flag::UseJson ) )
          std::cout << QStringLiteral( "\tAvailable values:\n%1\n" ).arg( options.join( '\n' ) ).toLocal8Bit().constData();
        else
          parameterJson.insert( QStringLiteral( "available_options" ), jsonOptions );
      }

      // acceptable command line values
      if ( !( mFlags & Flag::UseJson ) )
      {
        if ( const QgsProcessingParameterType *type = QgsApplication::processingRegistry()->parameterType( p->type() ) )
        {
          const QStringList values = type->acceptedStringValues();
          if ( !values.isEmpty() )
          {
            std::cout << "\tAcceptable values:\n";
            for ( const QString &val : values )
            {
              std::cout << QStringLiteral( "\t\t- %1" ).arg( val ).toLocal8Bit().constData() << "\n";
            }
          }
        }
      }

      parametersJson.insert( p->name(), parameterJson );
    }

    QVariantMap outputsJson;
    if ( !( mFlags & Flag::UseJson ) )
    {
      std::cout << "\n----------------\n";
      std::cout << "Outputs\n";
      std::cout << "----------------\n\n";
    }
    const QgsProcessingOutputDefinitions outputs = alg->outputDefinitions();
    for ( const QgsProcessingOutputDefinition *o : outputs )
    {
      QVariantMap outputJson;
      if ( !( mFlags & Flag::UseJson ) )
      {
        std::cout << QStringLiteral( "%1: <%2>\n" ).arg( o->name(), o->type() ).toLocal8Bit().constData();
        if ( !o->description().isEmpty() )
          std::cout << "\t" << o->description().toLocal8Bit().constData() << '\n';
      }
      else
      {
        outputJson.insert( QStringLiteral( "description" ), o->description() );
        outputJson.insert( QStringLiteral( "type" ), o->type() );
        outputsJson.insert( o->name(), outputJson );
      }
    }

    if ( !( mFlags & Flag::UseJson ) )
    {
      std::cout << "\n\n";
    }
    else
    {
      json.insert( QStringLiteral( "parameters" ), parametersJson );
      json.insert( QStringLiteral( "outputs" ), outputsJson );
      std::cout << QgsJsonUtils::jsonFromVariant( json ).dump( 2 );
      QString jsonString = QString::fromStdString(QgsJsonUtils::jsonFromVariant(json).dump(2));
      mParamDockWidget->detailBrowser()->clear();
      mParamDockWidget->detailBrowser()->setText(jsonString);
    }

    return 0;

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
