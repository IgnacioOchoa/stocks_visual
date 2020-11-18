#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDataStream>
#include <QVariantMap>
#include <QVector>
#include <QDateTime>

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
    void getStockNames(QStringList&);
    void getStockData();
    void populateCBStockInfo();

    Ui::MainWindow *ui;
    QNetworkAccessManager manager;
    QStringList stockNames;
    QByteArray binaryInfoReply;
    QByteArray binaryDataReply;
    QJsonDocument jdocInfo;
    QJsonDocument jdocData;

    QStringList symbols;
    QStringList types;
    QStringList currencies;
    QStringList descriptions;
    QStringList displaysymbols;

    QVector<double> c_data;
    QVector<double> h_data;
    QVector<double> l_data;
    QVector<double> o_data;
    QVector<long long> t_data;
    QVector<long long> v_data;

private slots:
    void InfoReadyRead();
    void InfoReplyFinished();
    void DataReadyRead();
    void DataReplyFinished();
    void stockCBchanged(int);
    void plotPressed();
};

#endif // MAINWINDOW_H
