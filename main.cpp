#include <QApplication>
#include <QFontDatabase>
#include "fontManager.h"
#include "loginwindow.h"
#include <qDebug>
#include "SerialTestHelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    // 设置自定义全局字体
    FontManager::setupGlobalFont(":/font/resources/fonts/PingFang/PingFang Medium_downcc.otf", 12);
    loginWindow w;
    //CustomMainWindow w;
    //TeachingMainWindow w;
    //MainWindow w;
    w.show();
    //w.showFullScreen();
    //SerialTestHelper helper;
    //helper.start("COM3"); // 串口号根据实际修改

    return a.exec();
}
