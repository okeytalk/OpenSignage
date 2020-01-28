#include "mainwindow.h"
#include <QApplication>

#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //QApplication::setOverrideCursor(Qt::BlankCursor);       // 마우스커서 Blank처리

    MainWindow w;
    //w.setWindowState(Qt::WindowFullScreen);                 // full Screen 모드
    w.show();

    return a.exec();
}
