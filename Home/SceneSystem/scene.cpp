/****************************************************
 *......................我佛慈悲.....................
 *                       _oo0oo_
 *                      o8888888o
 *                      88" . "88
 *                      (| -_- |)
 *                      0\  =  /0
 *                    ___/`---'\___
 *                  .' \\|     |// '.
 *                 / \\|||  :  |||// \
 *                / _||||| -卍-|||||- \
 *               |   | \\\  -  /// |   |
 *               | \_|  ''\---/''  |_/ |
 *               \  .-\__  '-'  ___/-. /
 *             ___'. .'  /--.--\  `. .'___
 *          ."" '<  `.___\_<|>_/___.' >' "".
 *         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *         \  \ `_.   \_ __\ /__ _/   .-` /  /
 *     =====`-.____`.___ \_____/___.-`___.-'=====
 *                       `=---='
 *
 *..................佛祖开光 ,永无BUG.................
 *场景部分：
 * 1.主要用于与底层硬件传感器触发的联动；
 * 2.不同模式的设置；
*****************************************************/
#include "scene.h"
#include <QDebug>
#include <philipshue.h>

QByteArray recData;
hueData lightData[12];
int mode=0;
int briNow=100;
int doorStates=0;
Scene::Scene(const int &usb1, const int &usb2, QObject *parent) :QObject(parent)
{

    times=0;
    //init the hue struct
    for(int i=0;i<12;i++){
        lightData[i].bri=180;
        lightData[i].color[0]=0.396154; //white黄色0.396154 0.412773
        lightData[i].color[1]=0.452974;
        lightData[i].ct=300;
        lightPerStatus[i]=0;
    }
    //创建hue灯光指针
    hue=new PhilipsHue;
    //创建音响的指针
    radio1=new DM836II(usb1,1);
    //创建窗帘的指针
    curtain1=new CurtainNew(usb2);
    //upd通信
    port=6000;
    udpSocket=new QUdpSocket;
    bool bindPort;
    bindPort=udpSocket->bind(port,QUdpSocket::ShareAddress);
    if(bindPort==false){
        qDebug()<<"bind port:5009 error";
        return;
    }
    //窗帘通信
//    curtainSocket=new QUdpSocket;
//    curtainPort=6666;
//    curtainSocket->bind(curtainPort);
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
    connect(timeThread,SIGNAL(hueCtHasChaged()),
            this,SLOT(updateCt()));


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

void Scene::setAllCurtainOpenTime(const int &time)
{
    timeThread->curOpenAllTime=time;
}

void Scene::stopAlarm()
{
    radio1->pause();
}

void Scene::openAllCurtain()
{
    //打开所有的窗帘
    curtain1->open(2,1);
    curtain1->open(2,2);
    curtain1->open(3,1);
    curtain1->open(3,2);
    curtain1->open(4,1);
    curtain1->open(4,2);
    curtain1->open(5,1);
    curtain1->open(5,2);
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

    doorPreStatus[0]=int(recData[12]);
    doorPreStatus[1]=int(recData[13]);
    doorPreStatus[2]=int(recData[14]);
}

void Scene::runFirstTime(const int &i)
{
    if(i==1){
        //一楼卫生间
        hue->groupControl(12,lightStatus[i],lightData[i].color,lightData[i].bri);

    }else if (i==4) {
        //二楼次卫生间
        hue->groupControl(14,lightStatus[i],lightData[i].color,lightData[i].bri);

    }else if(i==6){
        //二楼主卫生间
        hue->groupControl(13,lightStatus[i],lightData[i].color,lightData[i].bri);
    }else if(i==8){
        //圆灯
        hue->groupControl(10,lightStatus[i],lightData[i].color,lightData[i].bri);

    }else if(i==11){
        //西厨
        hue->lightsCt(38,lightStatus[i],lightData[i].bri);
        hue->groupStatus(11,lightStatus[i]);
    }else{
        hue->lightsCt(id[i],lightStatus[i],lightData[i].bri);
    }
}

void Scene::udpRecData()
{
    while(udpSocket->hasPendingDatagrams()){
        recData.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(recData.data(),recData.size());
    }
    times+=1;

    //according the door status to change the light status
    if(recData[12]==0x00){

        recData[1]=0x01;
    }
    if(recData[13]==0x00){
        recData[4]=0x01;
    }
    if(recData[14]==0x00){
        recData[6]=0x01;
    }
    openLights();
}

void Scene::updateScene()
{
    //循环两次以防信息没有接收成功
    for(int j=0;j<2;j++){
        for(int i=0;i<12;i++){
            //set the hue light id z
            if(i==1&&lightStatus[1]==1){
                //一楼卫生间
                hue->groupControl(12,lightStatus[i],lightData[i].ct,lightData[i].bri);
            }
            if (i==4&&lightStatus[4]==1) {
                //二楼次卫生间
                hue->groupControl(14,lightStatus[i],lightData[i].ct,lightData[i].bri);
            }
            if(i==6&&lightStatus[6]==1){
                //二楼主卫生间
                hue->groupControl(13,lightStatus[i],lightData[i].ct,lightData[i].bri);
            }
            if(i==8&&lightStatus[8]==1){
                //圆灯
                hue->groupControl(10,lightStatus[i],lightData[i].ct,lightData[i].bri);
            }
            if(i==11&&lightStatus[11]==1){
                //西厨
                hue->lightsCt(38,lightStatus[i],lightData[i].ct,lightData[i].bri);
                hue->groupControl(11,lightStatus[i],lightData[i].ct,lightData[i].bri);
            }
            if(lightStatus[i]==1){
                hue->lightsCt(id[i],lightStatus[i],lightData[i].ct,lightData[i].bri);
            }
        }
    }
}

void Scene::openAlarm()
{
    //play the music
    radio1->setSongSource(1);
    radio1->setVolume(7);
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
    }else if(i==3){
        openAllCurtain();
    }

}

void Scene::updateCt()
{
    for(int k=0;k<2;k++){
        for(int i=0;i<12;i++){
            //set the hue light id
            if(i==1&&lightStatus[1]==1){
                //一楼卫生间
                hue->groupCt(12,lightData[i].ct,lightData[i].bri);
            }
            if (i==4&&lightStatus[4]==1) {
                //二楼次卫生间
                hue->groupCt(14,lightData[i].ct,lightData[i].bri);
            }
            if(i==6&&lightStatus[6]==1){
                //二楼主卫生间
                hue->groupCt(13,lightData[i].ct,lightData[i].bri);
            }
            if(i==8&&lightStatus[8]==1){
                //圆灯
                hue->groupCt(10,lightData[i].ct,lightData[i].bri);
            }
            if(i==11&&lightStatus[11]==1){
                //西厨
                hue->lightsCt(38,lightData[i].ct);
                hue->groupCt(11,lightData[i].ct,lightData[i].bri);//light 21,22,23,24
            }
            if(lightStatus[i]==1){
                hue->lightsCt(id[i],lightData[i].ct,lightData[i].bri);
            }
        }

    }

}

void Scene::setStatus(const int &i)
{

    int hour=QDateTime::currentDateTime().toString("hh").toInt();
    //first floor WC

    if(i==1&&(lightPerStatus[1]!=lightStatus[1]||doorPreStatus[0]!=int(recData[12]))){
        if((hour<=7||hour>=22)&&int(recData[12])==0){//first floor
            lightData[1].bri=160;
            qDebug()<<"bri=160";
        }else if((hour>7&&hour<22)&&int(recData[12])==0){
            lightData[1].bri=240;
        }else{
            lightData[1].bri=briNow;
        }
        hue->groupControl(12,lightStatus[i],lightData[i].ct,lightData[i].bri);

    }else if (i==4&&(lightPerStatus[4]!=lightStatus[4]||doorPreStatus[1]!=int(recData[13]))) {
        //二楼次卫生间
        //second WC in second floor
        qDebug()<<"recSecond="<<int (recData[13]);
        if((hour<=7||hour>=22)&&int(recData[13])==0){//sconed floor wc
            lightData[4].bri=170;
            qDebug()<<"bri4=160";
        }else if((hour>7&&hour<22)&&int(recData[13])==0){
            lightData[4].bri=240;
        }else{
            lightData[4].bri=briNow;
            qDebug()<<"briNow4="<<lightData[4].bri;
        }
        hue->groupControl(14,lightStatus[i],lightData[i].ct,lightData[i].bri);

    }else if(i==6&&(lightPerStatus[6]!=lightStatus[6]||doorPreStatus[2]!=int(recData[14]))){
        //main WC in second floor
        if((hour<=7||hour>=22)&&int(recData[14])==0){//sconed floor main wc
            lightData[6].bri=160;
            qDebug()<<"bri6=160";
        }else if((hour>7&&hour<22)&&int(recData[14])==0){
            lightData[6].bri=240;
        }else{
            lightData[6].bri=briNow;
            qDebug()<<"briNow6="<<lightData[6].bri;
        }
        //二楼主卫生间
         hue->groupControl(13,lightStatus[i],lightData[i].ct,lightData[i].bri);

    }else if(i==8&&lightPerStatus[8]!=lightStatus[8]){
        //圆灯
        hue->groupControl(10,lightStatus[i],lightData[i].ct,lightData[i].bri);

    }else if(i==11&&lightPerStatus[11]!=lightStatus[11]){
        //西厨
//        qDebug()<<"light west"<<lightStatus[i];
        hue->lightsCt(38,lightStatus[i],lightData[i].ct,lightData[i].bri);
        hue->groupControl(11,lightStatus[i],lightData[i].ct,lightData[i].bri);

    }else{
        if(lightPerStatus[i]!=lightStatus[i]){
            //灯
            hue->lightsStatus(id[i],lightStatus[i]);
            hue->lightsBri(id[i],lightData[i].bri);
//            hue->lightsCt(id[i],lightStatus[i],lightData[i].ct,lightData[i].bri);
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

    int hour,perHour,min,perMin;
    int briChangeTime[8]={80,60,50,50,50,50,50,250};
    int ct[8]={500,460,420,380,340,300,260,220};
//    float colorHour[20]={0.640075,0.329971,//红色
//                         0.403924,0.19988,//淡紫色
//                         0.452269,0.479094,//橙色
//                         0.419321,0.505255,//黄橙色
//                         0.510352,0.432974,//深黄色
//                         0.18725,0.19406,//淡蓝色
//                         0.150017,0.0600066,//深蓝色
//                         0.224658,0.328741,//淡青色
//                         0.325305,0.402063,//青黄色
//                         0.312716,0.329001};//白色

    int flagAlarm=0;
    int flagOpen1=0;
    int flagOpen2=0;
    int flagClose=0;
    while(1){
        timeNow=QDateTime::currentDateTime();
        hour=timeNow.toString("hh").toInt();
        min=timeNow.toString("mm").toInt();

        if(mode==1){
            for(int i=0;i<8;i++){//check the hour
                if(hour==ctChangeTime[i]){
                    for(int j=0;j<4;j++){//check the min
                        if(perMin==59) {
                            perMin=-1;
                        }
                        if(min==(j*15)&&perMin<min){
                            //emit the ct change signal
                            qDebug()<<"ctChanged"<<ct[i]-10*j;
                            emit hueCtHasChaged();
                            //change the ct data
                            for(int k=0;k<12;k++){
                                lightData[k].ct=ct[i]-10*j;
                                lightData[k].bri=briChangeTime[i];
                                briNow=briChangeTime[i];

                            }
                        }
                    }
                }
            }
            perMin=min;
            perHour=hour;
            msleep(100);
            //set the alarm
//            int min=QDateTime::currentDateTime().toString("mm").toInt();
            if(hour==curOpenMorning&&flagAlarm==0){
                emit onAlarm();//emit the alarm that is time to get up
                qDebug()<<"emit onalarm";
                flagAlarm+=1;
            }else if(hour==(curOpenMorning+1)){
                flagAlarm=0;
            }
            msleep(200);
            //close the curtain in the night
            if(hour==curCloseNight&&flagClose==0){
                emit onCurtainChanged(5,1);
                qDebug()<<"emit closeCurtain";
                flagClose=1;
            }else if(hour==(curCloseNight+1)){
                flagClose=0;
            }
            msleep(200);
            //open the curtain in the morning
            if(hour==curOpenMorning&&flagOpen1==0){
                emit onCurtainChanged(5,2);
                qDebug()<<"emit openHalf";
                flagOpen1=1;
            }else if (hour==curOpenMorning+1) {
                flagOpen1=0;
            }
            msleep(200);
            //open the curtain in the morning
            if(hour==curOpenAllTime&&flagOpen2==0){
                emit onCurtainChanged(5,3);
                qDebug()<<"emit openAll";
                flagOpen2=1;
            }else if (hour==curOpenAllTime+1) {
                flagOpen2=0;
            }

        }//end mode

    }

}



