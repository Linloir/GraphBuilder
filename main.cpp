#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    //int id = QFontDatabase::addApplicationFont(":/fonts/fonts/corbel.ttf");
    //qDebug() << id;
    //QFontDatabase::addApplicationFont(":/fonts/fonts/corbell.ttf");
    //Qt::AA_EnableHighDpiScaling;
#ifdef Q_OS_LINUX
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowFlag(Qt::FramelessWindowHint);
    w.setAttribute(Qt::WA_TranslucentBackground);
    w.show();
    return a.exec();

}
