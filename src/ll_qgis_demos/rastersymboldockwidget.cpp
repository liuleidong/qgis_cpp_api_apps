#include "rastersymboldockwidget.h"
#include "ui_rastersymboldockwidget.h"

RasterSymbolDockWidget::RasterSymbolDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::RasterSymbolDockWidget)
{
    ui->setupUi(this);
}

RasterSymbolDockWidget::~RasterSymbolDockWidget()
{
    delete ui;
}

void RasterSymbolDockWidget::on_pushButton_rasterMultiband_clicked()
{
    emit rasterMultibandSignal();
}

void RasterSymbolDockWidget::on_pushButton_rasterPaletted_clicked()
{
    emit rasterPalettedSignal();
}

void RasterSymbolDockWidget::on_pushButton_rasterSinglebandGray_clicked()
{
    emit rasterSinglebandGraySignal();
}

void RasterSymbolDockWidget::on_pushButton_rasterSinglebandPseudo_clicked()
{
    emit rasterSinglebandPseudoSignal();
}

void RasterSymbolDockWidget::on_pushButton_rasterHillshade_clicked()
{
    emit rasterHillshadeSignal();
}

void RasterSymbolDockWidget::on_pushButton_rasterContours_clicked()
{
    emit rasterContoursSignal();
}
