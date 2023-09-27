#ifndef RASTERSYMBOLDOCKWIDGET_H
#define RASTERSYMBOLDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class RasterSymbolDockWidget;
}

class RasterSymbolDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit RasterSymbolDockWidget(QWidget *parent = nullptr);
    ~RasterSymbolDockWidget();

public slots:
    void on_pushButton_rasterMultiband_clicked();
    void on_pushButton_rasterPaletted_clicked();
    void on_pushButton_rasterSinglebandGray_clicked();
    void on_pushButton_rasterSinglebandPseudo_clicked();
    void on_pushButton_rasterHillshade_clicked();
    void on_pushButton_rasterContours_clicked();

signals:
    void rasterMultibandSignal();
    void rasterPalettedSignal();
    void rasterSinglebandGraySignal();
    void rasterSinglebandPseudoSignal();
    void rasterHillshadeSignal();
    void rasterContoursSignal();

private:
    Ui::RasterSymbolDockWidget *ui;
};

#endif // RASTERSYMBOLDOCKWIDGET_H
