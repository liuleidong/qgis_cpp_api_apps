#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QStatusBar>

#include "ll_qgis_base_lib.h"
#include "qgsproject.h"
#include "qgslayertreeview.h"
#include "qgslayertreeviewdefaultactions.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QGridLayout;
class QgsMapToolZoom;
class QgsRubberBand;
class QgsVertexMarker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initialize();
    void initToolbar();
    void initStackwidgetPageCanvas();
    void initPanels();
    void initMapTools();
    void initGroupboxInPanel();
    void init_groupBox_maps();
    void init_groupBox_canvas();
    void init_groupBox_vector_point_symbol();
    void init_groupBox_vector_line_symbol();
    void init_groupBox_vector_polygon_symbol();

public slots:
    void actionToPanelsSlot();
    void actionToCanvasSlot();
    void panelImageButtonClickedSlot(QString slotName);
    void stackWidgetCurentChangedSlot(int index);
    //
    void addShpSlot();
    void addGpxSlot();
    void addGpkgSlot();
    void addGeoJsonSlot();
    void addGmlSlot();
    void addKmlSlot();
    void addDxfSlot();
    void addCoverageSlot();
    void addGpx1Slot();
    void addCsvSlot();
    void addSpatiaLiteSlot();
    void addMemorySlot();
    void addWfsSlot();
    void addRasterSlot();
    void addGpkg1Slot();
    void addWmsSlot();
    void addGdalOfflineSlot();
    //
    void canvasBrSlot();
    void canvasCenterSlot();
    void canvasRotationSlot();
    void rubberBandLineSlot();
    void rubberBandPolygonSlot();
    void vertexMarkerSlot();
    void mapToolPanSlot();
    void mapToolZoomInSlot();
    void mapToolZoomOutSlot();
    //
    void pointSimpleMarkerSlot();
    void pointSvgMarkerSlot();
    void pointRasterMarkerSlot();
    void pointAnimatedMarkerSlot();
    void pointEllipseMarkerSlot();
    void pointFontMarkerSlot();
    void pointFilledMarkerSlot();
    void pointGeometryGeneratorMarkerSlot();
    void pointVectorFieldMarkerSlot();
    void pointMaskSlot();
    void pointCategorizedSlot();
    void pointGraduatedSlot();
    void pointRuleBasedSlot();
    void pointDisplacementSlot();
    void pointClusterSlot();
    void pointHeatmapSlot();
    //
    void lineSimplelineSlot();
    void lineArrowSlot();
    void lineInterpolatedSlot();
    void lineHashedSlot();
    void lineMarkerSlot();
    void lineRasterSlot();
    void lineBurstSlot();
    //
    void polygonSimplefillSlot();
    void polygonCentroidSlot();
    void polygonGradientSlot();
    void polygonLinePatternSlot();
    void polygonPointPatternSlot();
    void polygonRasterImageSlot();
    void polygonSvgSlot();
    void polygonRandomMarkerSlot();
    void polygonShapeburstSlot();
    void polygonOutlineSimpleSlot();
    void polygonOutlineArrowSlot();
    void polygonOutlineHashedSlot();
    void polygonOutlineInterpolatedSlot();
    void polygonOutlineLineburstSlot();
    void polygonOutlineMarkerSlot();
    void polygonOutlineRasterSlot();
    void polygonInvertedRendererSlot();
    void polygon25DSlot();
private:
    void addPanelItem(QGridLayout *layout,const QString &objectName,const QString &title,const QString &url,int row,int column);
    template <typename T>
    void zoomToFirstLayer()
    {
        statusBar()->showMessage(tr("loading data...."));
        auto layer = QgsProject::instance()->layers<T>().first();
        mApp->layerTreeView()->setCurrentLayer(layer);
        QTimer::singleShot(1000*1,this,[=]
        {mApp->layerTreeView()->defaultActions()->zoomToLayers( mApp->mapCanvas() );statusBar()->showMessage(tr("loading data done"));});
    }

private:
    ll_qgis_base_lib *mApp = nullptr;
    Ui::MainWindow *ui;

    QgsRubberBand *mRubberBandPoint = nullptr;
    QgsRubberBand *mRubberBandLine = nullptr;
    QgsRubberBand *mRubberBandPolygon = nullptr;
    QgsVertexMarker *mVertexMarker = nullptr;

    QgsMapToolPan *mMapToolPan = nullptr;
    QgsMapToolZoom *mMapToolZoomOut = nullptr;
    QgsMapToolZoom *mMapToolZoomIn = nullptr;

};
#endif // MAINWINDOW_H
