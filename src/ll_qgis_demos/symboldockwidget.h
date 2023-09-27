#ifndef SYMBOLDOCKWIDGET_H
#define SYMBOLDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class SymbolDockWidget;
}

class SymbolDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit SymbolDockWidget(QWidget *parent = nullptr);
    ~SymbolDockWidget();

public slots:
    void on_pushButton_pointSimpleMarker_clicked();
    void on_pushButton_pointSvgMarker_clicked();
    void on_pushButton_pointRasterMarker_clicked();
    void on_pushButton_pointAnimatedMarker_clicked();
    void on_pushButton_pointEllipseMarker_clicked();
    void on_pushButton_pointFontMarker_clicked();
    void on_pushButton_pointFilledMarker_clicked();
    void on_pushButton_pointGeometryGenerator_clicked();
    void on_pushButton_pointVectorFieldMarker_clicked();
    void on_pushButton_pointMask_clicked();
    void on_pushButton_pointCategorized_clicked();
    void on_pushButton_pointGraduated_clicked();
    void on_pushButton_pointRuleBased_clicked();
    void on_pushButton_pointDisplacement_clicked();
    void on_pushButton_pointCluster_clicked();
    void on_pushButton_pointHeatmap_clicked();

signals:
    void pointSimpleMarkerSignal();
    void pointSvgMarkerSignal();
    void pointRasterMarkerSignal();
    void pointAnimatedMarkerSignal();
    void pointEllipseMarkerSignal();
    void pointFontMarkerSignal();
    void pointFilledMarkerSignal();
    void pointGeometryGeneratorMarkerSignal();
    void pointVectorFieldMarkerSignal();
    void pointMaskSignal();
    void pointCategorizedSignal();
    void pointGraduatedSignal();
    void pointRuleBasedSignal();
    void pointDisplacementSignal();
    void pointClusterSignal();
    void pointHeatmapSignal();

private:
    Ui::SymbolDockWidget *ui;
};

#endif // SYMBOLDOCKWIDGET_H
