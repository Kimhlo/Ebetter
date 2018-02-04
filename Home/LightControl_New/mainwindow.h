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

    QUdpSocket *udpSocket_5000 ;

    RecThread *myThread_1;

    int fd_usb2;
    QHostAddress dIP;
    int send_cnt;

    //for 192.168.31.110
    unsigned char pre_doorLightSwitch;
    unsigned char doorLightSwitch;

private slots:
       void on_pushButton_clicked();

       void udp_rec_5000();
       void UpdateSlot(int num1,int num2,int s);

       void readIni();
       void setIni();
       void on_lineEdit_IP_textEdited(const QString &arg1);

 };

#endif // MAINWINDOW_H
