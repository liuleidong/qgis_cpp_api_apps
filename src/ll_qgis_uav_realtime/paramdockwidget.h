#ifndef PARAMDOCKWIDGET_H
#define PARAMDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class ParamDockWidget;
}

typedef struct SParams_
{
    int mode;   //符号 or Svg
    QString shape;
    QString svgPath;
    QColor color;
    double size;
    bool showPath;
    int pathLength;
    bool centerShow;
}SParams;

class ParamDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ParamDockWidget(QWidget *parent = nullptr);
    ~ParamDockWidget();
    void simpleOrSvgVisible(bool simpleVisible);


public slots:
    void on_radioButton_simple_clicked(bool checked);
    void on_radioButton_svg_clicked(bool checked);
    void on_pushButton_selectSvg_clicked();
    void on_pushButton_selectColor_clicked();
    void on_pushButton_Set_clicked();

signals:
    void setParamsSignal(SParams params);

private:
    Ui::ParamDockWidget *ui;
    int mMode = 0;
    QColor mColor = "red";
    double mSize;
};

#endif // PARAMDOCKWIDGET_H
