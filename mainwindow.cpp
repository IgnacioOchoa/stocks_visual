#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Prepare all the information

    //Connect to the API and get stock names
    getStocksInfo();
    //Set ending date to current date
    setDates();
    //Set starting date to 3 months before the current date

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDates()
{
    ui->cb_final_date->setDate(QDate::currentDate());
    ui->cb_initial_date->setDate(QDate::currentDate().addMonths(-3));
}

void MainWindow:: getStocksInfo()
{
    QNetworkReply* reply = manager.get(QNetworkRequest(QUrl("https://finnhub.io/api/v1/stock/symbol?exchange=US&token=bubf32748v6ouqkj0ffg")));
    connect(reply, &QNetworkReply::readyRead, this, &MainWindow::readyRead);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::replyFinished);
}

void MainWindow::populateStockInfo()
{
    ui->stockNameCB->addItems(displaysymbols);
}

void MainWindow::readyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
    if(reply)
    {
        binaryReply += reply->readAll();
    }
}

void MainWindow::replyFinished()
{
    qInfo()<<"replyFinished is executed";

    jdoc = QJsonDocument::fromJson(binaryReply);
    if(jdoc.isArray()) qInfo() << "The document is an array";
    else if(jdoc.isObject()) qInfo() << "The document is an object";
    else if (jdoc.isNull()) qInfo() << "The document is null";

    QJsonArray jsonArr = jdoc.array();
    qInfo() << "Length of array = " <<  jsonArr.size();

    //Currencies:  ("USD", "")
    //Types:  ("EQS", "", "ETF", "DR", "UNT", "STP", "WAR", "PRF", "BND", "TRT", "SP", "PFS")
    //Keys: ("currency", "description", "displaySymbol", "symbol", "type")

    for (int i=0; i<jsonArr.size(); i++)
    {
        QJsonObject obj = jsonArr[i].toObject();
        symbols.append(obj["symbol"].toString());
        types.append(obj["type"].toString());
        currencies.append(obj["currency"].toString());
        descriptions.append(obj["description"].toString());
        displaysymbols.append(obj["displaySymbol"].toString());
    }

    populateStockInfo();

}
