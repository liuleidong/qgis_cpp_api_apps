#include "processingdockwidget.h"
#include "ui_processingdockwidget.h"

ProcessingDockWidget::ProcessingDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ProcessingDockWidget)
{
    ui->setupUi(this);
}

ProcessingDockWidget::~ProcessingDockWidget()
{
    delete ui;
}

void ProcessingDockWidget::on_pushButton_processingRandomPoints_clicked()
{
    emit processingRandomPointsSignal();
}

void ProcessingDockWidget::on_pushButton_processingClip_clicked()
{
    emit processingClipSignal();
}

void ProcessingDockWidget::on_pushButton_processingBuffer_clicked()
{
    emit processingBufferSignal();
}
