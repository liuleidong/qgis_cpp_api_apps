#ifndef PARAMDOCKWIDGET_H
#define PARAMDOCKWIDGET_H

#include <QDockWidget>
#include <QJsonObject>

class QTreeWidget;
class QTreeWidgetItem;
class QTextBrowser;
class QLineEdit;

namespace Ui {
class ParamDockWidget;
}

class ParamDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ParamDockWidget(QWidget *parent = nullptr);
    ~ParamDockWidget();

    // 设置算法数据
    void setAlgorithmsData(const QJsonObject& data);

private slots:
    void onAlgorithmItemClicked(QTreeWidgetItem* item, int column);
    void onSearchTextChanged(const QString& text);
    void onTreeWidgetCustomContextMenuRequested(const QPoint &pos);

private:
    void setupAlgorithmTree();
    void populateAlgorithmTree(const QJsonObject& data);
    void showAlgorithmDetails(const QJsonObject& algorithmData);
    void filterAlgorithms(const QString& filterText);

private:
    Ui::ParamDockWidget *ui;
    QTreeWidget* m_treeWidget;
    QTextBrowser* m_detailBrowser;
    QLineEdit* m_searchEdit;

    QJsonObject m_algorithmsData; // 存储原始数据
    QTreeWidgetItem* m_currentSelectedItem;
};

#endif // PARAMDOCKWIDGET_H
