#ifndef CURTAIN_H
#define CURTAIN_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QByteArray>
#include <QDateTime>
#include <QUdpSocket>
#include <QHostAddress>

struct position
{
    int curtain;
    int lace;
    int id;
};

class Curtain : public QObject
{
    Q_OBJECT
public:
    explicit Curtain(const int &usb1,QObject *parent = 0);

    void setPostion(const int &id, const int &curtain,const int &position);
    void open(const int &id, const int &curtain);
    void close(const int &id, const int &curtain);
    void stop(const int &id, const int &curtain);
    int checkStatus(const int &id);
    void inTimeOpenAndClose(const int &id,const int &time);
    void setID(const int &id);
private:
    void setStatus(const int &id, const int &status);
    unsigned char sendData[7];
    unsigned char udpRecData[10];
    int status=0;
    int status_id=0;
    int fd_usb0;
    QTimer *timer1;

signals:

public slots:
    void openAndClose();
};

#endif // CURTAIN_H
