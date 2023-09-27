#ifndef LD_LIB_APP_LAYERHANDLING_H
#define LD_LIB_APP_LAYERHANDLING_H

#include "qgis.h"
#include "qgsmaplayer.h"

#include <QObject>

class QgsMapLayer;
class QgsProviderSublayerDetails;
class QgsPointCloudLayer;
class QgsVectorLayer;
class QgsRasterLayer;
class QgsMeshLayer;
class QgsPluginLayer;
class QgsVectorTileLayer;

class ll_qgis_base_lib_layerhandling
{
    Q_GADGET
public:
    static QgsVectorLayer *addVectorLayer( const QString &uri, const QString &baseName, const QString &provider = QLatin1String( "ogr" ) );
    static QgsRasterLayer *addRasterLayer( QString const &uri, const QString &baseName, const QString &provider = QLatin1String( "gdal" ) );
private:
    template<typename T> static T *addLayerPrivate( QgsMapLayerType type, const QString &uri, const QString &baseName, const QString &providerKey, bool guiWarnings = false );
    static QList< QgsMapLayer * > addSublayers( const QList< QgsProviderSublayerDetails> &layers, const QString &baseName, const QString &groupName );
};

#endif // LD_LIB_APP_LAYERHANDLING_H
