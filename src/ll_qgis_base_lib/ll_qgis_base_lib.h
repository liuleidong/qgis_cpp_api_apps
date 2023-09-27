#ifndef LD_LIB_APP_H
#define LD_LIB_APP_H

#include "ll_qgis_base_lib_global.h"
#include "ll_qgis_base_lib_singleton.h"

#include "qgis.h"
#include "qgspallabeling.h"
#include "qgsvectorlayerlabeling.h"
#include "qgsraster.h"
#include "qgssymbol.h"
#include "qgsvectorlayer.h"
#include "qgssinglesymbolrenderer.h"
#include "qgsmarkersymbol.h"

#include <QObject>

#include "ld_symbol_property.h"
#include "ld_geometry.h"

class QMainWindow;
class QDockWidget;
class QLabel;
class QValidator;

class QgsMapCanvas;
class QgsMapToolPan;
class QgsMapLayer;
class QgsDockWidget;
class QgsLayerTreeView;
class QgsLayerTreeMapCanvasBridge;
class QgsDoubleSpinBox;
class QgsVectorLayer;
class QgsRasterLayer;
class QgsAnnotationLayer;
class QgsSymbolLayer;
class QgsRasterDataProvider;


class LD_LIB_APP_EXPORT ll_qgis_base_lib : public QObject
{
    DECLARE_SINGLETON(ll_qgis_base_lib);
    Q_OBJECT
private:
    ll_qgis_base_lib();
public:
    ///
    /// \brief 返回库版本
    /// \return
    ///
    QString version();

    // getters
    QgsLayerTreeView *layerTreeView() const;
    QgsMapCanvas *mapCanvas() const;
    QgsDockWidget *layerTreeDock() const;

    ///
    /// \brief
    /// 1. 创建QgsMapCanvas
    /// 2. 创建QgsLayerTreeView等相关类，实现图层管理
    /// 3. 创建QgsMapTool
    /// \param mainWindow - 传入MainWindow指针
    ///
    void initialize(QMainWindow *mainWindow);
    ///
    /// \brief 图层管理初始化
    /// QgsLayerTreeView QgsLayerTreeModel
    /// QgsLayerTreeMapCanvasBridge
    /// QgsLayerTreeViewMenuProvider
    void initLayerTreeView();
    ///
    /// \brief 初始化QgsMaptools
    ///
    void initMaptools();

    //***************projects qgs****************
    bool saveProjects(const QString &filename);
    bool readProjects(const QString &filename,Qgis::ProjectReadFlags flags = Qgis::ProjectReadFlags());
    void setCrs(const QgsCoordinateReferenceSystem &crs, bool adjustEllipsoid);

    QgsMapLayer *addVectorLayer(const QString &uri, const QString &baseName, const QString &provider="ogr");
    QgsMapLayer *addRasterLayer(const QString &uri, const QString &baseName, const QString &provider="gdal");
    // copy code from qgisapp
    QList< QgsMapCanvas * > mapCanvases();  //qgisapp
    void refreshMapCanvas(bool redrawAllLayers = false);//qgisapp
    void legendLayerZoomNative();//qgisapp
    void legendLayerStretchUsingCurrentExtent();//qgisapp

public slots:
    void slot_autoSelectAddedLayer(const QList<QgsMapLayer *> layers);

private slots:
    void showAllLayers();
    void hideAllLayers();
    void showSelectedLayers();
    void hideSelectedLayers();
    void removeLayer();
    void userRotation();

private:
    QMainWindow *mMainWindow = nullptr;

    QgsMapCanvas *mMapCanvas = nullptr;
    QgsDockWidget* mLayerTreeDock = nullptr;
    QgsLayerTreeView* mLayerTreeView = nullptr;
    QgsLayerTreeMapCanvasBridge* mLayerTreeMapCanvasBridge = nullptr;
    QgsMapToolPan *mMapToolPan = nullptr;

    QList<QgsMapLayer *> mLayersList;
};

#endif // LD_LIB_APP_H
