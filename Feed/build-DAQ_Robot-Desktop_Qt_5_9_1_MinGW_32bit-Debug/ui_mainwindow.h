/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_6;
    QLabel *label_8;
    QLabel *label_10;
    QLabel *label_12;
    QLabel *label_car;
    QLabel *label_pig_temp;
    QLabel *label_rfid;
    QLabel *label_21;
    QLabel *label_23;
    QLabel *label_weight;
    QFrame *line;
    QFrame *line_2;
    QFrame *line_3;
    QPushButton *pushButton_Open_up;
    QPushButton *pushButton_Close_up;
    QLabel *label_7;
    QLabel *label_rfid_2;
    QLabel *label_13;
    QListWidget *listWidget;
    QPushButton *pushButton_Open_down;
    QPushButton *pushButton_Close_down;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1352, 723);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 50, 761, 501));
        label->setFrameShape(QFrame::Box);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(20, 9, 401, 31));
        label_3->setStyleSheet(QLatin1String("font: 75 16pt \"Agency FB\";\n"
"text-decoration: underline;"));
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(780, 320, 111, 31));
        label_6->setStyleSheet(QStringLiteral("font: 75 16pt \"Agency FB\";"));
        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(780, 190, 111, 31));
        label_8->setStyleSheet(QStringLiteral("font: 75 16pt \"Agency FB\";"));
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(780, 240, 121, 31));
        label_10->setStyleSheet(QStringLiteral("font: 75 16pt \"Agency FB\";"));
        label_12 = new QLabel(centralWidget);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(780, 140, 101, 31));
        label_12->setStyleSheet(QStringLiteral("font: 75 16pt \"Agency FB\";"));
        label_car = new QLabel(centralWidget);
        label_car->setObjectName(QStringLiteral("label_car"));
        label_car->setGeometry(QRect(880, 320, 371, 31));
        label_car->setStyleSheet(QStringLiteral("font: 75 16pt \"Agency FB\";"));
        label_car->setFrameShape(QFrame::Box);
        label_pig_temp = new QLabel(centralWidget);
        label_pig_temp->setObjectName(QStringLiteral("label_pig_temp"));
        label_pig_temp->setGeometry(QRect(880, 190, 111, 31));
        label_pig_temp->setStyleSheet(QStringLiteral("font: 75 16pt \"Agency FB\";"));
        label_pig_temp->setFrameShape(QFrame::Box);
        label_rfid = new QLabel(centralWidget);
        label_rfid->setObjectName(QStringLiteral("label_rfid"));
        label_rfid->setGeometry(QRect(880, 140, 461, 31));
        label_rfid->setStyleSheet(QStringLiteral("font: 75 16pt \"Agency FB\";"));
        label_rfid->setFrameShape(QFrame::Box);
        label_21 = new QLabel(centralWidget);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(10, 560, 761, 41));
        label_21->setStyleSheet(QStringLiteral("font: 12pt \"MS Mincho\";"));
        label_21->setFrameShape(QFrame::Box);
        label_23 = new QLabel(centralWidget);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setGeometry(QRect(10, 610, 761, 41));
        label_23->setStyleSheet(QStringLiteral("font: 12pt \"MS Mincho\";"));
        label_23->setFrameShape(QFrame::Box);
        label_weight = new QLabel(centralWidget);
        label_weight->setObjectName(QStringLiteral("label_weight"));
        label_weight->setGeometry(QRect(880, 240, 111, 31));
        label_weight->setStyleSheet(QStringLiteral("font: 75 16pt \"Agency FB\";"));
        label_weight->setFrameShape(QFrame::Box);
        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(780, 110, 561, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(centralWidget);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setGeometry(QRect(780, 360, 561, 16));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(centralWidget);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setGeometry(QRect(780, 290, 561, 16));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        pushButton_Open_up = new QPushButton(centralWidget);
        pushButton_Open_up->setObjectName(QStringLiteral("pushButton_Open_up"));
        pushButton_Open_up->setGeometry(QRect(790, 390, 141, 41));
        pushButton_Close_up = new QPushButton(centralWidget);
        pushButton_Close_up->setObjectName(QStringLiteral("pushButton_Close_up"));
        pushButton_Close_up->setGeometry(QRect(1190, 390, 121, 41));
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(510, 10, 401, 31));
        label_7->setStyleSheet(QLatin1String("font: 75 16pt \"Agency FB\";\n"
"text-decoration: underline;"));
        label_rfid_2 = new QLabel(centralWidget);
        label_rfid_2->setObjectName(QStringLiteral("label_rfid_2"));
        label_rfid_2->setGeometry(QRect(880, 70, 461, 31));
        label_rfid_2->setStyleSheet(QStringLiteral("font: 75 16pt \"Agency FB\";"));
        label_rfid_2->setFrameShape(QFrame::Box);
        label_13 = new QLabel(centralWidget);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(780, 70, 91, 31));
        label_13->setStyleSheet(QStringLiteral("font: 75 16pt \"Agency FB\";"));
        listWidget = new QListWidget(centralWidget);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(790, 500, 531, 151));
        pushButton_Open_down = new QPushButton(centralWidget);
        pushButton_Open_down->setObjectName(QStringLiteral("pushButton_Open_down"));
        pushButton_Open_down->setGeometry(QRect(790, 442, 141, 41));
        pushButton_Close_down = new QPushButton(centralWidget);
        pushButton_Close_down->setObjectName(QStringLiteral("pushButton_Close_down"));
        pushButton_Close_down->setGeometry(QRect(1190, 440, 121, 41));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1352, 23));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        label->setText(QString());
        label_3->setText(QString());
        label_6->setText(QApplication::translate("MainWindow", "\345\260\217\350\275\246\347\212\266\346\200\201\357\274\232", Q_NULLPTR));
        label_8->setText(QApplication::translate("MainWindow", "\347\214\252\347\232\204\344\275\223\346\270\251\357\274\232", Q_NULLPTR));
        label_10->setText(QApplication::translate("MainWindow", "\347\214\252\347\232\204\351\207\215\351\207\217\357\274\232", Q_NULLPTR));
        label_12->setText(QApplication::translate("MainWindow", "RFID:", Q_NULLPTR));
        label_car->setText(QString());
        label_pig_temp->setText(QString());
        label_rfid->setText(QString());
        label_21->setText(QString());
        label_23->setText(QString());
        label_weight->setText(QString());
        pushButton_Open_up->setText(QApplication::translate("MainWindow", "\345\274\200\344\270\212\346\226\271\347\252\227\346\210\267", Q_NULLPTR));
        pushButton_Close_up->setText(QApplication::translate("MainWindow", "\345\205\263\344\270\212\346\226\271\347\252\227\346\210\267", Q_NULLPTR));
        label_7->setText(QString());
        label_rfid_2->setText(QString());
        label_13->setText(QApplication::translate("MainWindow", "RFID:", Q_NULLPTR));
        pushButton_Open_down->setText(QApplication::translate("MainWindow", "\345\274\200\344\270\213\346\226\271\347\252\227\346\210\267", Q_NULLPTR));
        pushButton_Close_down->setText(QApplication::translate("MainWindow", "\345\205\263\344\270\212\346\226\271\347\252\227\346\210\267", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
