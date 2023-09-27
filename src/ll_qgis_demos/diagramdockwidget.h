#ifndef DIAGRAMDOCKWIDGET_H
#define DIAGRAMDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class DiagramDockWidget;
}

class DiagramDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit DiagramDockWidget(QWidget *parent = nullptr);
    ~DiagramDockWidget();

public slots:
    void on_pushButton_diagramPie_clicked();
    void on_pushButton_diagramText_clicked();
    void on_pushButton_diagramHistogram_clicked();
    void on_pushButton_diagramStackedBar_clicked();

signals:
    void diagramPieSignal();
    void diagramTextSignal();
    void diagramHistogramSignal();
    void diagramStackedBarSignal();
private:
    Ui::DiagramDockWidget *ui;
};

#endif // DIAGRAMDOCKWIDGET_H
