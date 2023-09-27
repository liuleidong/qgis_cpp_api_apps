#include "linesymboldockwidget.h"
#include "ui_linesymboldockwidget.h"

LineSymbolDockWidget::LineSymbolDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::LineSymbolDockWidget)
{
    ui->setupUi(this);
}

LineSymbolDockWidget::~LineSymbolDockWidget()
{
    delete ui;
}

void LineSymbolDockWidget::on_pushButton_lineSimpleline_clicked()
{
    emit lineSimplelineSignal();
}

void LineSymbolDockWidget::on_pushButton_lineArrow_clicked()
{
    emit lineArrowSignal();
}

void LineSymbolDockWidget::on_pushButton_lineInterpolated_clicked()
{
    emit lineInterpolatedSignal();
}

void LineSymbolDockWidget::on_pushButton_lineHashed_clicked()
{
    emit lineHashedSignal();
}

void LineSymbolDockWidget::on_pushButton_lineMarker_clicked()
{
    emit lineMarkerSignal();
}

void LineSymbolDockWidget::on_pushButton_lineRaster_clicked()
{
    emit lineRasterSignal();
}

void LineSymbolDockWidget::on_pushButton_lineBurst_clicked()
{
    emit lineBurstSignal();
}
