#ifndef PARAMDOCKWIDGET_H
#define PARAMDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class ParamDockWidget;
}

class ParamDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ParamDockWidget(QWidget *parent = nullptr);
    ~ParamDockWidget();

private:
    Ui::ParamDockWidget *ui;
};

#endif // PARAMDOCKWIDGET_H
