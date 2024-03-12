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
    enum class SublayerHandling
    {
      AskUser,
      LoadAll,
      AbortLoading
    };

    static QgsVectorLayer *addVectorLayer( const QString &uri, const QString &baseName, const QString &provider = QLatin1String( "ogr" ) );
    static QList< QgsMapLayer * > addOgrVectorLayers( const QStringList &uris, const QString &encoding, const QString &dataSourceType, bool &ok, bool showWarningOnInvalid = true );
    static QgsRasterLayer *addRasterLayer( QString const &uri, const QString &baseName, const QString &provider = QLatin1String( "gdal" ) );
    static QList< QgsMapLayer * > addGdalRasterLayers( const QStringList &uris, bool &ok, bool showWarningOnInvalid = true );
private:
    template<typename T> static T *addLayerPrivate( QgsMapLayerType type, const QString &uri, const QString &baseName, const QString &providerKey, bool guiWarnings = false );
    static QList< QgsMapLayer * > addSublayers( const QList< QgsProviderSublayerDetails> &layers, const QString &baseName, const QString &groupName );
    static bool askUserForZipItemLayers( const QString &path, const QList< QgsMapLayerType > &acceptableTypes );
    static SublayerHandling shouldAskUserForSublayers( const QList< QgsProviderSublayerDetails > &layers, bool hasNonLayerItems = false );
};

#endif // LD_LIB_APP_LAYERHANDLING_H
