#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ll_qgis_base_lib.h"
#include "dockwidget.h"
#include "mapcanvasdockwidget.h"
#include "symboldockwidget.h"
#include "linesymboldockwidget.h"
#include "polygondockwidget.h"
#include "annotationdockwidget.h"
#include "diagramdockwidget.h"
#include "rastersymboldockwidget.h"
#include "vectordatadockwidget.h"
#include "processingdockwidget.h"

class QgsMapToolZoom;
class QgsProcessingContext;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initialize();
    void initDockWidget();
    void initMapCanvasDockWidget();
    void initSymbolDockWidget();
    void initLineSymbolDockWidget();
    void initPolygonSymbolDockWidget();
    void initAnnotationDockWidget();
    void initDiagramDockWidget();
    void initRasterSymbolDockWidget();
    void initVectorDataDockWidget();
    void initProcessingDockWidget();
    void initMapTools();

public slots:
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

    void canvasBrSlot();
    void canvasCenterSlot();
    void canvasRotationSlot();
    void rubberBandLineSlot();
    void rubberBandPolygonSlot();
    void vertexMarkerSlot();
    void mapToolPanSlot();
    void mapToolZoomInSlot();
    void mapToolZoomOutSlot();

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

    void lineSimplelineSlot();
    void lineArrowSlot();
    void lineInterpolatedSlot();
    void lineHashedSlot();
    void lineMarkerSlot();
    void lineRasterSlot();
    void lineBurstSlot();

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

    void labelSlot();
    void annotationTextSlot();
    void annotationPointSlot();
    void annotationLineSlot();
    void annotationPolygonSlot();

    void diagramPieSlot();
    void diagramTextSlot();
    void diagramHistogramSlot();
    void diagramStackedBarSlot();

    void rasterMultibandSlot();
    void rasterPalettedSlot();
    void rasterSinglebandGraySlot();
    void rasterSinglebandPseudoSlot();
    void rasterHillshadeSlot();
    void rasterContoursSlot();

    void getFieldsSlot(QPlainTextEdit *plainTextEdit);
    void getFeaturesSlot(QPlainTextEdit *plainTextEdit);
    void selectFeaturesSlot(QPlainTextEdit *plainTextEdit);
    void spatialIndexSlot(QPlainTextEdit *plainTextEdit);
    void distanceAreaSlot(QPlainTextEdit *plainTextEdit);
    void vectorFileWriterSlot(QPlainTextEdit *plainTextEdit);

    void processingClipSlot();
    void processingRandomPointsSlot();
    void processingBufferSlot();

    void algExecuted( bool successful, const QVariantMap &results );
private:
    QgsRasterLayer* addTestRaster(const QString &rasterFile);
    QgsVectorLayer* addTestShape(const QString &shapeFile);
    QgsAnnotationLayer* addTestAnnotationLayer();
    bool minMaxValuesForBand( int band, QgsRasterDataProvider *provider, double &minValue, double &maxValue ) const;

private:
    ll_qgis_base_lib *mApp = nullptr;
    Ui::MainWindow *ui;    
    DockWidget *mDockWidget = nullptr;
    MapCanvasDockWidget *mMapCanvasDockWidget = nullptr;
    SymbolDockWidget *mSymbolDockWidget = nullptr;
    LineSymbolDockWidget *mLineSymbolDockWidget = nullptr;
    PolygonDockWidget *mPolygonDockWidget = nullptr;
    AnnotationDockWidget *mAnnotationDockWidget = nullptr;
    DiagramDockWidget *mDiagramDockWidget = nullptr;
    RasterSymbolDockWidget *mRasterSymbolDockWidget = nullptr;
    VectorDataDockWidget *mVectorDataDockWidget = nullptr;
    ProcessingDockWidget *mProcessingDockWidget = nullptr;
    QgsProcessingContext *mContext = nullptr;

    QgsMapToolPan *mMapToolPan = nullptr;
    QgsMapToolZoom *mMapToolZoomOut = nullptr;
    QgsMapToolZoom *mMapToolZoomIn = nullptr;
};
#endif // MAINWINDOW_H
