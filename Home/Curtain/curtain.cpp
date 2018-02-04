#include "curtain.h"
#include <wiringPi.h>
#include <wiringSerial.h>
#include <QByteArray>

Curtain::Curtain(const int &usb, QObject *parent): QObject(parent)
{
    fd_usb0=usb;

    //set the sendData
    sendData[0]=0x88;//STX
    sendData[6]=0xFF;//ETX
    timer1=new QTimer;

    connect(timer1,SIGNAL(timeout()),
            this,SLOT(openAndClose()));
}


/******************************************
 * id: is the curtain ID;
 * curtain: 1 is the curtain;2 is the lace;
 * position: 0-100;
******************************************/

void Curtain::setPostion(const int &id, const int &curtain,const int &position)
{
    sendData[1]=id; //ID
    sendData[2]=0x02;//CMD:行程模式
    sendData[3]=curtain;
    if(curtain==1){
        sendData[4]=position;//curtain position
        sendData[5]=0xFF; //lace position
    }else{
        sendData[4]=0xFF;
        sendData[5]=position; //lace position
    }

    for(int i=0;i<7;i++){
        serialPutchar(fd_usb0,sendData[i]);
    }
    delay(100);
}

void Curtain::open(const int &id, const int &curtain)
{
    if(curtain==1){
        setStatus(id,4);
    }else{
        setStatus(id,1);
    }
}

void Curtain::close(const int &id, const int &curtain)
{
    if(curtain==1){
        setStatus(id,5);
    }else{
        setStatus(id,2);
    }
}

void Curtain::stop(const int &id, const int &curtain)
{
    if(curtain==1){
        setStatus(id,6);
    }else{
        setStatus(id,3);
    }
}

int Curtain::checkStatus(const int &id)
{
    sendData[1]=id;
    sendData[2]=0x10;//CMD:查询模式
    sendData[3]=0xFF;
    sendData[4]=0xFF;
    sendData[5]=0xFF;
    int status=0;

    for(int i=0;i<7;i++){
        serialPutchar(fd_usb0,sendData[i]);
    }
    delay(100);
    int k0=serialDataAvail (fd_usb0);
    unsigned char data[64];
    if(k0>6){
        if(k0>62) k0=16;
        for(int i=0;i<k0;i++){
            data[i]=serialGetchar(fd_usb0);
        }
        for(int i=0;i<k0;i++){
            if(data[i]==0x88&&data[i+2]==0x90&&data[i+6]==0xFF){

                if(int(data[i+4])>=60&&int(data[i+5])>=60){
                    status=1;//布帘和纱帘都关闭
                }else if(int(data[i+4])<60&&int(data[i+5])>=60){
                    status=2;//布帘打开纱帘关闭
                }else if(int(data[i+4])>=60&&int(data[i+5])<60){
                    status=3;//纱帘打开布帘关闭
                }else{
                    status=4;//纱帘布帘都打开
                }

            }//end if data
        }//end i<k0
    }//end if k0>6

    return status;

}

void Curtain::inTimeOpenAndClose(const int &id, const int &time)
{
    status=checkStatus(id);
    status_id=id;
    if(status==1){
        setPostion(id,1,50);
        setPostion(id,2,50);
        timer1.start(time);
    }else if(status==2){
        setPostion(id,2,50);
        timer1.start(time);
    }else if(status==3){
        setPostion(id,1,50);
        timer1.start(time);
    }

}

void Curtain::  setID(const int &id)
{
    sendData[1]=0xFF;
    sendData[2]=0xAA;//CMD:设置模式
    sendData[3]=id;//纱帘
    sendData[4]=0xFF;
    sendData[5]=0xFF;

    for(int i=0;i<7;i++){
        serialPutchar(fd_usb0,sendData[i]);
    }
    delay(100);
}

void Curtain::setStatus(const int &id, const int &status)
{
    sendData[1]=id;
    sendData[2]=0x01;//CMD:单控模式
    if(status==1){
        sendData[3]=0x02;//纱帘
        sendData[4]=0x00;
        sendData[5]=0x01;//开
    }else if(status==2){
        sendData[3]=0x02;//纱帘
        sendData[4]=0x00;
        sendData[5]=0x02;//关
    }else if(status==3){
        sendData[3]=0x02;//纱帘
        sendData[4]=0x00;
        sendData[5]=0x04;//停
    }else if(status==4){
        sendData[3]=0x01;//布帘
        sendData[4]=0x00;
        sendData[5]=0x01;//开
    }else if(status==5){
        sendData[3]=0x01;//布帘
        sendData[4]=0x00;
        sendData[5]=0x02;//关
    }else if(status==6){
        sendData[3]=0x01;//布帘
        sendData[4]=0x00;
        sendData[5]=0x04;//关
    }
    for(int i=0;i<7;i++){
        serialPutchar(fd_usb0,sendData[i]);
    }
    delay(100);
}

void Curtain::openAndClose()
{
    timer1.stop();
    if(status==1){
        setPostion(status_id,1,100);
        setPostion(status_id,2,100);
    }else if(status==2){
        setPostion(status_id,2,100);
    }else if(status==3){
        setPostion(status_id,1,100);
    }
}


