#include "symboldockwidget.h"
#include "ui_symboldockwidget.h"

SymbolDockWidget::SymbolDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SymbolDockWidget)
{
    ui->setupUi(this);
}

SymbolDockWidget::~SymbolDockWidget()
{
    delete ui;
}

void SymbolDockWidget::on_pushButton_pointSimpleMarker_clicked()
{
    emit pointSimpleMarkerSignal();
}

void SymbolDockWidget::on_pushButton_pointSvgMarker_clicked()
{
    emit pointSvgMarkerSignal();
}

void SymbolDockWidget::on_pushButton_pointRasterMarker_clicked()
{
    emit pointRasterMarkerSignal();
}

void SymbolDockWidget::on_pushButton_pointAnimatedMarker_clicked()
{
    emit pointAnimatedMarkerSignal();
}

void SymbolDockWidget::on_pushButton_pointEllipseMarker_clicked()
{
    emit pointEllipseMarkerSignal();
}

void SymbolDockWidget::on_pushButton_pointFontMarker_clicked()
{
    emit pointFontMarkerSignal();
}

void SymbolDockWidget::on_pushButton_pointFilledMarker_clicked()
{
    emit pointFilledMarkerSignal();
}

void SymbolDockWidget::on_pushButton_pointGeometryGenerator_clicked()
{
    emit pointGeometryGeneratorMarkerSignal();
}

void SymbolDockWidget::on_pushButton_pointVectorFieldMarker_clicked()
{
    emit pointVectorFieldMarkerSignal();
}

void SymbolDockWidget::on_pushButton_pointMask_clicked()
{
    emit pointMaskSignal();
}

void SymbolDockWidget::on_pushButton_pointCategorized_clicked()
{
    emit pointCategorizedSignal();
}

void SymbolDockWidget::on_pushButton_pointGraduated_clicked()
{
    emit pointGraduatedSignal();
}

void SymbolDockWidget::on_pushButton_pointRuleBased_clicked()
{
    emit pointRuleBasedSignal();
}

void SymbolDockWidget::on_pushButton_pointDisplacement_clicked()
{
    emit pointDisplacementSignal();
}

void SymbolDockWidget::on_pushButton_pointCluster_clicked()
{
    emit pointClusterSignal();
}

void SymbolDockWidget::on_pushButton_pointHeatmap_clicked()
{
    emit pointHeatmapSignal();
}
