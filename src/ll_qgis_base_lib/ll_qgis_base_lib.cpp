#include "ll_qgis_base_lib.h"

#include <QMainWindow>
#include <QColor>
#include <QAction>
#include <QToolButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QUrlQuery>
#include <QStatusBar>
#include <QLabel>
#include <QValidator>
#include <QPainter>

#include "qgsapplication.h"
#include "qgsmapcanvas.h"
#include "qgsmaptoolpan.h"
#include "qgsmaplayer.h"
#include "qgsdockwidget.h"
#include "qgslayertree.h"
#include "qgslayertreeview.h"
#include "qgslayertreemapcanvasbridge.h"
#include "qgslayertreeregistrybridge.h"
#include "qgslayertreeviewdefaultactions.h"
#include "qgslayertreemodel.h"
#include "qgsvectorlayer.h"
#include "qgsrasterlayer.h"
#include "qgsmaplayerfactory.h"
#include "qgsbrightnesscontrastfilter.h"
#include "qgshuesaturationfilter.h"
#include "qgsrasterresampler.h"
#include "qgscubicrasterresampler.h"
#include "qgsrasterresamplefilter.h"

#include "qgspallabeling.h"
#include "qgsvectorlayerlabeling.h"
#include "qgsdoublespinbox.h"

#include "qgsdiagramrenderer.h"
#include "qgspiediagram.h"

#include "qgssinglesymbolrenderer.h"
#include "qgspointdisplacementrenderer.h"
#include "qgspointclusterrenderer.h"
#include "qgsheatmaprenderer.h"
#include "qgsinvertedpolygonrenderer.h"
#include "qgsembeddedsymbolrenderer.h"
#include "qgs25drenderer.h"
#include "qgsgraduatedsymbolrenderer.h"
#include "qgsclassificationmethod.h"
#include "qgsclassificationmethodregistry.h"
#include "qgscategorizedsymbolrenderer.h"
#include "qgsexpressioncontextutils.h"
#include "qgscolorrampimpl.h"
#include "qgsrastershader.h"
#include "qgssinglebandpseudocolorrenderer.h"
#include "qgshillshaderenderer.h"
#include "qgsmultibandcolorrenderer.h"
#include "qgspalettedrasterrenderer.h"
#include "qgsrastercontourrenderer.h"
#include "qgssymbol.h"
#include "qgsmarkersymbol.h"
#include "qgsmarkersymbollayer.h"
#include "qgsfillsymbol.h"
#include "qgsfillsymbollayer.h"
#include "qgslinesymbol.h"
#include "qgslinesymbollayer.h"
#include "qgsannotationlayer.h"
#include "qgsannotationmarkeritem.h"

#include "qgsvirtuallayerdefinition.h"
#include "qgsvectorfilewriter.h"
//Raster
#include "qgssinglebandgrayrenderer.h"
#include "qgscontrastenhancement.h"
#include "qgsrastertransparency.h"

#include "ll_qgis_base_lib_layertreeview_menu.h"
#include "ll_qgis_base_lib_layerhandling.h"

ll_qgis_base_lib::ll_qgis_base_lib()
{

}

QString ll_qgis_base_lib::version()
{
    QString ver = QString::fromLocal8Bit("1.0.0");
    return ver;
}

void ll_qgis_base_lib::initialize(QMainWindow *mainWindow)
{
    mMainWindow = mainWindow;

    mMapCanvas = new QgsMapCanvas;
    mMapCanvas->enableAntiAliasing(true);
    mMapCanvas->setCachingEnabled(true);
    mMapCanvas->setCanvasColor(QColor(255,255,255));
    mMapCanvas->setVisible(true);

    initLayerTreeView();
    initMaptools();
}

void ll_qgis_base_lib::initLayerTreeView()
{
    //添加DockWidget作为图层树的容器
    mLayerTreeDock = new QgsDockWidget(tr("Layer Tree"));
    mLayerTreeDock->setObjectName( QStringLiteral( "Layers" ) );
    mLayerTreeDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

    //创建Model
    QgsLayerTreeModel* model = new QgsLayerTreeModel(QgsProject::instance()->layerTreeRoot(),this);
    model->setFlag( QgsLayerTreeModel::AllowNodeReorder );
    model->setFlag( QgsLayerTreeModel::AllowNodeRename );
    model->setFlag( QgsLayerTreeModel::AllowNodeChangeVisibility );
    model->setFlag( QgsLayerTreeModel::ShowLegendAsTree );
    model->setFlag( QgsLayerTreeModel::UseEmbeddedWidgets );
    model->setFlag( QgsLayerTreeModel::UseTextFormatting );
    model->setAutoCollapseLegendNodes( 10 );

    //创建View，mLayerTreeView会在库外使用，所以需要添加getter方法，
    mLayerTreeView = new QgsLayerTreeView();
    mLayerTreeView->setModel(model);

    //Map Canvas和Layer Tree View建立联系，这样通过QgsProject::instance()->addMapLayer添加图层后，会自动添加在图层树以及会在canvas渲染图层
    mLayerTreeMapCanvasBridge = new QgsLayerTreeMapCanvasBridge(QgsProject::instance()->layerTreeRoot(),mMapCanvas,this);
    connect( mLayerTreeMapCanvasBridge, &QgsLayerTreeMapCanvasBridge::canvasLayersChanged, mMapCanvas, &QgsMapCanvas::setLayers );

    //Layer Tree View右键菜单实现
    mLayerTreeView->setMenuProvider(new ll_qgis_base_layertreeview_menu(mMainWindow,mLayerTreeView,mMapCanvas));
    connect(QgsProject::instance()->layerTreeRegistryBridge(),SIGNAL(addedLayersToLayerTree(QList<QgsMapLayer*>)),
            this,SLOT(slot_autoSelectAddedLayer(QList<QgsMapLayer*>)));

    // QgsLayerTreeViewDefaultActions包含了默认实现的函数如添加Group
    QAction *actionAddGroup = new QAction( tr( "Add Group" ), this );
    actionAddGroup->setIcon( QIcon(":/images/themes/default/mActionAddGroup.png"));
    actionAddGroup->setToolTip( tr( "Add Group" ) );
    connect( actionAddGroup, &QAction::triggered, mLayerTreeView->defaultActions(), &QgsLayerTreeViewDefaultActions::addGroup );

    QAction *actionShowAllLayers = new QAction( tr( "Show All Layers" ), this );
    actionShowAllLayers->setIcon( QIcon(":/images/themes/default/mActionShowAllLayers.png"));
    actionShowAllLayers->setToolTip( tr( "Show All Layers" ) );
    connect(actionShowAllLayers,&QAction::triggered,this,&ll_qgis_base_lib::showAllLayers);

    QAction *actionHideAllLayers = new QAction( tr( "Hide All Layers" ), this );
    actionHideAllLayers->setIcon( QIcon(":/images/themes/default/mActionHideAllLayers.png"));
    actionHideAllLayers->setToolTip( tr( "Hide All Layers" ) );
    connect(actionHideAllLayers,&QAction::triggered,this,&ll_qgis_base_lib::hideAllLayers);

    QAction *actionShowSelectedLayers = new QAction( tr( "Show Selected Layers" ), this );
    actionShowSelectedLayers->setIcon( QIcon(":/images/themes/default/mActionShowSelectedLayers.png"));
    actionShowSelectedLayers->setToolTip( tr( "Show Selected Layers" ) );
    connect(actionShowSelectedLayers,&QAction::triggered,this,&ll_qgis_base_lib::showSelectedLayers);

    QAction *actionHideSelectedLayers = new QAction( tr( "Hide Selected Layers" ), this );
    actionHideSelectedLayers->setIcon( QIcon(":/images/themes/default/mActionHideSelectedLayers.png"));
    actionHideSelectedLayers->setToolTip( tr( "Hide Selected Layers" ) );
    connect(actionHideSelectedLayers,&QAction::triggered,this,&ll_qgis_base_lib::hideSelectedLayers);

    // expand / collapse tool buttons
    QAction *actionExpandAll = new QAction( tr( "Expand All" ), this );
    actionExpandAll->setIcon( QIcon(":/images/themes/default/mActionExpandTree.png") );
    actionExpandAll->setToolTip( tr( "Expand All" ) );
    connect( actionExpandAll, &QAction::triggered, mLayerTreeView, &QgsLayerTreeView::expandAllNodes );
    QAction *actionCollapseAll = new QAction( tr( "Collapse All" ), this );
    actionCollapseAll->setIcon( QIcon(":/images/themes/default/mActionCollapseTree.png") );
    actionCollapseAll->setToolTip( tr( "Collapse All" ) );
    connect( actionCollapseAll, &QAction::triggered, mLayerTreeView, &QgsLayerTreeView::collapseAllNodes );

    QAction *actionRemoveLayer = new QAction( tr( "Remove Layer" ), this );
    actionRemoveLayer->setIcon( QIcon(":/images/themes/default/mActionRemoveLayer.png") );
    actionRemoveLayer->setToolTip(tr( "Remvoe Layer" ));
    connect(actionRemoveLayer,&QAction::triggered,this,&ll_qgis_base_lib::removeLayer);

    //在DockWidget上部添加一个工具栏
    QToolBar *toolbar = new QToolBar();
//    toolbar->setIconSize( iconSize( true ) );
//    toolbar->addAction( mActionStyleDock );
    toolbar->addAction( actionAddGroup );
    toolbar->addAction( actionShowAllLayers );
    toolbar->addAction( actionHideAllLayers );
    toolbar->addAction( actionShowSelectedLayers );
    toolbar->addAction( actionHideSelectedLayers );
    toolbar->addAction( actionExpandAll );
    toolbar->addAction( actionCollapseAll );
    toolbar->addAction( actionRemoveLayer );

    //将工具栏和Layer Tree View添加到界面
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->setContentsMargins( 0, 0, 0, 0 );
    vboxLayout->setSpacing( 0 );
    vboxLayout->addWidget( toolbar );
    vboxLayout->addWidget( mLayerTreeView );

    QWidget *w = new QWidget;
    w->setLayout( vboxLayout );
    mLayerTreeDock->setWidget( w );

//    mMainWindow->addDockWidget( Qt::LeftDockWidgetArea, mLayerTreeDock );
//    mMainWindow->setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
//    mMainWindow->setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
//    mMainWindow->setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
//    mMainWindow->setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );
//    mLayerTreeDock->show();
//    refreshMapCanvas();
}

void ll_qgis_base_lib::initMaptools()
{
    mMapToolPan = new QgsMapToolPan(mMapCanvas);
    mMapCanvas->setMapTool(mMapToolPan);
}

QgsMapLayer *ll_qgis_base_lib::addVectorLayer(const QString &uri, const QString &baseName, const QString &provider)
{
    QgsMapLayer *layer = ll_qgis_base_lib_layerhandling::addVectorLayer( uri, baseName, provider );
    if ( layer )
    {
        mLayersList << layer;
        return layer;
    }
    return nullptr;
}

QgsMapLayer *ll_qgis_base_lib::addRasterLayer(const QString &uri, const QString &baseName, const QString &provider)
{
    QgsMapLayer *layer = ll_qgis_base_lib_layerhandling::addRasterLayer( uri, baseName, provider );
    if ( layer )
    {
        mLayersList << layer;
        return layer;
    }
    return nullptr;
}

QgsMapLayer *ll_qgis_base_lib::addWmsLayer(const QString &uri, const QString &baseName)
{
    QString urlWithParams;
    QString type = QStringLiteral("xyz");
    int zMin = 0;
    int zMax = 18;
#if 1
    QgsDataSourceUri urik;
    urik.setParam( QStringLiteral( "url" ), uri );
    urik.setParam( QStringLiteral( "type" ), type );
    urik.setParam( QStringLiteral( "zmin" ), QString::number( zMin ) );
    urik.setParam( QStringLiteral( "zmax" ), QString::number( zMax ) );
    urlWithParams = urik.encodedUri();
#else
    QString urlEncode = QUrl::toPercentEncoding(url);
    urlWithParams = QString("type=xyz&url=%1&zmax=18&zmin=0").arg(urlEncode);
#endif
    QgsRasterLayer *rlayer = new QgsRasterLayer(urlWithParams,baseName,"wms");
    QgsProject::instance()->addMapLayer(rlayer);
    return rlayer;
}

QList<QgsMapCanvas *> ll_qgis_base_lib::mapCanvases()
{
    // filter out browser canvases -- they are children of app, but a different
    // kind of beast, and here we only want the main canvas or dock canvases
    auto canvases = findChildren< QgsMapCanvas * >();
    canvases.erase( std::remove_if( canvases.begin(), canvases.end(),
                                    []( QgsMapCanvas * canvas )
    {
      return !canvas || canvas->property( "browser_canvas" ).toBool();
    } ), canvases.end() );
    return canvases;
}

void ll_qgis_base_lib::refreshMapCanvas(bool redrawAllLayers)
{
    const auto canvases = mapCanvases();
    for ( QgsMapCanvas *canvas : canvases )
    {
        //stop any current rendering
        canvas->stopRendering();
        if ( redrawAllLayers )
            canvas->refreshAllLayers();
        else
            canvas->refresh();
    }
}

void ll_qgis_base_lib::legendLayerZoomNative()
{
    if ( !mLayerTreeView )
      return;

    //find current Layer
    QgsMapLayer *currentLayer = mLayerTreeView->currentLayer();
    if ( !currentLayer )
      return;

    if ( QgsRasterLayer *layer = qobject_cast<QgsRasterLayer *>( currentLayer ) )
    {
      QgsDebugMsgLevel( "Raster units per pixel  : " + QString::number( layer->rasterUnitsPerPixelX() ), 2 );
      QgsDebugMsgLevel( "MapUnitsPerPixel before : " + QString::number( mMapCanvas->mapUnitsPerPixel() ), 2 );

      QList< double >nativeResolutions;
      if ( layer->dataProvider() )
      {
        nativeResolutions = layer->dataProvider()->nativeResolutions();
      }

      // get length of central canvas pixel width in source raster crs
      QgsRectangle e = mMapCanvas->extent();
      QSize s = mMapCanvas->mapSettings().outputSize();
      QgsPointXY p1( e.center().x(), e.center().y() );
      QgsPointXY p2( e.center().x() + e.width() / s.width(), e.center().y() + e.height() / s.height() );
      QgsCoordinateTransform ct( mMapCanvas->mapSettings().destinationCrs(), layer->crs(), QgsProject::instance() );
      p1 = ct.transform( p1 );
      p2 = ct.transform( p2 );
      const double diagonalSize = std::sqrt( p1.sqrDist( p2 ) ); // width (actually the diagonal) of reprojected pixel
      if ( !nativeResolutions.empty() )
      {
        // find closest native resolution
        QList< double > diagonalNativeResolutions;
        diagonalNativeResolutions.reserve( nativeResolutions.size() );
        for ( double d : std::as_const( nativeResolutions ) )
          diagonalNativeResolutions << std::sqrt( 2 * d * d );

        int i;
        for ( i = 0; i < diagonalNativeResolutions.size() && diagonalNativeResolutions.at( i ) < diagonalSize; i++ )
        {
          QgsDebugMsgLevel( QStringLiteral( "test resolution %1: %2" ).arg( i ).arg( diagonalNativeResolutions.at( i ) ), 2 );
        }
        if ( i == nativeResolutions.size() ||
             ( i > 0 && ( ( diagonalNativeResolutions.at( i ) - diagonalSize ) > ( diagonalSize - diagonalNativeResolutions.at( i - 1 ) ) ) ) )
        {
          QgsDebugMsgLevel( QStringLiteral( "previous resolution" ), 2 );
          i--;
        }

        mMapCanvas->zoomByFactor( nativeResolutions.at( i ) / mMapCanvas->mapUnitsPerPixel() );
      }
      else
      {
        mMapCanvas->zoomByFactor( std::sqrt( layer->rasterUnitsPerPixelX() * layer->rasterUnitsPerPixelX() + layer->rasterUnitsPerPixelY() * layer->rasterUnitsPerPixelY() ) / diagonalSize );
      }

      mMapCanvas->refresh();
      QgsDebugMsgLevel( "MapUnitsPerPixel after  : " + QString::number( mMapCanvas->mapUnitsPerPixel() ), 2 );
    }
}

void ll_qgis_base_lib::legendLayerStretchUsingCurrentExtent()
{
    if ( !mLayerTreeView )
      return;

    //find current Layer
    QgsMapLayer *currentLayer = mLayerTreeView->currentLayer();
    if ( !currentLayer )
      return;

    QgsRasterLayer *layer = qobject_cast<QgsRasterLayer *>( currentLayer );
    if ( layer )
    {
      QgsRectangle myRectangle;
      myRectangle = mMapCanvas->mapSettings().outputExtentToLayerExtent( layer, mMapCanvas->extent() );
      layer->refreshContrastEnhancement( myRectangle );

      mLayerTreeView->refreshLayerSymbology( layer->id() );
      refreshMapCanvas();
    }
}

bool ll_qgis_base_lib::saveProjects(const QString &filename)
{
    return QgsProject::instance()->write(filename);
}

bool ll_qgis_base_lib::readProjects(const QString &filename, Qgis::ProjectReadFlags flags)
{
    return QgsProject::instance()->read(filename,flags);
}

void ll_qgis_base_lib::setCrs(const QgsCoordinateReferenceSystem &crs, bool adjustEllipsoid)
{
    QgsProject::instance()->setCrs(crs,adjustEllipsoid);
    //layer
    //    layer()->setCrs( crs );
}


void ll_qgis_base_lib::slot_autoSelectAddedLayer(const QList<QgsMapLayer *> layers)
{
    if ( !layers.isEmpty() )
    {
        QgsLayerTreeLayer *nodeLayer = QgsProject::instance()->layerTreeRoot()->findLayer( layers[0]->id() );

        if ( !nodeLayer )
            return;

        QModelIndex index = mLayerTreeView->layerTreeModel()->node2index( nodeLayer );
        mLayerTreeView->setCurrentIndex( index );
    }
}

void ll_qgis_base_lib::showAllLayers()
{
    QgsDebugMsgLevel( QStringLiteral( "Showing all layers!" ), 3 );
    mLayerTreeView->layerTreeModel()->rootGroup()->setItemVisibilityCheckedRecursive( true );
}

void ll_qgis_base_lib::hideAllLayers()
{
    QgsDebugMsgLevel( QStringLiteral( "hiding all layers!" ), 3 );

    const auto constChildren = mLayerTreeView->layerTreeModel()->rootGroup()->children();
    for ( QgsLayerTreeNode *node : constChildren )
    {
      node->setItemVisibilityCheckedRecursive( false );
    }
}

void ll_qgis_base_lib::showSelectedLayers()
{
    QgsDebugMsgLevel( QStringLiteral( "show selected layers!" ), 3 );

    const auto constSelectedNodes = mLayerTreeView->selectedNodes();
    for ( QgsLayerTreeNode *node : constSelectedNodes )
    {
        QgsLayerTreeNode *nodeIter = node;
        while ( nodeIter )
        {
            nodeIter->setItemVisibilityChecked( true );
            nodeIter = nodeIter->parent();
        }
    }
}

void ll_qgis_base_lib::hideSelectedLayers()
{
    QgsDebugMsgLevel( QStringLiteral( "hiding selected layers!" ), 3 );

    const auto constSelectedNodes = mLayerTreeView->selectedNodes();
    for ( QgsLayerTreeNode *node : constSelectedNodes )
    {
      node->setItemVisibilityChecked( false );
    }
}

void ll_qgis_base_lib::removeLayer()
{
    if(!mLayerTreeView)
        return;
//    const QList<QgsMapLayer *> selectedLayers = mLayerTreeView->selectedLayersRecursive();

    if ( QMessageBox::warning( mMainWindow, tr( "Remove layers and groups" ), tr("Are you sure?"), QMessageBox::Ok | QMessageBox::Cancel ) == QMessageBox::Cancel )
    {
      return;
    }

    const QList<QgsLayerTreeNode *> selectedNodes = mLayerTreeView->selectedNodes( true );
    for ( QgsLayerTreeNode *node : selectedNodes )
    {
      if ( QgsLayerTreeGroup *group = qobject_cast< QgsLayerTreeGroup * >( node ) )
      {
        if ( QgsGroupLayer *groupLayer = group->groupLayer() )
        {
          QgsProject::instance()->removeMapLayer( groupLayer );
        }
      }
      QgsLayerTreeGroup *parentGroup = qobject_cast<QgsLayerTreeGroup *>( node->parent() );
      if ( parentGroup )
        parentGroup->removeChildNode( node );
    }
    refreshMapCanvas();
}

void ll_qgis_base_lib::userRotation()
{
    double degrees = 0.0;//mRotationEdit->value();
    mMapCanvas->setRotation( degrees );
    mMapCanvas->refresh();
}

QgsDockWidget *ll_qgis_base_lib::layerTreeDock() const
{
    return mLayerTreeDock;
}

QgsMapCanvas *ll_qgis_base_lib::mapCanvas() const
{
    return mMapCanvas;
}

QgsLayerTreeView *ll_qgis_base_lib::layerTreeView() const
{
    return mLayerTreeView;
}


