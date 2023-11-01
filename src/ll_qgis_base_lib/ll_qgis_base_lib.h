#ifndef LD_LIB_APP_H
#define LD_LIB_APP_H

#include <QObject>

#include "ll_qgis_base_lib_global.h"
#include "ll_qgis_base_lib_singleton.h"
#include "ld_symbol_property.h"
#include "ld_geometry.h"


#include "qgis.h"
#include "qgspallabeling.h"
#include "qgsvectorlayerlabeling.h"
#include "qgsraster.h"
#include "qgssymbol.h"
#include "qgsvectorlayer.h"
#include "qgssinglesymbolrenderer.h"
#include "qgsmarkersymbol.h"

// 这两个属于app库，所以这里直接将源码拷贝到这里
#include "qgsstatusbarscalewidget.h"
#include "qgsstatusbarcoordinateswidget.h"


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

/**
 * @brief 一些常用的功能模块放到库里
 */
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
    QgsStatusBarScaleWidget *scaleWidget() const;
    QgsStatusBarCoordinatesWidget *coordsEdit() const;

    ///
    /// \brief
    /// 1. 创建QgsMapCanvas
    /// 2. 创建QgsLayerTreeView等相关类，实现图层管理
    /// 3. 创建QgsMapTool
    /// 4. 创建状态栏widget
    /// \param mainWindow - 传入MainWindow指针
    ///
    void initialize(QMainWindow *mainWindow);
    ///
    /// \brief 程序退出时清理资源
    ///
    void cleanup();
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

    ///
    /// \brief 初始化状态栏，目前有坐标栏和缩放栏
    void initStatusbarWidget();

    ///
    /// \brief 保存项目
    /// \param filename
    /// \return
    ///
    bool saveProjects(const QString &filename);
    ///
    /// \brief 读取项目
    /// \param filename
    /// \param flags
    /// \return
    ///
    bool readProjects(const QString &filename,Qgis::ProjectReadFlags flags = Qgis::ProjectReadFlags());
    ///
    /// \brief 设置整体项目的坐标系
    /// \param crs
    /// \param adjustEllipsoid
    ///
    void setCrs(const QgsCoordinateReferenceSystem &crs, bool adjustEllipsoid);

    /**
     * @brief addVectorLayer
     * @param uri
     * @param baseName
     * @param provider
     * @return
     */
    QgsMapLayer *addVectorLayer(const QString &uri, const QString &baseName, const QString &provider="ogr");
    /**
     * @brief addRasterLayer
     * @param uri
     * @param baseName
     * @param provider
     * @return
     */
    QgsMapLayer *addRasterLayer(const QString &uri, const QString &baseName, const QString &provider="gdal");

    ///
    /// \brief addWmsLayer
    /// \param uri
    /// \param baseName
    /// \return
    ///
    QgsMapLayer *addWmsLayer(const QString &uri,const QString &baseName);

    ///
    /// \brief MapCanvas有可能有多个
    /// \return
    ///
    QList< QgsMapCanvas * > mapCanvases();
    ///
    /// \brief refreshMapCanvas
    /// \param redrawAllLayers
    ///
    void refreshMapCanvas(bool redrawAllLayers = false);

    ///
    /// \brief legendLayerZoomNative
    /// 图层树右键菜单使用该函数
    void legendLayerZoomNative();
    /**
     * @brief legendLayerStretchUsingCurrentExtent
     * 图层树右键菜单使用该函数
     */
    void legendLayerStretchUsingCurrentExtent();

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
    QgsStatusBarScaleWidget *mScaleWidget = nullptr;
    QgsStatusBarCoordinatesWidget *mCoordsEdit = nullptr;

    QList<QgsMapLayer *> mLayersList;
};

#endif // LD_LIB_APP_H
