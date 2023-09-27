#include "mapcanvasdockwidget.h"
#include "ui_mapcanvasdockwidget.h"

MapCanvasDockWidget::MapCanvasDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MapCanvasDockWidget)
{
    ui->setupUi(this);
}

MapCanvasDockWidget::~MapCanvasDockWidget()
{
    delete ui;
}

void MapCanvasDockWidget::on_pushButton_canvasBr_clicked()
{
    emit canvasBrSignal();
}

void MapCanvasDockWidget::on_pushButton_canvasCenter_clicked()
{
    emit canvasCenterSignal();
}

void MapCanvasDockWidget::on_pushButton_canvasRotation_clicked()
{
    emit canvasRotationSignal();
}

void MapCanvasDockWidget::on_pushButton_rubberBandLine_clicked()
{
    emit rubberBandLineSignal();
}

void MapCanvasDockWidget::on_pushButton_rubberBandPolygon_clicked()
{
    emit rubberBandPolygonSignal();
}

void MapCanvasDockWidget::on_pushButton_vertexMarker_clicked()
{
    emit vertexMarkerSignal();
}

void MapCanvasDockWidget::on_pushButton_mapToolPan_clicked()
{
    emit mapToolPanSignal();
}

void MapCanvasDockWidget::on_pushButton_mapToolZoomIn_clicked()
{
    emit mapToolZoomInSignal();
}

void MapCanvasDockWidget::on_pushButton_mapToolZoomOut_clicked()
{
    emit mapToolZoomOutSignal();
}
