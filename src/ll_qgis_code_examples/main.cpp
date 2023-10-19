#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QTextCodec>

#include "qgsapplication.h"
#include "qgsproviderregistry.h"
#include "qgsprocessingregistry.h"
#include "qgsnativealgorithms.h"

int main(int argc, char *argv[])
{
    qputenv("QGIS_DEBUG",QString("2").toStdString().c_str());
    qputenv("QGIS_LOG_FILE",QString("gislog.log").toStdString().c_str());
    QgsApplication a(argc, argv,true);
    QString strAppDir = QCoreApplication::applicationDirPath();

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
#endif

    //proj的配置目录，坐标映射相关 proj.db
    QString strProjDir = strAppDir + QString("/share/proj/");
    qputenv("PROJ_LIB",strProjDir.toStdString().c_str());
    //Alters prefix path - used by 3rd party apps.
    QgsApplication::setPrefixPath( strAppDir, false );
    QString strPluginDir = strAppDir + QString("/qgis_runtime_dlls/plugins/");
    QgsApplication::setPluginPath(strPluginDir);
    QString strPkgDir = strAppDir + QString("/qgis_runtime_dlls/share/qgis");
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
