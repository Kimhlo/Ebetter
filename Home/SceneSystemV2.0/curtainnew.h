#ifndef CURTAINNEW_H
#define CURTAINNEW_H

#include <QObject>
#include <QDateTime>
#include <QThread>
#include <QTimer>

class CurtainNew : public QObject
{
    Q_OBJECT
public:
    explicit CurtainNew(const int &usb1,const int &usb2=0,QObject *parent = 0);
    void openHalf(const int &id,const int &curtain);
    void open(const int &id, const int &curtain);
    void close(const int &id, const int &curtain);
    void stop(const int &id, const int &curtain);
    void setStatus(const int &id, const int &status);
private:
    int fd_usb0;
    int fd_usb1;

    QTimer *timer1;
    QTimer *timer2;
    QTimer *timer3;
    QTimer *timer4;
    QTimer *timer5;

    unsigned char sendData[24];
public slots:
    void stopOpenHalf1();
    void stopOpenHalf2();
    void stopOpenHalf3();
    void stopOpenHalf4();
    void stopOpenHalf5();


};


#endif // CURTAINNEW_H
