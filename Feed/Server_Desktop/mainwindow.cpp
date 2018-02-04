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
    /********************************************
     * udp通信部分定义
    ********************************************/
    bool retBind;
    udpSocket_5000 = new QUdpSocket;
    udpSocket_6000 = new QUdpSocket;
    //现场数据
    udpSocket_5000->setReadBufferSize(512);
    retBind=udpSocket_5000-> bind(5000,QUdpSocket::ShareAddress);
    if(false==retBind)
    {
        qDebug()<<"bind port:5000 error";
        return;
    }
    this->ui->label_carStatus->setText("正在待机中：待机");

    //app数据
    udpSocket_6000->setReadBufferSize(512);
    retBind=udpSocket_6000-> bind(6000,QUdpSocket::ShareAddress);
    if(false==retBind)
    {
        qDebug()<<"bind port:6000 error";
        return;
    }

    //udp通信信号和槽函数连接
     connect(udpSocket_5000, SIGNAL(readyRead()), this, SLOT(udp_rec_5000()));
     connect(udpSocket_6000, SIGNAL(readyRead()), this, SLOT(udp_rec_6000()));

     //小车的地址
     IP_car.setAddress("192.168.1.200");
    //---------------------------------------------------------------------------------------------
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

float MainWindow::getTemper(unsigned char data[])
{
    float temper=0.0;
    int temp= (data[29]*256+data[30]);
    temper=temp/10.0;
    qDebug()<<"temper="<<temper;
    return temper;
}

float MainWindow::getWeight(unsigned char data[])
{
    float nuitKg=29376.0;
    unsigned long beginWeight=4294399467;
    unsigned long weightNumber=0;
    float weight=0.0;
    long temp1=long (data[31])<<24;
    long temp2=long (data[32])<<16;
    long temp3=long (data[33])<<8;
    long temp4=long (data[34]);
    weightNumber=temp1|temp2|temp3|temp4;//得到传上来的数据
    if(weightNumber<=1000){
        weightNumber=4294399467;
    }
    weight=(4294399467-weightNumber);
    weight=weight/29376.0;
    if(weight<=3.0){
        weight=0.0;
    }
    qDebug()<<"weight="<<weight;
    return weight;
}

/********************************
 * 视频获取部分
********************************/
//处理获取的图片
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
//再次获取图片
void MainWindow::updatePic()
{
//    qDebug()<<"updata pic";
    request.setUrl(QUrl(PU));//设置访问的网址
    manager->get(request);//用get方法获取图像
}

/********************************
 * 5000端口收到的报文及其处理
********************************/
void MainWindow::udp_rec_5000()
{
    QByteArray arr;
    quint16 port;
    while(udpSocket_5000->hasPendingDatagrams())
    {
        arr.resize(udpSocket_5000->pendingDatagramSize());
        udpSocket_5000->readDatagram(arr.data(), arr.size(),&IP_sensor,&port);
    }
    QString strIP= IP_sensor.toString().mid(7,13);

    //获取数据包地址
    this->ui->label_21->setText(arr.toHex());


    //食槽1
    if(strIP=="192.168.1.103")
    {
        unsigned char data[arr.size()];
        float temper=0.0;
        float weight=0.0;
        for(int m=0;m<arr.size();m++){
            data[m]=arr.at(m);
        }
        temper=getTemper(data);
        weight=getWeight(data);

        ui->label_pig_temp->setText(QString::number(temper)+"℃");
        ui->label_weight->setText(QString::number(weight)+"Kg");
        ui->label_rfid->setText(this->ui->label_21->text().mid(0,54));

        if(arr[36]==0x03)
        {
            ui->listWidget->addItem(new QListWidgetItem(QString("【" + ui->label_rfid->text() + "】 投食完毕！")));
        }
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
    if(strIP=="192.168.1.200")  //car
    {
        QString carStatus="";
        if(arr.at(0)==0x01){
            carStatus.append("自动投食中：");
        }else if(arr.at(0)==0x00){
            carStatus.append("正在待机中：");
        }else if(arr.at(0)==0x03){
            carStatus.append("手动操作中：");
        }
        if(arr.at(1)==0x01){
            carStatus.append("前进");
        }else if(arr.at(1)==0x02){
            carStatus.append("后退");
        }else if(arr.at(1)==0x03){
            carStatus.append("投食");
        }else if(arr.at(1)==0x04){
            carStatus.append("停止");
        }else{
            carStatus.append("待机");
        }

        this->ui->label_carStatus->setText(carStatus);
    }
}

/********************************
 * 6000端口收到的报文及其处理
********************************/
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

/********************************
 * 数据库处理部分
********************************/
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

/********************************
 * 控制窗户部分
********************************/
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

/********************************
 * 控制小车部分
********************************/
//小车运行
void MainWindow::on_buttonRun_clicked()
{

    QByteArray data;
    data.resize(2);
    data[0]=0x01;
    data[1]=0x01;
    udpSocket_5000->writeDatagram(data,IP_car,7001);
}
//小车休眠
void MainWindow::on_buttonSleep_clicked()
{
    QByteArray data;
    data.resize(2);
    data[0]=0x00;
    data[1]=0x04;
    udpSocket_5000->writeDatagram(data,IP_car,7001);
}
//小车手动前进
void MainWindow::on_buttonForward_clicked()
{
    QByteArray data;
    data.resize(2);
    data[0]=0x00;
    data[1]=0x01;
    udpSocket_5000->writeDatagram(data,IP_car,7001);
}
//小车手动返回
void MainWindow::on_buttonBack_clicked()
{
    QByteArray data;
    data.resize(2);
    data[0]=0x00;
    data[1]=0x02;
    udpSocket_5000->writeDatagram(data,IP_car,7001);
}
//小车手动投料
void MainWindow::on_buttonThrow_clicked()
{
    QByteArray data;
    data.resize(2);
    data[0]=0x00;
    data[1]=0x03;
    udpSocket_5000->writeDatagram(data,IP_car,7001);
}
//小车手动停止
void MainWindow::on_buttonStop_clicked()
{
    QByteArray data;
    data.resize(2);
    data[0]=0x00;
    data[1]=0x04;
    udpSocket_5000->writeDatagram(data,IP_car,7001);
}
