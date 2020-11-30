#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    getStocksInfo();
    setDates();

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
// Populates the dates combo boxes and sets a proper date format
{
    ui->CB_final_date->setDate(QDate::currentDate());
    ui->CB_initial_date->setDate(QDate::currentDate().addMonths(-1));

    ui->CB_final_date->setDisplayFormat("dd.MM.yyyy");
    ui->CB_initial_date->setDisplayFormat("dd.MM.yyyy");
}

void MainWindow:: getStocksInfo()
// Prepare the connections for getting the list of all available stocks, this is done automatically upon startup
{
    QNetworkReply* reply = manager.get(QNetworkRequest(QUrl("https://finnhub.io/api/v1/stock/symbol?exchange=US&token=bubf32748v6ouqkj0ffg")));
    connect(reply, &QNetworkReply::readyRead, this, &MainWindow::InfoReadyRead);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::InfoReplyFinished);
}


void MainWindow::plotPressed()
// Slot for catching the press of the Plot button
{
    getStockData();
}

void MainWindow::getStockData()
// Performs the dates calculation to set the dates for the url request. Also calculates the expected number of data points.
// Constructs the complete url request and connects to API
{
    QDateTime final_dt = ui->CB_final_date->date().endOfDay();
    qint64 final_date = final_dt.toSecsSinceEpoch();

    QDateTime initial_dt = ui->CB_initial_date->date().startOfDay();
    qint64 initial_date = initial_dt.toSecsSinceEpoch();

    QDate loopDay = initial_dt.date();
    expectedDataPoints = 0;

    //Sums up all the working days, leaving out saturday and sunday
    for (int i=0; i<initial_dt.daysTo(final_dt); i++)
    {
        if (loopDay.addDays(i).dayOfWeek() != 6 && loopDay.addDays(i).dayOfWeek() != 7) expectedDataPoints++;
    }
    QString symbol = ui->CB_stockName->currentText();

    QString strUrl = "https://finnhub.io/api/v1/stock/candle?symbol=" + symbol +"&resolution=D&from=" + QString::number(initial_date)
            + "&to=" + QString::number(final_date) + "&token=bubf32748v6ouqkj0ffg";
    QUrl url(strUrl);
    QNetworkReply* reply = manager.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::readyRead, this, &MainWindow::DataReadyRead);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::DataReplyFinished);
}

void MainWindow::populateCBStockInfo()
//  Once the general info about all stocks is received and processed, it populates the stocks combo box
//  and sets the default one
{
    ui->CB_stockName->addItems(displaysymbols);
    ui->CB_stockName->setCurrentIndex(ui->CB_stockName->findText("AAPL"));
}

void MainWindow::plotData()
// Once the information about the specific stock has been received and processed, this functions plots the data
{
    QCandlestickSeries* series = new QCandlestickSeries();
    QLineSeries *lineSeries = new QLineSeries();

    series->setName("Stocks");
    series->setIncreasingColor(QColor(Qt::green));
    series->setDecreasingColor(QColor(Qt::red));

    QStringList categories;

    for (int i=0; i<numPlotPoints; i++)
    {
        QCandlestickSet* cdlSet = new QCandlestickSet(o_data[i], h_data[i], l_data[i], c_data[i], t_data[i],this);
        series->append(cdlSet);
        QDateTime moment = QDateTime::fromSecsSinceEpoch(cdlSet->timestamp());
        categories << moment.toString("dd.MM");
        lineSeries->append(moment.toMSecsSinceEpoch(),(o_data[i]+c_data[i])/2.0);
    }

    QList<QCandlestickSet*> list = series->sets();

    QString initialDate = QDateTime::fromSecsSinceEpoch(list[0]->timestamp()).toString("dd.MM");
    QString finalDate = QDateTime::fromSecsSinceEpoch(list[list.size()-1]->timestamp()).toString("dd.MM");

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Stock series from " + initialDate + " to " + finalDate);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chart->createDefaultAxes();

    QBarCategoryAxis *axisX = qobject_cast<QBarCategoryAxis *>(chart->axes(Qt::Horizontal).at(0));
    axisX->setCategories(categories);
    axisX->setVisible(false);

    chart->addSeries(lineSeries);

    QDateTimeAxis *DTaxisX = new QDateTimeAxis;
    DTaxisX->setTickCount(10);
    DTaxisX->setFormat("dd MM");
    DTaxisX->setTitleText("Date");
    chart->addAxis(DTaxisX, Qt::AlignBottom);
    lineSeries->attachAxis(DTaxisX);


    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).at(0));
    axisY->setMax(axisY->max() * 1.01);
    axisY->setMin(axisY->min() * 0.99);

    //lineSeries->hide();

    chart->legend()->setVisible(false);
    //chart->legend()->setAlignment(Qt::AlignRight);

    ui->GV_chartView->setChart(chart);
    ui->GV_chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::DataReadyRead()
// Slot called when a new chunk of data for a particular stock is received.
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
    if(reply)
    {
        binaryDataReply += reply->readAll();
    }
}

void MainWindow::DataReplyFinished()
// Slot called when the stock data is completely received. Performs some consistency checks.
{
    jdocData = QJsonDocument::fromJson(binaryDataReply);
    binaryDataReply.clear();  // If another stock data is requested we need the buffer clean

    if (jdocData.isNull())
    {
        QMessageBox::warning(this,"Problem with data", "Could not transform the provided"
                             " stock data to a Json document");
        return;
    }

    QJsonObject obj = jdocData.object();
    if (obj.isEmpty())
    {
        QJsonArray arr = jdocData.array();
        if (!arr.isEmpty())
        {
            QMessageBox::warning(this,"Problem with data", "Could not transform the provided"
                             " stock Json document into a Json Object because it is a Json Array");
            return;
        }
        else
        {
            QMessageBox::warning(this,"Problem with data", "Could not transform the provided"
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

    c_data.clear();
    h_data.clear();
    l_data.clear();
    o_data.clear();
    t_data.clear();
    v_data.clear();

    numPlotPoints = vl_t.size();
    if (numPlotPoints != expectedDataPoints)
    {
        QMessageBox::warning(this,"Problem with data", "Could not retrieve"
        " the correct amount of data points: Expected " + QString::number(expectedDataPoints) +
        " points, but got " + QString::number(numPlotPoints) + " points");
        return;
    }

    // Populate internal data containers
    for(int i=0; i<numPlotPoints; i++)
    {
        c_data.append(vl_c[i].toDouble());
        h_data.append(vl_h[i].toDouble());
        l_data.append(vl_l[i].toDouble());
        o_data.append(vl_o[i].toDouble());
        t_data.append(vl_t[i].toLongLong());
        v_data.append(vl_v[i].toLongLong());
    }

    plotData();
}

void MainWindow::InfoReadyRead()
// Slot called when a new chunk of general info data is received
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
    if(reply)
    {
        binaryInfoReply += reply->readAll();
    }
}

void MainWindow::InfoReplyFinished()
// Slot called when the info data is completely received. Performs some consistency checks.
// After processing updates the stocks combo box
//    Currencies:  ("USD", "")
//    Types:  ("EQS", "", "ETF", "DR", "UNT", "STP", "WAR", "PRF", "BND", "TRT", "SP", "PFS")
//    Keys: ("currency", "description", "displaySymbol", "symbol", "type")
{
    jdocInfo = QJsonDocument::fromJson(binaryInfoReply);
    if (jdocData.isNull())
    {
        QMessageBox::warning(this,"Problem with data", "Could not transform the provided"
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
            QMessageBox::warning(this,"Problem with data", "Could not transform the provided"
                             " general info data Json document into a Json Array because it is a Json Object");
            return;
        }
        else
        {
            QMessageBox::warning(this,"Problem with data", "Could not transform the provided"
                             " stock Json document into a Json Array and it is not an object either");
            return;
        }
    }

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
// Slot called when another stock in the combo box is selected. It fills up the info tab.
{
    ui->LE_currency->setText(currencies[indx]);
    ui->LE_symbol->setText(symbols[indx]);
    ui->LE_type->setText(types[indx]);
    ui->LE_description->setText(descriptions[indx]);
    ui->LE_displaysymbol->setText(displaysymbols[indx]);
}

