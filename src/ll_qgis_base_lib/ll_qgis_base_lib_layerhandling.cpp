#include "ll_qgis_base_lib_layerhandling.h"

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

#include "ll_qgis_base_lib.h"

QgsVectorLayer *ll_qgis_base_lib_layerhandling::addVectorLayer(const QString &uri, const QString &baseName, const QString &provider)
{
    return addLayerPrivate< QgsVectorLayer >( QgsMapLayerType::VectorLayer, uri, baseName, !provider.isEmpty() ? provider : QLatin1String( "ogr" ), true );
}

QgsRasterLayer *ll_qgis_base_lib_layerhandling::addRasterLayer(const QString &uri, const QString &baseName, const QString &provider)
{
    return addLayerPrivate< QgsRasterLayer >( QgsMapLayerType::RasterLayer, uri, baseName, !provider.isEmpty() ? provider : QLatin1String( "gdal" ), true );
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
              layer->setName( QStringLiteral( "%1 — %2" ).arg( baseName, layerName ) );
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
