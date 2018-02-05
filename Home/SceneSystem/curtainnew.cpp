#include "curtainnew.h"
#include <QDateTime>
#include <QThread>
#include <QDebug>
#include <wiringPi.h>
#include <wiringSerial.h>


int lace[5]=0;
int curtain[5]=0;

CurtainNew::CurtainNew(const int &usb1, QObject *parent) : QObject(parent)
{
    //init the timer
    timer1=new QTimer;
    timer2=new QTimer;
    timer3=new QTimer;
    timer4=new QTimer;
    timer5=new QTimer;
    fd_usb0=usb1;
    connect(timer1,SIGNAL(timeout()),this,SLOT(stopOpenHalf1()));
    connect(timer2,SIGNAL(timeout()),this,SLOT(stopOpenHalf2()));
    connect(timer3,SIGNAL(timeout()),this,SLOT(stopOpenHalf3()));
    connect(timer4,SIGNAL(timeout()),this,SLOT(stopOpenHalf4()));
    connect(timer5,SIGNAL(timeout()),this,SLOT(stopOpenHalf5()));
    //the order
    sendData[0]=0x7E;
}

int CurtainNew::checkStatus(const int &id)
{
    int status=0;
    //judge the id
    if(id==1){
        //茶室
        sendData[1]=0x01;
    }else if(id==2){
        //大厅
        sendData[1]=0x02;
    }else if(id==3){
        //老人房
        sendData[1]=0x01;
    }else if(id==4){
        //次卧
        sendData[1]=0x04;
    }else if(id==5){
        //主卧
        sendData[1]=0x03;
    }
    sendData[2]=0x05;
    sendData[3]=0x00;
    sendData[4]=0x7F;
    for(int i=0;i<5;i++)
    {
        serialPutchar(fd_usb0,sendData[i]);
    }

    delay(100);
    unsigned char recData[16];
    int k0=serialDataAvail (fd_usb0);

    for(int j=0;j<k0;j++){
        recData[j]=serialGetchar(fd_usb0);
    }
    for(int j=0;j<k0;j++){
        if(recData[j]==0x7E&&recData[j+2]==0x15&&recData[j+4]==0x7F){//
            if(recData[j+3]==0x00){
                status=1;//都闭合
            }else if(recData[j+3]==0x01){
                status=2;//纱帘闭合，窗帘打开
            }else if(recData[j+3]==0x10){
                status=3;//纱帘打开，窗帘闭合
            }else if(recData[j+3]==0x11){
                status=4;//都打开
            }else if(recData[j+3]==0xFF){
                status=5;//都停在任意位置
            }else if(recData[j+3]==0x0F){
                status=6;//纱帘闭合，窗帘停在任意位置
            }else if(recData[j+3]==0xF0){
                status=7;//纱帘闭合，窗帘停在任意位置
            }
        }
    }
    return status;



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
        sendData[1]=0x02;
    }else if(id==3){
        //老人房
        sendData[1]=0x01;
    }else if(id==4){
        //次卧
        sendData[1]=0x04;
    }else if(id==5){
        //主卧
        sendData[1]=0x03;
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
        serialPutchar(fd_usb0,sendData[i]);
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
    }else if(id==3){
        timer3->start(6000);
    }else if(id==4){
        timer4->start(6000);
    }else if(id==5){
        timer5->start(6000);
    }else{
        timer2->start(9000);
        setStatus(id,1);//开纱帘
    }
}

void CurtainNew::open(const int &id,const int &curtain)
{
    if(curtain==1){
        setStatus(id,1);//开纱帘

    }else if(curtain==2){
        setStatus(id,4);//开窗帘

    }
}

//close the realtive curtain
//curtain =1 is the lace curtain
//curtain =2 is the curtain
void CurtainNew::close(const int &id,const int &curtain)
{
    if(curtain==1){
        setStatus(id,2);//关纱帘

    }else if(curtain==2){
        setStatus(id,5);//关窗帘

    }
}

void CurtainNew::stop(const int &id, const int &curtain)
{
    if(curtain==1){
        setStatus(id,3);//停止纱帘

    }else if(curtain==2){
        setStatus(id,6);//停关窗帘

    }
}

/*****************************************************************************
* slot function is used to handle the timer out signal,
* when the realtive timer is time out it will run and stop the timer run again
******************************************************************************/
void CurtainNew::stopOpenHalf1()
{
    setStatus(1,6);
    setStatus(1,3);
    timer1->stop();
}
void CurtainNew::stopOpenHalf2()
{
    setStatus(2,6);
    setStatus(2,3);
    timer2->stop();
}
void CurtainNew::stopOpenHalf3()
{
    setStatus(3,6);
    setStatus(3,3);
    timer3->stop();
}
void CurtainNew::stopOpenHalf4()
{
    setStatus(4,6);
    setStatus(4,3);
    timer4->stop();
}
void CurtainNew::stopOpenHalf5()
{
    setStatus(5,6);
    setStatus(5,3);
    timer5->stop();
}

