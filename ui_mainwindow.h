/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "QtCharts"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_2;
    QChartView *GV_chartView;
    QPlainTextEdit *plainTextEdit;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QDateEdit *CB_final_date;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label;
    QComboBox *CB_stockName;
    QDateEdit *CB_initial_date;
    QPushButton *PB_plot;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_3;
    QLabel *label_8;
    QLineEdit *LE_symbol;
    QLabel *label_4;
    QLineEdit *LE_displaysymbol;
    QLabel *label_5;
    QLineEdit *LE_currency;
    QLabel *label_6;
    QTextEdit *LE_description;
    QLabel *label_7;
    QLineEdit *LE_type;
    QWidget *tab_2;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuAnalyze;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(975, 594);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        frame_2 = new QFrame(centralwidget);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy);
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame_2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        GV_chartView = new QChartView(frame_2);
        GV_chartView->setObjectName(QString::fromUtf8("GV_chartView"));

        verticalLayout_2->addWidget(GV_chartView);

        plainTextEdit = new QPlainTextEdit(frame_2);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy1);

        verticalLayout_2->addWidget(plainTextEdit);


        gridLayout_2->addWidget(frame_2, 0, 1, 1, 1);

        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy2);
        frame->setMinimumSize(QSize(400, 0));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(frame);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        CB_final_date = new QDateEdit(groupBox);
        CB_final_date->setObjectName(QString::fromUtf8("CB_final_date"));

        gridLayout->addWidget(CB_final_date, 2, 1, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        CB_stockName = new QComboBox(groupBox);
        CB_stockName->setObjectName(QString::fromUtf8("CB_stockName"));

        gridLayout->addWidget(CB_stockName, 0, 1, 1, 1);

        CB_initial_date = new QDateEdit(groupBox);
        CB_initial_date->setObjectName(QString::fromUtf8("CB_initial_date"));

        gridLayout->addWidget(CB_initial_date, 1, 1, 1, 1);

        PB_plot = new QPushButton(groupBox);
        PB_plot->setObjectName(QString::fromUtf8("PB_plot"));

        gridLayout->addWidget(PB_plot, 3, 1, 1, 1);


        verticalLayout->addWidget(groupBox);

        tabWidget = new QTabWidget(frame);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setMinimumSize(QSize(0, 200));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout_3 = new QGridLayout(tab);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_8 = new QLabel(tab);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_3->addWidget(label_8, 0, 0, 1, 1);

        LE_symbol = new QLineEdit(tab);
        LE_symbol->setObjectName(QString::fromUtf8("LE_symbol"));

        gridLayout_3->addWidget(LE_symbol, 0, 1, 1, 1);

        label_4 = new QLabel(tab);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_3->addWidget(label_4, 1, 0, 1, 1);

        LE_displaysymbol = new QLineEdit(tab);
        LE_displaysymbol->setObjectName(QString::fromUtf8("LE_displaysymbol"));

        gridLayout_3->addWidget(LE_displaysymbol, 1, 1, 1, 1);

        label_5 = new QLabel(tab);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_3->addWidget(label_5, 2, 0, 1, 1);

        LE_currency = new QLineEdit(tab);
        LE_currency->setObjectName(QString::fromUtf8("LE_currency"));

        gridLayout_3->addWidget(LE_currency, 2, 1, 1, 1);

        label_6 = new QLabel(tab);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_3->addWidget(label_6, 3, 0, 1, 1);

        LE_description = new QTextEdit(tab);
        LE_description->setObjectName(QString::fromUtf8("LE_description"));
        LE_description->setMaximumSize(QSize(16777215, 80));

        gridLayout_3->addWidget(LE_description, 3, 1, 1, 1);

        label_7 = new QLabel(tab);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_3->addWidget(label_7, 4, 0, 1, 1);

        LE_type = new QLineEdit(tab);
        LE_type->setObjectName(QString::fromUtf8("LE_type"));

        gridLayout_3->addWidget(LE_type, 4, 1, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidget->addTab(tab_2, QString());

        verticalLayout->addWidget(tabWidget);


        gridLayout_2->addWidget(frame, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 975, 25));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuAnalyze = new QMenu(menubar);
        menuAnalyze->setObjectName(QString::fromUtf8("menuAnalyze"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuAnalyze->menuAction());

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Data selection", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Ending Date", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Starting Date", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Stock Name", nullptr));
        PB_plot->setText(QCoreApplication::translate("MainWindow", "Plot", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Symbol", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Display Symbol", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Currency", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Description", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Type", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "Details", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "Other info", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("MainWindow", "Edit", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
        menuAnalyze->setTitle(QCoreApplication::translate("MainWindow", "Analyze", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
