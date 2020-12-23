#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QPushButton>
#include <QMessageBox>

#include <QDebug>

#include <QDataStream>
#include <QVariantMap>
#include <QVector>
#include <QDateTime>

#include <QtCharts>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QCandlestickSet>

#include "stockdata.h"
#include "networkinterface.h"
#include "datavisualization.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    void setDates();
    void populateCBStockInfo();
    void reportDataDays();
    void plotData();

    Ui::MainWindow *ui;

    StockData* stockData;
    NetworkInterface* networkInterface;
    DataVisualization* dataVisualization;

    QStatusBar * statBar;
    QLabel * statLabel;

    QString searchSymbol;

    QDateTime final_dt;
    QDateTime initial_dt;

private slots:

    void stockCBchanged(int);
    void plotPressed();
    void on_PB_zoomIn_clicked();
    void on_PB_zoomOut_clicked();
    void stockDataReadyToPlot();
    void stockInfoReady();

};

#endif // MAINWINDOW_H
