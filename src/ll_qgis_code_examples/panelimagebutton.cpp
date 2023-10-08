#include "panelimagebutton.h"

#include <QPainter>

PanelImageButton::PanelImageButton(QWidget *parent)
    : QWidget{parent}
{

}

QSize PanelImageButton::sizeHint() const
{
    return QSize(250, 200);
}

QSize PanelImageButton::minimumSizeHint() const
{
    return QSize(250, 200);
}

int PanelImageButton::titleHeight() const
{
    return mTitleHeight;
}

void PanelImageButton::setTitleHeight(int newTitleHeight)
{
    mTitleHeight = newTitleHeight;
    update();
}

QFont PanelImageButton::titleFont() const
{
    return mTitleFont;
}

void PanelImageButton::setTitleFont(const QFont &newTitleFont)
{
    mTitleFont = newTitleFont;
    update();
}

PanelImageButton::Alignment PanelImageButton::titleAlignment() const
{
    return mTitleAlignment;
}

void PanelImageButton::setTitleAlignment(Alignment newTitleAlignment)
{
    mTitleAlignment = newTitleAlignment;
    update();
}

QColor PanelImageButton::titleColor() const
{
    return mTitleColor;
}

void PanelImageButton::setTitleColor(const QColor &newTitleColor)
{
    mTitleColor = newTitleColor;
    update();
}

QString PanelImageButton::titleText() const
{
    return mTitleText;
}

void PanelImageButton::setTitleText(const QString &newTitleText)
{
    mTitleText = newTitleText;
    update();
}

QString PanelImageButton::imageUrl() const
{
    return mImageUrl;
}

void PanelImageButton::setImageUrl(const QString &newImageUrl)
{
    mImageUrl = newImageUrl;
    update();
}

QColor PanelImageButton::imageBackColor() const
{
    return mImageBackColor;
}

void PanelImageButton::setImageBackColor(const QColor &newImageBackColor)
{
    mImageBackColor = newImageBackColor;
    update();
}

int PanelImageButton::borderWidth() const
{
    return mBorderWidth;
}

void PanelImageButton::setBorderWidth(int newBorderWidth)
{
    mBorderWidth = newBorderWidth;
    update();
}

int PanelImageButton::borderRadius() const
{
    return mBorderRadius;
}

void PanelImageButton::setBorderRadius(int newBorderRadius)
{
    mBorderRadius = newBorderRadius;
    update();
}

QColor PanelImageButton::borderColor() const
{
    return mBorderColor;
}

void PanelImageButton::setBorderColor(const QColor &newBorderColor)
{
    mBorderColor = newBorderColor;
    update();
}

void PanelImageButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //边框
    drawBorder(&painter);
    //标题
    drawTitle(&painter);
    //图片
    drawImage(&painter);
}

void PanelImageButton::drawBorder(QPainter *painter)
{
    if (mBorderWidth <= 0) {
        return;
    }

    painter->save();

    QPen pen;
    pen.setWidth(mBorderWidth);
    pen.setColor(mBorderColor);

    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    QRect rect(mBorderWidth / 2, mBorderWidth / 2, width() - mBorderWidth, height() - mBorderWidth);
    painter->drawRoundedRect(rect, mBorderRadius, mBorderRadius);

    painter->restore();

}

void PanelImageButton::drawTitle(QPainter *painter)
{
    painter->save();

    painter->setPen(Qt::NoPen);
    painter->setBrush(mBorderColor);

    int offset = mBorderWidth - mBorderWidth / 3;
    QRect rect(offset, offset, width() - offset * 2, mTitleHeight);
    painter->drawRect(rect);

    painter->setPen(mTitleColor);
    painter->setFont(mTitleFont);

    //文字区域要重新计算
    offset = mBorderWidth * 3;
    QRect textRect(offset, 0, width() - offset * 2, mTitleHeight);

    Qt::Alignment align;
    if (mTitleAlignment == Alignment_Left) {
        align = Qt::AlignLeft | Qt::AlignVCenter;
    } else if (mTitleAlignment == Alignment_Center) {
        align = Qt::AlignHCenter | Qt::AlignVCenter;
    } else if (mTitleAlignment == Alignment_Right) {
        align = Qt::AlignRight | Qt::AlignVCenter;
    }

    painter->drawText(textRect, align, mTitleText);

    painter->restore();

}

void PanelImageButton::drawImage(QPainter *painter)
{
    QImage image(mImageUrl);
    painter->save();

    int offset = mBorderWidth - mBorderWidth / 3;

    //填充背景
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(192,192,192));
    QRect rect(offset, offset+mTitleHeight, width() - offset * 2, height()-mTitleHeight);
    painter->drawRect(rect);

    float ratio = (float)image.width()/(float)image.height();
    float ww = width()-offset*2;
    float wh = height()-mTitleHeight-offset*3;
    if(ratio > 1.0)
    {
        ww = wh * ratio;
        QRect rect(offset+(width()-ww)/2, offset+mTitleHeight, ww,wh);
        painter->drawImage(rect,image);
    }
    else
    {
        ww = wh / ratio;
        QRect rect(offset+(width()-ww)/2, offset+mTitleHeight, ww,wh);
        painter->drawImage(rect,image);
    }

    painter->restore();

}
