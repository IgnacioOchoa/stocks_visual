#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    stockData = new StockData(this);
    dataVisualization = new DataVisualization(ui->GV_chartView, stockData, this);

    statLabel = new QLabel();
    statBar = this->statusBar();
    statBar->addWidget(statLabel);

    statLabel->setText("Loading...");

    setDates();

    connect(ui->CB_stockName,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&MainWindow::stockCBchanged);
    connect(ui->PB_plot, &QPushButton::pressed, this, &MainWindow::plotPressed);
    connect(stockData, &StockData::stockInfoReady, this, &MainWindow::stockInfoReady);
    connect(stockData, &StockData::stockDataReady, this, &MainWindow::stockDataReadyToPlot);

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

void MainWindow::plotPressed()
// Slot for catching the press of the Plot button
{
    initial_dt = ui->CB_initial_date->date().startOfDay();
    final_dt = ui->CB_final_date->date().endOfDay();
    int numDays = initial_dt.daysTo(final_dt) + 1;
    if (numDays < 1){
        QMessageBox::warning(this,"Problem with dates", "The final date has to be later than the initial date");
        return;
    }
    stockData->getStockData(ui->CB_stockName->currentText(), initial_dt, final_dt);
}

void MainWindow::populateCBStockInfo()
//  Once the general info about all stocks is received and processed, it populates the stocks combo box
//  and sets the default one
{
    ui->CB_stockName->addItems(stockData->getDisplaySymbols());
    ui->CB_stockName->setCurrentIndex(ui->CB_stockName->findText("AAPL"));
}

void MainWindow::reportDataDays()
{
    QDateTime moment;
    foreach (long long x, stockData->currentStockTimeStamps())
    {
        moment = QDateTime::fromSecsSinceEpoch(x);
        ui->LW_daysList->addItem(moment.toString("ddd \tdd \tMMM"));
    }
}

void MainWindow::stockCBchanged(int indx)
// Slot called when another stock in the combo box is selected. It fills up the info tab.
{
    ui->LE_currency->setText(stockData->getCurrencies()[indx]);
    ui->LE_symbol->setText(stockData->getSymbols()[indx]);
    ui->LE_type->setText(stockData->getTypes()[indx]);
    ui->LE_description->setText(stockData->getDescriptions()[indx]);
    ui->LE_displaysymbol->setText(stockData->getDisplaySymbols()[indx]);
}

void MainWindow::on_PB_zoomIn_clicked()
{
    ui->GV_chartView->scale(1.2,1.2);
}

void MainWindow::on_PB_zoomOut_clicked()
{
    ui->GV_chartView->scale(0.8,0.8);
}

void MainWindow::stockDataReadyToPlot()
{
    dataVisualization->plotData();
}

void MainWindow::stockInfoReady()
{
    populateCBStockInfo();
    ui->PB_plot->setEnabled(true);
    statLabel->setText("Info loaded");
}
