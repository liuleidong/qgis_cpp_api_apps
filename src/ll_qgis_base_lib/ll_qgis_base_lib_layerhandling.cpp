#include "ll_qgis_base_lib_layerhandling.h"

#include <QMessageBox>

#include "qgsproviderregistry.h"
#include "qgsprovidermetadata.h"
#include "qgsprovidersublayerdetails.h"
#include "qgsproviderutils.h"
#include "qgsvectorlayer.h"
#include "qgsrasterlayer.h"
#include "qgsmaplayerfactory.h"
#include "qgsproject.h"
#include "qgslayertree.h"
#include "qgslayertreeview.h"
#include "qgslayertreenode.h"
#include "qgszipitem.h"
#include "qgsguiutils.h"
#include "qgsgdalutils.h"
#include "qgslayertreeregistrybridge.h"
#include "qgsogrproviderutils.h"
#include "qgsgui.h"

#include "ll_qgis_base_lib.h"

QList<QgsVectorLayer *>ll_qgis_base_lib_layerhandling::addVectorLayer( const QString &uri, const QString &baseName, const QString &provider, bool addToLegend )
{
  return addLayerPrivate< QgsVectorLayer >( Qgis::LayerType::Vector, uri, baseName, !provider.isEmpty() ? provider : QLatin1String( "ogr" ), true, addToLegend );
}

QList<QgsMapLayer *> ll_qgis_base_lib_layerhandling::addOgrVectorLayers( const QStringList &layers, const QString &encoding, const QString &dataSourceType, bool &ok, bool showWarningOnInvalid )
{
    //note: this method ONLY supports vector layers from the OGR provider!
    ok = false;

//    QgsCanvasRefreshBlocker refreshBlocker;

    QList<QgsMapLayer *> layersToAdd;
    QList<QgsMapLayer *> addedLayers;
    QgsSettings settings;
    bool userAskedToAddLayers = false;

    for ( const QString &layerUri : layers )
    {
      const QString uri = layerUri.trimmed();
      QString baseName;
      if ( dataSourceType == QLatin1String( "file" ) )
      {
        QString srcWithoutLayername( uri );
        int posPipe = srcWithoutLayername.indexOf( '|' );
        if ( posPipe >= 0 )
          srcWithoutLayername.resize( posPipe );
        baseName = QgsProviderUtils::suggestLayerNameFromFilePath( srcWithoutLayername );

        // if needed prompt for zipitem layers
        QString vsiPrefix = QgsZipItem::vsiPrefix( uri );
        if ( ! uri.startsWith( QLatin1String( "/vsi" ), Qt::CaseInsensitive ) &&
             ( vsiPrefix == QLatin1String( "/vsizip/" ) || vsiPrefix == QLatin1String( "/vsitar/" ) ) )
        {
          if ( askUserForZipItemLayers( uri, { Qgis::LayerType::Vector } ) )
            continue;
        }
      }
      else if ( dataSourceType == QLatin1String( "database" ) )
      {
        // Try to extract the database name and use it as base name
        // sublayers names (if any) will be appended to the layer name
        const QVariantMap parts( QgsProviderRegistry::instance()->decodeUri( QStringLiteral( "ogr" ), uri ) );
        if ( parts.value( QStringLiteral( "databaseName" ) ).isValid() )
          baseName = parts.value( QStringLiteral( "databaseName" ) ).toString();
        else
          baseName = uri;
      }
      else //directory //protocol
      {
        baseName = QgsProviderUtils::suggestLayerNameFromFilePath( uri );
      }

      if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
      {
        baseName = QgsMapLayer::formatLayerName( baseName );
      }

      QgsDebugMsgLevel( "completeBaseName: " + baseName, 2 );
      const bool isVsiCurl { uri.startsWith( QLatin1String( "/vsicurl" ), Qt::CaseInsensitive ) };
      const auto scheme { QUrl( uri ).scheme() };
      const bool isRemoteUrl { scheme.startsWith( QLatin1String( "http" ) ) || scheme == QLatin1String( "ftp" ) };

      std::unique_ptr< QgsTemporaryCursorOverride > cursorOverride;
      if ( isVsiCurl || isRemoteUrl )
      {
        cursorOverride = std::make_unique< QgsTemporaryCursorOverride >( Qt::WaitCursor );
        qApp->processEvents();
      }

      QList< QgsProviderSublayerDetails > sublayers = QgsProviderRegistry::instance()->providerMetadata( QStringLiteral( "ogr" ) )->querySublayers( uri, Qgis::SublayerQueryFlag::IncludeSystemTables );
      // filter out non-vector sublayers
      sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), []( const QgsProviderSublayerDetails & sublayer )
      {
        return sublayer.type() != Qgis::LayerType::Vector;
      } ), sublayers.end() );

      cursorOverride.reset();

      const QVariantMap uriParts = QgsProviderRegistry::instance()->decodeUri( QStringLiteral( "ogr" ), uri );
      const QString path = uriParts.value( QStringLiteral( "path" ) ).toString();

      if ( !sublayers.empty() )
      {
        userAskedToAddLayers = true;

        const bool detailsAreIncomplete = QgsProviderUtils::sublayerDetailsAreIncomplete( sublayers, QgsProviderUtils::SublayerCompletenessFlag::IgnoreUnknownFeatureCount );
        const bool singleSublayerOnly = sublayers.size() == 1;
        QString groupName;

        if ( !singleSublayerOnly || detailsAreIncomplete )
        {
          // ask user for sublayers (unless user settings dictate otherwise!)
          switch ( shouldAskUserForSublayers( sublayers ) )
          {
          /*
            case SublayerHandling::AskUser:
            {
              // prompt user for sublayers
              QgsProviderSublayersDialog dlg( uri, path, sublayers, {QgsMapLayerType::VectorLayer}, QgisApp::instance() );

              if ( dlg.exec() )
                sublayers = dlg.selectedLayers();
              else
                sublayers.clear(); // dialog was canceled, so don't add any sublayers
              groupName = dlg.groupName();
              break;
            }
           */
            case SublayerHandling::AskUser:
            case SublayerHandling::LoadAll:
            {
              if ( detailsAreIncomplete )
              {
                // requery sublayers, resolving geometry types
                sublayers = QgsProviderRegistry::instance()->querySublayers( uri, Qgis::SublayerQueryFlag::ResolveGeometryType );
                // filter out non-vector sublayers
                sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), []( const QgsProviderSublayerDetails & sublayer )
                {
                  return sublayer.type() != Qgis::LayerType::Vector;
                } ), sublayers.end() );
              }
              break;
            }

            case SublayerHandling::AbortLoading:
              sublayers.clear(); // don't add any sublayers
              break;
          };
        }
        else if ( detailsAreIncomplete )
        {
          // requery sublayers, resolving geometry types
          sublayers = QgsProviderRegistry::instance()->querySublayers( uri, Qgis::SublayerQueryFlag::ResolveGeometryType );
          // filter out non-vector sublayers
          sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), []( const QgsProviderSublayerDetails & sublayer )
          {
            return sublayer.type() != Qgis::LayerType::Vector;
          } ), sublayers.end() );
        }

        // now add sublayers
        if ( !sublayers.empty() )
        {
          addedLayers << addSublayers( sublayers, baseName, groupName );
        }

      }
      else
      {
        QString msg = QObject::tr( "%1 is not a valid or recognized data source." ).arg( uri );
        // If the failed layer was a vsicurl type, give the user a chance to try the normal download.
        if ( isVsiCurl &&
             QMessageBox::question( nullptr, QObject::tr( "Invalid Data Source" ),
                                    QObject::tr( "Download with \"Protocol\" source type has failed, do you want to try the \"File\" source type?" ) ) == QMessageBox::Yes )
        {
          QString fileUri = uri;
          fileUri.replace( QLatin1String( "/vsicurl/" ), " " );
          return addOgrVectorLayers( QStringList() << fileUri, encoding, dataSourceType, showWarningOnInvalid );
        }
//        else if ( showWarningOnInvalid )
//        {
//          QgisApp::instance()->visibleMessageBar()->pushMessage( QObject::tr( "Invalid Data Source" ), msg, Qgis::MessageLevel::Critical );
//        }
      }
    }

    // make sure at least one layer was successfully added
    if ( layersToAdd.isEmpty() )
    {
      // we also return true if we asked the user for sublayers, but they choose none. In this case nothing
      // went wrong, so we shouldn't return false and cause GUI warnings to appear
      ok = userAskedToAddLayers || !addedLayers.isEmpty();
    }

    // Register this layer with the layers registry
    QgsProject::instance()->addMapLayers( layersToAdd );
//    for ( QgsMapLayer *l : std::as_const( layersToAdd ) )
//    {
//      QgisApp::instance()->askUserForDatumTransform( l->crs(), QgsProject::instance()->crs(), l );
//      QgsAppLayerHandling::postProcessAddedLayer( l );
//    }
//    QgisApp::instance()->activateDeactivateLayerRelatedActions( QgisApp::instance()->activeLayer() );

    ok = true;
    addedLayers.append( layersToAdd );

    for ( QgsMapLayer *l : std::as_const( addedLayers ) )
    {
      if ( !encoding.isEmpty() )
      {
        if ( QgsVectorLayer *vl = qobject_cast< QgsVectorLayer * >( l ) )
          vl->setProviderEncoding( encoding );
      }
    }

    return addedLayers;

}

QList<QgsRasterLayer *>ll_qgis_base_lib_layerhandling::addRasterLayer( const QString &uri, const QString &baseName, const QString &provider, bool addToLegend )
{
  return addLayerPrivate< QgsRasterLayer >( Qgis::LayerType::Raster, uri, baseName, !provider.isEmpty() ? provider : QLatin1String( "gdal" ), true, addToLegend );
}

QList<QgsMapLayer *> ll_qgis_base_lib_layerhandling::addGdalRasterLayers(const QStringList &uris, bool &ok, bool showWarningOnInvalid)
{
  ok = false;
  if ( uris.empty() )
  {
    return {};
  }

  // QgsCanvasRefreshBlocker refreshBlocker;

  // this is messy since some files in the list may be rasters and others may
  // be ogr layers. We'll set returnValue to false if one or more layers fail
  // to load.

  QList< QgsMapLayer * > res;

  for ( const QString &uri : uris )
  {
    QString errMsg;

    // if needed prompt for zipitem layers
    const QString vsiPrefix = QgsGdalUtils::vsiPrefixForPath( uri );
    if ( ( !uri.startsWith( QLatin1String( "/vsi" ), Qt::CaseInsensitive )
           || uri.endsWith( QLatin1String( ".zip" ) )
           || uri.endsWith( QLatin1String( ".tar" ) ) ) &&
         QgsGdalUtils::isVsiArchivePrefix( vsiPrefix ) )
    {
      if ( askUserForZipItemLayers( uri, { Qgis::LayerType::Raster } ) )
        continue;
    }

    const bool isVsiCurl { uri.startsWith( QLatin1String( "/vsicurl" ), Qt::CaseInsensitive ) };
    const bool isRemoteUrl { uri.startsWith( QLatin1String( "http" ) ) || uri == QLatin1String( "ftp" ) };

    std::unique_ptr< QgsTemporaryCursorOverride > cursorOverride;
    if ( isVsiCurl || isRemoteUrl )
    {
      cursorOverride = std::make_unique< QgsTemporaryCursorOverride >( Qt::WaitCursor );
      // QgisApp::instance()->visibleMessageBar()->pushInfo( QObject::tr( "Remote layer" ), QObject::tr( "loading %1, please wait …" ).arg( uri ) );
      qApp->processEvents();
    }

    if ( QgsRasterLayer::isValidRasterFileName( uri, errMsg ) )
    {
      QFileInfo myFileInfo( uri );

      // set the layer name to the file base name unless provided explicitly
      QString layerName;
      const QVariantMap uriDetails = QgsProviderRegistry::instance()->decodeUri( QStringLiteral( "gdal" ), uri );
      if ( !uriDetails[ QStringLiteral( "layerName" ) ].toString().isEmpty() )
      {
        layerName = uriDetails[ QStringLiteral( "layerName" ) ].toString();
      }
      else
      {
        layerName = QgsProviderUtils::suggestLayerNameFromFilePath( uri );
      }

      // try to create the layer
      cursorOverride.reset();
      const QList<QgsRasterLayer *> layersList { addLayerPrivate< QgsRasterLayer >( Qgis::LayerType::Raster, uri, layerName, QStringLiteral( "gdal" ), showWarningOnInvalid ) };

      // loop and cast
      for ( QgsRasterLayer *layer : std::as_const( layersList ) )
      {
        res.append( layer );
      }

      if ( ! layersList.isEmpty() && layersList.first()->isValid() )
      {
        //only allow one copy of a ai grid file to be loaded at a
        //time to prevent the user selecting all adfs in 1 dir which
        //actually represent 1 coverage,

        if ( myFileInfo.fileName().endsWith( QLatin1String( ".adf" ), Qt::CaseInsensitive ) )
        {
          break;
        }
      }
      // if layer is invalid addLayerPrivate() will show the error

    } // valid raster filename
    else
    {
      ok = false;

      // Issue message box warning unless we are loading from cmd line since
      // non-rasters are passed to this function first and then successfully
      // loaded afterwards (see main.cpp)
      if ( showWarningOnInvalid )
      {
        QString msg = QObject::tr( "%1 is not a supported raster data source" ).arg( uri );
        if ( !errMsg.isEmpty() )
          msg += '\n' + errMsg;

        // QgisApp::instance()->visibleMessageBar()->pushMessage( QObject::tr( "Unsupported Data Source" ), msg, Qgis::MessageLevel::Critical );
      }
    }
  }
  return res;
}

QList<QgsMapLayer *> ll_qgis_base_lib_layerhandling::addSublayers(const QList<QgsProviderSublayerDetails> &layers, const QString &baseName, const QString &groupName, bool addToLegend)
{
  QgsLayerTreeGroup *group = nullptr;
  if ( !groupName.isEmpty() )
  {
    int index { 0 };
    if ( QgsProject::instance()->layerTreeRegistryBridge()->layerInsertionMethod() == Qgis::LayerTreeInsertionMethod::TopOfTree )
    {
      group = QgsProject::instance()->layerTreeRoot()->insertGroup( 0, groupName );
    }
    else
    {
      QgsLayerTreeNode *currentNode { ll_qgis_base_lib::Instance()->layerTreeView()->currentNode() };
      if ( currentNode && currentNode->parent() )
      {
        if ( QgsLayerTree::isGroup( currentNode ) )
        {
          group = qobject_cast<QgsLayerTreeGroup *>( currentNode )->insertGroup( 0, groupName );
        }
        else if ( QgsLayerTree::isLayer( currentNode ) )
        {
          const QList<QgsLayerTreeNode *> currentNodeSiblings { currentNode->parent()->children() };
          int nodeIdx { 0 };
          for ( const QgsLayerTreeNode *child : std::as_const( currentNodeSiblings ) )
          {
            nodeIdx++;
            if ( child == currentNode )
            {
              index = nodeIdx;
              break;
            }
          }
          group = qobject_cast<QgsLayerTreeGroup *>( currentNode->parent() )->insertGroup( index, groupName );
        }
        else
        {
          group = QgsProject::instance()->layerTreeRoot()->insertGroup( 0, groupName );
        }
      }
      else
      {
        group = QgsProject::instance()->layerTreeRoot()->insertGroup( 0, groupName );
      }
    }
  }

  QgsSettings settings;
  const bool formatLayerNames = settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool();

  // if we aren't adding to a group, we need to add the layers in reverse order so that they maintain the correct
  // order in the layer tree!
  QList<QgsProviderSublayerDetails> sortedLayers = layers;
  if ( groupName.isEmpty() )
  {
    std::reverse( sortedLayers.begin(), sortedLayers.end() );
  }

  QList< QgsMapLayer * > result;
  result.reserve( sortedLayers.size() );

  QgsOgrProviderUtils::DeferDatasetClosing deferDatasetClosing;

  for ( const QgsProviderSublayerDetails &sublayer : std::as_const( sortedLayers ) )
  {
    QgsProviderSublayerDetails::LayerOptions options( QgsProject::instance()->transformContext() );
    options.loadDefaultStyle = false;
    options.loadAllStoredStyle = true;

    std::unique_ptr<QgsMapLayer> layer( sublayer.toLayer( options ) );
    if ( !layer )
      continue;

    QgsMapLayer *ml = layer.get();
    // if we aren't adding to a group, then we're iterating the layers in the reverse order
    // so account for that in the returned list of layers
    if ( groupName.isEmpty() )
      result.insert( 0, ml );
    else
      result << ml;

    QString layerName = layer->name();
    if ( formatLayerNames )
    {
      layerName = QgsMapLayer::formatLayerName( layerName );
    }

    const bool projectWasEmpty = QgsProject::instance()->mapLayers().empty();

    // if user has opted to add sublayers to a group, then we don't need to include the
    // filename in the layer's name, because the group is already titled with the filename.
    // But otherwise, we DO include the file name so that users can differentiate the source
    // when multiple layers are loaded from a GPX file or similar (refs https://github.com/qgis/QGIS/issues/37551)
    if ( !groupName.isEmpty() )
    {
      if ( !layerName.isEmpty() )
        layer->setName( layerName );
      else if ( !baseName.isEmpty() )
        layer->setName( baseName );

      QgsProject::instance()->addMapLayer( layer.release(), false );
      group->addLayer( ml );
    }
    else
    {
      if ( layerName != baseName && !layerName.isEmpty() && !baseName.isEmpty() &&
           !layerName.startsWith( baseName ) )
      {
        layer->setName( QStringLiteral( "%1 — %2" ).arg( baseName, layerName ) );
      }
      else if ( !layerName.isEmpty() )
        layer->setName( layerName );
      else if ( !baseName.isEmpty() )
        layer->setName( baseName );
      QgsProject::instance()->addMapLayer( layer.release(), addToLegend );
    }

    // Some of the logic relating to matching a new project's CRS to the first layer added CRS is deferred to happen when the event loop
    // next runs -- so in those cases we can't assume that the project's CRS has been matched to the actual desired CRS yet.
    // In these cases we don't need to show the coordinate operation selection choice, so just hardcode an exception in here to avoid that...
    QgsCoordinateReferenceSystem projectCrsAfterLayerAdd = QgsProject::instance()->crs();
    const QgsGui::ProjectCrsBehavior projectCrsBehavior = QgsSettings().enumValue( QStringLiteral( "/projections/newProjectCrsBehavior" ),  QgsGui::UseCrsOfFirstLayerAdded, QgsSettings::App );
    switch ( projectCrsBehavior )
    {
      case QgsGui::UseCrsOfFirstLayerAdded:
      {
        if ( projectWasEmpty )
          projectCrsAfterLayerAdd = ml->crs();
        break;
      }

      case QgsGui::UsePresetCrs:
        break;
    }

    // QgisApp::instance()->askUserForDatumTransform( ml->crs(), projectCrsAfterLayerAdd, ml );
  }

  if ( group )
  {
    // Respect if user don't want the new group of layers visible.
    QgsSettings settings;
    const bool newLayersVisible = settings.value( QStringLiteral( "/qgis/new_layers_visible" ), true ).toBool();
    if ( !newLayersVisible )
      group->setItemVisibilityCheckedRecursive( newLayersVisible );
  }

  // Post process all added layers
  // for ( QgsMapLayer *ml : std::as_const( result ) )
  // {
  //   QgsAppLayerHandling::postProcessAddedLayer( ml );
  //   if ( group && !addToLegend )
  //   {
  //     // Take note of the fact that the group name took over the intent to defer legend addition
  //     ml->setCustomProperty( QStringLiteral( "_legend_added" ), true );
  //   }
  // }

  return result;
}

template<typename T>
QList<T *>ll_qgis_base_lib_layerhandling::addLayerPrivate( Qgis::LayerType type, const QString &uri, const QString &name, const QString &providerKey, bool guiWarnings, bool addToLegend )
{
  QgsSettings settings;

  // QgsCanvasRefreshBlocker refreshBlocker;

  QString baseName = settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() ? QgsMapLayer::formatLayerName( name ) : name;

  // if the layer needs authentication, ensure the master password is set
  const thread_local QRegularExpression rx( "authcfg=([a-z]|[A-Z]|[0-9]){7}" );
  if ( rx.match( uri ).hasMatch() )
  {
    //20250407 暂时不处理这种情况
#if 0

    if ( !QgsAuthGuiUtils::isDisabled( QgisApp::instance()->messageBar() ) )
    {
      QgsApplication::authManager()->setMasterPassword( true );
    }
#endif
  }

  QVariantMap uriElements = QgsProviderRegistry::instance()->decodeUri( providerKey, uri );
  QString path = uri;
  if ( uriElements.contains( QStringLiteral( "path" ) ) )
  {
    // run layer path through QgsPathResolver so that all inbuilt paths and other localised paths are correctly expanded
    path = QgsPathResolver().readPath( uriElements.value( QStringLiteral( "path" ) ).toString() );
    uriElements[ QStringLiteral( "path" ) ] = path;
  }
  // Not all providers implement decodeUri(), so use original uri if uriElements is empty
  const QString updatedUri = uriElements.isEmpty() ? uri : QgsProviderRegistry::instance()->encodeUri( providerKey, uriElements );

  QgsProviderMetadata *providerMetadata = QgsProviderRegistry::instance()->providerMetadata( providerKey );
  const bool canQuerySublayers = providerMetadata &&
                                 ( providerMetadata->capabilities() & QgsProviderMetadata::QuerySublayers );

  QList<T *> result;
  if ( canQuerySublayers )
  {
    // query sublayers
    QList< QgsProviderSublayerDetails > sublayers = providerMetadata ?
        providerMetadata->querySublayers( updatedUri, Qgis::SublayerQueryFlag::IncludeSystemTables )
        : QgsProviderRegistry::instance()->querySublayers( updatedUri );

    // filter out non-matching sublayers
    sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), [type]( const QgsProviderSublayerDetails & sublayer )
    {
      return sublayer.type() != type;
    } ), sublayers.end() );

    if ( sublayers.empty() )
    {
      if ( guiWarnings )
      {
        QString msg = QObject::tr( "%1 is not a valid or recognized data source." ).arg( uri );
        qDebug() << msg;
        //QgisApp::instance()->visibleMessageBar()->pushMessage( QObject::tr( "Invalid Data Source" ), msg, Qgis::MessageLevel::Critical );
      }

      // since the layer is bad, stomp on it
      return QList<T *>();
    }
    else if ( sublayers.size() > 1 || QgsProviderUtils::sublayerDetailsAreIncomplete( sublayers, QgsProviderUtils::SublayerCompletenessFlag::IgnoreUnknownFeatureCount ) )
    {
      // ask user for sublayers (unless user settings dictate otherwise!)
      switch ( shouldAskUserForSublayers( sublayers ) )
      {
        //20250407 不问用户，直接加载
#if 0
        case SublayerHandling::AskUser:
        {
          QgsProviderSublayersDialog dlg( updatedUri, providerKey, path, sublayers, {type}, QgisApp::instance() );
          QString groupName = providerMetadata->suggestGroupNameForUri( uri );
          if ( !groupName.isEmpty() )
            dlg.setGroupName( groupName );
          if ( dlg.exec() )
          {
            const QList< QgsProviderSublayerDetails > selectedLayers = dlg.selectedLayers();
            if ( !selectedLayers.isEmpty() )
            {
              const QList<QgsMapLayer *> layers { addSublayers( selectedLayers, baseName, dlg.groupName(), addToLegend ) };
              for ( QgsMapLayer *layer : std::as_const( layers ) )
              {
                result << qobject_cast<T *>( layer );
              }
            }
          }
          break;
        }
#endif
        case SublayerHandling::LoadAll:
        {
          const QList<QgsMapLayer *> layers { addSublayers( sublayers, baseName, QString(), addToLegend ) };
          for ( QgsMapLayer *layer : std::as_const( layers ) )
          {
            result << qobject_cast<T *>( layer );
          }
          break;
        }
        case SublayerHandling::AbortLoading:
          break;
      };
    }
    else
    {
      const QList<QgsMapLayer *> layers { addSublayers( sublayers, name, QString(), addToLegend ) };

      if ( ! layers.isEmpty() )
      {
        QString base( baseName );
        if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
        {
          base = QgsMapLayer::formatLayerName( base );
        }
        for ( QgsMapLayer *layer : std::as_const( layers ) )
        {
          layer->setName( base );
          result << qobject_cast<T *>( layer );
        }
      }
    }
  }
  else
  {
    // Handle single layers (no sublayers available for this provider): result will
    // contain at most one single layer
    QgsMapLayerFactory::LayerOptions options( QgsProject::instance()->transformContext() );
    options.loadDefaultStyle = false;
    result.push_back( qobject_cast< T * >( QgsMapLayerFactory::createLayer( uri, name, type, options, providerKey ) ) );
    if ( ! result.isEmpty() )
    {
      QString base( baseName );
      if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
      {
        base = QgsMapLayer::formatLayerName( base );
      }
      result.first()->setName( base );
      QgsProject::instance()->addMapLayer( result.first(), addToLegend );

      //QgisApp::instance()->askUserForDatumTransform( result.first()->crs(), QgsProject::instance()->crs(), result.first() );
      //QgsAppLayerHandling::postProcessAddedLayer( result.first() );
    }
  }

  //QgisApp::instance()->activateDeactivateLayerRelatedActions( QgisApp::instance()->activeLayer() );
  return result;
}

bool ll_qgis_base_lib_layerhandling::askUserForZipItemLayers( const QString &path, const QList<Qgis::LayerType> &acceptableTypes )
{
  // query sublayers
  QList< QgsProviderSublayerDetails > sublayers = QgsProviderRegistry::instance()->querySublayers( path, Qgis::SublayerQueryFlag::IncludeSystemTables );

  // filter out non-matching sublayers
  sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), [acceptableTypes]( const QgsProviderSublayerDetails & sublayer )
  {
    return !acceptableTypes.empty() && !acceptableTypes.contains( sublayer.type() );
  } ), sublayers.end() );

  if ( sublayers.empty() )
    return false;

  const bool detailsAreIncomplete = QgsProviderUtils::sublayerDetailsAreIncomplete( sublayers, QgsProviderUtils::SublayerCompletenessFlag::IgnoreUnknownFeatureCount );
  const bool singleSublayerOnly = sublayers.size() == 1;
  QString groupName;

  if ( !singleSublayerOnly || detailsAreIncomplete )
  {
    // ask user for sublayers (unless user settings dictate otherwise!)
    switch ( shouldAskUserForSublayers( sublayers ) )
    {
#if 0 //20250407 不问用户
      case SublayerHandling::AskUser:
      {
        // prompt user for sublayers
        QgsProviderSublayersDialog dlg( path, QString(), path, sublayers, acceptableTypes, QgisApp::instance() );

        if ( dlg.exec() )
          sublayers = dlg.selectedLayers();
        else
          sublayers.clear(); // dialog was canceled, so don't add any sublayers
        groupName = dlg.groupName();
        break;
      }
#endif
      case SublayerHandling::LoadAll:
      {
        if ( detailsAreIncomplete )
        {
          // requery sublayers, resolving geometry types
          sublayers = QgsProviderRegistry::instance()->querySublayers( path, Qgis::SublayerQueryFlag::ResolveGeometryType );
          sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), [acceptableTypes]( const QgsProviderSublayerDetails & sublayer )
          {
            return !acceptableTypes.empty() && !acceptableTypes.contains( sublayer.type() );
          } ), sublayers.end() );
        }
        break;
      }

      case SublayerHandling::AbortLoading:
        sublayers.clear(); // don't add any sublayers
        break;
    };
  }
  else if ( detailsAreIncomplete )
  {
    // requery sublayers, resolving geometry types
    sublayers = QgsProviderRegistry::instance()->querySublayers( path, Qgis::SublayerQueryFlag::ResolveGeometryType );
    sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), [acceptableTypes]( const QgsProviderSublayerDetails & sublayer )
    {
      return !acceptableTypes.empty() && !acceptableTypes.contains( sublayer.type() );
    } ), sublayers.end() );
  }

  // now add sublayers
  if ( !sublayers.empty() )
  {
    // QgsCanvasRefreshBlocker refreshBlocker;
    QgsSettings settings;

    QString base = QgsProviderUtils::suggestLayerNameFromFilePath( path );
    if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
    {
      base = QgsMapLayer::formatLayerName( base );
    }

    addSublayers( sublayers, base, groupName );
    //QgisApp::instance()->activateDeactivateLayerRelatedActions( QgisApp::instance()->activeLayer() );
  }

  return true;
}

ll_qgis_base_lib_layerhandling::SublayerHandling ll_qgis_base_lib_layerhandling::shouldAskUserForSublayers( const QList<QgsProviderSublayerDetails> &layers, bool hasNonLayerItems )
{
  if ( hasNonLayerItems )
    return SublayerHandling::AskUser;

  QgsSettings settings;
  const Qgis::SublayerPromptMode promptLayers = settings.enumValue( QStringLiteral( "qgis/promptForSublayers" ), Qgis::SublayerPromptMode::AlwaysAsk );

  switch ( promptLayers )
  {
    case Qgis::SublayerPromptMode::AlwaysAsk:
      return SublayerHandling::AskUser;

    case Qgis::SublayerPromptMode::AskExcludingRasterBands:
    {
      // if any non-raster layers are found, we ask the user. Otherwise we load all
      for ( const QgsProviderSublayerDetails &sublayer : layers )
      {
        if ( sublayer.type() != Qgis::LayerType::Raster )
          return SublayerHandling::AskUser;
      }
      return SublayerHandling::LoadAll;
    }

    case Qgis::SublayerPromptMode::NeverAskSkip:
      return SublayerHandling::AbortLoading;

    case Qgis::SublayerPromptMode::NeverAskLoadAll:
      return SublayerHandling::LoadAll;
  }

  return SublayerHandling::AskUser;
}
