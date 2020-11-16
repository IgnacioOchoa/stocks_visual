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
    void getStocksInfo();
    void getStockNames(QStringList& );
    void populateCBStockInfo();


    Ui::MainWindow *ui;
    QNetworkAccessManager manager;
    QStringList stockNames;
    QByteArray binaryReply;
    QJsonDocument jdoc;

    QStringList symbols;
    QStringList types;
    QStringList currencies;
    QStringList descriptions;
    QStringList displaysymbols;

private slots:
    void readyRead();
    void replyFinished();
    void stockCBchanged(int);

};

#endif // MAINWINDOW_H
