#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ll_qgis_base_lib.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QGridLayout;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initialize();
    void initPanels();
    void init_groupBox_maps();

public slots:
    void panelImageButtonClickedSlot(QString slotName);

    void addShpSlot();
private:
    void addPanelItem(QGridLayout *layout,const QString &objectName,const QString &title,const QString &url,int row,int column);

private:
    ll_qgis_base_lib *mApp = nullptr;
    Ui::MainWindow *ui;


};
#endif // MAINWINDOW_H
