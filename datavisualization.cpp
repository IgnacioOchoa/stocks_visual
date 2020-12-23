#include "datavisualization.h"

DataVisualization::DataVisualization(QGraphicsView *UiGraphicsView, StockData* sData, QObject *parent) :
    QObject(parent),
    stockData(sData),
    graphicsView(UiGraphicsView)
{
    mainChart = new QChart();
    mainScene = new QGraphicsScene(this);

    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void DataVisualization::plotData()
// Once the information about the specific stock has been received and processed, this functions plots the data
{
    mainChart->removeAllSeries();
    foreach (QAbstractAxis* ax, mainChart->axes())
    {
        mainChart->removeAxis(ax);
    }
    QCandlestickSeries* candleSeries = new QCandlestickSeries();
    QLineSeries *lineSeries = new QLineSeries();

    candleSeries->setName("Stocks");
    candleSeries->setIncreasingColor(QColor(Qt::green));
    candleSeries->setDecreasingColor(QColor(Qt::red));

    for (int i=0; i<stockData->numPoints(); i++)
    {
        QCandlestickSet* cdlSet = new QCandlestickSet(stockData->currentStockOpenData()[i],
                                                      stockData->currentStockHighData()[i],
                                                      stockData->currentStockLowData()[i],
                                                      stockData->currentStockCloseData()[i],
                                                      stockData->currentStockTimeStamps()[i]);
        candleSeries->append(cdlSet);
        lineSeries->append(i+0.5,stockData->currentStockCloseData()[i]);
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
    graphicsView->setScene(mainScene);
    graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    mainChart->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    mainChart->resize(800,800);
    qInfo() << "mainScene->itemsBoundingRect() = " << mainScene->itemsBoundingRect();
    qInfo() << "GraphicsView->sceneRect() = " << graphicsView->sceneRect();
    //mainScene->addRect(-0.5, -0.5, 99.5, 147.5);
    //ui->GV_chartView->setSceneRect(QRectF(-50,-50,200,200));
    qInfo() << "GraphicsView->size() = " << graphicsView->size();
    mainChart->resize(mainScene->sceneRect().width(),mainScene->sceneRect().height());
    graphicsView->fitInView(mainScene->itemsBoundingRect(),Qt::KeepAspectRatio);
    graphicsView->show();

    //ui->GV_chartView->setChart(mainChart);
    //ui->GV_chartView->setRenderHint(QPainter::Antialiasing);
}

void DataVisualization::calculateYticks(QValueAxis* axisY)
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

void DataVisualization::calculateXticks()
{
    const QVector<long long>& timeData = stockData->currentStockTimeStamps();
    int days = timeData.size();
    mainAxisX->setMin(0);
    int numDays = timeData.length();
    if (numDays < 20) {
        for (int i=0; i<days; i++)
        {
            mainAxisX->append(QDateTime::fromSecsSinceEpoch(timeData[i]).toString("dd MMM"), i+1);
        }
        mainAxisX->setMax(days);
    }
    else
    {
        int prevDay = QDateTime::fromSecsSinceEpoch(timeData.first()).date().day();
        int thisDay;
        mainAxisX->setStartValue(0);

        for (int i=0; i<days; i++)
        {
            thisDay = QDateTime::fromSecsSinceEpoch(timeData[i]).date().day();
            qInfo() << "thisDay = " << thisDay;
            if (thisDay<prevDay) //change in month
            {
                qInfo() << "Change in month";
                mainAxisX->append(QDateTime::fromSecsSinceEpoch(timeData[i-1]).toString("MMM"), i);
            }
            prevDay = thisDay;
        }
        mainAxisX->append(QDateTime::fromSecsSinceEpoch(timeData.last()).toString("MMM"),days);

        mainAxisX->setMax(days);
    }
}

void DataVisualization::calculateWeekLines()
{
    const QVector<long long>& timeData = stockData->currentStockTimeStamps();
    int days = timeData.size();
    int prevWeek = QDateTime::fromSecsSinceEpoch(timeData.first()).date().weekNumber();
    int thisWeek;
    for(int i=0; i<days; i++)
    {
        thisWeek = QDateTime::fromSecsSinceEpoch(timeData[i]).date().weekNumber();
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
