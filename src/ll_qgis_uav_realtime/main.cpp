#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QDebug>

#include "qgsapplication.h"
#include "qgsproviderregistry.h"
#include "qgsprocessingregistry.h"
#include "qgsnativealgorithms.h"

int main(int argc, char *argv[])
{
    QgsApplication a(argc, argv,true);
    QString strAppDir = QCoreApplication::applicationDirPath();

    QFile qssFile("qrc:/../../inc/qss/Aqua.qss");//1.
    if(qssFile.open(QFile::ReadOnly)){//2.
        a.setStyleSheet(qssFile.readAll());//3.
    }
    qssFile.close();//4.

    //proj的配置目录，坐标映射相关 proj.db
    QString strProjDir = strAppDir + QString("/share/proj/");
    qputenv("PROJ_LIB",strProjDir.toStdString().c_str());
    //Alters prefix path - used by 3rd party apps.
    QgsApplication::setPrefixPath( strAppDir, false );
    QString strPluginDir = strAppDir + QString("/plugins/");
    QgsApplication::setPluginPath(strPluginDir);
    QString strPkgDir = strAppDir + QString("/share/qgis");
    QgsApplication::setPkgDataPath(strPkgDir);
    //loads providers
    QgsApplication::initQgis();
    //使用process toolbox 需要初始化
    QgsApplication::processingRegistry()->addProvider( new QgsNativeAlgorithms( QgsApplication::processingRegistry() ) );

    qDebug() << QgsApplication::qmlImportPath();

    MainWindow w;
    w.initialize();
    w.show();
    return a.exec();
}
