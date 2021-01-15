#include "stockdata.h"

StockData::StockData(QWidget* parent) : QObject(parent)
{
    networkInterface = new NetworkInterface(parent, symbols, types, currencies, descriptions, displaySymbols);
    connect(networkInterface, &NetworkInterface::stockInfoReady, this, &StockData::stockInfoAvailable);
    connect(networkInterface, &NetworkInterface::stockDataReady, this, &StockData::stockDataAvailable);
    networkInterface->getStockInfo();
}

void StockData::getStockData(QString name, QDateTime initTime, QDateTime endTime)
{
    sstock = new SingleStock(name, initTime, endTime);
    networkInterface->getStockData(sstock);
}

