#ifndef DATAVISUALIZATION_H
#define DATAVISUALIZATION_H

#include <QObject>
#include <QtCharts>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QCandlestickSet>
#include "stockdata.h"

class DataVisualization : public QObject
{
    Q_OBJECT
public:
    explicit DataVisualization(QGraphicsView* graphicsView, StockData* sData, QObject *parent = nullptr);
    void plotData();

private:
    void calculateYticks();
    void calculateXticks();
    void calculateWeekLines();
    void createElements();
    void drawElements();
    QPointF scene2series(const QPointF &pt);
    void logRects(const QString& origin);
    void createButtons();

    StockData* stockData;

    QCandlestickSeries* candleSeries;

    QList<QGraphicsItem*> drawnElements;

    QGraphicsScene* mainScene;
    QGraphicsView* graphicsView;
    QChart *mainChart;

    QBarCategoryAxis *barCatAxisX;
    QCategoryAxis *mainAxisX;
    QCategoryAxis *weekAxis;

    QValueAxis* axisY;

    double zoomLevel;
    const double maxZoom = 3.0;
    const double minZoom = 0.4;

    QRectF masterRect;

    float axisYmin;
    float axisYmax;

    bool mouseJustPressed;
    bool mouseJustReleased;

    QPointF pressPos; //Scene coordinates
    QPointF releasePos;  //Secene coordinates
    QGraphicsLineItem* movingLine;

    QPushButton* handleButton;
    QPushButton* pointButton;
    QPushButton* lineButton;
    QPushButton* splineButton;

    QButtonGroup* drawingButtons;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void chartPlotAreaChanged(const QRectF &plotArea);

};

#endif // DATAVISUALIZATION_H
