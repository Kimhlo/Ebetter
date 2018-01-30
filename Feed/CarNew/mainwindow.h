#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include <QtNetwork/QUdpSocket>
#include <QHostAddress>
#include <QUdpSocket>
#include<QtNetwork/QTcpSocket>
#include <QProcess>
#include <QSettings>
#include "recthread.h"
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    //udp socket
    QUdpSocket *udpSocket;
    QHostAddress serverIP;
    qint64 serverPort;

    //thread
    RecThread *myThread_1;

    //timer
    QTimer *timer1;//in order to get miss the crib2
    QTimer *timer2;//send the data to server

private slots:
    //init slots
    void on_pushButton_clicked();
    void UpdateSlot(int num1,int num2,int s);
    void readIni();
    void setIni();
    void on_lineEdit_IP_textEdited(const QString &arg1);

    //car status
    void on_buttonSleep_clicked();
    void on_buttonRun_clicked();

    //car action
    void on_buttonForward_clicked();
    void on_buttonBack_clicked();
    void on_buttonThrow_clicked();
    void on_buttonStop_clicked();

    //udp receive data and control
    void recAndControl();

    //timer slots
    void sendToServer();
    void missRFID();
};

#endif // MAINWINDOW_H
