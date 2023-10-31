#include "vectorlayersaveasdialog.h"

VectorLayerSaveAsDialog::VectorLayerSaveAsDialog( QgsVectorLayer *layer, Options options, QWidget *parent, Qt::WindowFlags fl )
    : QDialog( parent, fl )
{
    setup();
}

void VectorLayerSaveAsDialog::setup()
{
    setupUi( this );
}
