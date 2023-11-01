#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>

#include "qgsapplication.h"
#include "qgsdockwidget.h"
#include "qgsproject.h"
#include "qgsmapcanvas.h"
#include "qgsvectorfilewriter.h"
#include "qgsprojectviewsettings.h"
#include "qgscoordinateutils.h"
#include "qgsvectorfilewritertask.h"
//#include "qgsdatumtransformdialog.h"
//#include "qgsmessageviewer.h"

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
    delete mExportDockWidget;
    mExportDockWidget = nullptr;

    mApp->cleanup();

    delete ui;
    // This function *MUST* be the last one called, as it destroys in
    // particular GDAL. As above objects can hold GDAL/OGR objects, it is not
    // safe destroying them afterwards
    QgsApplication::exitQgis();
    // Do *NOT* add anything here !
}

void MainWindow::initialize()
{
    mApp->initialize(this);
    auto gridLayout = new QGridLayout;
    gridLayout->addWidget((QWidget*)mApp->mapCanvas());
    centralWidget()->setLayout(gridLayout);

    addDockWidget(Qt::LeftDockWidgetArea,mApp->layerTreeDock());    
    ui->menuView->addAction(mApp->layerTreeDock()->toggleViewAction());

    mExportDockWidget = new ExportDockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea,mExportDockWidget);
    connect(mExportDockWidget,&ExportDockWidget::setExportParamsSignal,this,&MainWindow::setExportParamsSlot);
    ui->menuView->addAction(mExportDockWidget->toggleViewAction());

    connect( mApp->mapCanvas(), &QgsMapCanvas::scaleChanged, this, &MainWindow::updateMouseCoordinatePrecisionSlot);
    statusBar()->addPermanentWidget( mApp->coordsEdit(), 0 );

    connect( mApp->mapCanvas(), &QgsMapCanvas::scaleChanged, this, &MainWindow::showScaleSlot);
    statusBar()->addPermanentWidget(mApp->scaleWidget());
}

void MainWindow::setExportParamsSlot(SExportParams eparam)
{
    auto layer = QgsProject::instance()->layers<QgsVectorLayer*>().first();

    QgsVectorFileWriter::SaveVectorOptions options;
    options.driverName = eparam.driverName; //ogr driver name
//    options.layerName = "";                 //输出的layer name
    options.actionOnExistingFile = QgsVectorFileWriter::CreateOrOverwriteFile;//文件已存在如何处理
    options.fileEncoding = "UTF-8";         //输出文件的编码格式
//    options.ct = ct;                      //用于坐标转换，这里直接用的图层坐标系，所以暂时不用
//    options.onlySelectedFeatures = false;   //dxf不可以选feature，shape文件默认权限，所以使用默认值
//    options.datasourceOptions =           //HEADER和TRAILER，使用默认值
//    options.layerOptions =                //dxf不能设置layerOptions
    options.skipAttributeCreation = true;   //dxf和shape都没有这个选项
//    options.symbologyExport = //选择导出符号类型
//    options.symbologyScale =  //选择地图的Scale
//    options.filterExtent = filterExtent; //设置导出地图的extent
//    options.overrideGeometryType = autoGeometryType ? QgsWkbTypes::Unknown : forcedGeometryType;  //选择Geometry type
//    options.forceMulti =  //是否创建多种类型的geometries
//    options.includeZ  //是否包含z dimension
//    options.attributes = //只输出选中的attribute
//    options.attributesExportNames =  //输出attribute的名字
//    options.fieldValueConverter = converterPtr;   //属性转换
//    options.saveMetadata =        //输出图层是否包含metadata
//    options.layerMetadata =       //当saveMetadata为true时，将layer的metadata复制
//    QgsVectorFileWriter::writeAsVectorFormatV3(layer,"maps/testExport/test_revert",QgsProject::instance()->transformContext(),options);

    VectorLayerSaveAsDialog *dialog = new VectorLayerSaveAsDialog( layer );
    dialog->exec();
}

void MainWindow::showScaleSlot(double scale)
{
    mApp->scaleWidget()->setScale( scale );
}

void MainWindow::updateMouseCoordinatePrecisionSlot()
{
    mApp->coordsEdit()->setMouseCoordinatesPrecision( QgsCoordinateUtils::calculateCoordinatePrecision( mApp->mapCanvas()->mapUnitsPerPixel(), mApp->mapCanvas()->mapSettings().destinationCrs() ) );
}

void MainWindow::on_action_open_triggered()
{
    QString filename = QFileDialog::getOpenFileName();
    if(filename.isNull())//如果未选择文件则返回
        return;

    QFileInfo ff(filename);
    mApp->addVectorLayer(filename,ff.baseName());
}

void MainWindow::on_action_export_triggered()
{
    auto vlayer = QgsProject::instance()->layers<QgsVectorLayer*>().first();
    saveAsVectorFileGeneral(vlayer,true,false,false);
}

QString MainWindow::saveAsVectorFileGeneral(QgsVectorLayer *vlayer, bool symbologyOption, bool onlySelected, bool defaultToAddToMap)
{
    if ( !vlayer )
      return QString();

    const QString layerId = vlayer->id();

    auto onSuccess = [this, layerId]( const QString & newFilename,
                                      bool addToCanvas,
                                      const QString & layerName,
                                      const QString & encoding,
                                      const QString & vectorFileName )
    {
      if ( addToCanvas )
      {
        QString uri( newFilename );
        if ( !layerName.isEmpty() )
          uri += "|layername=" + layerName;
        bool ok = false;
//        QgsAppLayerHandling::addOgrVectorLayers( {uri}, encoding, QStringLiteral( "file" ), ok );
      }

      // We need to re-retrieve the map layer here, in case it's been deleted during the lifetime of the task
//      if ( QgsVectorLayer *vlayer = qobject_cast< QgsVectorLayer * >( QgsProject::instance()->mapLayer( layerId ) ) )
//        this->emit layerSavedAs( vlayer, vectorFileName );
    };

    auto onFailure = []( int error, const QString & errorMessage )
    {
      if ( error != QgsVectorFileWriter::Canceled )
      {
//        QgsMessageViewer *m = new QgsMessageViewer( nullptr );
//        m->setWindowTitle( tr( "Save Error" ) );
//        m->setMessageAsPlainText( tr( "Export to vector file failed.\nError: %1" ).arg( errorMessage ) );
//        m->exec();
      }
    };

    return saveAsVectorFileGeneral( vlayer, symbologyOption, onlySelected, defaultToAddToMap, onSuccess, onFailure );
}

QString MainWindow::saveAsVectorFileGeneral( QgsVectorLayer *vlayer, bool symbologyOption, bool onlySelected, bool defaultToAddToMap, const std::function<void( const QString &, bool, const QString &, const QString &, const QString & )> &onSuccess, const std::function<void ( int, const QString & )> &onFailure, VectorLayerSaveAsDialog::Options options, const QString &dialogTitle )
{
    QgsCoordinateReferenceSystem destCRS;

    if ( !symbologyOption )
    {
      options &= ~VectorLayerSaveAsDialog::Symbology;
    }

    VectorLayerSaveAsDialog *dialog = new VectorLayerSaveAsDialog( vlayer, options, this );
    if ( !dialogTitle.isEmpty() )
      dialog->setWindowTitle( dialogTitle );

    dialog->setMapCanvas( mApp->mapCanvas() );
    dialog->setIncludeZ( QgsWkbTypes::hasZ( vlayer->wkbType() ) );
    dialog->setOnlySelected( onlySelected );
    dialog->setAddToCanvas( defaultToAddToMap );

    QString vectorFilename;
    if ( dialog->exec() == QDialog::Accepted )
    {
      QString encoding = dialog->encoding();
      vectorFilename = dialog->filename();
      QString format = dialog->format();
      QStringList datasourceOptions = dialog->datasourceOptions();
      bool autoGeometryType = dialog->automaticGeometryType();
      QgsWkbTypes::Type forcedGeometryType = dialog->geometryType();

      QgsCoordinateTransform ct;
      destCRS = dialog->crsObject();

//      if ( destCRS.isValid() )
//      {
//        QgsDatumTransformDialog::run( vlayer->crs(), destCRS, this, mApp->mapCanvas() );
//        ct = QgsCoordinateTransform( vlayer->crs(), destCRS, QgsProject::instance() );
//      }

      QgsRectangle filterExtent = dialog->filterExtent();
//      QgisAppFieldValueConverter converter( vlayer, dialog->attributesAsDisplayedValues() );
//      QgisAppFieldValueConverter *converterPtr = nullptr;
//      // No need to use the converter if there is nothing to convert
//      if ( !dialog->attributesAsDisplayedValues().isEmpty() )
//        converterPtr = &converter;

      QgsVectorFileWriter::SaveVectorOptions options;
      options.driverName = format;
      options.layerName = dialog->layername();
      options.actionOnExistingFile = dialog->creationActionOnExistingFile();
      options.fileEncoding = encoding;
      options.ct = ct;
      options.onlySelectedFeatures = dialog->onlySelected();
      options.datasourceOptions = datasourceOptions;
      options.layerOptions = dialog->layerOptions();
      options.skipAttributeCreation = dialog->selectedAttributes().isEmpty();
      options.symbologyExport = static_cast< QgsVectorFileWriter::SymbologyExport >( dialog->symbologyExport() );
      options.symbologyScale = dialog->scale();
      if ( dialog->hasFilterExtent() )
        options.filterExtent = filterExtent;
      options.overrideGeometryType = autoGeometryType ? QgsWkbTypes::Unknown : forcedGeometryType;
      options.forceMulti = dialog->forceMulti();
      options.includeZ = dialog->includeZ();
      options.attributes = dialog->selectedAttributes();
      options.attributesExportNames = dialog->attributesExportNames();
//      options.fieldValueConverter = converterPtr;
      options.saveMetadata = dialog->persistMetadata();
      options.layerMetadata = vlayer->metadata();

      bool addToCanvas = dialog->addToCanvas();
      QgsVectorFileWriterTask *writerTask = new QgsVectorFileWriterTask( vlayer, vectorFilename, options );

      // when writer is successful:
      connect( writerTask, &QgsVectorFileWriterTask::completed, this, [onSuccess, addToCanvas, encoding, vectorFilename]( const QString & newFilename, const QString & newLayer )
      {
        onSuccess( newFilename, addToCanvas, newLayer, encoding, vectorFilename );
      } );

      // when an error occurs:
      connect( writerTask, &QgsVectorFileWriterTask::errorOccurred, this, [onFailure]( int error, const QString & errorMessage )
      {
        onFailure( error, errorMessage );
      } );

      QgsApplication::taskManager()->addTask( writerTask );
    }

    delete dialog;
    return vectorFilename;
}
