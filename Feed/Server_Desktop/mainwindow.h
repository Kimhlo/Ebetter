#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QtNetwork/QTcpSocket>
#include <QHostAddress>
#include <QImageReader>
#include<QTcpServer>
#include <QImage>
#include <QTimer>
#include <QAbstractSocket>
#include <QUdpSocket>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

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
    QUdpSocket *udpSocket_6000 ;

    QHostAddress IP_sensor;
//    bool Geted_8080;

    QTcpSocket *tcpSocket_8080;  //
    QByteArray data2;
    bool valid_data;
    int cnt;

    QByteArray data3;

    QHostAddress IP_window;
    QHostAddress IP_car;

    //pig
    float pig_temp;
    float pig_weight;
    unsigned short rfid;

    float air_temp;
    //car
    QString car_status;

    QTimer *showTime;//获取图像的定时器
    QImage image;//图像

    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QString Port,Adress,PU;

    float getTemper(unsigned char data[]);
    float getWeight(unsigned char data[]);
public slots:
    void getReply(QNetworkReply *reply);
    void updatePic();
    void udp_rec_5000();
    void udp_rec_6000();

    void db_test();
    void on_pushButton_Open_up_clicked();
    void on_pushButton_Close_up_clicked();
    void on_pushButton_Open_down_clicked();
    void on_pushButton_Close_down_clicked();
private slots:
    void on_buttonRun_clicked();
    void on_buttonSleep_clicked();
    void on_buttonForward_clicked();
    void on_buttonBack_clicked();
    void on_buttonThrow_clicked();
    void on_buttonStop_clicked();
};

#endif // MAINWINDOW_H
