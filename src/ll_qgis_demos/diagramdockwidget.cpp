#include "diagramdockwidget.h"
#include "ui_diagramdockwidget.h"

DiagramDockWidget::DiagramDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DiagramDockWidget)
{
    ui->setupUi(this);
}

DiagramDockWidget::~DiagramDockWidget()
{
    delete ui;
}

void DiagramDockWidget::on_pushButton_diagramPie_clicked()
{
    emit diagramPieSignal();
}

void DiagramDockWidget::on_pushButton_diagramText_clicked()
{
    emit diagramTextSignal();
}

void DiagramDockWidget::on_pushButton_diagramHistogram_clicked()
{
    emit diagramHistogramSignal();
}

void DiagramDockWidget::on_pushButton_diagramStackedBar_clicked()
{
    emit diagramStackedBarSignal();
}
