#include "paramdockwidget.h"
#include "ui_paramdockwidget.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QTreeWidget>
#include <QTextBrowser>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

ParamDockWidget::ParamDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ParamDockWidget),
    m_treeWidget(nullptr),
    m_detailBrowser(nullptr),
    m_searchEdit(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("算法浏览器");

    setupAlgorithmTree();
}

ParamDockWidget::~ParamDockWidget()
{
    delete ui;
}

void ParamDockWidget::setupAlgorithmTree()
{
    // 创建主窗口部件
    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);

    // 搜索框
    QHBoxLayout* searchLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("搜索算法名称、分组或标签...");
    m_searchEdit->setClearButtonEnabled(true);
    searchLayout->addWidget(new QLabel("搜索:"));
    searchLayout->addWidget(m_searchEdit);

    // 分割器
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    // 左侧：算法树
    QWidget* leftWidget = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(new QLabel("可用算法"));

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderLabels({"名称", "分组", "状态"});
    m_treeWidget->setColumnCount(3);
    m_treeWidget->setSortingEnabled(true);
    m_treeWidget->sortByColumn(0, Qt::AscendingOrder);

    leftLayout->addWidget(m_treeWidget);

    // 右侧：详细信息
    QWidget* rightWidget = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->addWidget(new QLabel("算法详情"));

    m_detailBrowser = new QTextBrowser(this);
    m_detailBrowser->setMinimumWidth(300);
    rightLayout->addWidget(m_detailBrowser);

    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    // 主布局
    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(splitter);

    setWidget(mainWidget);

    // 连接信号槽
    connect(m_treeWidget, &QTreeWidget::itemClicked,
            this, &ParamDockWidget::onAlgorithmItemClicked);
    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &ParamDockWidget::onSearchTextChanged);
}

void ParamDockWidget::setAlgorithmsData(const QJsonObject& data)
{
    m_algorithmsData = data;
    populateAlgorithmTree(data);
}

void ParamDockWidget::populateAlgorithmTree(const QJsonObject& data)
{
    m_treeWidget->clear();

    // 遍历providers
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        QString providerName = it.key();
        QJsonObject providerObj = it.value().toObject();

        // 创建provider节点
        QTreeWidgetItem* providerItem = new QTreeWidgetItem(m_treeWidget);
        providerItem->setText(0, providerName);
        providerItem->setText(1, "Provider");

        // 设置provider信息
        QString providerInfo = QString("激活: %1, 支持栅格: %2, 支持矢量: %3")
            .arg(providerObj.value("is_active").toBool() ? "是" : "否")
            .arg(providerObj.value("default_raster_file_extension").toString())
            .arg(providerObj.value("default_vector_file_extension").toString());
        providerItem->setToolTip(0, providerInfo);

        // 遍历algorithms
        QJsonObject algorithmsObj = providerObj.value("algorithms").toObject();
        for (auto algoIt = algorithmsObj.constBegin(); algoIt != algorithmsObj.constEnd(); ++algoIt) {
            QString algoId = algoIt.key();
            QJsonObject algoObj = algoIt.value().toObject();

            QTreeWidgetItem* algoItem = new QTreeWidgetItem(providerItem);
            algoItem->setText(0, algoObj.value("name").toString());
            algoItem->setText(1, algoObj.value("group").toString());

            // 状态信息
            QString status;
            if (algoObj.value("deprecated").toBool()) {
                status = "已弃用";
                algoItem->setForeground(0, QColor(Qt::red));
            } else if (algoObj.value("has_known_issues").toBool()) {
                status = "已知问题";
                algoItem->setForeground(0, QColor(Qt::darkYellow));
            } else {
                status = "可用";
                algoItem->setForeground(0, QColor(Qt::darkGreen));
            }
            algoItem->setText(2, status);

            // 存储完整算法数据
            QVariant algoData(algoObj.toVariantMap());
            algoItem->setData(0, Qt::UserRole, algoData);
            algoItem->setData(0, Qt::UserRole + 1, providerName);
        }

        providerItem->setExpanded(true);
    }

    // 调整列宽
    for (int i = 0; i < m_treeWidget->columnCount(); ++i) {
        m_treeWidget->resizeColumnToContents(i);
    }
}

void ParamDockWidget::onAlgorithmItemClicked(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column)

    if (!item || !item->parent()) {
        // 点击的是provider节点，不是算法节点
        m_detailBrowser->setHtml("<h3>选择具体算法查看详情</h3>");
        return;
    }

    // 获取算法数据
    QVariant algoData = item->data(0, Qt::UserRole);
    if (!algoData.isValid()) {
        return;
    }

    showAlgorithmDetails(QJsonObject::fromVariantMap(algoData.toMap()));
}

void ParamDockWidget::showAlgorithmDetails(const QJsonObject& algorithmData)
{
    QString html;
    html += "<html><body style='font-family: Arial, sans-serif;'>";

    // 算法名称
    html += QString("<h2 style='color: #2E86AB;'>%1</h2>").arg(algorithmData.value("name").toString());

    // 基本信息表格
    html += "<table width='100%' cellspacing='5'>";
    html += QString("<tr><td width='30%'><b>分组:</b></td><td>%1</td></tr>")
        .arg(algorithmData.value("group").toString());
    html += QString("<tr><td><b>可取消:</b></td><td>%1</td></tr>")
        .arg(algorithmData.value("can_cancel").toBool() ? "是" : "否");
    html += QString("<tr><td><b>已弃用:</b></td><td><span style='color: %1;'>%2</span></td></tr>")
        .arg(algorithmData.value("deprecated").toBool() ? "red" : "green")
        .arg(algorithmData.value("deprecated").toBool() ? "是" : "否");
    html += QString("<tr><td><b>已知问题:</b></td><td><span style='color: %1;'>%2</span></td></tr>")
        .arg(algorithmData.value("has_known_issues").toBool() ? "orange" : "green")
        .arg(algorithmData.value("has_known_issues").toBool() ? "是" : "否");
    html += QString("<tr><td><b>需要匹配CRS:</b></td><td>%1</td></tr>")
        .arg(algorithmData.value("requires_matching_crs").toBool() ? "是" : "否");
    html += "</table>";

    // 标签
    QJsonArray tagsArray = algorithmData.value("tags").toArray();
    if (!tagsArray.isEmpty()) {
        html += "<h3>标签:</h3><div style='margin: 5px 0;'>";
        for (const QJsonValue& tag : tagsArray) {
            html += QString("<span style='background: #E8E8E8; padding: 2px 6px; margin: 2px; "
                          "border-radius: 3px; display: inline-block;'>%1</span>")
                .arg(tag.toString());
        }
        html += "</div>";
    }

    // 描述
    QString description = algorithmData.value("short_description").toString();
    if (!description.isEmpty()) {
        html += QString("<h3>描述:</h3><p>%1</p>").arg(description);
    }

    // 帮助链接
    QString helpUrl = algorithmData.value("help_url").toString();
    if (!helpUrl.isEmpty() && helpUrl != "null") {
        html += QString("<h3>帮助:</h3><a href='%1'>%1</a>").arg(helpUrl);
    }

    html += "</body></html>";

    m_detailBrowser->setHtml(html);
}

void ParamDockWidget::onSearchTextChanged(const QString& text)
{
    filterAlgorithms(text);
}

void ParamDockWidget::filterAlgorithms(const QString& filterText)
{
    if (filterText.isEmpty()) {
        // 显示所有项
        for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
            QTreeWidgetItem* providerItem = m_treeWidget->topLevelItem(i);
            providerItem->setHidden(false);
            for (int j = 0; j < providerItem->childCount(); ++j) {
                providerItem->child(j)->setHidden(false);
            }
        }
        return;
    }

    QString filter = filterText.toLower();

    for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* providerItem = m_treeWidget->topLevelItem(i);
        bool providerVisible = false;

        for (int j = 0; j < providerItem->childCount(); ++j) {
            QTreeWidgetItem* algoItem = providerItem->child(j);
            bool match = algoItem->text(0).toLower().contains(filter) ||  // 名称
                        algoItem->text(1).toLower().contains(filter) ||   // 分组
                        algoItem->text(2).toLower().contains(filter);     // 状态

            // 检查标签
            if (!match) {
                QVariant algoData = algoItem->data(0, Qt::UserRole);
                if (algoData.isValid()) {
                    QJsonObject algoObj = QJsonObject::fromVariantMap(algoData.toMap());
                    QJsonArray tags = algoObj.value("tags").toArray();
                    for (const QJsonValue& tag : tags) {
                        if (tag.toString().toLower().contains(filter)) {
                            match = true;
                            break;
                        }
                    }
                }
            }

            algoItem->setHidden(!match);
            if (match) {
                providerVisible = true;
            }
        }

        providerItem->setHidden(!providerVisible);
        if (providerVisible) {
            providerItem->setExpanded(true);
        }
    }
}
