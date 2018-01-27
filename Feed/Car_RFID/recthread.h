#ifndef RECTHREAD_H
#define RECTHREAD_H

#include <global.h>
#include <QtCore>
#include <QThread>

//ds18b20
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define  BUFSIZE  128
//ds18b20

class RecThread:public QThread
{
    Q_OBJECT

private:

protected:
    void run();

public:
    RecThread(QObject *parent=0);
    ~RecThread();
private:
    void initCar();

signals:
    void UpdateSignal(int num1,int num2,int s);

public slots:
    void ResetSlot();
    void forword();
    void back();
    void throwFood();
    void stop();

};
#endif // RECTHREAD_H
