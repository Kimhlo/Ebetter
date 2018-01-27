#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wiringPi.h"
#include "wiringSerial.h"
#include <QDebug>

#define  IN1_1 29
#define  IN1_2 28
#define  IN1_3 27
#define  IN1_4 26

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //int the ui information
    readIni();
    flag_run=0;

    setWindowTitle("Car");
    //udpSocket init
    serverIP=ui->lineEdit_IP->text();
    serverPort=5000;
    udpSocket=new QUdpSocket;
    int port=7001;//receive port
    udpSocket->bind(port);//bind the port
    connect(udpSocket,SIGNAL(readyRead()),
            this,SLOT(recAndControl()));

    //start the thread
    myThread_1 = new RecThread;
    connect(myThread_1, SIGNAL(UpdateSignal(int,int,int)),
            this, SLOT(UpdateSlot(int,int,int)));
    myThread_1->start(QThread::InheritPriority);

    //init the timer
    timer1=new QTimer;
    timer2=new QTimer;
//    timer1->setInterval(500);
    timer1->start(500);//send the status to the server in the frequence of 0.5s
    connect(timer1,SIGNAL(timeout()),
            this,SLOT(sendToServer()));
    connect(timer2,SIGNAL(timeout()),
            this,SLOT(missRFID()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//send
void MainWindow::on_pushButton_clicked()
{
    QString str=this->ui->lineEdit->text();
    QByteArray arr;
    QString temStr;

    const QStringList & result = str.split(" ");
    char len=result.size();
     arr.resize(len+2);
     arr[0]=len;
     arr[1]=0;

     for(int i=2;i<len+2;i++)
     {
          temStr=result.at(i-2).trimmed();
          arr[i]=temStr.toInt(0, 16);
     }
}

void MainWindow::UpdateSlot(int num1,int num2,int s)
{
   if(s==100){
       ui->label_4->setText("forward");
   }
   if(s==101){
       ui->label_4->setText("back");
   }
   if(s==102){
       ui->label_4->setText("stop");
   }
   if(s==1){
       ui->label_4->setText("arrive at 1");
       timer2->start(7000);
   }
   if(s==2){
       ui->label_4->setText("arrive at 2");
       timer2->stop();
   }

}

void MainWindow::setIni()
{
    QString f1=QCoreApplication::applicationDirPath();
    f1.append("/my.ini");
    QSettings *configIniWrite = new QSettings(f1, QSettings::IniFormat);
    configIniWrite->setValue("/DAQ/IP", ui->lineEdit_IP->text());
    delete configIniWrite;
}

void MainWindow::readIni()
{
    QString iniPath=QCoreApplication::applicationDirPath();
    iniPath.append("/my.ini");
    QSettings *configIniRead = new QSettings(iniPath, QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    ui->lineEdit_IP->setText(configIniRead->value("/DAQ/IP").toString());
    delete configIniRead;
}


void MainWindow::on_lineEdit_IP_textEdited(const QString &arg1)
{
    setIni();
}

/************************************
 * car status
**********************************/
//run
void MainWindow::on_buttonRun_clicked()
{
    flag_run=1;
    sendData[0]=0x01;//运行
    sendData[1]=0x01;
    digitalWrite(IN1_4, HIGH);
    digitalWrite(IN1_3, LOW);
    digitalWrite(IN1_2, HIGH);
    qDebug()<<"run";
}

//sleep
void MainWindow::on_buttonSleep_clicked()
{
    digitalWrite(IN1_3, HIGH);
    digitalWrite(IN1_4, HIGH);
    digitalWrite(IN1_2, HIGH);
    flag_run=0;
    sendData[0]=0x00;//运行
    sendData[1]=0x04;
    qDebug()<<"sleep";
}

/************************************
 * car action
**********************************/
//forward
void MainWindow::on_buttonForward_clicked()
{
    digitalWrite(IN1_4, HIGH);
    digitalWrite(IN1_3, LOW);
    digitalWrite(IN1_2, HIGH);
    sendData[0]=0x03;
    sendData[1]=0x01;//前进
    qDebug()<<"forward";
}
//back
void MainWindow::on_buttonBack_clicked()
{
    digitalWrite(IN1_3, HIGH);
    digitalWrite(IN1_2, LOW);
    digitalWrite(IN1_4, HIGH);
    sendData[0]=0x03;
    sendData[1]=0x02;//返回
    qDebug()<<"back";
}
//throw the food
void MainWindow::on_buttonThrow_clicked()
{
    //stop the car
    digitalWrite(IN1_4, HIGH);
    digitalWrite(IN1_3, HIGH);
    digitalWrite(IN1_2, HIGH);
    delay(1000);
    //throw the food
    digitalWrite(IN1_1, LOW);
    delay(5000);
    //stop throw food
    digitalWrite(IN1_1, HIGH);
     //forward
    digitalWrite(IN1_3, LOW);
    delay(1000);
    sendData[0]=0x03;
    sendData[1]=0x03;//投食
    qDebug()<<"throw food";
}
//stop
void MainWindow::on_buttonStop_clicked()
{
    digitalWrite(IN1_3, HIGH);
    digitalWrite(IN1_4, HIGH);
    digitalWrite(IN1_2, HIGH);
    sendData[0]=0x03;
    sendData[1]=0x04;//停止
    qDebug()<<"stop";
}

/********************************************
 * udpSocket receive the data and take action
**********************************************/
void MainWindow::recAndControl()
{
    //get the received data
    QByteArray datagram;
    while (udpSocket->hasPendingDatagrams()) {
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());
    }//end while udp

    //check the first bit
    if(datagram.at(0)==0x01){
        on_buttonRun_clicked();
    }else if(datagram.at(0)==0x00){
        on_buttonSleep_clicked();
    }//end if first bit

    //check the second bit
    if(datagram.at(1)==0x01){//forward
        on_buttonForward_clicked();
    }else if (datagram.at(1)==0x02) {//back
        on_buttonBack_clicked();
    }else if (datagram.at(1)==0x03) {//throw the food
        on_buttonThrow_clicked();
    }else if (datagram.at(1)==0x04) {//stop
        on_buttonStop_clicked();
    }//end if second bit
}

/********************************************
 * timer slots
**********************************************/
//
void MainWindow::sendToServer()
{
    QByteArray data;
    data.resize(4);
    data[0]=sendData[0];
    data[1]=sendData[1];
    data[2]=sendData[2];
    data[3]=sendData[3];
    //send the datagram
    udpSocket->writeDatagram(data,serverIP,serverPort);
//    qDebug()<<"toServer";
}

void MainWindow::missRFID()
{
    timer2->stop();
    on_buttonBack_clicked();
    qDebug()<<"miss RFID";
}



