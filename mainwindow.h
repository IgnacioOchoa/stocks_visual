#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDataStream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    void setDates();
    void populateStockNames();
    void getStockNames(QStringList& );


    Ui::MainWindow *ui;
    QNetworkAccessManager manager;
    QStringList stockNames;
    QByteArray binaryReply;
    QJsonDocument jdoc;

private slots:
    void readyRead();
    void replyFinished();

};

#endif // MAINWINDOW_H
