#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <philipshue.h>
#include <dm836ii.h>
#include <curtainnew.h>
#include <QDebug>
#include <QString>
#include <QUdpSocket>
#include <QByteArray>
#include <QThread>
#include <QDateTime>

/*****************************************
 * 全局变量的定义，包括hue灯的结构体和底层灯光控制
 * 系统发送的开关状态接受的数据recData，以及当前
 * 的模式mode
******************************************/
struct hueData
{
    float color[2];
    int bri;
    int ct;
};

extern hueData lightData[12];
extern QByteArray recData;
extern int mode;
extern int briNow;
extern int doorStates[8];
/*****************************************
 * 时间检查线程的定义
******************************************/
class CheckThread:public QThread
{
    Q_OBJECT
public:
    explicit CheckThread(QObject *parent=0);
    QDateTime timeNow;
    int curCloseNight=22;
    int curOpenMorning=7;
    int curOpenAllTime=8;
    int alarmOpenTime;
    bool alarmStatus=false;
private:
    int colorChangeTime[10]={22,23,0,1,2,3,4,5,6,7};
    int ctChangeTime[8]={23,00,1,2,3,4,5,6};
protected:
    void run();
signals:
    void hueInfoChanged();
    void hueCtHasChaged();
    void onAlarm();
    void onCurtainChanged(int id,int i);

};

/*****************************************
 * 场景类的定义
******************************************/
class Scene : public QObject
{
    Q_OBJECT
public:
    explicit Scene(const int &usb1, const int &usb2, QObject *parent = 0);
    void sleepMode(const bool &state);
    void setCurCloseTime(const int&time);
    void setCurOpenTime(const int&time);
    void setAlarmTime(const int&time);
    void setAllCurtainOpenTime(const int&time);
    void stopAlarm();
    void openAllCurtain();
private:
    //light status
    int lightStatus[12];
    int lightPerStatus[12];
    PhilipsHue *hue;
    //network
    QUdpSocket *udpSocket;
    int port;
    QUdpSocket *curtainSocket;
    int curtainPort;
    int times;//received the udp datagram times
    //the thread that set the change and emit the signal
    CheckThread *timeThread;
    //the light id
    int id[12]={19,25,37,30,34,36,39,32,27,31,29,20};
    //operate the light
    void setStatus(const int &i);
    void runFirstTime(const int &i);
    void openLights();
    //the curtain
    CurtainNew *curtain1;
    QTimer *curtainTimer;
    int curtainStatus;
    //the music
    DM836II *radio1;
signals:

public slots:
    void udpRecData();
    void updateScene();
    void openAlarm();
    void operateCurtain(int id,int i);
    void updateCt();
    void curtainAndDoor();
    void curtainTimeOut();
};

#endif // SCENE_H


