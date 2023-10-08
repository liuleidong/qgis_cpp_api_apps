#ifndef PANELIMAGEBUTTON_H
#define PANELIMAGEBUTTON_H

#include <QWidget>

/**
 * @brief The PanelImageButton class
 * 该类用于实现类QtCreator示例主页面的效果
 * 由一个Title，一个图片和边框组成
 * 点击后切换到map canvas界面
 */
class PanelImageButton : public QWidget
{
    Q_OBJECT
public:
    explicit PanelImageButton(QWidget *parent = nullptr);
    enum Alignment {
        Alignment_Left = 0,     //左对齐
        Alignment_Center = 1,   //居中对齐
        Alignment_Right = 2     //右对齐
    };

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    int titleHeight() const;
    void setTitleHeight(int newTitleHeight);

    QFont titleFont() const;
    void setTitleFont(const QFont &newTitleFont);

    Alignment titleAlignment() const;
    void setTitleAlignment(Alignment newTitleAlignment);

    QColor titleColor() const;
    void setTitleColor(const QColor &newTitleColor);

    QString titleText() const;
    void setTitleText(const QString &newTitleText);

    QString imageUrl() const;
    void setImageUrl(const QString &newImageUrl);

    QColor imageBackColor() const;
    void setImageBackColor(const QColor &newImageBackColor);

    int borderWidth() const;
    void setBorderWidth(int newBorderWidth);

    int borderRadius() const;
    void setBorderRadius(int newBorderRadius);

    QColor borderColor() const;
    void setBorderColor(const QColor &newBorderColor);

protected:
    void paintEvent(QPaintEvent *) override;
    void drawBorder(QPainter *painter);
    void drawTitle(QPainter *painter);
    void drawImage(QPainter *painter);
    bool eventFilter(QObject *obj, QEvent *ev) override;

signals:
    void panelImageButtonClicked(QString slotName);

private:
    //标题相关
    int mTitleHeight = 30;//高度
    QFont mTitleFont = QFont(font().family(), 15);//字体
    Alignment mTitleAlignment = Alignment_Center;//对其方式
    QColor mTitleColor = QColor(255, 255, 255); //颜色
    QString mTitleText = ""; //文本
    //图片相关
    QString mImageUrl = "";
    QColor mImageBackColor = QColor(192,192,192);

    //边框相关
    int mBorderWidth = 3;
    int mBorderRadius = 5;
    QColor mBorderColor = QColor(21, 156, 119);
};

#endif // PANELIMAGEBUTTON_H
