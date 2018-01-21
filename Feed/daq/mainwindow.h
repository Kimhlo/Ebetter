#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QProcess>

#include <QtNetwork/QUdpSocket>
#include <QHostAddress>
#include <QUdpSocket>
#include<QtNetwork/QTcpSocket>
#include <QProcess>
#include <QSettings>
#include "recthread.h"

namespace Ui {
class MainWindow;
class musicProcess;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;    //5000
    RecThread *myThread_1;
    Ui:: musicProcess *procress;

private slots:
    void on_pushButton_clicked();
    void UpdateSlot(int num1,int num2,int s);
    void readIni();
    void setIni();
    void on_lineEdit_IP_textEdited(const QString &arg1);
    void on_pushButton_Play_clicked();
};

#endif // MAINWINDOW_H
