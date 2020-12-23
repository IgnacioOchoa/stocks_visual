#include "networkinterface.h"

NetworkInterface::NetworkInterface(QWidget *par,
                                   QStringList& symbols,
                                   QStringList& types,
                                   QStringList& currencies,
                                   QStringList& descriptions,
                                   QStringList& displaysymbols) :
    QObject(par),
    parent(par),
    n_symbols(symbols),
    n_types(types) ,
    n_currencies(currencies),
    n_descriptions(descriptions),
    n_displaysymbols(displaysymbols)
{
    manager = new QNetworkAccessManager;
}

void NetworkInterface::getStockInfo()
// Prepare the connections for getting the list of all available stocks, this is done automatically upon startup
{
    binaryInfoReply.clear();
    binaryDataReply.clear();

    QNetworkReply* reply = manager->get(QNetworkRequest(QUrl("https://finnhub.io/api/v1/stock/symbol?exchange=US&token=bubf32748v6ouqkj0ffg")));
    connect(reply, &QNetworkReply::readyRead, this, &NetworkInterface::InfoReadyRead);
    connect(reply, &QNetworkReply::finished, this, &NetworkInterface::InfoReplyFinished);
}

void NetworkInterface::getStockData(SingleStock* singleStock)
// Performs the dates calculation to set the dates for the url request. Also calculates the expected number of data points.
// Constructs the complete url request and connects to API
{
    sstock = singleStock;
    qint64 final_date = singleStock->getEndTime().toSecsSinceEpoch();
    qint64 initial_date = singleStock->getInitTime().toSecsSinceEpoch();

    QString strUrl = "https://finnhub.io/api/v1/stock/candle?symbol=" + singleStock->getName() +"&resolution=D&from=" + QString::number(initial_date)
            + "&to=" + QString::number(final_date) + "&token=bubf32748v6ouqkj0ffg";
    QUrl url(strUrl);
    QNetworkReply* reply = manager->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::readyRead, this, &NetworkInterface::DataReadyRead);
    connect(reply, &QNetworkReply::finished, this, &NetworkInterface::DataReplyFinished);
}

void NetworkInterface::DataReadyRead()
// Slot called when a new chunk of data for a particular stock is received.
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
    if(reply)
    {
        binaryDataReply += reply->readAll();
    }
}

void NetworkInterface::DataReplyFinished()
// Slot called when the stock data is completely received. Performs some consistency checks.
{
    jdocData = QJsonDocument::fromJson(binaryDataReply);
    binaryDataReply.clear();  // If another stock data is requested we need the buffer clean

    if (jdocData.isNull())
    {
        QMessageBox::warning(parent,"Problem with data", "Could not transform the provided"
                             " stock data to a Json document");
        return;
    }

    QJsonObject obj = jdocData.object();
    if (obj.isEmpty())
    {
        QJsonArray arr = jdocData.array();
        if (!arr.isEmpty())
        {
            QMessageBox::warning(parent,"Problem with data", "Could not transform the provided"
                             " stock Json document into a Json Object because it is a Json Array");
            return;
        }
        else
        {
            QMessageBox::warning(parent,"Problem with data", "Could not transform the provided"
                             " stock Json document into a Json Object and it is not an array either");
            return;
        }
    }

    // Prepare containers for receiving the Json data
    QVariantMap vm = obj.toVariantMap();
    QVariantList vl_c = vm["c"].toList();
    QVariantList vl_h = vm["h"].toList();
    QVariantList vl_l = vm["l"].toList();
    QVariantList vl_o = vm["o"].toList();
    QVariantList vl_t = vm["t"].toList();
    QVariantList vl_v = vm["v"].toList();

    int numPlotPoints = vl_t.size();

    // Populate internal data containers
    for(int i=0; i<numPlotPoints; i++)
    {
        sstock->appendCloseData(vl_c[i].toDouble());
        sstock->appendHighData(vl_h[i].toDouble());
        sstock->appendLowData(vl_l[i].toDouble());
        sstock->appendOpenData(vl_o[i].toDouble());
        sstock->appendTimestampData(vl_t[i].toLongLong());
        sstock->appendVolumeData(vl_v[i].toLongLong());
    }
    emit stockDataReady();
}

void NetworkInterface::InfoReadyRead()
// Slot called when a new chunk of general info data is received
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
    if(reply)
    {
        binaryInfoReply += reply->readAll();
    }
}

void NetworkInterface::InfoReplyFinished()
// Slot called when the info data is completely received. Performs some consistency checks.
// After processing updates the stocks combo box
//    Currencies:  ("USD", "")
//    Types:  ("EQS", "", "ETF", "DR", "UNT", "STP", "WAR", "PRF", "BND", "TRT", "SP", "PFS")
//    Keys: ("currency", "description", "displaySymbol", "symbol", "type")
{
    jdocInfo = QJsonDocument::fromJson(binaryInfoReply);
    if (jdocInfo.isNull())
    {
        QMessageBox::warning(parent,"Problem with data", "Could not transform the provided"
                             " general info data to a Json document");
        return;
    }
    binaryInfoReply.clear();  // If another info data is requested we need the buffer clean
    QJsonArray jsonArr = jdocInfo.array();
    if (jsonArr.isEmpty())
    {
        QJsonObject obj = jdocInfo.object();
        if (!obj.isEmpty())
        {
            QMessageBox::warning(parent,"Problem with data", "Could not transform the provided"
                             " general info data Json document into a Json Array because it is a Json Object");
            return;
        }
        else
        {
            QMessageBox::warning(parent,"Problem with data", "Could not transform the provided"
                             " stock Json document into a Json Array and it is not an object either");
            return;
        }
    }

    QMap<QString,QJsonObject> symbolMap;

    for (int i=0; i<jsonArr.size(); i++)
    {
        QJsonObject obj = jsonArr[i].toObject();
        n_displaysymbols.append(obj["displaySymbol"].toString());
        symbolMap.insert(n_displaysymbols[i],obj);
    }

    n_displaysymbols.sort();
    foreach(QString s, n_displaysymbols)
    {
        QJsonObject obj = symbolMap[s];
        n_symbols.append(obj["symbol"].toString());
        n_types.append(obj["type"].toString());
        n_currencies.append(obj["currency"].toString());
        n_descriptions.append(obj["description"].toString());
    }

    emit stockInfoReady();
}
