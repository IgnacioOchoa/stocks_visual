#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Prepare all the information

    //Connect to the API and get stock names
    populateStockNames();
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

void MainWindow::populateStockNames()
{
    QNetworkReply* reply = manager.get(QNetworkRequest(QUrl("https://finnhub.io/api/v1/stock/symbol?exchange=US&token=bubf32748v6ouqkj0ffg")));
    connect(reply, &QNetworkReply::readyRead, this, &MainWindow::readyRead);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::replyFinished);
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

    if (jsonArr[0].isObject()) qInfo() << "jsonArr[0] is an object";
    qInfo() << jsonArr[0].toObject().keys();

    //QJsonValue jsonObj = jsonArr[3];
    //qInfo() << jsonVal.toString();

}
