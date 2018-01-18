#include "dm836ii.h"
#include <wiringPi.h>
#include <wiringSerial.h>
#include <QDebug>

/**************************************************************
**accroding to the data foramt define the relative byte
**heard1+header2+number+device type+channel+ordercode+function code+
**data length+data content+check sum
**encode method is CRC-8 and it including
**channel+ordercode+function code+data length+data content
*****************************************************************/
#define HEADER1 0xFC //the data header1
#define HEADER2 0xFE //the data header2
#define NUMBER 0x03 //the number,there is invalid
#define DEVICE_TYPE 0x08 //the device type is DM836II

#define SET 0x08  //set the radio status
#define QUERY 0x80 //query the radio status
//function code
#define VOLUME 0x06
#define SOURCE 0x07

DM836II::DM836II(const int &id,QObject *parent) :
    QObject(parent)
{
    if((fd_usb0 = serialOpen("/dev/ttyUSB0",38400)) < 0)
                qDebug()<<"open usb0 error\n";
    data[0]=HEADER1;
    data[1]=HEADER2;
    data[2]=NUMBER;
    data[3]=DEVICE_TYPE;
    data[4]=id;
}

//this function is used to set the sound volume;
void DM836II::setVolume(const int &vol)
{

    int volume=vol;

    if(volume>100){
        volume=100;
    }else if (volume<0) {
        volume=0;
    }

    data[5]=0x05;//function code
    data[6]=0x02;//data length
    data[7]=SET;//data content 1
    data[8]=volume;//data content 2
    data[9]=(data[4]+data[5]+data[6]+data[7]+data[8]);//


    //send the data
    for(int i=0;i<10;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }

    delay(400);

}

int DM836II::nowVolume()
{
    data[5]=0x05;//function code
    data[6]=0x01;//data length
    data[7]=QUERY;//data content 1
    data[8]=(data[4]+data[5]+data[6]+data[7]);//


    //send the data
    for(int i=0;i<9;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }

    delay(400);

    unsigned char recData[16];
    int k0=serialDataAvail (fd_usb0);

    for(int j=0;j<k0;j++){
        recData[j]=serialGetchar(fd_usb0);
        if(j==k0-1) qDebug()<<"get now volume";
    }
    for(int j=0;j<k0;j++){
        if(recData[j]==HEADER1&&recData[j+1]==HEADER2){
            return int(recData[7]);
        }

    }
}

/**********************
 * set the where play
 * 0 inter music
 * 1 SD card music
 * 2 Bluetooth music
 * 3 net radio
*********************/
void DM836II::setSongSource(const int &source=2)
{
    data[5]=0x07;//function code
    data[6]=0x02;//data length
    data[7]=SET;//data content 1
    if(source==0) data[8]=0x01;//inter music
    else if(source==1) data[8]=0x91;//SD
    else if(source==2) data[8]=0xA1;//Bluetooth
    else if(source==3) data[8]=0xC1;//net
    data[9]=(data[4]+data[5]+data[6]+data[7]+data[8]);//
    //send the data

    for(int i=0;i<10;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }
    delay(400);
}

int DM836II::nowSource()
{
    data[5]=0x07;//function code
    data[6]=0x01;//data length
    data[7]=QUERY;//data content 1
    data[8]=(data[4]+data[5]+data[6]+data[7]);//
    //send the data

    for(int i=0;i<9;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }
    delay(400);

    int k0;
    unsigned char recData[16];

    k0=serialDataAvail (fd_usb0);

    for(int j=0;j<k0;j++){
        recData[j]=serialGetchar(fd_usb0);
        if(j==k0-1) qDebug()<<"get now source";
    }

    if(recData[7]==0x01) return 0;
    else if(recData[7]==0x91) return 1;
    else if(recData[7]==0xA1) return 2;
    else if(recData[7]==0xC1) return 3;

}

//operate the radio
void DM836II::next()
{
    data[5]=0x0D;//function code
    data[6]=0x01;//data length
    data[7]=0x10;//data content 2
    data[8]=(data[4]+data[5]+data[6]+data[7]);//
    //send the data

    for(int i=0;i<9;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }
    delay(400);

}

void DM836II::previous()
{
    data[5]=0x0D;//function code
    data[6]=0x01;//data length
    data[7]=0x01;//data content 2
    data[8]=(data[4]+data[5]+data[6]+data[7]);//
    //send the data

    for(int i=0;i<9;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }
    delay(400);

}

//pause the radio
void DM836II::pause()
{
    data[5]=0x0C;//function code
    data[6]=0x02;//data length
    data[7]=SET;//data content 1
    data[8]=0x02;//data content 2
    data[9]=(data[4]+data[5]+data[6]+data[7]+data[8]);//
    //send the data
    for(int i=0;i<10;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }
    delay(400);

}
//play the radio
void DM836II::play()
{

    data[5]=0x0C;//function code
    data[6]=0x02;//data length
    data[7]=SET;//data content 1
    data[8]=0x01;//data content 2
    data[9]=(data[4]+data[5]+data[6]+data[7]+data[8]);//
    //send the data
    for(int i=0;i<10;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }
    delay(400);
}

/***************************
 * set the radio play mode,
 * 0 单曲播放，
 * 1 单曲循环，
 * 2 顺序播放，
 * 3 列表循环，
 * 4 随机播放.
****************************/
void DM836II::setPalyMode(const int &mode)
{
    data[5]=0x0E;//order code
    data[6]=0x02;//data length
    data[7]=SET;//data is query
    if(mode==0) data[8]=0x01;
    else if(mode==1) data[8]=0x02;
    else if(mode==2) data[8]=0x03;
    else if(mode==3) data[8]=0x04;
    else if(mode==4) data[8]=0x05;
    data[9] =(data[4]+data[5]+data[6]+data[7]+data[8]);//check data

    //send the data
    for(int i=0;i<10;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }

    delay(400);
}

int DM836II::nowPlayMode()
{
    data[5]=0x0E;//order code
    data[6]=0x01;//data length
    data[7]=QUERY;//data is query
    data[8]=(data[4]+data[5]+data[6]+data[7]);//check data

    //send the data
    for(int i=0;i<9;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }

    delay(400);
    int k0;
    unsigned char recData[16];

    k0=serialDataAvail (fd_usb0);
    for(int j=0;j<k0;j++){
        recData[j]=serialGetchar(fd_usb0);
        if(j==k0-1) qDebug()<<"get now playMode ";
    }
    if(recData[7]==0x01) return 0;
    else if (recData[7]==0x02) return 1;
    else if(recData[7]==0x03) return 2;
    else if(recData[7]==0x04) return 3;
    else if(recData[7]==0x05) return 4;

}

void DM836II::setRadioStatus(const int &status)
{
    data[5]=0x01;//order code
    data[6]=0x02;//data length
    data[7]=SET;//data is query
    if(status==0) data[8]=0x00;
    else if(status==1) data[8]=0x01;
    data[9] =(data[4]+data[5]+data[6]+data[7]+data[8]);//check data

    //send the data
    for(int i=0;i<10;i++){
        serialPutchar(fd_usb0,data[i]);
    }
    delay(400);
}

int DM836II::nowRadioStatus()
{

    data[5]=0x01;//order code
    data[6]=0x01;//data length
    data[7]=QUERY;//data is query
    data[8]=(data[4]+data[5]+data[6]+data[7]);//check data

    //send the data
    for(int i=0;i<9;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }

    delay(400);

    int k0;
    int k1;
    unsigned char recData[16];

    k0=serialDataAvail (fd_usb0);

    if(k0<=0){
        k1=2;
    }else{
        for(int j=0;j<k0;j++){
            recData[j]=serialGetchar(fd_usb0);
//            qDebug()<<"Received"<<j<<"=" <<int(recData[j]);
            if(j==k0-1) qDebug()<<"get now radio status";

        }

        for(int j=0;j<k0;j++){
            if(recData[j]==HEADER1&&recData[j+1]==HEADER2){
                if(int(recData[7])==1)
                    k1=1;
                else if(int(recData[7])==0)
                    k1=0;
            }

        }
    }
    return k1;

}

void DM836II::playNow(const int &source)
{
    data[5]=0x17;//function code
    data[6]=0x03;//data length
    if(source==1) data[7]=0x01; //SD卡
    else if(source==0) data[7]=0x02;//内置音乐
    else if(source==3) data[7]=0x03;//网络
    data[8]=0x00;//data content 2
    data[9]=0x01;//data content 2
    data[10]=(data[4]+data[5]+data[6]+data[7]+data[8]+data[9]);//
    //send the data

    for(int i=0;i<11;i++)
    {
        serialPutchar(fd_usb0,data[i]);
    }
    delay(400);
}



