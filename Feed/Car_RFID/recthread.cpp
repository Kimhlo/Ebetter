

#include "recthread.h"

#include <QByteArray>
#include <global.h>

#include "wiringPi.h"
#include "wiringSerial.h"
#include <QDebug>
#include <sys/stat.h>

#define  IN1_1 29 //throw the food
#define  IN1_2 28 //back
#define  IN1_3 27 //forward
#define  IN1_4 26 //stop

RecThread::RecThread(QObject *parent)
{

}

RecThread::~RecThread()
{


}

void RecThread::initCar()
{
    if(wiringPiSetup() < 0);
      qDebug()<<"wp  ini  error\n";

    //init the Car
    pinMode(IN1_1, OUTPUT);
    pinMode(IN1_2, OUTPUT);
    pinMode(IN1_3, OUTPUT);
    pinMode(IN1_4, OUTPUT);

    digitalWrite(IN1_1, HIGH);//throw the food
    digitalWrite(IN1_2, HIGH);//back
    digitalWrite(IN1_3, HIGH);//forward
    digitalWrite(IN1_4, HIGH);//stop
}

void RecThread::run()
{
    int i=0;
    int k=0;
    //RFID data
    unsigned char test_rx[128];
    unsigned char rfid_ret[13]={0x02,0x31,0x31,0x30,0x30,0x30,0x41,0x32,0x43,0x33,0x33,0x04,0x03} ; //go home 02 31 31 30 30 30 41 32 43 33 33 04 03
    unsigned char rfid_stop1[13]={0x02,0x31,0x31,0x30,0x30,0x30,0x41,0x32,0x43,0x32,0x45,0x19,0x03};//crib1 02 31 31 30 30 30 41 32 43 33 30 07 03
    unsigned char rfid_stop2[13]={0x02,0x31,0x31,0x30,0x30,0x30,0x41,0x32,0x43,0x33,0x30,0x07,0x03};//crib2 02 31 31 30 30 30 41 32 43 32 45 19 03
    int sum1=0;
    int sum2=0;
    int sum3=0;

    //usb id
    int fd_usb0;
    if((fd_usb0 = serialOpen("/dev/ttyUSB0",9600)) < 0)
        qDebug()<<"open usb0 error\n";

    //init the GPIO
    initCar();

    //record the car status
    curr_status=0;

    while(1)
   {
        //delay
        delay(1000);
        k=serialDataAvail (fd_usb0);

        //get the read data
        if(k>12)
        {
           for(i=0;i<k;i++){
              test_rx[i]=serialGetchar(fd_usb0);
//              qDebug()<< "i= "<<i<<  test_rx[i];
           }//end for i<k
           for(int k=0;k<13;k++){

               if(test_rx[k]==rfid_ret[k]){
                   sum1+=1;
               }
               if(test_rx[k]==rfid_stop1[k]){
                   sum2+=1;
               }
               if(test_rx[k]==rfid_stop2[k]){
                   sum3+=1;
               }
           }//end for k<13

           qDebug()<<"sum"<<sum1<<sum2<<sum3;
        }//end if k>12

        if(flag_run==1){
            sendData[2]=0x01;//on the way that thow the food
            if(sum1==13&&curr_status==0){//check the start point forward
                qDebug()<<"forward";
                emit UpdateSignal(0,0,100);
                curr_status=1;
                sum1=0;
                sum2=0;
                sum3=0;
            }
            if(sum2==13&&curr_status==1){//reach the first crib
                qDebug()<<"stop1";
                throwFood();
                curr_status=2;
                forword();

                emit UpdateSignal(0,0,100);
                emit UpdateSignal(0,0,1);

                sum1=0;
                sum2=0;
                sum3=0;
            }
            if(sum3==13&&curr_status==2){//reach the second crib
                //Please don't change the emit signal order
                emit UpdateSignal(0,0,2);
                emit UpdateSignal(0,0,101);
                qDebug()<<"stop2";

                throwFood();
                curr_status=3;
                back();
                sendData[2]=0x02;//on the back way

                sum1=0;
                sum2=0;
                sum3=0;
            }
            if(sum1==13&&curr_status==3){//reback to the start point
                qDebug()<<"back to the start point";
                emit UpdateSignal(0,0,102);
                curr_status=1;
                delay(1000);
                stop();
                sendData[2]=0x00;//at start point
                sum1=0;
                sum2=0;
                sum3=0;
            }
        }
        //reset the sum number
        sum1=0;
        sum2=0;
        sum3=0;
    }//end while(1)
}

void RecThread::ResetSlot()
{

}

//car forward
void RecThread::forword()
{
    digitalWrite(IN1_4, HIGH);
    digitalWrite(IN1_3, LOW);
    digitalWrite(IN1_2, HIGH);
    sendData[1]=0x01;

}

//throw the food to the crib
void RecThread::throwFood()
{
    digitalWrite(IN1_4, HIGH);
    digitalWrite(IN1_3, HIGH);
    digitalWrite(IN1_2, HIGH);
    QThread::msleep(1000);
    digitalWrite(IN1_1, LOW);  //throw the food
    QThread::msleep(5000);
    digitalWrite(IN1_1, HIGH);  //stop throw food
    digitalWrite(IN1_3, LOW);  //forward
    QThread::msleep(1000);
    sendData[1]=0x03;
}

//car back
void RecThread::back()
{
    digitalWrite(IN1_3, HIGH);
    digitalWrite(IN1_2, LOW);
    digitalWrite(IN1_4, HIGH);
    sendData[1]=0x02;
}

//stop car
void RecThread::stop()
{
     digitalWrite(IN1_4, HIGH);
     digitalWrite(IN1_3, HIGH);
     digitalWrite(IN1_2, HIGH);
     digitalWrite(IN1_1,HIGH);
     sendData[1]=0x04;
}


