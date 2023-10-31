#ifndef VECTORLAYERSAVEASDIALOG_H
#define VECTORLAYERSAVEASDIALOG_H

#include <QDialog>

#include "ui_qgsvectorlayersaveasdialogbase.h"

class QgsVectorLayer;

class VectorLayerSaveAsDialog : public QDialog, private Ui::QgsVectorLayerSaveAsDialogBase
{
    Q_OBJECT
public:

    //! Bitmask of options to be shown
    enum Option
    {
      Symbology = 1, //!< Show symbology options
      DestinationCrs = 1 << 2, //!< Show destination CRS (reprojection) option
      Fields = 1 << 3, //!< Show field customization group
      AddToCanvas = 1 << 4, //!< Show add to map option
      SelectedOnly = 1 << 5, //!< Show selected features only option
      GeometryType = 1 << 6, //!< Show geometry group
      Extent = 1 << 7, //!< Show extent group
      Metadata = 1 << 8, //!< Show metadata options
      AllOptions = ~0 //!< Show all options
    };
    Q_DECLARE_FLAGS( Options, Option )

    VectorLayerSaveAsDialog( QgsVectorLayer *layer, Options options = AllOptions, QWidget *parent = nullptr, Qt::WindowFlags fl = Qt::WindowFlags() );

private:
    void setup();
};

#endif // VECTORLAYERSAVEASDIALOG_H
