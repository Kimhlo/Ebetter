#include "recthread.h"

#include <QByteArray>
#include <global.h>

#include "wiringPi.h"
#include "wiringSerial.h"
#include <QDebug>
#include <sys/stat.h>

#define  IN1 29
#define  IN2 28
#define  IN3 27
#define  IN4 26

RecThread::RecThread(QObject *parent)
{

}

RecThread::~RecThread()
{


}

void RecThread::run()
{
    int i=0;
    int k;
    int k0=0;
    int k1=0;
    int k2=0;

    int kk=0;
    int inLevel;

    char tx_temp[4];
    unsigned char rx_temp[100];
    unsigned char rx_rfid[140];
    unsigned char tx_weight[8];
    unsigned char rx_weight[100];

    char buf[BUFSIZE];
    char tempBuf[5];

    int fd,fd_usb0,fd_usb1,fd_usb2;
    if(wiringPiSetup() < 0);
      qDebug()<<"ini  error\n";
  //  if((fd = serialOpen("/dev/ttyAMA0",115200)) < 0)
   //     qDebug()<<"open ttyAMA0 error\n";


     if((fd_usb0 = serialOpen("/dev/ttyUSB0",9600)) < 0)
        qDebug()<<"open usb0 error\n";

      if((fd_usb1 = serialOpen("/dev/ttyUSB1",9600)) < 0)
        qDebug()<<"open usb1 error\n";

      if((fd_usb2 = serialOpen("/dev/ttyUSB2",9600)) < 0)
        qDebug()<<"open usb2 error\n";

    //serialPrintf(fd,"Hello World!!!");
    tx_weight[0]=0x01;
    tx_weight[1]=0x03;
    tx_weight[2]=0x00;
    tx_weight[3]=0x00;
    tx_weight[4]=0x00;
    tx_weight[5]=0x02;
    tx_weight[6]=0xc4;
    tx_weight[7]=0x0b;

    tx_temp[0] = 0x3F;
    tx_temp[1] = 0x54;
    tx_temp[2] = 0x0D;  //回车
    tx_temp[3] = 0x0A;  //换行


    for(i=0;i<27;i++)
    {
        pre_rfid[i]=1;
    }

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

   //     digitalWrite(IN2, HIGH);



//       digitalWrite(IN1, LOW);
//       digitalWrite(IN2, LOW);
//       digitalWrite(IN3, LOW);
//       digitalWrite(IN4, LOW);
//       QThread::msleep(4000);

        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, HIGH);
    //DS18B20
    int fd_18b20;
    fd_18b20 = open("/sys/bus/w1/devices/28-0316a0381fff/w1_slave", O_RDONLY); //以只读方式打开ds18b20设备文件
    if(-1 == fd_18b20){              //打开设备文件出错
        qDebug()<<"open DS18B20  device file error";  //打印出错信息（原因）
    }
    //DS18B20

    pinMode(7, INPUT);

    while(1)
    {
        inLevel=digitalRead(0);

       qDebug()<<"inLevel="<<inLevel;

        for(i=0;i<8;i++)
        {
            serialPutchar(fd_usb2,tx_weight[i]);
        }

       //usb0 tx start  weight
        for(i=0;i<4;i++)
        {
            serialPutchar(fd_usb1,tx_temp[i]);
        }
       //usb0 tx end

        //DS18B20
        float temp;
        int i, j;


        if(read(fd_18b20, buf, BUFSIZE)<0) //将设备文件中的内容读入buf中
        {
                qDebug()<<"read DS18B20 error";
        }

        for(i=0;i<sizeof(buf);i++){
            if(buf[i] == 't'){               //如果读到‘t’，说明后面马上出现温度值
                for(j=0;j<sizeof(tempBuf);j++){
                    tempBuf[j] = buf[i+2+j]; //将温度值以字符串的形式写入tempBuf中
                }
            }
        }

     //   qDebug()<<tempBuf[0]<<tempBuf[1]<<tempBuf[2]<<tempBuf[3]<<tempBuf[4];
       // temp = (float)atoi(tempBuf) / 1000;  //将字符串转换为浮点型温度数据

       // printf("%.3f C\n",temp);             //打印出温度值
        //DS18B20

        QThread::msleep(2000);

        //usb0 rx start
        k0=serialDataAvail (fd_usb0);  //rfid
        qDebug()<<"k0="<<k0;
       if(k0>26)
        {
            for(i=0;i<k0;i++)
            {
               rx_rfid[i]=serialGetchar(fd_usb0);
//               qDebug()<< rx_rfid[i];
            }

            for(i=0;i<27;i++)
            {
                curr_rfid[i]=rx_rfid[i];
            }

            for(i=0;i<27;i++)
            {
               if(pre_rfid[i]!=curr_rfid[i])
                   kk=kk+1;
            }
           qDebug()<<"kk="<<kk;
        }
        //usb0 rx end
        if(kk==27 && k0>26)
        {
           // kk=0;
            for(i=0;i<27;i++)
            {
                pre_rfid[i]=curr_rfid[i];
                recData[i]=curr_rfid[i];
            }
           //======================
           emit UpdateSignal(0,0,1);  //play music
           Push_w();
          //======================

        //usb1 rx start   232
        k1=serialDataAvail (fd_usb1);  //hongwai
        qDebug()<<"k1="<<k1;
        //if(k>8)
        //{
        for(i=0;i<k1;i++)
        {
            rx_temp[i]=serialGetchar(fd_usb1);
          // qDebug()<< rx_temp[i];
        }
        for(i=0;i<4;i++)
        {
            recData[i+27]=rx_temp[i+3];
        }
       // }
        //usb1 rx end

        //usb2 rx start
        k2=serialDataAvail (fd_usb2); //weight
      //  qDebug()<<"k2="<<k2;
        //if(k>8)
        //{
        for(i=0;i<k2;i++)
        {
            rx_weight[i]=serialGetchar(fd_usb2);
          //  qDebug()<< rx_weight[i];
        }

        for(i=0;i<4;i++)
        {
            recData[i+31]=rx_temp[i+3];
        }
       // }
        //usb2 rx end

        recData[36]=inLevel;

        emit UpdateSignal(0,0,1); //显示收到的数据
     }

        qDebug()<<"inLevel="<<inLevel<<"kk="  <<kk;
       if(inLevel==1)
       {
         recData[36]=1;
         if(kk==27)
         {
            emit UpdateSignal(0,0,1); //显示收到的数据
             kk=0;
         }

       }
       else
         recData[36]=0;


        //JiDianQi   start
      //  digitalWrite(IN1, HIGH);
      //  digitalWrite(IN1, HIGH);

     // QThread::msleep(100);
    }
    serialClose(fd_usb0);
    serialClose(fd_usb1);
    serialClose(fd_usb2);

    close(fd_18b20);                           //关闭文件
}

void RecThread::ResetSlot()
{

}


void RecThread::Push_w()
{
    digitalWrite(IN3, HIGH); //down close
    QThread::msleep(4000);

    digitalWrite(IN1, LOW);  //up  open
    QThread::msleep(4000);

    digitalWrite(IN1 ,HIGH); //up close
    QThread::msleep(4000);

    digitalWrite(IN3, LOW);  //down open
    QThread::msleep(3000);

    digitalWrite(IN1,HIGH);
    digitalWrite(IN3,HIGH);
}
