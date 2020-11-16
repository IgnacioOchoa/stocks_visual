#include <QApplication>
#include <QMainWindow>
#include <QFont>
#include "mainwindow.h"


int main(int argc, char** argv)
{
    QApplication app (argc,argv);
    MainWindow mw;
    QFont fnt("Lucida Sans",10);
    app.setFont(fnt);
    mw.show();
    return app.exec();
}
