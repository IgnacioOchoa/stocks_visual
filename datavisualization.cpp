#include "datavisualization.h"

DataVisualization::DataVisualization(QGraphicsView *UiGraphicsView, StockData* sData, QObject *parent) :
    QObject(parent),
    stockData(sData),
    graphicsView(UiGraphicsView)
{
    mainScene = new QGraphicsScene(parent);
    mainChart = new QChart();

    graphicsView->installEventFilter(this);
    mainScene->installEventFilter(this);
    mainChart->installEventFilter(this);

    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    axisY = nullptr;
    candleSeries = nullptr;

    connect(mainChart, &QChart::plotAreaChanged, this, &DataVisualization::chartPlotAreaChanged);

}

void DataVisualization::plotData()
// Once the information about the specific stock has been received and processed, this functions plots the data
{
    zoomLevel = 1;
    mainChart->removeAllSeries();
    foreach (QAbstractAxis* ax, mainChart->axes())
    {
        mainChart->removeAxis(ax);
    }
    candleSeries = new QCandlestickSeries();
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
    //mainChart->setAnimationOptions(QChart::SeriesAnimations);

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
    //mainChart->addAxis(weekAxis, Qt::AlignBottom);

    //Axis X for Line Series

    //QValueAxis * xValAx = new QValueAxis();
    //xValAx->setMin(0);
    //xValAx->setMax(numPlotPoints);
    //chart->addAxis(xValAx, Qt::AlignBottom);
    //xValAx->setVisible(false);

    //Axis Y

    axisY = new QValueAxis;

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

    calculateYticks();

    mainChart->legend()->setVisible(false);

    //chart->legend()->setAlignment(Qt::AlignRight);

    mainScene->addItem(mainChart);
    //mainChart->resetTransform();
    graphicsView->setScene(mainScene);
    graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    //mainChart->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    //mainChart->resize(800,800);
    //graphicsView->fitInView(mainScene->itemsBoundingRect(),Qt::KeepAspectRatio);
    masterRect = graphicsView->sceneRect();
    graphicsView->show();

    createElements();

}

void DataVisualization::calculateYticks()
{
    axisYmax = ceil(axisY->max()*1.0);
    axisYmin = floor(axisY->min()*1.0);
    int numtks = axisYmax-axisYmin+1;


    while (numtks > 15)
    {
        if (numtks%2 == 1) numtks = (numtks-1)/2 + 1;
        else {
            numtks++;
            axisYmax++;
        }
    }

    axisY->setMax(axisYmax);
    axisY->setMin(axisYmin);
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
            if (thisDay<prevDay) //change in month
            {
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

void DataVisualization::createElements()
{
    QPointF posInChart = QPointF(8,130);
    QPointF pos = mainChart->mapToPosition(posInChart, candleSeries);
    QGraphicsItem* itm = mainScene->addEllipse(-2.5,-2.5,5,5);
    itm->setPos(pos);
    itm->setData(0,posInChart);
    drawnElements.append(itm);
}

void DataVisualization::drawElements()
{
    if (!drawnElements.isEmpty())
    {
       foreach(QGraphicsItem* itm, drawnElements)
       {
           QPointF newPos = mainChart->mapToPosition(qvariant_cast<QPointF>(itm->data(0)),candleSeries);
           itm->setPos(newPos);
       }
    }
}

bool DataVisualization::eventFilter(QObject *watched, QEvent *event)
{

    if (watched == graphicsView && event->type() == QEvent::Resize)
    {
        QResizeEvent* re = static_cast<QResizeEvent*>(event);
        if(re)
        {

            qInfo() << "*****Resize event:\nOld size = " << re->oldSize() << "\nNew size = " << re->size();
            mainChart->resize(re->size());
            masterRect = QRectF(QPointF(0,0),re->size());
            mainScene->setSceneRect(masterRect);
            qInfo() << "Scene rect: " << mainScene->sceneRect();
            qInfo() << "Items bounding rect: " << mainScene->itemsBoundingRect();
            qInfo() << "Graphics view rect: " << graphicsView->sceneRect();
            return false;
        }
    }
    else if(watched == mainChart && event->type()==QEvent::GraphicsSceneWheel)
    {
        return true;
    }
    else if(watched == graphicsView && event->type()==QEvent::Wheel) return true;
    else if(watched == mainScene && event->type()==QEvent::GraphicsSceneWheel)
    {
        QGraphicsSceneWheelEvent* wEvent = static_cast<QGraphicsSceneWheelEvent*>(event);
        if(wEvent)
        {
            if (wEvent->delta() > 0)
            {
                if (maxZoom/zoomLevel < 1.2)
                {
                    mainChart->zoom(maxZoom/zoomLevel);
                    zoomLevel = maxZoom;
                }
                else
                {
                    mainChart->zoom(1.2);
                    zoomLevel *= 1.2;
                }
                axisY->setMax(axisYmax);
                axisY->setMin(axisYmin);
                //qInfo() << "zoomLevel = " << zoomLevel;
            }
            else if (wEvent->delta() < 0)
            {
                if (minZoom/zoomLevel > 1/1.2)
                {
                    mainChart->zoom(minZoom/zoomLevel);
                    zoomLevel = minZoom;
                }
                else
                {
                    mainChart->zoom(1/1.2);
                    zoomLevel *= 1/1.2;
                }
                //qInfo() << "zoomLevel = " << zoomLevel;
                axisY->setMax(axisYmax);
                axisY->setMin(axisYmin);
            }
            mainScene->setSceneRect(masterRect);
            drawElements();
            //qInfo() << "Graphics Scence Item bounding rect = " << mainScene->itemsBoundingRect();
            //qInfo() << "Graphics Scence rect = " << mainScene->sceneRect();
            //qInfo() << "Chart rect = " << mainChart->rect();
            //qInfo() << "Graphics view Scen rect = " << graphicsView->sceneRect() << "\n";
        }
        return true;
    }
    else if (watched == graphicsView && event->type()==QEvent::MouseButtonPress)
    {

        QMouseEvent* mouseEv = static_cast<QMouseEvent*>(event);
        if(mouseEv)
        {
            if (mouseEv->button()==Qt::LeftButton)
            {
                QPointF scenePoint = graphicsView->mapToScene(mouseEv->pos());
                QPointF chartPoint = mainChart->mapFromScene(scenePoint);
                QPointF seriesPoint = mainChart->mapToValue(chartPoint, candleSeries);

                QGraphicsItem* itm = mainScene->addEllipse(-2.5,-2.5,5,5);
                itm->setPos(chartPoint);
                itm->setData(0,seriesPoint);
                drawnElements.append(itm);

                qInfo() << "Left button clicked on scene: " << seriesPoint;
            }
        }
    }

    //else if (watched == mainScene && event->type()==QEvent::MetaCall) return false;
    //else if (watched == graphicsView && event->type()==QEvent::MetaCall) return false;
    //else if (watched == mainScene && event->type()==QEvent::GraphicsSceneMouseMove) return false;
    //qInfo() << "Object: " << watched << "  Event: " << event;
    return false;
}

void DataVisualization::chartPlotAreaChanged(const QRectF &plotArea)
{
    drawElements();
    if(axisY)
    {
        qInfo() << "axis Y max = " << axisY->max();
        qInfo() << "axis Y min = " << axisY->min();
        //axisY->setMax(axisYmax);
        //axisY->setMin(axisYmin);
    }
    //qInfo() << "chart plot area changed";
}
