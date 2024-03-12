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

#include "ll_qgis_base_lib.h"

QgsVectorLayer *ll_qgis_base_lib_layerhandling::addVectorLayer(const QString &uri, const QString &baseName, const QString &provider)
{
    return addLayerPrivate< QgsVectorLayer >( QgsMapLayerType::VectorLayer, uri, baseName, !provider.isEmpty() ? provider : QLatin1String( "ogr" ), true );
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
          if ( askUserForZipItemLayers( uri, { QgsMapLayerType::VectorLayer } ) )
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
        return sublayer.type() != QgsMapLayerType::VectorLayer;
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
                  return sublayer.type() != QgsMapLayerType::VectorLayer;
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
            return sublayer.type() != QgsMapLayerType::VectorLayer;
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

QgsRasterLayer *ll_qgis_base_lib_layerhandling::addRasterLayer(const QString &uri, const QString &baseName, const QString &provider)
{
    return addLayerPrivate< QgsRasterLayer >( QgsMapLayerType::RasterLayer, uri, baseName, !provider.isEmpty() ? provider : QLatin1String( "gdal" ), true );
}

QList<QgsMapLayer *> ll_qgis_base_lib_layerhandling::addGdalRasterLayers(const QStringList &uris, bool &ok, bool showWarningOnInvalid)
{
    ok = false;
    if ( uris.empty() )
    {
      return {};
    }

//    QgsCanvasRefreshBlocker refreshBlocker;

    // this is messy since some files in the list may be rasters and others may
    // be ogr layers. We'll set returnValue to false if one or more layers fail
    // to load.

    QList< QgsMapLayer * > res;

    for ( const QString &uri : uris )
    {
      QString errMsg;

      // if needed prompt for zipitem layers
      QString vsiPrefix = QgsZipItem::vsiPrefix( uri );
      if ( ( !uri.startsWith( QLatin1String( "/vsi" ), Qt::CaseInsensitive ) || uri.endsWith( QLatin1String( ".zip" ) ) || uri.endsWith( QLatin1String( ".tar" ) ) ) &&
           ( vsiPrefix == QLatin1String( "/vsizip/" ) || vsiPrefix == QLatin1String( "/vsitar/" ) ) )
      {
        if ( askUserForZipItemLayers( uri, { QgsMapLayerType::RasterLayer } ) )
          continue;
      }

      const bool isVsiCurl { uri.startsWith( QLatin1String( "/vsicurl" ), Qt::CaseInsensitive ) };
      const bool isRemoteUrl { uri.startsWith( QLatin1String( "http" ) ) || uri == QLatin1String( "ftp" ) };

      std::unique_ptr< QgsTemporaryCursorOverride > cursorOverride;
      if ( isVsiCurl || isRemoteUrl )
      {
        cursorOverride = std::make_unique< QgsTemporaryCursorOverride >( Qt::WaitCursor );
//        QgisApp::instance()->visibleMessageBar()->pushInfo( QObject::tr( "Remote layer" ), QObject::tr( "loading %1, please wait …" ).arg( uri ) );
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
        QgsRasterLayer *layer = addLayerPrivate< QgsRasterLayer >( QgsMapLayerType::RasterLayer, uri, layerName, QStringLiteral( "gdal" ), showWarningOnInvalid );
        res << layer;

        if ( layer && layer->isValid() )
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

//          QgisApp::instance()->visibleMessageBar()->pushMessage( QObject::tr( "Unsupported Data Source" ), msg, Qgis::MessageLevel::Critical );
        }
      }
    }
    return res;

}

QList<QgsMapLayer *> ll_qgis_base_lib_layerhandling::addSublayers(const QList<QgsProviderSublayerDetails> &layers, const QString &baseName, const QString &groupName)
{
    QgsLayerTreeGroup *group = nullptr;
    //deal groud conditiation
    if ( !groupName.isEmpty() )
    {
        int index { 0 };
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
    // if we aren't adding to a group, we need to add the layers in reverse order so that they maintain the correct
    // order in the layer tree!
    QList<QgsProviderSublayerDetails> sortedLayers = layers;
    if ( groupName.isEmpty() )
    {
      std::reverse( sortedLayers.begin(), sortedLayers.end() );
    }
    QList< QgsMapLayer * > result;
    result.reserve( sortedLayers.size() );
    for ( const QgsProviderSublayerDetails &sublayer : std::as_const( sortedLayers ) )
    {
        QgsProviderSublayerDetails::LayerOptions options( QgsProject::instance()->transformContext() );
        options.loadDefaultStyle = false;
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
        const bool projectWasEmpty = QgsProject::instance()->mapLayers().empty();
        // if user has opted to add sublayers to a group, then we don't need to include the
        // filename in the layer's name, because the group is already titled with the filename.
        // But otherwise, we DO include the file name so that users can differentiate the source
        // when multiple layers are loaded from a GPX file or similar (refs https://github.com/qgis/QGIS/issues/37551)
        if ( group )
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
            if ( layerName != baseName && !layerName.isEmpty() && !baseName.isEmpty() )
              layer->setName( QString::fromLocal8Bit( "%1 — %2" ).arg( baseName, layerName ) );
            else if ( !layerName.isEmpty() )
              layer->setName( layerName );
            else if ( !baseName.isEmpty() )
              layer->setName( baseName );
            QgsProject::instance()->addMapLayer( layer.release() );
        }
        // Some of the logic relating to matching a new project's CRS to the first layer added CRS is deferred to happen when the event loop
        // next runs -- so in those cases we can't assume that the project's CRS has been matched to the actual desired CRS yet.
        // In these cases we don't need to show the coordinate operation selection choice, so just hardcode an exception in here to avoid that...
        QgsCoordinateReferenceSystem projectCrsAfterLayerAdd = QgsProject::instance()->crs();
        if ( projectWasEmpty )
            projectCrsAfterLayerAdd = ml->crs();
    }

    return result;

}

template<typename T>
T *ll_qgis_base_lib_layerhandling::addLayerPrivate(QgsMapLayerType type, const QString &uri, const QString &baseName, const QString &providerKey, bool guiWarnings)
{
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

    const bool canQuerySublayers = QgsProviderRegistry::instance()->providerMetadata( providerKey ) &&
                                     ( QgsProviderRegistry::instance()->providerMetadata( providerKey )->capabilities() & QgsProviderMetadata::QuerySublayers );

    T *result = nullptr;
    if ( canQuerySublayers )
    {
        // query sublayers
        QList< QgsProviderSublayerDetails > sublayers = QgsProviderRegistry::instance()->providerMetadata( providerKey ) ?
            QgsProviderRegistry::instance()->providerMetadata( providerKey )->querySublayers( updatedUri, Qgis::SublayerQueryFlag::IncludeSystemTables )
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
//            QgisApp::instance()->visibleMessageBar()->pushMessage( QObject::tr( "Invalid Data Source" ), msg, Qgis::MessageLevel::Critical );
          }

          // since the layer is bad, stomp on it
          return nullptr;
        }
        else if ( sublayers.size() > 1 || QgsProviderUtils::sublayerDetailsAreIncomplete( sublayers, QgsProviderUtils::SublayerCompletenessFlag::IgnoreUnknownFeatureCount ) )
        {
            //lld_temp just load all layers
            //qgsapplayerhandling line 1314 will choose askUser or loadall based on settings
            result = qobject_cast< T * >( addSublayers( sublayers, baseName, QString() ).value( 0 ) );
        }
        else
        {
            result = qobject_cast< T * >( addSublayers( sublayers, baseName, QString() ).value( 0 ) );
            result->setName(  baseName );
        }
    }
    else
    {
        QgsMapLayerFactory::LayerOptions options( QgsProject::instance()->transformContext() );
        options.loadDefaultStyle = false;
        result = qobject_cast< T * >( QgsMapLayerFactory::createLayer( uri, baseName, type, options, providerKey ) );
        if ( result )
        {

          result->setName( baseName );
          QgsProject::instance()->addMapLayer( result );

//          QgisApp::instance()->askUserForDatumTransform( result->crs(), QgsProject::instance()->crs(), result );
//          QgsAppLayerHandling::postProcessAddedLayer( result );
        }
    }
    return result;

}
bool ll_qgis_base_lib_layerhandling::askUserForZipItemLayers( const QString &path, const QList<QgsMapLayerType> &acceptableTypes )
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
    /*
      case SublayerHandling::AskUser:
      {
        // prompt user for sublayers
        QgsProviderSublayersDialog dlg( path, path, sublayers, acceptableTypes, QgisApp::instance() );

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
//    QgsCanvasRefreshBlocker refreshBlocker;
    QgsSettings settings;

    QString base = QgsProviderUtils::suggestLayerNameFromFilePath( path );
    if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
    {
      base = QgsMapLayer::formatLayerName( base );
    }

    addSublayers( sublayers, base, groupName );
//    QgisApp::instance()->activateDeactivateLayerRelatedActions( QgisApp::instance()->activeLayer() );
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
        if ( sublayer.type() != QgsMapLayerType::RasterLayer )
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
