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
    QPlainTextEdit* plainTextEdit();

private:
    Ui::VectorDataDockWidget *ui;
};

#endif // VECTORDATADOCKWIDGET_H
