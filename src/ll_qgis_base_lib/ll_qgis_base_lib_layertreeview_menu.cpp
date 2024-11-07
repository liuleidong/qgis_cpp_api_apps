#include "ll_qgis_base_lib_layertreeview_menu.h"

#include <QMainWindow>
#include <QMenu>
#include <QModelIndex>
#include <QIcon>
#include <qgswkbtypes.h>

#include "qgslayertreeviewdefaultactions.h"
#include "qgslayertreenode.h"
#include "qgslayertree.h"
#include "qgsrasterlayer.h"
#include "qgsvectorlayer.h"

#include "ll_qgis_base_lib.h"

ll_qgis_base_layertreeview_menu::ll_qgis_base_layertreeview_menu(QMainWindow* mainWindow,QgsLayerTreeView *view, QgsMapCanvas *canvas)
{
    mMainWindow = mainWindow;
    mLayerTreeView = view;
    mMapCanvas = canvas;
}

ll_qgis_base_layertreeview_menu::~ll_qgis_base_layertreeview_menu()
{

}

QMenu *ll_qgis_base_layertreeview_menu::createContextMenu()
{
    QMenu* menu = new QMenu;
    QgsLayerTreeViewDefaultActions* actions = mLayerTreeView->defaultActions();
    QModelIndex idx = mLayerTreeView->currentIndex();

    // global menu
    if(!idx.isValid())
    {
        menu->addAction(actions->actionAddGroup(menu));
        menu->addAction(tr("&Expand All"),mLayerTreeView,SLOT(expandAll()));
        menu->addAction(tr("&Collapse All"),mLayerTreeView,SLOT(collapseAll()));
    }else
    {
        QgsLayerTreeNode* node = mLayerTreeView->index2node(idx);
        //如果是组，显示对应菜单项目
        if(QgsLayerTree::isGroup(node))
        {
            menu->addAction(actions->actionZoomToGroup(mMapCanvas,menu));
            menu->addAction(actions->actionRemoveGroupOrLayer(menu));
            menu->addAction(actions->actionRenameGroupOrLayer());
            if(mLayerTreeView->selectedNodes(true).count() >= 2)
                menu->addAction(actions->actionGroupSelected(menu));
            menu->addAction(actions->actionAddGroup(menu));
        }
        else if(QgsLayerTree::isLayer(node))
        {
            QgsMapLayer* layer = QgsLayerTree::toLayer(node)->layer();
            if ( layer && layer->isSpatial() )
            {
                //添加zoom to layer右键菜单项目
                QAction *zoomToLayers = actions->actionZoomToLayers( mMapCanvas, menu );
                zoomToLayers->setEnabled( layer->isValid() );
                menu->addAction(zoomToLayers);
            }
            //删除图层
            menu->addAction(actions->actionRemoveGroupOrLayer(menu));
            //矢量图层特有
            QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer );
            if ( vlayer )
            {
                //显示图层Feature数量
                QAction *showFeatureCount = actions->actionShowFeatureCount( menu );
                menu->addAction( showFeatureCount );
                showFeatureCount->setEnabled( vlayer->isValid() );

                const QString iconName = vlayer && vlayer->labeling() && vlayer->labeling()->type() == QLatin1String( "rule-based" )
                                       ? QStringLiteral( "labelingRuleBased.png" )
                                       : QStringLiteral( "labelingSingle.png" );
                QString aIconName = QString(":/images/themes/default/%1").arg(iconName);
                //是否显示标签
                QAction *actionShowLabels = new QAction( QIcon(aIconName), tr( "Show &Labels" ), menu );
                actionShowLabels->setCheckable( true );
                actionShowLabels->setChecked( vlayer->labelsEnabled() );
                connect( actionShowLabels, &QAction::toggled, this, &ll_qgis_base_layertreeview_menu::toggleLabels );
                menu->addAction( actionShowLabels );
            }
            //raster图层特有
            QgsRasterLayer *rlayer = qobject_cast<QgsRasterLayer *>( layer );
            if ( rlayer )
            {
                QAction *zoomToNative = menu->addAction( QIcon( QString( ":/images/themes/default/mActionZoomActual.png" ) ), tr( "Zoom to Nat&ive Resolution (100%)" ), ll_qgis_base_lib::Instance(), &ll_qgis_base_lib::legendLayerZoomNative );
                zoomToNative->setEnabled( rlayer->isValid() );

                if ( rlayer->rasterType() != QgsRasterLayer::Palette )
                {
                    QAction *stretch = menu->addAction( tr( "&Stretch Using Current Extent" ), ll_qgis_base_lib::Instance(), &ll_qgis_base_lib::legendLayerStretchUsingCurrentExtent );
                    stretch->setEnabled( rlayer->isValid() );
                }
            }
        }
    }
    return menu;

}

void ll_qgis_base_layertreeview_menu::toggleLabels(bool enabled)
{
    const QList<QgsLayerTreeLayer *> selectedLayerNodes = mLayerTreeView->selectedLayerNodes();
    for ( QgsLayerTreeLayer *l : selectedLayerNodes )
    {
      QgsVectorLayer *vlayer = qobject_cast< QgsVectorLayer * >( l->layer() );
      if ( !vlayer || !vlayer->isSpatial() )
        continue;

      if ( enabled && !vlayer->labeling() )
      {
        // no labeling setup - create default labeling for layer
        const QgsPalLayerSettings settings = QgsAbstractVectorLayerLabeling::defaultSettingsForLayer( vlayer );
        vlayer->setLabeling( new QgsVectorLayerSimpleLabeling( settings ) );
        vlayer->setLabelsEnabled( true );
      }
      else
      {
        vlayer->setLabelsEnabled( enabled );
      }
      vlayer->emitStyleChanged();
      vlayer->triggerRepaint();
    }
}
