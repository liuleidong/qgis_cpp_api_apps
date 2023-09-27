#include "annotationdockwidget.h"
#include "ui_annotationdockwidget.h"

AnnotationDockWidget::AnnotationDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::AnnotationDockWidget)
{
    ui->setupUi(this);
}

AnnotationDockWidget::~AnnotationDockWidget()
{
    delete ui;
}

void AnnotationDockWidget::on_pushButton_Label_clicked()
{
    emit labelSignal();
}

void AnnotationDockWidget::on_pushButton_annotationText_clicked()
{
    emit annotationTextSignal();
}

void AnnotationDockWidget::on_pushButton_annotationPoint_clicked()
{
    emit annotationPointSignal();
}

void AnnotationDockWidget::on_pushButton_annotationLine_clicked()
{
    emit annotationLineSignal();
}

void AnnotationDockWidget::on_pushButton_annotationPolygon_clicked()
{
    emit annotationPolygonSignal();
}
