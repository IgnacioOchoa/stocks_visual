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
}

void MainWindow::replyFinished(QNetworkReply *)
{

}

void MainWindow::readyRead()
{
    qInfo() << "Reached ready read";
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
    if(reply) qInfo() << reply->readAll();
}
