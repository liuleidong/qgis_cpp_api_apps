#ifndef LD_LIB_LAYERTREEVIEW_MENU_H
#define LD_LIB_LAYERTREEVIEW_MENU_H

#include "ll_qgis_base_lib_global.h"

#include <QObject>

#include "qgslayertreeview.h"

class QAction;
class QgsMapCanvas;
class QMainWindow;

class ll_qgis_base_layertreeview_menu : public QObject, public QgsLayerTreeViewMenuProvider
{
    Q_OBJECT
public:
    ll_qgis_base_layertreeview_menu(QMainWindow* mainWindow,QgsLayerTreeView* view,QgsMapCanvas* canvas);
    ~ll_qgis_base_layertreeview_menu() override;
    QMenu *createContextMenu() override;

private slots:
    void toggleLabels( bool enabled );

private:
    QMainWindow* mMainWindow = nullptr;
    QgsLayerTreeView* mLayerTreeView = nullptr;
    QgsMapCanvas* mMapCanvas = nullptr;

};

#endif // LD_LIB_LAYERTREEVIEW_MENU_H
