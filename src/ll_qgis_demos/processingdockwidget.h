#ifndef PROCESSINGDOCKWIDGET_H
#define PROCESSINGDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class ProcessingDockWidget;
}

class ProcessingDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ProcessingDockWidget(QWidget *parent = nullptr);
    ~ProcessingDockWidget();

public slots:
    void on_pushButton_processingRandomPoints_clicked();
    void on_pushButton_processingClip_clicked();
    void on_pushButton_processingBuffer_clicked();

signals:
    void processingRandomPointsSignal();
    void processingClipSignal();
    void processingBufferSignal();

private:
    Ui::ProcessingDockWidget *ui;
};

#endif // PROCESSINGDOCKWIDGET_H
