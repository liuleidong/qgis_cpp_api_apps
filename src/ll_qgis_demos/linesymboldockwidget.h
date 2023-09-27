#ifndef LINESYMBOLDOCKWIDGET_H
#define LINESYMBOLDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class LineSymbolDockWidget;
}

class LineSymbolDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit LineSymbolDockWidget(QWidget *parent = nullptr);
    ~LineSymbolDockWidget();

public slots:
    void on_pushButton_lineSimpleline_clicked();
    void on_pushButton_lineArrow_clicked();
    void on_pushButton_lineInterpolated_clicked();
    void on_pushButton_lineHashed_clicked();
    void on_pushButton_lineMarker_clicked();
    void on_pushButton_lineRaster_clicked();
    void on_pushButton_lineBurst_clicked();
signals:
    void lineSimplelineSignal();
    void lineArrowSignal();
    void lineInterpolatedSignal();
    void lineHashedSignal();
    void lineMarkerSignal();
    void lineRasterSignal();
    void lineBurstSignal();
private:
    Ui::LineSymbolDockWidget *ui;
};

#endif // LINESYMBOLDOCKWIDGET_H
