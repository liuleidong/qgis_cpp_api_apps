#include "paramdockwidget.h"
#include "ui_paramdockwidget.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>

ParamDockWidget::ParamDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ParamDockWidget)
{
    ui->setupUi(this);
    simpleOrSvgVisible(false);
    ui->label_5->setText(mColor.name());
    mMode = 1;
}

ParamDockWidget::~ParamDockWidget()
{
    delete ui;
}

void ParamDockWidget::simpleOrSvgVisible(bool simpleVisible)
{
    ui->label_6->setVisible(simpleVisible);
    ui->comboBox_shape->setVisible(simpleVisible);
    ui->label_7->setVisible(!simpleVisible);
    ui->lineEdit_svgPath->setVisible(!simpleVisible);
    ui->pushButton_selectSvg->setVisible(!simpleVisible);
}

void ParamDockWidget::on_radioButton_simple_clicked(bool checked)
{
    simpleOrSvgVisible(true);
    mMode = 0;
}

void ParamDockWidget::on_radioButton_svg_clicked(bool checked)
{
    simpleOrSvgVisible(false);
    mMode = 1;
}

void ParamDockWidget::on_pushButton_selectSvg_clicked()
{
    QString filename = QFileDialog::getOpenFileName();
    ui->lineEdit_svgPath->setText(filename);
}

void ParamDockWidget::on_pushButton_selectColor_clicked()
{
    QColor color = QColorDialog::getColor(Qt::red);
    mColor = color;
    ui->label_5->setText(mColor.name());
}

void ParamDockWidget::on_pushButton_Set_clicked()
{
    if(mMode == 1 && ui->lineEdit_svgPath->text().length() == 0)
    {
        QMessageBox msgBox;
        msgBox.setText("svg not set");
        msgBox.exec();
    }
    QFileInfo ff(ui->lineEdit_svgPath->text());
    if(mMode == 1 && !ff.isFile())
    {
        QMessageBox msgBox;
        msgBox.setText("invaild svg file");
        msgBox.exec();
    }
    SParams params;
    params.mode = mMode;
    params.shape = ui->comboBox_shape->currentText();
    params.svgPath = ui->lineEdit_svgPath->text();
    params.color = mColor;
    params.size = ui->doubleSpinBox_size->value();
    params.showPath = ui->checkBox_showPath->isChecked();
    params.pathLength = ui->spinBox_size->value();
    params.centerShow = ui->checkBox_centerShow->isChecked();
    emit setParamsSignal(params);
}
