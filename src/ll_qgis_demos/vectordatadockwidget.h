#ifndef VECTORDATADOCKWIDGET_H
#define VECTORDATADOCKWIDGET_H

#include <QDockWidget>
#include <QPlainTextEdit>

namespace Ui {
class VectorDataDockWidget;
}

class VectorDataDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit VectorDataDockWidget(QWidget *parent = nullptr);
    ~VectorDataDockWidget();

public slots:
    void on_pushButton_getFields_clicked();
    void on_pushButton_getFeatures_clicked();
    void on_pushButton_selectFeatures_clicked();
    void on_pushButton_spatialIndex_clicked();
    void on_pushButton_distanceArea_clicked();
    void on_pushButton_vectorFileWriter_clicked();

signals:
    void getFieldsSignal(QPlainTextEdit *);
    void getFeaturesSignal(QPlainTextEdit *);
    void selectFeaturesSignal(QPlainTextEdit *);
    void spatialIndexSignal(QPlainTextEdit *);
    void distanceAreaSignal(QPlainTextEdit *);
    void vectorFileWriterSignal(QPlainTextEdit *);

private:
    Ui::VectorDataDockWidget *ui;
};

#endif // VECTORDATADOCKWIDGET_H
