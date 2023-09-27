#ifndef LDCOMMON_H
#define LDCOMMON_H

#include <QString>
#include <QColor>

typedef struct _SSimpleMarker{
    QString shape;
    QString color;
    QString size;
    QString angle;
}SSimpleMarker;

typedef struct _SSvgMarker{
    QString filePath;
    QString color;
    QString size;
    QString angle;
}SSvgMarker;

typedef struct _SSimpleLineStyle{
    int penStyle;
    double penWidth;
    QColor penColor;

}SSimpleLineStyle;

#endif // LDCOMMON_H
