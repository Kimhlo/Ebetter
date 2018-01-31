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

    scene=new Scene(usb1,usb0,6666);
    scene->sleepMode(true);
    scene->setAllCurtainOpenTime(9);
    scene->setCurOpenTime(7);
    scene->setCurCloseTime(22);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    scene->stopAlarm();
}
