#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QPushButton>
#include <QMessageBox>
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
#include <QtCharts>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QCandlestickSet>

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
    void reportDataDays();
    void plotData();

    Ui::MainWindow *ui;
    QStatusBar * statBar;
    QLabel * statLabel;
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

    int numPlotPoints;
    int numDays;
    long long timestamp;
    QString searchSymbol;

    QDateTime final_dt;
    QDateTime initial_dt;

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
