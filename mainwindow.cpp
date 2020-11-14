#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Prepare all the information

    //Connect to the API and get stock names
    //Set ending date to current date
    //Set starting date to 3 months before the current date

}

MainWindow::~MainWindow()
{
    delete ui;
}
