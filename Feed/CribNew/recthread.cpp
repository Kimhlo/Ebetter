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

bool RecThread::checkIsNew(char data[], int &k0)
{
    int judgeLen=17;
    int samePos=0;

    for(int i=0;i<k0-judgeLen;i++){
        for(int j=0;j<judgeLen;j++){//判断前17位是否有一致的
            if(data[i+j]==pre_rfid[j+10]){
                samePos+=1;
            }
            //读取相同的数据个数是否与判断的长度一致，允许有两位突变
            if(samePos>=judgeLen-2){
                qDebug()<<"The same="<<samePos;
                return false;
            }
        }
        samePos=0;
    }
    return true;

}

void RecThread::initGPIO()
{
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
}

float RecThread::getTemper(unsigned char data[])
{
    float temper=0.0;
    int size=sizeof(data)/sizeof(data[0]);
    for(int i=0;i<size;i++){
        if(data[i]==0x54&&data[i+1]==0x50&&data[i+3]==0xF1){
            temper= (data[i+4]*256+data[i+5])/10.0;
            qDebug()<<"temper="<<temper;
            return temper;
        }
    }
    return temper;
}

long RecThread::getWeight(unsigned char data[])
{
    long weight=0;
    int size=sizeof(data)/sizeof(data[0]);
    for(int i=0;i<size;i++){
        if(data[i]==0x3D&&data[i+2]==0x01&&data[i+8]==0x23){
            long temp1=long (data[i+3])<<24;
            long temp2=long (data[i+4])<<16;
            long temp3=long (data[i+5])<<8;
            long temp4=long (data[i+6]);
            weight=temp1|temp2|temp3|temp4;
            qDebug()<<"weight="<<weight;
            return weight;
        }
    }
    return weight;
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

    unsigned char tx_temp[5];//读取温度要发送的是数据
    unsigned char rx_temp[100];//收到的数据信息
    unsigned char rx_rfid[140];//收到的RFID数据
    unsigned char tx_weight[8];//读取温度要发送的数据
    unsigned char rx_weight[100];//收到的体重数据

    //初始化温度和体重为0
    for(int m=0;m<100;m++){
        rx_temp[m]=0;
        rx_weight[m]=0;
    }

    char buf[BUFSIZE];
    char tempBuf[5];

    //检查是否为RFID
    isNew=false;
    //初始化当前curr_rfid
    for(int m=0;m<28;m++){
        curr_rfid[m]=0;
        pre_rfid[m]=1;
    }
    //初始化preRfidAll全为1
    preRfidAll[30][28]={1};

    int fd_usb0,fd_usb1,fd_usb2;
    if(wiringPiSetup() < 0)
         qDebug()<<"ini  error\n";

    if((fd_usb0 = serialOpen("/dev/ttyUSB0",9600)) < 0)//RFID
         qDebug()<<"open usb0 error\n";

    if((fd_usb1 = serialOpen("/dev/ttyUSB1",9600)) < 0)//temperature
         qDebug()<<"open usb1 error\n";

    if((fd_usb2 = serialOpen("/dev/ttyUSB2",9600)) < 0)//weight
         qDebug()<<"open usb2 error\n";

    //temperature order format
    tx_temp[0]=0x54;//命令报头1
    tx_temp[1]=0x50;//命令报头2
    tx_temp[2]=0x01;//地址
    tx_temp[3]=0xF1;//命令报头4
    tx_temp[4]=0x96;//校验和
    //weight oder format
    tx_weight[0]=0x3F;//命令报头
    tx_weight[1]=0x00;//设备地址，0x00广播地址
    tx_weight[2]=0x01;//读测量数据
    tx_weight[3]=0x3E;//校验值，前面的数据异或
    tx_weight[4]=0x23;//结束符
    //初始化GPIO
    initGPIO();
    //初始化已经吃过的猪的个数
    eatenPig=0;

    //DS18B20
    int fd_18b20;
    fd_18b20 = open("/sys/bus/w1/devices/28-0316a0381fff/w1_slave", O_RDONLY); //以只读方式打开ds18b20设备文件
    if(-1 == fd_18b20){
        //打开设备文件出错
        qDebug()<<"open DS18B20  device file error";  //打印出错信息（原因）
    }
    //DS18B20
    pinMode(7, INPUT);
    //吃完的标志
    int eatFinish=0;
    //have eaten
    int numberOld=0;

    //循环检测
    while(1)
    {

        inLevel=digitalRead(0);//食槽是否还有食物


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
        qDebug()<<"RFID="<<k0;
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
                qDebug()<<"current="<<int(curr_rfid[i]);
            }

            //检测二维数组中是否有已经存在的RFID
            for(int num=0;num<eatenPig+1;num++){
                for(int m=0;m<27;m++){
                    pre_rfid[m]=preRfidAll[num][m];
                }
                //判断是否为新的ID，判断前几位是否有连续一致
                if(!checkIsNew(curr_rfid,k0)){
                    numberOld+=1;
                }
            }//end for number


            qDebug()<<"numberOld="<<numberOld;
            if(numberOld>=1){
                isNew=false;
            }else {
                isNew=true;
            }
            numberOld=0;
        }

        //给新的猪哥哥做个全身检查
        if(isNew && k0>26)
        {
            /*************************
             * usb0用于RFID
            **************************/
            qDebug()<<"新的风暴已经来临";
            for(i=0;i<27;i++){
                preRfidAll[eatenPig][i]=curr_rfid[i];
//                qDebug()<<"per"<<int(pre_rfid[i]);
                recData[i]=curr_rfid[i];
            }
//            emit UpdateSignal(0,0,1);
            /*************************
             * usb1用于红外检测
            **************************/
            for(i=0;i<5;i++){
                serialPutchar(fd_usb1,tx_temp[i]);
            }
            delay(200);
            //读取红外温度数据
            k1=serialDataAvail (fd_usb1);
            qDebug()<<"Temper="<<k1;
            for(i=0;i<k1;i++){
                rx_temp[i]=serialGetchar(fd_usb1);
                qDebug()<<"temper"<<i<<"="<< rx_temp[i];
            }
            getTemper(rx_temp);
            //把猪哥哥的温柔告诉其他人
            for(i=0;i<4;i++){
                recData[i+27]=rx_temp[i+2];
            }
            /*************************
             * usb2用于称重
            **************************/
            for(i=0;i<5;i++){
                serialPutchar(fd_usb2,tx_weight[i]);
            }
            delay(200);
            //读取体重秤的数据
            k2=serialDataAvail (fd_usb2);
            qDebug()<<"Weight="<<k2;

            for(i=0;i<k2;i++){
                rx_weight[i]=serialGetchar(fd_usb2);
                qDebug()<<"weight"<<i<<"="<< rx_weight[i];
            }
            //将重量数据写入相应的位置
            for(i=0;i<4;i++){
                recData[i+31]=rx_weight[i+3];
            }
            //开始给猪哥哥上菜hui'kui
            getFood();
            //当前的碗里給猪哥哥饭了么？
            recData[36]=inLevel;
            //你的数据我要告诉所有人
            emit UpdateSignal(0,0,1);
            //新增一只吃过的猪哥哥
            eatenPig+=1;

        }//读取新猪的数据结束
        if(inLevel==1&&eatFinish==0){
          recData[36]=1;//吃完
          eatFinish=1;
          qDebug()<<"劳资整完了饭";
          emit UpdateSignal(0,0,1); //显示收到的数据
        }else if(inLevel==0&&eatFinish==1){
            recData[36]=0;
            eatFinish=0;
            qDebug()<<"人家还没吃完";
          }//end if inLevel



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

    //气缸关闭
    digitalWrite(IN1,HIGH);
    digitalWrite(IN3,HIGH);
}
