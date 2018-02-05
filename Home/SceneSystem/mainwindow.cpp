#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>
#include <wiringSerial.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowTitle("Scene");

    ui->setupUi(this);
    int usb0,usb1;
    usb0 = serialOpen("/dev/ttyUSB0",9600);
    usb1 = serialOpen("/dev/ttyUSB1",38400);
    if(usb0 < 0) qDebug()<<"open usb0 error\n";
    if(usb1 < 0) qDebug()<<"open usb1 error\n";


    scene=new Scene(usb1,usb0);
    scene->sleepMode(true);
    scene->setCurCloseTime(22);
    scene->setCurOpenTime(7);
    scene->setAllCurtainOpenTime(9);

    udp=new QUdpSocket;
    udp->bind(6666);
    curtain=new CurtainNew(usb0);
    timer=new QTimer;

    connect(udp,SIGNAL(readyRead()),
            this,SLOT(closeCurtain()));
    connect(timer,SIGNAL(timeout()),
            this,SLOT(isTimeOut()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    scene->stopAlarm();
}

void MainWindow::closeCurtain()
{
    QByteArray recData;
    while (udp->hasPendingDatagrams()) {
        recData.resize(udp->pendingDatagramSize());
        udp->readDatagram(recData.data(),recData.size());
    }
    int curtainStatus=curtain->checkStatus(2);
    qDebug()<<"cur="<<curtainStatus;
    if(recData[0]==0x01&&curtainStatus==1){//门打开且窗帘关闭
        curtain->openHalf(2,2);//窗帘和纱帘都开一半
        timer->start(50000);
    }
}

void MainWindow::isTimeOut()
{
    curtain->close(2,1);
    curtain->close(2,2);
    qDebug()<<"close curtain";
    if(timer->isActive()){
        timer->stop();
    }


}
