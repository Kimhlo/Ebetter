#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QByteArray>
#include <QDebug>
#include <QTimer>
#include <QString>
#include <QPixmap>
#include <QImage>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTimer *showTime;//获取图像的定时器
    QImage image;//图像
    QImage *scaledImage;//缩放后的图像

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QString Port,Adress,PU;

public slots:
    void getReply(QNetworkReply *reply);
    void updatePic();

};

#endif // MAINWINDOW_H
