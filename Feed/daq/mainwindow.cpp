#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wiringPi.h"
#include "wiringSerial.h"
#include <QDebug>

#include <QString>
#include <QMessageBox>
#include<QFile>
#include <QTextStream>
#include <QProcess>
#include <QStringList>
#include <QMouseEvent>
#include<QMovie>
#include<QPoint>
#include<QThread>
#include<QPixmap>
#include<QPainter>
#include<QTableWidgetItem>
#include<QHeaderView>

namespace MyNamespace {
QString baseListPath="/home/pi/music/music.list";
QString baseFilePath="/home/pi/music/mp3/";
QProcess musicProcess;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

     MyNamespace::musicProcess.start("mplayer -slave -quiet -playlist music.list " );

     MyNamespace::musicProcess.write( "loadfile LiBai.mp3\n");
     readIni();

    this->tcpSocket = new QTcpSocket(this);
    newTCPConnect();

    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(RecvTCP()));
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(newTCPConnect()));

     myThread_1 = new RecThread;
     connect(myThread_1, SIGNAL(UpdateSignal(int,int,int)), this, SLOT(UpdateSlot(int,int,int)));
     myThread_1->start(QThread::InheritPriority);



}

MainWindow::~MainWindow()
{
    delete ui;
}


//rec
void MainWindow::RecvTCP()
{
    int len=0;
    QByteArray datagram_1;
    len=tcpSocket->bytesAvailable();
    datagram_1.resize(len);
    //
    datagram_1 = tcpSocket->readAll();

    tcpSocket->write(datagram_1,len);

    ui->label->setText(datagram_1.toHex());
  //  qDebug()<<datagram_1;

}

void MainWindow::newTCPConnect()
{
    tcpSocket->abort();
    tcpSocket->connectToHost(ui->lineEdit_IP->text(),5000);
}

void MainWindow::displayError(QAbstractSocket::SocketError)
{
    ui->label_2->setText( tcpSocket->errorString());
    tcpSocket->close();
    newTCPConnect();
}


//send
void MainWindow::on_pushButton_clicked()
{
    QString str=this->ui->lineEdit->text();
    QByteArray arr;

    QString temStr;

    const QStringList & result = str.split(" ");
    char len=result.size();
          // qDebug()<<"len="<<len;

     arr.resize(len+2);
     arr[0]=len;
     arr[1]=0;
     for(int i=2;i<len+2;i++)
     {
          temStr=result.at(i-2).trimmed();
          arr[i]=temStr.toInt(0, 16);
     }
     this->tcpSocket->write(arr); //5000
}

void MainWindow::UpdateSlot(int num1,int num2,int s)
{
    MyNamespace::musicProcess.write("pause\n");
   QByteArray arr;
   int i=0;

    arr.resize(37);
    for(i=0;i<37;i++)
    {
        arr[i]=recData[i];
    }
    this->tcpSocket->write(arr); //5000

     delay(1000);
   MyNamespace::musicProcess.write( "loadfile LiBai.mp3\n");

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


//play mp3
void MainWindow::on_pushButton_Play_clicked()
{
    MyNamespace::musicProcess.write( "loadfile LiBai.mp3\n");
}
