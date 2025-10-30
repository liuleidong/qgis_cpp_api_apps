#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QDebug>

#include "qgsapplication.h"
#include "qgsproviderregistry.h"
#include "qgsprocessingregistry.h"


int main(int argc, char *argv[])
{
    QgsApplication a(argc, argv,true);
    QString strAppDir = QCoreApplication::applicationDirPath();

    QFile qssFile(strAppDir + QString("/qss/Aqua.qss"));
    if(qssFile.open(QFile::ReadOnly))
    {
        a.setStyleSheet(qssFile.readAll());
    }
    qssFile.close();

    QgsApplication::init();
    QgsApplication::initQgis();
    QgsProviderRegistry::instance( QgsApplication::pluginPath() );
    ( void ) QgsApplication::resolvePkgPath(); // trigger storing of application path in QgsApplication

    MainWindow w;
    w.initialize();
    w.show();
    return a.exec();
}
