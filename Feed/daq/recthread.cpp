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

bool RecThread::checkIsNew(char data[])
{
    int len=sizeof(data)/sizeof(data[0]);
    int judgeLen=7;
    int samePos=0;

    for(int i=0;i<len-judgeLen;i++){
        for(int j=0;j<judgeLen;j++){//判断前六位是否有一致的
            if(data[i+j]!=pre_rfid[j]&&len>=i+27){
                samePos+=1;
            }
        }
        //取后面judgeLen的长度来判断是否一致
        if(samePos=judgeLen){
            return false;
        }
    }
    return true;

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

    //初始化温度和体重为0
    for(int m=0;m<100;m++){
        rx_temp[m]=0;
        rx_weight[m]=0;
    }

    char buf[BUFSIZE];
    char tempBuf[5];

    //检查是否为RFID
    isNew=true;
    //初始化当前curr_rfid
    for(int m=0;m<28;m++){
        curr_rfid[m]=0;
    }

    int fd,fd_usb0,fd_usb1,fd_usb2;
    if(wiringPiSetup() < 0)
         qDebug()<<"ini  error\n";

    if((fd_usb0 = serialOpen("/dev/ttyUSB2",9600)) < 0)
         qDebug()<<"open usb0 error\n";

    if((fd_usb1 = serialOpen("/dev/ttyUSB0",9600)) < 0)
         qDebug()<<"open usb1 error\n";

    if((fd_usb2 = serialOpen("/dev/ttyUSB3",9600)) < 0)
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


    for(i=0;i<28;i++)
    {
        pre_rfid[i]=1;
    }

    //init the relay
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    //close the relay
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);

    //DS18B20
    int fd_18b20;
    fd_18b20 = open("/sys/bus/w1/devices/28-0316a0381fff/w1_slave", O_RDONLY); //以只读方式打开ds18b20设备文件
    if(-1 == fd_18b20){
        //打开设备文件出错
        qDebug()<<"open DS18B20  device file error";  //打印出错信息（原因）
    }
    //DS18B20
    pinMode(7, INPUT);//

    while(1)
    {
        inLevel=digitalRead(0);//食槽是否还有食物
        qDebug()<<"inLevel="<<inLevel;

        //usb2用于称重
        for(i=0;i<8;i++)
        {
            serialPutchar(fd_usb2,tx_weight[i]);
        }

        //usb1用于红外检测
        for(i=0;i<4;i++)
        {
            serialPutchar(fd_usb1,tx_temp[i]);
        }

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


//        qDebug()<<tempBuf[0]<<tempBuf[1]<<tempBuf[2]<<tempBuf[3]<<tempBuf[4];
//        temp = (float)atoi(tempBuf) / 1000;  //将字符串转换为浮点型温度数据
//        printf("%.3f C\n",temp);             //打印出温度值
        QThread::msleep(2000);

        //RFID收到数据
        k0=serialDataAvail (fd_usb0);
//        qDebug()<<"RFID="<<k0;
        //有客官来了，翠花上酸菜
       if(k0>26)
        {
           qDebug()<<"有客官来了，翠花上酸菜";
            for(i=0;i<k0;i++)
            {
               rx_rfid[i]=serialGetchar(fd_usb0);
            }
            //更新当前的RFID数据
            for(i=0;i<27;i++)
            {
                curr_rfid[i]=rx_rfid[i];
            }
            //判断是否为新的ID，判断前几位是否有连续一致
            isNew=checkIsNew(pre_rfid);

        }

        //给新的猪哥哥做个全身检查
        if(isNew)
        {
            qDebug()<<"新的风暴已经来临";
            for(i=0;i<27;i++)
            {
                pre_rfid[i]=curr_rfid[i];
                recData[i]=curr_rfid[i];
            }

            //why not come some music!!!
            emit UpdateSignal(0,0,1);

            //开始给猪哥哥上菜
            getFood();

            //读取红外温度数据
            k1=serialDataAvail (fd_usb1);
            qDebug()<<"Temper="<<k1;
            for(i=0;i<k1;i++)
            {
                rx_temp[i]=serialGetchar(fd_usb1);
                qDebug()<<"temper"<<i<<"="<< rx_temp[i];
            }
            //把猪哥哥的温柔告诉其他人
            for(i=0;i<4;i++)
            {
                recData[i+27]=rx_temp[i+3];
            }

            //读取体重秤的数据
            k2=serialDataAvail (fd_usb2);
            qDebug()<<"Weight="<<k2;

            for(i=0;i<k2;i++)
            {
                rx_weight[i]=serialGetchar(fd_usb2);
                qDebug()<<"weight"<<i<<"="<< rx_weight[i];
            }
            //你的重量我知道了
            for(i=0;i<4;i++)
            {
                recData[i+31]=rx_weight[i+3];
            }

            //当前的碗里給猪哥哥饭了么？
            recData[36]=inLevel;

            //你的数据我要告诉所有人
            emit UpdateSignal(0,0,1);

        }//读取新猪的数据结束

       if(inLevel==1){
         recData[36]=1;//吃完
         qDebug()<<"劳资整完了饭";
         emit UpdateSignal(0,0,1); //显示收到的数据
       }else{
         recData[36]=0;
         qDebug()<<"前个猪不行啊，没吃完";
       }

    }

}

void RecThread::ResetSlot()
{

}


void RecThread::getFood()
{
    digitalWrite(IN3, HIGH); //下面的继电器关
    QThread::msleep(4000);

    digitalWrite(IN1, LOW);  //上面的继电器开
    QThread::msleep(4000);

    digitalWrite(IN1 ,HIGH); //上面的继电器关
    QThread::msleep(4000);

    digitalWrite(IN3, LOW);  //下面的继电器开
    QThread::msleep(3000);

    //继电器关
    digitalWrite(IN1,HIGH);
    digitalWrite(IN3,HIGH);
}
