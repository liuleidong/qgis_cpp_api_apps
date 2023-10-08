#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QGridLayout>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QPlainTextEdit>

#include "panelimagebutton.h"

#include "qgsapplication.h"
#include "qgsproject.h"
#include "qgsrasterlayer.h"
#include "qgsdockwidget.h"
#include "qgsmapcanvas.h"
#include "qgsrubberband.h"
#include "qgsvertexmarker.h"
#include "qgsmaptoolpan.h"
#include "qgsmaptoolzoom.h"
#include "qgsmarkersymbollayer.h"
#include "qgsellipsesymbollayer.h"
#include "qgsgeometrygeneratorsymbollayer.h"
#include "qgsvectorfieldsymbollayer.h"
#include "qgsmasksymbollayer.h"
#include "qgsfillsymbollayer.h"
#include "qgsfillsymbol.h"
#include "qgscategorizedsymbolrenderer.h"
#include "qgsexpressioncontextutils.h"
#include "qgsgraduatedsymbolrenderer.h"
#include "qgsclassificationmethod.h"
#include "qgsclassificationmethodregistry.h"
#include "qgscolorrampimpl.h"
#include "qgsrulebasedrenderer.h"
#include "qgspointdisplacementrenderer.h"
#include "qgspointclusterrenderer.h"
#include "qgsheatmaprenderer.h"
#include "qgslinesymbollayer.h"
#include "qgslinesymbol.h"
#include "qgsinterpolatedlinerenderer.h"
#include "qgsinvertedpolygonrenderer.h"
#include "qgs25drenderer.h"
#include "qgscallout.h"
#include "qgsannotationlayer.h"
#include "qgsannotationpolygonitem.h"
#include "qgsannotationlineitem.h"
#include "qgsannotationmarkeritem.h"
#include "qgsannotationpointtextitem.h"
#include "qgsdiagramrenderer.h"
#include "qgspiediagram.h"
#include "qgstextdiagram.h"
#include "qgshistogramdiagram.h"
#include "qgsstackedbardiagram.h"
#include "qgsmultibandcolorrenderer.h"
#include "qgsbrightnesscontrastfilter.h"
#include "qgshuesaturationfilter.h"
#include "qgsrasterresamplefilter.h"
#include "qgsrasterresampler.h"
#include "qgscubicrasterresampler.h"
#include "qgspalettedrasterrenderer.h"
#include "qgssinglebandgrayrenderer.h"
#include "qgssinglebandpseudocolorrenderer.h"
#include "qgshillshaderenderer.h"
#include "qgsrastercontourrenderer.h"
#include "qgsrastershader.h"
#include "qgsmapcanvasutils.h"
#include "qgsvirtuallayerdefinition.h"
#include "qgsvectorfilewriter.h"
#include "qgsprocessingregistry.h"
#include "qgsprocessingalgrunnertask.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mApp = ll_qgis_base_lib::Instance();
    mApp->initialize(this);
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    auto gridLayout = new QGridLayout;
    gridLayout->addWidget((QWidget*)mApp->mapCanvas());
    ui->page_canvas->setLayout(gridLayout);
    ui->stackedWidget->setCurrentIndex(0);
    initPanels();
}

void MainWindow::initPanels()
{
    init_groupBox_maps();
}

void MainWindow::init_groupBox_maps()
{
    int row = 0,column = -1;
    QGridLayout *layout = (QGridLayout *)ui->groupBox_maps->layout();
    QLabel *label_ogr = new QLabel("OGR data provider(ogr)");
    layout->addWidget(label_ogr,row,0);
    ++row;
    addPanelItem(layout,"addShpSlot","添加shapefile文件",":/res/big.png",row,++column);
    addPanelItem(layout,"","添加gpx文件",":/res/l_tiledMapLayer4326.png",row,++column);
    addPanelItem(layout,"","添加gpkg文件",":/res/l_tiledMapLayer4326.png",row,++column);
    addPanelItem(layout,"","添加geojson文件",":/res/l_tiledMapLayer4326.png",row,++column);
    ++row;column = -1;
    addPanelItem(layout,"","添加gml文件",":/res/l_tiledMapLayer4326.png",row,++column);
    addPanelItem(layout,"","添加kml文件",":/res/l_tiledMapLayer4326.png",row,++column);
    addPanelItem(layout,"","添加dxf文件",":/res/l_tiledMapLayer4326.png",row,++column);
    addPanelItem(layout,"","添加coverage文件",":/res/l_tiledMapLayer4326.png",row,++column);
    ++row;
    QLabel *label_gpx = new QLabel("GPX data provider(gpx)");
    layout->addWidget(label_gpx,row,0);
    ++row;column = -1;
    addPanelItem(layout,"","添加gpx文件",":/res/l_tiledMapLayer4326.png",row,++column);
}

void MainWindow::panelImageButtonClickedSlot(QString slotName)
{
    addShpSlot();
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::addShpSlot()
{
    QString filename = QStringLiteral("maps/shapefile/protected_areas.shp");
    QFileInfo ff(filename);
    //创建图层
    QgsVectorLayer* vecLayer = new QgsVectorLayer(filename,ff.baseName(),"ogr");
    if(!vecLayer->isValid())
    {
        QMessageBox::critical(this,tr("error"),tr("invalid layer"));
        return;
    }
    QgsProject::instance()->addMapLayer(vecLayer);
}

void MainWindow::addPanelItem(QGridLayout *layout,const QString &objectName, const QString &title,const QString &url, int row, int column)
{
    auto p = new PanelImageButton;
    connect(p,&PanelImageButton::panelImageButtonClicked,this,&MainWindow::panelImageButtonClickedSlot);
    p->setObjectName(objectName);
    p->setTitleText(title);
    p->setImageUrl(url);
    layout->addWidget(p,row,column);
}
