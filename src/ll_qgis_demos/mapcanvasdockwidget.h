#ifndef MAPCANVASDOCKWIDGET_H
#define MAPCANVASDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class MapCanvasDockWidget;
}

class MapCanvasDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit MapCanvasDockWidget(QWidget *parent = nullptr);
    ~MapCanvasDockWidget();

public slots:
    void on_pushButton_canvasBr_clicked();
    void on_pushButton_canvasCenter_clicked();
    void on_pushButton_canvasRotation_clicked();
    void on_pushButton_rubberBandLine_clicked();
    void on_pushButton_rubberBandPolygon_clicked();
    void on_pushButton_vertexMarker_clicked();
    void on_pushButton_mapToolPan_clicked();
    void on_pushButton_mapToolZoomIn_clicked();
    void on_pushButton_mapToolZoomOut_clicked();

signals:
    void canvasBrSignal();
    void canvasCenterSignal();
    void canvasRotationSignal();
    void rubberBandLineSignal();
    void rubberBandPolygonSignal();
    void vertexMarkerSignal();
    void mapToolPanSignal();
    void mapToolZoomInSignal();
    void mapToolZoomOutSignal();

private:
    Ui::MapCanvasDockWidget *ui;
};

#endif // MAPCANVASDOCKWIDGET_H
