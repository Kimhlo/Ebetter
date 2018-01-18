#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowTitle("Scene");
    ui->setupUi(this);
    scene=new Scene;
    scene->sleepMode(true);
    scene->setCurOpenTime(13);
//    scene->setAlarmTime(13);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    scene->stopAlarm();
}
