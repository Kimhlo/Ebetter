#include "curtainnew.h"
#include <QDateTime>
#include <QThread>
#include <QDebug>
#include <wiringPi.h>
#include <wiringSerial.h>


int lace[5]=0;
int curtain[5]=0;

CurtainNew::CurtainNew(const int &usb1, const int &usb2, QObject *parent) : QObject(parent)
{

    //init the timer
    timer1=new QTimer;
    timer2=new QTimer;
    timer3=new QTimer;
    timer4=new QTimer;
    timer5=new QTimer;
    fd_usb0=usb1;
    fd_usb1=usb2;


    connect(timer1,SIGNAL(timeout()),this,SLOT(stopOpenHalf1()));
    connect(timer2,SIGNAL(timeout()),this,SLOT(stopOpenHalf2()));
    connect(timer3,SIGNAL(timeout()),this,SLOT(stopOpenHalf3()));
    connect(timer4,SIGNAL(timeout()),this,SLOT(stopOpenHalf4()));
    connect(timer5,SIGNAL(timeout()),this,SLOT(stopOpenHalf5()));

    //the order
    sendData[0]=0x7E;
}

/*****************************************************************
* id编号：1.茶室 2.大厅 3.老人房 4.次卧 5.主卧
* 状态号：1.开纱帘 2.关纱帘 3.停止纱帘 4.开窗帘 5.关窗帘 6.停止窗帘
******************************************************************/

void CurtainNew::setStatus(const int &id,const int &status)
{
    //judge the id
    if(id==1){
        //茶室
        sendData[1]=0x01;
    }else if(id==2){
        //大厅
        sendData[1]=0x04;
    }else if(id==3){
        //老人房
        sendData[1]=0x02;
    }else if(id==4){
        //次卧
        sendData[1]=0x03;
    }else if(id==5){
        //主卧
        sendData[1]=0x01;
    }
    //order
    sendData[2]=0x04;
    if(status==1){
        //开纱帘
        sendData[3]=0x12;
    }else if(status==2){
        //关纱帘
        sendData[3]=0x22;
    }else if(status==3){
        //停纱帘
        sendData[3]=0x02;
    }else if(status==4){
        //开布帘
        sendData[3]=0x11;
    }else if(status==5){
        //关布帘
        sendData[3]=0x21;
    }else if(status==6){
        //停布帘
        sendData[3]=0x01;
    }

    sendData[4]=0x7F;

    for(int i=0;i<5;i++)
    {
        if(id==1){
            serialPutchar(fd_usb1,sendData[i]);
        }else{
            serialPutchar(fd_usb0,sendData[i]);
        }
    }

    delay(100);

}

void CurtainNew::openHalf(const int &id,const int &curtain)
{
    if(curtain==1){
        setStatus(id,1);//开纱帘
    }else if(curtain==2){
        setStatus(id,4);//开窗帘
    }

    //start the timer
    if(id==1){
        timer1->start(6000);
    }else if (id==2) {
        timer2->start(9000);
    }else if(id==3){
        timer3->start(6000);
    }else if(id==4){
        timer4->start(6000);
    }else if(id==5){
        timer5->start(6000);
    }
}

void CurtainNew::open(const int &id,const int &curtain)
{
    if(curtain==1){
        setStatus(id,1);//开纱帘
        curtain1=1;
    }else if(curtain==2){
        setStatus(id,4);//开窗帘
        curtain2=1;
    }
}

//close the realtive curtain
//curtain =1 is the lace curtain
//curtain =2 is the curtain
void CurtainNew::close(const int &id,const int &curtain)
{
    if(curtain==1){
        setStatus(id,2);//关纱帘
        curtain1=0;
    }else if(curtain==2){
        setStatus(id,5);//关窗帘
        curtain2=0;
    }
}

void CurtainNew::stop(const int &id, const int &curtain)
{
    if(curtain==1){
        setStatus(id,3);//停止纱帘
        curtain1=0;
    }else if(curtain==2){
        setStatus(id,6);//停关窗帘
        curtain2=0;
    }
}

/*****************************************************************************
* slot function is used to handle the timer out signal,
* when the realtive timer is time out it will run and stop the timer run again
******************************************************************************/
void CurtainNew::stopOpenHalf1()
{
    setStatus(1,6);
    timer1->stop();
}
void CurtainNew::stopOpenHalf2()
{
    setStatus(2,6);
    timer2->stop();
}
void CurtainNew::stopOpenHalf3()
{
    setStatus(3,6);
    timer3->stop();
}
void CurtainNew::stopOpenHalf4()
{
    setStatus(4,6);
    timer4->stop();
}
void CurtainNew::stopOpenHalf5()
{
    setStatus(5,6);
    timer5->stop();
}

