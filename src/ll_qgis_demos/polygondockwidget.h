#ifndef POLYGONDOCKWIDGET_H
#define POLYGONDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class PolygonDockWidget;
}

class PolygonDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit PolygonDockWidget(QWidget *parent = nullptr);
    ~PolygonDockWidget();

public slots:
    void on_pushButton_polygonSimplefill_clicked();
    void on_pushButton_polygonCentroid_clicked();
    void on_pushButton_polygonGradient_clicked();
    void on_pushButton_polygonLinePattern_clicked();
    void on_pushButton_polygonPointPattern_clicked();
    void on_pushButton_polygonRasterImage_clicked();
    void on_pushButton_polygonSvg_clicked();
    void on_pushButton_polygonRandomMarker_clicked();
    void on_pushButton_polygonShapeburst_clicked();
    void on_pushButton_polygonOutlineSimple_clicked();
    void on_pushButton_polygonOutlineArrow_clicked();
    void on_pushButton_polygonOutlineHashed_clicked();
    void on_pushButton_polygonOutlineInterpolated_clicked();
    void on_pushButton_polygonOutlineLineburst_clicked();
    void on_pushButton_polygonOutlineMarker_clicked();
    void on_pushButton_polygonOutlineRaster_clicked();
    void on_pushButton_polygonInvertedRenderer_clicked();
    void on_pushButton_polygon25D_clicked();
signals:
    void polygonSimplefillSignal();
    void polygonCentroidSignal();
    void polygonGradientSignal();
    void polygonLinePatternSignal();
    void polygonPointPatternSignal();
    void polygonRasterImageSignal();
    void polygonSvgSignal();
    void polygonRandomMarkerSignal();
    void polygonShapeburstSignal();
    void polygonOutlineSimpleSignal();
    void polygonOutlineArrowSignal();
    void polygonOutlineHashedSignal();
    void polygonOutlineInterpolatedSignal();
    void polygonOutlineLineburstSignal();
    void polygonOutlineMarkerSignal();
    void polygonOutlineRasterSignal();
    void polygonInvertedRendererSignal();
    void polygon25DSignal();

private:
    Ui::PolygonDockWidget *ui;
};

#endif // POLYGONDOCKWIDGET_H
