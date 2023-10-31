#ifndef EXPORTDOCKWIDGET_H
#define EXPORTDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class ExportDockWidget;
}

typedef struct SExportParams_
{
    QString fileName;
    QString driverName;
}SExportParams;

class ExportDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ExportDockWidget(QWidget *parent = nullptr);
    ~ExportDockWidget();

public slots:
    void on_pushButton_export_clicked();

signals:
    void setExportParamsSignal(SExportParams exportParams);

private:
    Ui::ExportDockWidget *ui;
};

#endif // EXPORTDOCKWIDGET_H