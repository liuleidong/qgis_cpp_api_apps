#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString strAppDir = QCoreApplication::applicationDirPath();
    QFile qssFile(strAppDir + QString("/qss/ElegantDark.qss"));
    if(qssFile.open(QFile::ReadOnly))
    {
        a.setStyleSheet(qssFile.readAll());
    }
    qssFile.close();

    MainWindow w;
    w.show();
    return a.exec();
}
