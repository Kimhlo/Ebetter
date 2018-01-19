#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBuffer>

#include <QSqlDatabase>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include <QtSql>
#include <QSqlQuery>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bool retBind;
    udpSocket_5000 = new QUdpSocket;
    udpSocket_6000 = new QUdpSocket;

    udpSocket_5000->setReadBufferSize(512);   //现场
    retBind=udpSocket_5000-> bind(5000,QUdpSocket::ShareAddress);
    if(false==retBind)
    {
        qDebug()<<"bind port:5000 error";
        return;
    }
    //------------------------------------------
    udpSocket_6000->setReadBufferSize(512);   //app
    retBind=udpSocket_6000-> bind(6000,QUdpSocket::ShareAddress);
    if(false==retBind)
    {
        qDebug()<<"bind port:6000 error";
        return;
    }
     connect(udpSocket_5000, SIGNAL(readyRead()), this, SLOT(udp_rec_5000()));
     connect(udpSocket_6000, SIGNAL(readyRead()), this, SLOT(udp_rec_6000()));

     /********************************************
      * 获取视频部分
     ********************************************/
     //网络获取图像的相应对象创建
     manager=new QNetworkAccessManager(this);
     //设置获取图片的网络信息
     Adress="192.168.1.103";//获取图像的IP
     Port="8080";//端口号
     PU="http://"+Adress+":"+Port+"/?action=snapshot";
     //刷新图像的定时器
     showTime=new QTimer;
     showTime->setInterval(80);//每次0.08s获取一次图像
     showTime->start();
     //槽函数连接
     connect(manager,SIGNAL(finished(QNetworkReply*)),
             this,SLOT(getReply(QNetworkReply*)));
     connect(showTime,SIGNAL(timeout()),
             this,SLOT(updatePic()));

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::getReply(QNetworkReply *reply)
{
    //获取图片数据
    QByteArray data=reply->readAll();
    //显示图片
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    QImageReader reader(&buffer,"jpg");
    QImage image2=reader.read();
    ui->label->setScaledContents(true);
    ui->label->setPixmap(QPixmap::fromImage(image2,Qt::AutoColor));

}

void MainWindow::updatePic()
{
    qDebug()<<"updata pic";
    request.setUrl(QUrl(PU));//设置访问的网址
    manager->get(request);//用get方法获取图像
}
void MainWindow::udp_rec_5000()
{
    QByteArray arr;
    while(udpSocket_5000->hasPendingDatagrams())
    {
        arr.resize(udpSocket_5000->pendingDatagramSize());
        udpSocket_5000->readDatagram(arr.data(), arr.size());
    }
    //获取数据包地址
    IP_sensor=udpSocket_5000->peerAddress();

    this->ui->label_21->setText(arr.toHex());

    QString strIP=IP_sensor.toString().mid(7,17);
    if(strIP=="192.168.1.103")  //sensor_1
    {
        //=====================
        // qDebug()<<arr.toHex();
        //====================
        char ch_1[2];
        char ch_2[2];
        unsigned short val;
        short dt;
        //=============================
        QString str_temp;
        str_temp.append(QString::number(arr[27]-48));
        str_temp.append(QString::number(arr[28]-48));
        str_temp.append(".");
        str_temp.append(QString::number(arr[30]-48));

        float weigh=0;
        ch_2[0]=arr[31];
        ch_2[1]=arr[32];
        ch_2[2]=arr[33];
        ch_2[3]=arr[34];
        val =*(int *)ch_2;
        weigh=(float)val *0.00001;

        ui->label_pig_temp->setText(str_temp);
        ui->label_weight->setText(QString::number(weigh));
        ui->label_rfid->setText(this->ui->label_21->text().mid(0,54));

        if(arr[36]==1)
        {
            ui->listWidget->addItem(new QListWidgetItem(QString("【" + ui->label_rfid->text() + "】 已吃完！")));
        }
        // ui->label_22->setText(QString::number(arr[43]));
    }
    //============================================
    if(strIP=="192.168.1.104")  //sensor_2
    {
        this->ui->label_23->setText(arr.toHex());
        ui->label_rfid_2->setText(this->ui->label_23->text().mid(0,54));
    }

    if(strIP=="192.168.1.107")  //windows_1
    {
       QString strIP=IP_sensor.toString().mid(7,17);
       this->ui->label_21->setText(arr.toHex());
    }
    if(strIP=="192.168.1.109")  //windows_2
    {
       QString strIP=IP_sensor.toString().mid(7,17);
       this->ui->label_21->setText(arr.toHex());
    }
}

void MainWindow::udp_rec_6000()
{
    QByteArray arr;
    while(udpSocket_6000->hasPendingDatagrams())
    {
        arr.resize(udpSocket_6000->pendingDatagramSize());
        udpSocket_6000->readDatagram(arr.data(), arr.size());
    }
    IP_sensor=udpSocket_6000->peerAddress(); //获取数据包地址

    this->ui->label_21->setText(arr.toHex());

    QString strIP=IP_sensor.toString().mid(7,17);

}

void MainWindow::db_test()
{
        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
        qDebug()<<"ODBC driver?"<<db.isValid();
        QString dsn = QString::fromLocal8Bit("qtdsn");
        db.setHostName("./SQLEXPRESS");
        db.setDatabaseName(dsn);
        db.setUserName("sa");
        db.setPassword("123456");
        if(!db.open())
        {
            qDebug()<<db.lastError().text();
            QMessageBox::critical(0, QObject::tr("Database error"), db.lastError().text());
            return;
        }
        else
            qDebug()<<"database open success!";
        QSqlQuery query(db);
        query.exec("select * from loginUser");
        while(query.next())
        {
            //qDebug()<<query.value(0).toString();
            qDebug()<<query.value(1).toString();
            //qDebug()<<query.value(2).toString();
            //qDebug()<<query.value(3).toString();
            //qDebug()<<" ";
        }

        //======================
        query.prepare("INSERT INTO loginUser (Id, Pwd, UserName,UserRole) "
                            "VALUES (:Id, :Pwd, :UserName,:UserRole)");
        query.bindValue(":Id", "1001");
        query.bindValue(":Pwd", "123");
        query.bindValue(":UserName", "z2");
        query.bindValue(":UserRole", "1,2,3");
        query.exec();
}

//开上方窗户
void MainWindow::on_pushButton_Open_up_clicked()
{
    QByteArray arr;
    arr.resize(2);
    arr[0]=0x02;  //第1字节01为打开，第2字节为上下窗户区分， 01开下面窗户 ，02 开上面窗户，
    arr[1]=0x01;

    IP_window.setAddress("192.168.1.107");
    qint64 rt ;
    rt= udpSocket_5000->writeDatagram(arr,2,IP_window,5001);
    if(rt<0)
    {
       qDebug()<<"open window error";
       return;
    }

    IP_window.setAddress("192.168.1.109");
    rt= udpSocket_5000->writeDatagram(arr,2,IP_window,5001);
    if(rt<0)
    {
       qDebug()<<"open window error";
       return;
    }
}

//关上方窗户
void MainWindow::on_pushButton_Close_up_clicked()
{
    QByteArray arr;
    arr.resize(2);
    arr[0]=0x03;  //第1字节00为关闭，第2字节为上下窗户区分， 01开下面窗户 ，02 开上面窗户，
    arr[1]=0x02;

    IP_window.setAddress("192.168.1.107");
    qint64 rt ;
    rt= udpSocket_5000->writeDatagram(arr,2,IP_window,5001);
    if(rt<0)
    {
       qDebug()<<"open window error";
       return;
    }

    IP_window.setAddress("192.168.1.109");

    rt= udpSocket_5000->writeDatagram(arr,2,IP_window,5001);
    if(rt<0)
    {
       qDebug()<<"open window error";
       return;
    }
}

//开下方窗户
void MainWindow::on_pushButton_Open_down_clicked()
{
    QByteArray arr;
    arr.resize(2);
    arr[0]=0x03;  //第1字节01为打开，第2字节为上下窗户区分， 01开下面窗户 ，02 开上面窗户，
    arr[1]=0x01;

    IP_window.setAddress("192.168.1.107");
    qint64 rt ;
    rt= udpSocket_5000->writeDatagram(arr,2,IP_window,5001);
    if(rt<0)
    {
       qDebug()<<"open window error";
       return;
    }

    IP_window.setAddress("192.168.1.109");

    rt= udpSocket_5000->writeDatagram(arr,2,IP_window,5001);
    if(rt<0)
    {
       qDebug()<<"open window error";
       return;
    }
}

//关下方窗户
void MainWindow::on_pushButton_Close_down_clicked()
{
    QByteArray arr;
    arr.resize(2);
    arr[0]=0x00;  //第1字节00为关闭，第2字节为上下窗户区分， 01开下面窗户 ，02 开上面窗户，
    arr[1]=0x01;

    IP_window.setAddress("192.168.1.107");
    qint64 rt ;
    rt= udpSocket_5000->writeDatagram(arr,2,IP_window,5001);
    if(rt<0)
    {
       qDebug()<<"open window error";
       return;
    }

    IP_window.setAddress("192.168.1.109");

    rt= udpSocket_5000->writeDatagram(arr,2,IP_window,5001);
    if(rt<0)
    {
       qDebug()<<"open window error";
       return;
    }
}
