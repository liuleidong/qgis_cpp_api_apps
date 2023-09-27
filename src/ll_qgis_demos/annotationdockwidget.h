#ifndef ANNOTATIONDOCKWIDGET_H
#define ANNOTATIONDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class AnnotationDockWidget;
}

class AnnotationDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit AnnotationDockWidget(QWidget *parent = nullptr);
    ~AnnotationDockWidget();

public slots:
    void on_pushButton_Label_clicked();
    void on_pushButton_annotationText_clicked();
    void on_pushButton_annotationPoint_clicked();
    void on_pushButton_annotationLine_clicked();
    void on_pushButton_annotationPolygon_clicked();
signals:
    void labelSignal();
    void annotationTextSignal();
    void annotationPointSignal();
    void annotationLineSignal();
    void annotationPolygonSignal();

private:
    Ui::AnnotationDockWidget *ui;
};

#endif // ANNOTATIONDOCKWIDGET_H
