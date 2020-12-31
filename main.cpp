#include <QApplication>
#include <QMainWindow>
#include <QFont>
#include "mainwindow.h"


int main(int argc, char** argv)
{
    QApplication app (argc,argv);

    QFile file(":/qss/style.qss");
    if(file.open(QFile::ReadOnly)) {
       QString StyleSheet = QLatin1String(file.readAll());
       app.setStyleSheet(StyleSheet);
    }
    MainWindow mw;
    QFont fnt("Lucida Sans",10);
    app.setFont(fnt);
    mw.show();
    return app.exec();
}
