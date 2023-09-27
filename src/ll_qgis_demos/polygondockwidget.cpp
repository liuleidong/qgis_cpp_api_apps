#include "polygondockwidget.h"
#include "ui_polygondockwidget.h"

PolygonDockWidget::PolygonDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PolygonDockWidget)
{
    ui->setupUi(this);
}

PolygonDockWidget::~PolygonDockWidget()
{
    delete ui;
}

void PolygonDockWidget::on_pushButton_polygonSimplefill_clicked()
{
    emit polygonSimplefillSignal();
}

void PolygonDockWidget::on_pushButton_polygonCentroid_clicked()
{
    emit polygonCentroidSignal();
}

void PolygonDockWidget::on_pushButton_polygonGradient_clicked()
{
    emit polygonGradientSignal();
}

void PolygonDockWidget::on_pushButton_polygonLinePattern_clicked()
{
    emit polygonLinePatternSignal();
}

void PolygonDockWidget::on_pushButton_polygonPointPattern_clicked()
{
    emit polygonPointPatternSignal();
}

void PolygonDockWidget::on_pushButton_polygonRasterImage_clicked()
{
    emit polygonRasterImageSignal();
}

void PolygonDockWidget::on_pushButton_polygonSvg_clicked()
{
    emit polygonSvgSignal();
}

void PolygonDockWidget::on_pushButton_polygonRandomMarker_clicked()
{
    emit polygonRandomMarkerSignal();
}

void PolygonDockWidget::on_pushButton_polygonShapeburst_clicked()
{
    emit polygonShapeburstSignal();
}

void PolygonDockWidget::on_pushButton_polygonOutlineSimple_clicked()
{
    emit polygonOutlineSimpleSignal();
}

void PolygonDockWidget::on_pushButton_polygonOutlineArrow_clicked()
{
    emit polygonOutlineArrowSignal();
}

void PolygonDockWidget::on_pushButton_polygonOutlineHashed_clicked()
{
    emit polygonOutlineHashedSignal();
}

void PolygonDockWidget::on_pushButton_polygonOutlineInterpolated_clicked()
{
    emit polygonOutlineInterpolatedSignal();
}

void PolygonDockWidget::on_pushButton_polygonOutlineLineburst_clicked()
{
    emit polygonOutlineLineburstSignal();
}

void PolygonDockWidget::on_pushButton_polygonOutlineMarker_clicked()
{
    emit polygonOutlineMarkerSignal();
}

void PolygonDockWidget::on_pushButton_polygonOutlineRaster_clicked()
{
    emit polygonOutlineRasterSignal();
}

void PolygonDockWidget::on_pushButton_polygonInvertedRenderer_clicked()
{
    emit polygonInvertedRendererSignal();
}

void PolygonDockWidget::on_pushButton_polygon25D_clicked()
{
    emit polygon25DSignal();
}
