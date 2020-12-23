#ifndef STOCKDATA_H
#define STOCKDATA_H

#include <QVector>
#include <QMessageBox>
#include "singlestock.h"
#include "networkinterface.h"

class StockData : public QObject
{
    Q_OBJECT
public:
    StockData(QWidget* parent);
    void getStockData(QString name, QDateTime initTime, QDateTime endTime);
    const QStringList& getSymbols() const {return symbols;}
    const QStringList& getTypes() const {return types;}
    const QStringList& getCurrencies() const {return currencies;}
    const QStringList& getDescriptions() const {return descriptions;}
    const QStringList& getDisplaySymbols() const {return displaySymbols;}

    const QVector<double>& currentStockOpenData() const {return sstock->getOpenData();}
    const QVector<double>& currentStockCloseData() const {return sstock->getCloseData();}
    const QVector<double>& currentStockHighData() const {return sstock->getHighData();}
    const QVector<double>& currentStockLowData() const {return sstock->getLowData();}
    const QVector<long long>& currentStockTimeStamps() const {return sstock->getTimestampData();}
    const QVector<long long>& currentStockVolume() const {return sstock->getVolumeData();}

    int numPoints() const {return sstock->getTimestampData().size();}

private:
    SingleStock* sstock;
    NetworkInterface* networkInterface;
    QStringList symbols;
    QStringList types;
    QStringList currencies;
    QStringList descriptions;
    QStringList displaySymbols;

signals:
    void stockInfoReady();
    void stockDataReady();

public slots:
    void stockInfoAvailable() {emit stockInfoReady();}
    void stockDataAvailable() {emit stockDataReady();}


};

#endif // STOCKDATA_H
