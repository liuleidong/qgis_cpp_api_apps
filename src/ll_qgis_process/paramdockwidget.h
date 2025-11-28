#ifndef PARAMDOCKWIDGET_H
#define PARAMDOCKWIDGET_H

#include <QDockWidget>
#include <QJsonObject>

class QTreeWidget;
class QTreeWidgetItem;
class QTextBrowser;
class QLineEdit;
class QMenu;

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

    QTextBrowser *detailBrowser() const;

signals:
    void showAlgHelp(QString id);
    void algRun(QString id);

private slots:
    void onAlgorithmItemClicked(QTreeWidgetItem* item, int column);
    void onSearchTextChanged(const QString& text);
    void onTreeWidgetCustomContextMenuRequested(const QPoint &pos);

    void onRunAlgorithm();
    void onShowAlgorithmHelp();

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

    // 右键菜单相关
    QMenu* m_contextMenu;
    QAction* m_runAlgorithmAction;
    QAction* m_showHelpAction;

    QJsonObject m_algorithmsData; // 存储原始数据
    QTreeWidgetItem* m_currentSelectedItem;
};

#endif // PARAMDOCKWIDGET_H
