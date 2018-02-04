#ifndef RECTHREAD_H
#define RECTHREAD_H

#include <global.h>
#include <QtCore>
#include <QThread>

#include "wiringPi.h"
#include "wiringSerial.h"


#define  BUFSIZE  128
//ds18b20

class RecThread:public QThread
{
    Q_OBJECT

private:

        int kk[25];
        int fd_usb0,fd_usb1;


protected:
    void run();

public:
    RecThread(QObject *parent=0);
    ~RecThread();
signals:
    void UpdateSignal(int num1,int num2,int s);

public slots:
    void bit_set(unsigned char *p_data, unsigned char position,int flag);
    void ResetSlot();

    void SePintMode();
    void SetSensorLightBit(int p,int val);
    void SetLightBit(int io_light,int val);

};
#endif // RECTHREAD_H
