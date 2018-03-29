#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <wiringPi.h>
#include <wiringSerial.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int usb0 = serialOpen("/dev/ttyUSB0",9600);
    cur=new Curtain(usb0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    cur->close(ui->lineEdit->text().toInt(),ui->lineEdit_2->text().toInt());
}

void MainWindow::on_pushButton_2_clicked()
{
    cur->open(ui->lineEdit->text().toInt(),ui->lineEdit_2->text().toInt());
}

void MainWindow::on_pushButton_3_clicked()
{
    cur->setPostion(ui->lineEdit->text().toInt(),ui->lineEdit_2->text().toInt(),ui->lineEdit_3->text().toInt());
}

void MainWindow::on_pushButton_4_clicked()
{
    cur->inTimeOpenAndClose(ui->lineEdit->text().toInt(),ui->lineEdit_4->text().toInt());
}