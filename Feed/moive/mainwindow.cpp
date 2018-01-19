#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QBuffer>
#include <QImageReader>
#include <QIODevice>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
//    //方法1
//    QPixmap pix;
//    pix.loadFromData(data,"JPG");
//    if(!pix.save("temp.jpg","JPG",100)){
//        qDebug()<<"保存失败";
//        QMessageBox::information(this,"警告","视频加载失败，请检查网络！");
//    }
//    if(!(image.load("temp.jpg"))){
//        showTime->stop();
//        return;
//    }
//    //显示收到的图像
//    ui->moiveLabel->setPixmap(QPixmap::fromImage(image));
//    ui->moiveLabel->resize(QSize(image.width(),image.height()));

    //方法2
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    QImageReader reader(&buffer,"jpg");
    QImage image2=reader.read();
    ui->moiveLabel->setScaledContents(true);
    ui->moiveLabel->setPixmap(QPixmap::fromImage(image2,Qt::AutoColor));

}

void MainWindow::updatePic()
{
    request.setUrl(QUrl(PU));//设置访问的网址
    manager->get(request);//用get方法获取图像
}
