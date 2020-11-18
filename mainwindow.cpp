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
    connect(ui->CB_stockName,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&MainWindow::stockCBchanged);
    connect(ui->PB_plot, &QPushButton::pressed, this, &MainWindow::plotPressed);
    ui->LE_currency->setReadOnly(true);
    ui->LE_symbol->setReadOnly(true);
    ui->LE_type->setReadOnly(true);
    ui->LE_description->setReadOnly(true);
    ui->LE_displaysymbol->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setDates()
{
    ui->CB_final_date->setDate(QDate::currentDate());
    ui->CB_initial_date->setDate(QDate::currentDate().addMonths(-3));

    ui->CB_final_date->setDisplayFormat("dd.MM.yyyy");
    ui->CB_initial_date->setDisplayFormat("dd.MM.yyyy");
}

void MainWindow:: getStocksInfo()
{
    QNetworkReply* reply = manager.get(QNetworkRequest(QUrl("https://finnhub.io/api/v1/stock/symbol?exchange=US&token=bubf32748v6ouqkj0ffg")));
    connect(reply, &QNetworkReply::readyRead, this, &MainWindow::InfoReadyRead);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::InfoReplyFinished);
}


void MainWindow::plotPressed()
{
    getStockData();
}

void MainWindow::getStockData()
{

    QDateTime final_dt = ui->CB_final_date->date().endOfDay();
    qint64 final_date = final_dt.toSecsSinceEpoch();

    QDateTime initial_dt = ui->CB_initial_date->date().startOfDay();
    qint64 initial_date =initial_dt.toSecsSinceEpoch();

    QString strUrl = "https://finnhub.io/api/v1/stock/candle?symbol=AAPL&resolution=D&from=" + QString::number(initial_date)
            + "&to=" + QString::number(final_date) + "&token=bubf32748v6ouqkj0ffg";
    QUrl url(strUrl);
    QNetworkReply* reply = manager.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::readyRead, this, &MainWindow::DataReadyRead);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::DataReplyFinished);
}

void MainWindow::populateCBStockInfo()
{
    ui->CB_stockName->addItems(displaysymbols);
}

void MainWindow::plotData()
{
    QCandlestickSeries* series = new QCandlestickSeries();

    series->setName("Stocks");
    series->setIncreasingColor(QColor(Qt::green));
    series->setDecreasingColor(QColor(Qt::red));

    QStringList categories;

    for (int i=0; i<numPlotPoints; i++)
    {
        QCandlestickSet* cdlSet = new QCandlestickSet(o_data[i], h_data[i], l_data[i], c_data[i], t_data[i]);
        series->append(cdlSet);
        categories << QDateTime::fromSecsSinceEpoch(cdlSet->timestamp()).toString("dd.MM");
    }

    qInfo() << categories;

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Stock series from this date to this date");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->createDefaultAxes();

    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(chart->axes(Qt::Horizontal).at(0));
    axisX->setCategories(categories);

    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    axisY->setMax(axisY->max() * 1.01);
    axisY->setMin(axisY->min() * 0.99);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->GV_chartView->setChart(chart);
    ui->GV_chartView->setRenderHint(QPainter::Antialiasing);
    qInfo() << "Reached the end of plot";
}

void MainWindow::DataReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
    if(reply)
    {
        binaryDataReply += reply->readAll();
    }
}

void MainWindow::DataReplyFinished()
{
    qInfo() << "replyData Finished is executed";
    jdocData = QJsonDocument::fromJson(binaryDataReply);
    binaryDataReply.clear();

    if (jdocData.isNull()) qInfo() << "The jdoc Data is null";

    QJsonObject obj = jdocData.object();
    if(obj.isEmpty()) qInfo() << "El objeto salio mal";

    qInfo() << obj.keys();

    QVariantMap vm = obj.toVariantMap();
    QVariantList vl_c = vm["c"].toList();
    QVariantList vl_h = vm["h"].toList();
    QVariantList vl_l = vm["l"].toList();
    QVariantList vl_o = vm["o"].toList();
    QVariantList vl_t = vm["t"].toList();
    QVariantList vl_v = vm["v"].toList();

    c_data.clear();
    h_data.clear();
    l_data.clear();
    o_data.clear();
    t_data.clear();
    v_data.clear();

    numPlotPoints = vl_t.size();
    qInfo() << "numPlotPoints = " << numPlotPoints;

    for(int i=0; i<numPlotPoints; i++)
    {
        c_data.append(vl_c[i].toDouble());
        h_data.append(vl_h[i].toDouble());
        l_data.append(vl_l[i].toDouble());
        o_data.append(vl_o[i].toDouble());
        t_data.append(vl_t[i].toLongLong());
        v_data.append(vl_v[i].toLongLong());
    }

    qInfo() << "h_data" << h_data;
    qInfo() << "Time stamps:";
    for(int i=0; i<t_data.size(); i++)
    {
        QDateTime dateTime;
        dateTime.setSecsSinceEpoch(t_data[i]);
        qInfo() << dateTime.toString();
    }

    plotData();
}

void MainWindow::InfoReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
    if(reply)
    {
        binaryInfoReply += reply->readAll();
    }
}

void MainWindow::InfoReplyFinished()
{
    qInfo()<<"replyInfo Finished is executed";

    jdocInfo = QJsonDocument::fromJson(binaryInfoReply);
    binaryInfoReply.clear();
    QJsonArray jsonArr = jdocInfo.array();

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

    populateCBStockInfo();

}

void MainWindow::stockCBchanged(int indx)
{
    ui->LE_currency->setText(currencies[indx]);
    ui->LE_symbol->setText(symbols[indx]);
    ui->LE_type->setText(types[indx]);
    ui->LE_description->setText(descriptions[indx]);
    ui->LE_displaysymbol->setText(displaysymbols[indx]);
}

