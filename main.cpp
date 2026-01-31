#include <QApplication>
#include <QFontDatabase>
#include "customMainWindow.h"
#include "fontManager.h"
#include "loginwindow.h"
#include "mainWindow.h"
#include "teachingMainWindow.h"
#include <qDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置自定义全局字体
    FontManager::setupGlobalFont(":/font/resources/fonts/PingFang/PingFang Medium_downcc.otf", 13);
    a.setStyle("Fusion");
    //loginWindow w;
    //CustomMainWindow w;
    //TeachingMainWindow w;
    MainWindow w;
    w.show();
    return a.exec();
}
