#include "datavisualization.h"

DataVisualization::DataVisualization(QGraphicsView *UiGraphicsView, StockData* sData, QObject *parent) :
    QObject(parent),
    stockData(sData),
    mainScene(new QGraphicsScene(parent)),
    graphicsView(UiGraphicsView),
    mainChart(new QChart())
{
    graphicsView->installEventFilter(this);
    mainScene->installEventFilter(this);
    mainChart->installEventFilter(this);

    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mainScene->addItem(mainChart);

    axisY = nullptr;
    candleSeries = nullptr;

    connect(mainChart, &QChart::plotAreaChanged, this, &DataVisualization::chartPlotAreaChanged);

    mouseJustPressed = false;
    mouseJustReleased = true;
    movingLine = nullptr;
    pointButton = nullptr;

    splineTempLines = new QList<QGraphicsLineItem*>;
    drawingPen = QPen(QBrush(Qt::black, Qt::SolidPattern), 2);
    transitoryPen = QPen(QBrush(Qt::black, Qt::SolidPattern), 1.5);

    drawingButtons = new QButtonGroup(graphicsView);
    createButtons();
}

void DataVisualization::plotData()
// Once the information about the specific stock has been received and processed, this functions plots the data
{
    clearScene();
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
    graphicsView->setScene(mainScene);
    graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    graphicsView->show();
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

void DataVisualization::drawElements()
{
    qInfo() << "drawElements started";
    if (!drawnElements.isEmpty())
    {
       foreach(QGraphicsItem* itm, drawnElements)
       {
           if(qgraphicsitem_cast<QGraphicsEllipseItem*>(itm))
           {
                QPointF newPos = mainChart->mapToPosition(qvariant_cast<QPointF>(itm->data(0)),candleSeries);
                itm->setPos(newPos);
           }
           else if(QGraphicsLineItem* linitm = qgraphicsitem_cast<QGraphicsLineItem*>(itm))
           {
               QPointF p1 = mainChart->mapToPosition(qvariant_cast<QPointF>(itm->data(0)),candleSeries);
               QPointF p2 = mainChart->mapToPosition(qvariant_cast<QPointF>(itm->data(1)),candleSeries);
               linitm->setLine(p1.x(),p1.y(),p2.x(),p2.y());
           }
           else if(QGraphicsPathItem* pathItm = qgraphicsitem_cast<QGraphicsPathItem*>(itm))
           {
               splinePath = new QPainterPath;
               QList<QVariant> lst = qvariant_cast<QList<QVariant>>(itm->data(0));
               QList<QVariant>::iterator itr = lst.begin();
               QPointF pt = mainChart->mapToPosition(qvariant_cast<QPointF>(*itr),candleSeries);
               splinePath->moveTo(pt);
               itr++;
               while(itr!=lst.end())
               {
                   pt = mainChart->mapToPosition(qvariant_cast<QPointF>(*itr),candleSeries);
                   splinePath->lineTo(pt);
                   itr++;
               }
               pathItm->setPath(*splinePath);
               delete splinePath;
           }
       }
    }
}

QPointF DataVisualization::scene2series(const QPointF& pt)
{
    QPointF chartPoint = mainChart->mapFromScene(pt);
    QPointF seriesPoint = mainChart->mapToValue(chartPoint, candleSeries);
    return seriesPoint;
}

void DataVisualization::logRects(const QString &origin)
{
    qInfo() << "*-*-*-* Rects called from : " << origin << " *-*-*-*";
    qInfo() << "mainChart->plotArea() = " << mainChart->plotArea();
    qInfo() << "mainChart->rect() = " << mainChart->rect();
    qInfo() << "mainScene->sceneRect() = " << mainScene->sceneRect();
    qInfo() << "mainScene->itemsBoundingRect() = " << mainScene->itemsBoundingRect();
    qInfo() << "graphicsView->sceneRect() = " << graphicsView->sceneRect();
}

void DataVisualization::createButtons()
{
    QIcon handIcon(QPixmap(":/iconImages/handIcon.png"));
    QIcon pointIcon(QPixmap(":/iconImages/pointIcon.png"));
    QIcon lineIcon(QPixmap(":/iconImages/lineIcon.png"));
    QIcon freeLineIcon(QPixmap(":/iconImages/freeLineIcon.png"));
    QPixmap paintBrushIcon(":/iconImages/paintBrush.png");

    handleButton = new QPushButton(handIcon,"",graphicsView);
    handleButton->setObjectName("handleButton");
    handleButton->resize(QSize(40,40));
    handleButton->setIconSize(QSize(40,40));
    handleButton->setCheckable(true);
    handleButton->setAutoExclusive(true);
    handleButton->setChecked(true);
    handleButtonPressed();
    handleButton->setCursor(Qt::PointingHandCursor);
    drawingButtons->addButton(handleButton);
    connect(handleButton, &QAbstractButton::pressed, this, &DataVisualization::handleButtonPressed);
    handleButton->show();

    pointButton = new QPushButton(pointIcon,"",graphicsView);
    pointButton->move(handleButton->width(),0);
    pointButton->setObjectName("pointButton");
    pointButton->resize(QSize(40,40));
    pointButton->setIconSize(QSize(40,40));
    pointButton->setCheckable(true);
    pointButton->setAutoExclusive(true);
    pointButton->setCursor(Qt::PointingHandCursor);
    drawingButtons->addButton(pointButton);
    connect(pointButton, &QAbstractButton::pressed, this, &DataVisualization::pointButtonPressed);
    pointButton->show();

    lineButton = new QPushButton(lineIcon,"",graphicsView);
    lineButton->setObjectName("lineButton");
    lineButton->resize(QSize(40,40));
    lineButton->setIconSize(QSize(40,40));
    lineButton->move(handleButton->width()+pointButton->width(),0);
    lineButton->setCheckable(true);
    lineButton->setCursor(Qt::PointingHandCursor);
    drawingButtons->addButton(lineButton);
    connect(lineButton, &QAbstractButton::pressed, this, &DataVisualization::lineButtonPressed);
    lineButton->show();

    splineButton = new QPushButton(freeLineIcon,"",graphicsView);
    splineButton->setObjectName("splineButton");
    splineButton->resize(QSize(40,40));
    splineButton->setIconSize(QSize(40,40));
    splineButton->move(handleButton->width()+pointButton->width()+lineButton->width(),0);
    splineButton->setCheckable(true);
    splineButton->setCursor(Qt::PointingHandCursor);
    drawingButtons->addButton(splineButton);
    connect(splineButton, &QAbstractButton::pressed, this, &DataVisualization::splineButtonPressed);
    splineButton->show();

    drawingButtons->setExclusive(true);

    penFrame = new QFrame(graphicsView);
    penFrame->setObjectName("penFrame");
    penFrame->setFrameStyle(QFrame::Box);

    brushLabel = new QLabel(penFrame);
    brushLabel->setPixmap(paintBrushIcon);
    brushLabel->setObjectName("brushLabel");
    brushLabel->resize(QSize(40,40));
    brushLabel->setScaledContents(true);

    //cbPenSize = new QComboBox(penFrame);
    penFrame->show();
    chartPlotAreaChanged(QRectF());


}

void DataVisualization::clearScene()
{
//    qInfo() << "Graphics Items:";
//    foreach(QGraphicsItem* itm, mainScene->items())
//    {
//        qInfo() << itm->type();
//    }
    //mainScene->clear();
}


bool DataVisualization::eventFilter(QObject *watched, QEvent *event)
{

    if (watched == graphicsView && event->type() == QEvent::Resize)
    {
        QResizeEvent* re = static_cast<QResizeEvent*>(event);
        if(re)
        {
            mainChart->resize(re->size());
            mainScene->setSceneRect(mainScene->itemsBoundingRect());
            //logRects("eventFilter: QEvent::Resize sent to graphicsView");
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
                axisY->setMax(axisYmax);
                axisY->setMin(axisYmin);
            }
            drawElements();
            //logRects("eventFilter: QEvent::GraphicsSceneWheel sent to mainScene");
        }
        return true;
    }
    else if (watched == mainScene && event->type()==QEvent::GraphicsSceneMousePress)
    {

        QGraphicsSceneMouseEvent* mouseEv = static_cast<QGraphicsSceneMouseEvent*>(event);
        if(mouseEv)
        {
            if (mouseEv->button()==Qt::LeftButton)
            {
                mouseJustPressed = true;
                mouseJustReleased = false;
                pressPos = mouseEv->scenePos();
                if (pointButton->isChecked())
                {
                    QPointF seriesPoint = scene2series(pressPos);
                    QGraphicsItem* itm = mainScene->addEllipse(-2.5,-2.5,5,5);
                    itm->setPos(mouseEv->scenePos());
                    itm->setData(0,seriesPoint);
                    drawnElements.append(itm);
                }
                else if (splineButton->isChecked())
                {
                    splineStarted = true;
                    splineSeriesPoints = new QList<QVariant>;
                    splinePath = new QPainterPath;
                    splineSeriesPoints->append(scene2series(pressPos));
                    splinePath->moveTo(pressPos);
                    prevPos = pressPos;
                    qInfo() << "Spline started";
                }
            }
        }
        return false;
    }

    else if (watched==mainScene && event->type()==QEvent::GraphicsSceneMouseRelease)
    {

        QGraphicsSceneMouseEvent* mouseEv = static_cast<QGraphicsSceneMouseEvent*>(event);
        if(mouseEv)
        {
            if (mouseEv->button()==Qt::LeftButton)
            {
                mouseJustPressed = false;
                mouseJustReleased = true;
                releasePos = mouseEv->scenePos();
                if (pressPos != releasePos && movingLine && lineButton->isChecked())
                {
                    QPointF seriesPoint1 = scene2series(pressPos);
                    QPointF seriesPoint2 = scene2series(releasePos);
                    movingLine->setData(0,seriesPoint1);
                    movingLine->setData(1,seriesPoint2);
                    drawnElements.append(movingLine);
                    movingLine = nullptr;
                }
                else if (splineStarted)
                {
                    qDeleteAll(*splineTempLines);
                    splineTempLines->clear();
                    splinePath->lineTo(releasePos);
                    splineSeriesPoints->append(scene2series(releasePos));
                    splineStarted = false;
                    splineItem = mainScene->addPath(*splinePath, drawingPen);
                    splineItem->setData(0, *splineSeriesPoints);
                    drawnElements.append(splineItem);
                    delete splinePath;
                }
            }
        }
        return false;
    }

    else if (watched==mainScene && event->type()==QEvent::GraphicsSceneMouseMove)
    {
        QGraphicsSceneMouseEvent* moveEv = static_cast<QGraphicsSceneMouseEvent*>(event);
        if(moveEv)
        {
            if (mouseJustPressed)
            {
                if (pressPos != moveEv->scenePos())
                {
                    if(lineButton->isChecked())
                    {
                        if(!movingLine)
                        {
                            movingLine = new QGraphicsLineItem(pressPos.x(), pressPos.y(),
                                                               moveEv->scenePos().x(), moveEv->scenePos().y());
                            movingLine->setPen(drawingPen);
                            mainScene->addItem(movingLine);
                        }
                        else
                        {
                            movingLine->setLine(pressPos.x(), pressPos.y(),
                                                moveEv->scenePos().x(), moveEv->scenePos().y());
                        }
                    }
                    else if (handleButton->isChecked())
                    {
                        QPointF delta = moveEv->scenePos() - moveEv->lastScenePos();

                        mainChart->scroll(-delta.x(), delta.y());
                        drawElements();
                    }
                    else if (splineStarted)
                    {
                        if (qSqrt(qPow(prevPos.x()-moveEv->scenePos().x(),2)+
                                  qPow(prevPos.y()-moveEv->scenePos().y(),2)) > splineDef)
                        {
                            splineTempLines->append(
                                        mainScene->addLine(prevPos.x(),prevPos.y(),moveEv->scenePos().x(),moveEv->scenePos().y(),
                                        transitoryPen));
                            splinePath->lineTo(moveEv->scenePos());
                            splineSeriesPoints->append(scene2series(moveEv->scenePos()));
                            prevPos = moveEv->scenePos();
                        }
                    }
                }
            }
        }
        //logRects("eventFilter: QEvent::GraphicsSceneMouseMove sent to mainScene");
        return true;
    }
    return false;
}

void DataVisualization::chartPlotAreaChanged(const QRectF &plotArea)
{
    drawElements();
    penFrame->move(graphicsView->width()-brushLabel->width()-70,0);
    //cbPenSize->move(-30,3);

}

void DataVisualization::handleButtonPressed()
{
    graphicsView->setCursor(QCursor(Qt::OpenHandCursor));
}

void DataVisualization::pointButtonPressed()
{
    graphicsView->setCursor(QCursor(Qt::CrossCursor));
}

void DataVisualization::lineButtonPressed()
{
    graphicsView->setCursor(QCursor(Qt::CrossCursor));
}

void DataVisualization::splineButtonPressed()
{
    graphicsView->setCursor(QCursor(Qt::CrossCursor));
}
