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
    void calculateYticks(QValueAxis* axisY);
    void calculateXticks();
    void calculateWeekLines();

    StockData* stockData;

    QGraphicsScene* mainScene;
    QGraphicsView* graphicsView;
    QChart *mainChart;

    QBarCategoryAxis *barCatAxisX;
    QCategoryAxis *mainAxisX;
    QCategoryAxis *weekAxis;

    double zoomLevel;
    const double maxZoom = 3.0;
    const double minZoom = 0.4;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void chartPlotAreaChanged(const QRectF &plotArea);

};

#endif // DATAVISUALIZATION_H
