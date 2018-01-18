#include "scene.h"
#include <QDebug>
#include <philipshue.h>

QByteArray recData;
hueData lightData[12];
int mode=0;

Scene::Scene(QObject *parent) :
    QObject(parent)
{
    times=0;
    //init the hue struct
    for(int i=0;i<12;i++){
        lightData[i].bri=180;
        lightData[i].color[0]=0.258841;
        lightData[i].color[1]=0.355881;
        lightPerStatus[i]=0;
    }
    //创建hue灯光指针
    hue=new PhilipsHue;
    //
    radio1=new DM836II(1);
    //
    curtain1=new CurtainNew;
    //upd通信
    port=6000;
    udpSocket=new QUdpSocket;
    bool bindPort;
    bindPort=udpSocket->bind(port,QUdpSocket::ShareAddress);
    if(bindPort==false){
        qDebug()<<"bind port:5009 error";
        return;
    }
    curtainSocket=new QUdpSocket;

    //创建timeThread
    timeThread=new CheckThread(this);
    timeThread->start();

    //connct the signals and slots
    connect(udpSocket,SIGNAL(readyRead()),
            this,SLOT(udpRecData()));
    connect(timeThread,SIGNAL(hueInfoChanged()),
            this,SLOT(updateScene()));
    connect(timeThread,SIGNAL(onAlarm()),
            this,SLOT(openAlarm()));
    connect(timeThread,SIGNAL(onCurtainChanged(int,int)),
            this,SLOT(operateCurtain(int,int)));

}

void Scene::sleepMode(const bool &state)
{
    if(state){
        mode=1;
    }
}

void Scene::setCurCloseTime(const int &time)
{
    timeThread->curCloseNight=time;
}

void Scene::setCurOpenTime(const int &time)
{
    timeThread->curOpenMorning=time;
}

void Scene::setAlarmTime(const int &time)
{
    timeThread->alarmOpenTime=time;
}

void Scene::stopAlarm()
{
   radio1->pause();
}


void Scene::openLights()
{
    for(int i=0;i<12;i++){
        if(recData.at(i)==0x01){
            lightStatus[i]=1;
        }else if(recData.at(i)==0x00){
            lightStatus[i]=0;
        }

        if(times==1){
            runFirstTime(i);
        }else{
            setStatus(i);
        }
        lightPerStatus[i]=lightStatus[i];
    }
}

void Scene::runFirstTime(const int &i)
{
    if(i==1){
        //一楼卫生间
        hue->groupControl(12,lightStatus[i],lightData[i].color,lightData[i].bri);

    }else if (i==4) {
        //二楼次卫生间
        qDebug()<<"updata"<<lightPerStatus[4];
        hue->groupControl(14,lightStatus[i],lightData[i].color,lightData[i].bri);

    }else if(i==6){
        //二楼主卫生间
        hue->groupControl(13,lightStatus[i],lightData[i].color,lightData[i].bri);

    }else if(i==8){
        //圆灯
        hue->groupControl(10,lightStatus[i],lightData[i].color,lightData[i].bri);

    }else if(i==11){
        //西厨
        hue->lightsStatus(id[i],lightStatus[i]);
        hue->lightsStatus(21,lightStatus[i]);
        hue->lightsStatus(22,lightStatus[i]);
        hue->lightsStatus(23,lightStatus[i]);
        hue->lightsStatus(38,lightStatus[i]);

    }else{
        hue->lightsStatus(id[i],lightStatus[i]);
        hue->lightsBri(id[i],lightData[i].bri);
        hue->lightsColor(id[i],lightData[i].color) ;
    }
}

void Scene::udpRecData()
{
    while(udpSocket->hasPendingDatagrams()){
        recData.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(recData.data(),recData.size());
    }
    times+=1;
//    qDebug()<<"getData"<<recData.toHex();

    openLights();

}

void Scene::updateScene()
{
    for(int i=0;i<12;i++){
        //set the hue light id

        if(i==1&&lightStatus[1]==1){
            //一楼卫生间
            hue->groupControl(12,lightStatus[i],lightData[i].color,lightData[i].bri);

        }
        if (i==4&&lightStatus[4]==1) {
            //二楼次卫生间
            qDebug()<<"updata"<<lightPerStatus[4];
            hue->groupControl(14,lightStatus[i],lightData[i].color,lightData[i].bri);

        }
        if(i==6&&lightStatus[6]==1){
            //二楼主卫生间
            hue->groupControl(13,lightStatus[i],lightData[i].color,lightData[i].bri);

        }
        if(i==8&&lightStatus[8]==1){
            //圆灯
            hue->groupControl(10,lightStatus[i],lightData[i].color,lightData[i].bri);

        }
        if(i==11&&lightStatus[11]==1){
            //西厨
            hue->lightsStatus(id[i],lightStatus[i]);
            hue->lightsStatus(21,lightStatus[i]);
            hue->lightsStatus(22,lightStatus[i]);
            hue->lightsStatus(23,lightStatus[i]);
            hue->lightsStatus(38,lightStatus[i]);

        }
        if(lightStatus[i]==1){
            hue->lightsStatus(id[i],lightStatus[i]);
            hue->lightsBri(id[i],lightData[i].bri);
            hue->lightsColor(id[i],lightData[i].color) ;
        }
    }

}

void Scene::openAlarm()
{
    //play the music
    radio1->setSongSource(1);
    radio1->playNow(1);
}

void Scene::operateCurtain(int id,int i)
{
    if(i==1){
        for(int j=1;j<6;j++){
            curtain1->close(j,1);
            curtain1->close(j,2);
        }
    }else if(i==2){
        qDebug()<<"inOpenHalf";
        curtain1->openHalf(id,2);//openHlaf
    }

}

void Scene::setStatus(const int &i)
{
    if(i==1&&lightPerStatus[1]!=lightStatus[1]){

        //一楼卫生间
        hue->groupControl(12,lightStatus[i],lightData[i].color,lightData[i].bri);

    }else if (i==4&&lightPerStatus[4]!=lightStatus[4]) {
        //二楼次卫生间
        hue->groupControl(14,lightStatus[i],lightData[i].color,lightData[i].bri);

    }else if(i==6&&lightPerStatus[6]!=lightStatus[6]){
        //二楼主卫生间
        hue->groupControl(13,lightStatus[i],lightData[i].color,lightData[i].bri);

    }else if(i==8&&lightPerStatus[8]!=lightStatus[8]){
        //圆灯
        hue->groupControl(10,lightStatus[i],lightData[i].color,lightData[i].bri);

    }else if(i==11&&lightPerStatus[11]!=lightStatus[11]){
        //西厨
        hue->lightsStatus(id[i],lightStatus[i]);
        hue->lightsStatus(21,lightStatus[i]);
        hue->lightsStatus(22,lightStatus[i]);
        hue->lightsStatus(23,lightStatus[i]);
        hue->lightsStatus(38,lightStatus[i]);

    }else{
        if(lightPerStatus[i]!=lightStatus[i]){
            hue->lightsStatus(id[i],lightStatus[i]);
            hue->lightsBri(id[i],lightData[i].bri);
            hue->lightsColor(id[i],lightData[i].color) ;
        }
    }
}

/****************************************
* class check the time thread
****************************************/
CheckThread::CheckThread(QObject *parent)
{

}

void CheckThread::run()
{
    int hour,perHour;
    int briChangeTime[10]={100,80,70,70,70,70,70,70,100,150};
    float colorHour[20]={0.640075,0.329971,//红色
                         0.403924,0.19988,//淡紫色
                         0.452269,0.479094,//橙色
                         0.419321,0.505255,//黄橙色
                         0.510352,0.432974,//深黄色
                         0.18725,0.19406,//淡蓝色
                         0.150017,0.0600066,//深蓝色
                         0.224658,0.328741,//淡青色
                         0.325305,0.402063,//青黄色
                         0.312716,0.329001};//白色

    int flagAlarm=0;
    int flagOpen=0;
    int flagClose=0;
    while(1){
        timeNow=QDateTime::currentDateTime();
        hour=timeNow.toString("hh").toInt();
        if(mode==1){
            for(int i=0;i<10;i++){
                if(hour==colorChangeTime[i]&&hour!=perHour){
                    for(int j=0;j<12;j++){//change the color
                        lightData[j].color[0]=colorHour[2*i];
                        lightData[j].color[1]=colorHour[2*i+1];
                    }
                    lightData[i].bri=briChangeTime[i];
                    emit hueInfoChanged();
                }
            }
            perHour=hour;
            //set the alarm
            if(hour==alarmOpenTime&&flagAlarm==0){
                emit onAlarm();//emit the alarm that is time to get up
                qDebug()<<"emit onalarm";
                flagAlarm+=1;
            }else if(hour==(alarmOpenTime+1)){
                flagAlarm=0;
            }
            //close the curtain in the night
            if(hour==curCloseNight&&flagClose==0){
                emit onCurtainChanged(5,1);
                qDebug()<<"emit closeCurtain";
                flagClose=1;
            }else if(hour==(curCloseNight+1)){
                flagClose=0;
            }
            //open the curtain in the morning
            if(hour==curOpenMorning&&flagOpen==0){
                emit onCurtainChanged(5,2);
                qDebug()<<"emit openHalf";
                flagOpen=1;
            }else if (hour==curOpenMorning+1) {
                flagOpen=0;
            }

        }
    }

}
