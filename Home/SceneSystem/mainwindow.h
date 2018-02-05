#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <scene.h>
#include <QUdpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Scene *scene;
    QUdpSocket *udp;
    CurtainNew *curtain;
    QTimer *timer;
private slots:
    void on_pushButton_clicked();
    void closeCurtain();
    void isTimeOut();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
