#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statLabel = new QLabel();
    statBar = this->statusBar();
    statBar->addWidget(statLabel);
    getStocksInfo();
    setDates();

    connect(ui->CB_stockName,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&MainWindow::stockCBchanged);
    connect(ui->PB_plot, &QPushButton::pressed, this, &MainWindow::plotPressed);

    mainChart = new QChart();
    mainScene = new QGraphicsScene(this);

    ui->LE_currency->setReadOnly(true);
    ui->LE_symbol->setReadOnly(true);
    ui->LE_type->setReadOnly(true);
    ui->LE_description->setReadOnly(true);
    ui->LE_displaysymbol->setReadOnly(true);

    ui->GV_chartView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->GV_chartView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //connect(ui->GV_chartView->resize()
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
    initial_dt = ui->CB_initial_date->date().startOfDay();
    final_dt = ui->CB_final_date->date().endOfDay();
    numDays = initial_dt.daysTo(final_dt) + 1;
    if (numDays < 1){
        QMessageBox::warning(this,"Problem with dates", "The final date has to be later than the initial date");
        return;
    }
    getStockData();
}

void MainWindow::getStockData()
// Performs the dates calculation to set the dates for the url request. Also calculates the expected number of data points.
// Constructs the complete url request and connects to API
{
    qint64 final_date = final_dt.toSecsSinceEpoch();
    qint64 initial_date = initial_dt.toSecsSinceEpoch();

    searchSymbol = ui->CB_stockName->currentText();

    QString strUrl = "https://finnhub.io/api/v1/stock/candle?symbol=" + searchSymbol +"&resolution=D&from=" + QString::number(initial_date)
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

void MainWindow::reportDataDays()
{
    QDateTime moment;
    foreach (long long x, t_data)
    {
        moment = QDateTime::fromSecsSinceEpoch(x);
        ui->LW_daysList->addItem(moment.toString("ddd \tdd \tMMM"));
    }
}

void MainWindow::plotData()
// Once the information about the specific stock has been received and processed, this functions plots the data
{
    QCandlestickSeries* candleSeries = new QCandlestickSeries();
    QLineSeries *lineSeries = new QLineSeries();

    candleSeries->setName("Stocks");
    candleSeries->setIncreasingColor(QColor(Qt::green));
    candleSeries->setDecreasingColor(QColor(Qt::red));

    for (int i=0; i<numPlotPoints; i++)
    {
        QCandlestickSet* cdlSet = new QCandlestickSet(o_data[i], h_data[i], l_data[i], c_data[i], t_data[i],this);
        candleSeries->append(cdlSet);
        lineSeries->append(i+0.5,c_data[i]);
    }

    QList<QCandlestickSet*> list = candleSeries->sets();

    QString initialDate = QDateTime::fromSecsSinceEpoch(list[0]->timestamp()).toString("ddd dd MMM");
    QString finalDate = QDateTime::fromSecsSinceEpoch(list[list.size()-1]->timestamp()).toString("ddd dd MMM");

    //Creation of QChart

    mainChart->setTitle("Stock series from " + initialDate + " to " + finalDate);
    mainChart->setAnimationOptions(QChart::SeriesAnimations);

    //when you create default axis, it makes the connections between the axis and the already added series
    //candleSeries will use the default axis, but lineSeries will not
    //chart->createDefaultAxes();

    //Creation of QAxes

    barCatAxisX = new QBarCategoryAxis;
    mainChart->addAxis(barCatAxisX, Qt::AlignBottom);
    barCatAxisX->setVisible(false);

    mainAxisX = new QCategoryAxis;
    weekAxis = new QCategoryAxis;
    calculateXticks();
    calculateWeekLines();
    mainChart->addAxis(mainAxisX, Qt::AlignBottom);
    mainChart->addAxis(weekAxis, Qt::AlignBottom);

    //Axis X for Line Series

    //QValueAxis * xValAx = new QValueAxis();
    //xValAx->setMin(0);
    //xValAx->setMax(numPlotPoints);
    //chart->addAxis(xValAx, Qt::AlignBottom);
    //xValAx->setVisible(false);

    //Axis Y

    QValueAxis *axisY = new QValueAxis;

    mainChart->addAxis(axisY, Qt::AlignLeft);

    //The lineSeries has to be added to the chart before axis can be attached to the series
    mainChart->addSeries(candleSeries);
    mainChart->addSeries(lineSeries);

    candleSeries->attachAxis(barCatAxisX);
    candleSeries->attachAxis(axisY);

    lineSeries->setColor("black");
    lineSeries->attachAxis(mainAxisX);
    lineSeries->setPointsVisible(true);
    lineSeries->attachAxis(axisY);

    calculateYticks(axisY);

    mainChart->legend()->setVisible(false);

    //chart->legend()->setAlignment(Qt::AlignRight);

    mainScene->addItem(mainChart);
    //mainChart->resetTransform();
    ui->GV_chartView->setScene(mainScene);
    ui->GV_chartView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    mainChart->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    mainChart->resize(800,800);
    qInfo() << "mainScene->itemsBoundingRect() = " << mainScene->itemsBoundingRect();
    qInfo() << "GraphicsView->sceneRect() = " << ui->GV_chartView->sceneRect();
    //mainScene->addRect(-0.5, -0.5, 99.5, 147.5);
    //ui->GV_chartView->setSceneRect(QRectF(-50,-50,200,200));
    qInfo() << "GraphicsView->size() = " << ui->GV_chartView->size();
    mainChart->resize(mainScene->sceneRect().width(),mainScene->sceneRect().height());
    ui->GV_chartView->fitInView(mainScene->itemsBoundingRect(),Qt::KeepAspectRatio);
    ui->GV_chartView->show();

    //ui->GV_chartView->setChart(mainChart);
    //ui->GV_chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::calculateYticks(QValueAxis* axisY)
{
    int max = ceil(axisY->max()*1.01);
    int min = floor(axisY->min()*0.99);
    int numtks = max-min+1;
    qInfo() << "max = " << max;
    qInfo() << "min = " << min;

    while (numtks > 15)
    {
        qInfo() << "numtks = " << numtks;
        if (numtks%2 == 1) numtks = (numtks-1)/2 + 1;
        else {
            numtks++;
            max++;
        }
    }

    axisY->setMax(max);
    axisY->setMin(min);
    axisY->setTickCount(numtks);
}

void MainWindow::calculateXticks()
{
    int days = t_data.size();
    mainAxisX->setMin(0);
    qInfo() << "numDays = " << numDays;
    if (numDays < 25) {
        for (int i=0; i<days; i++)
        {
            mainAxisX->append(QDateTime::fromSecsSinceEpoch(t_data[i]).toString("dd MMM"), i+1);
        }
        mainAxisX->setMax(days);
    }
    else
    {
        int prevDay = QDateTime::fromSecsSinceEpoch(t_data.first()).date().day();
        int thisDay;
        mainAxisX->setStartValue(0);

        for (int i=0; i<days; i++)
        {
            thisDay = QDateTime::fromSecsSinceEpoch(t_data[i]).date().day();
            qInfo() << "thisDay = " << thisDay;
            if (thisDay<prevDay) //change in month
            {
                qInfo() << "Change in month";
                mainAxisX->append(QDateTime::fromSecsSinceEpoch(t_data[i-1]).toString("MMM"), i);
            }
            prevDay = thisDay;
        }
        mainAxisX->append(QDateTime::fromSecsSinceEpoch(t_data.last()).toString("MMM"),days);

        mainAxisX->setMax(days);
    }
}

void MainWindow::calculateWeekLines()
{
    int days = t_data.size();
    int prevWeek = QDateTime::fromSecsSinceEpoch(t_data.first()).date().weekNumber();
    int thisWeek;
    for(int i=0; i<days; i++)
    {
        thisWeek = QDateTime::fromSecsSinceEpoch(t_data[i]).date().weekNumber();
        if (thisWeek>prevWeek)
        {
            weekAxis->append(QString::number(thisWeek),i);
            prevWeek = thisWeek;
        }
    }
    weekAxis->append(QString::number(thisWeek),days);
    weekAxis->setMax(days);
    weekAxis->setLabelsVisible(false);
    weekAxis->setLineVisible(false);
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

    statLabel->setText("Data from " + searchSymbol + " loaded correctly");

    reportDataDays();
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
    if (jdocInfo.isNull())
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
    ui->PB_plot->setEnabled(true);
    ui->CB_stockName->setEnabled(true);

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

void MainWindow::on_PB_zoomIn_clicked()
{
    ui->GV_chartView->scale(1.2,1.2);
}

void MainWindow::on_PB_zoomOut_clicked()
{
    ui->GV_chartView->scale(0.8,0.8);
}
