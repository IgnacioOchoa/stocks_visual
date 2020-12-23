#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include "singlestock.h"

class NetworkInterface : public QObject
{
    Q_OBJECT
public:
    explicit NetworkInterface(QWidget *parent,
                              QStringList& symbols,
                              QStringList& types,
                              QStringList& currencies,
                              QStringList& descriptions,
                              QStringList& displaysymbols);
    void getStockInfo();
    void getStockData(SingleStock* sstock);

signals:
    void stockInfoReady();
    void stockDataReady();

private slots:
    void InfoReadyRead();
    void InfoReplyFinished();
    void DataReadyRead();
    void DataReplyFinished();

private:
    QWidget* parent;
    QNetworkAccessManager* manager;
    SingleStock* sstock;
    QStringList stockNames;
    QByteArray binaryInfoReply;
    QByteArray binaryDataReply;
    QJsonDocument jdocInfo;
    QJsonDocument jdocData;

    QStringList& n_symbols;
    QStringList& n_types;
    QStringList& n_currencies;
    QStringList& n_descriptions;
    QStringList& n_displaysymbols;
};

#endif // NETWORKINTERFACE_H
