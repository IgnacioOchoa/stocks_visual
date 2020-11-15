#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>

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
    void populateStockNames();
    void getStockNames(QStringList& );


    Ui::MainWindow *ui;
    QNetworkAccessManager manager;
    QStringList stockNames;

private slots:
    void replyFinished(QNetworkReply *);
    void readyRead();

};

#endif // MAINWINDOW_H
