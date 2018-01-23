

#include "recthread.h"

#include <QByteArray>
#include <global.h>

#include <QDebug>
#include <sys/stat.h>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <stdio.h>

#define  out1 2//27
#define  out2 3//22
#define  out3 12//10
#define  out4 13//9
#define  out5 14//11
#define  out6 21//5
#define  out7 22//6
#define  out8 23//13
#define  out9 24//19
#define  out10 25//26
#define  out11 15//14
#define  out12 16//15
#define  out13 1//18
#define  out14 4//23
#define  out15 5//24
#define  out16 6//25
#define  out17 10//8
#define  out18 11//7
#define  out19 28//20
#define  out20 29//21

#define  in_1  27 //16

//int k= digitalRead(in_1);

RecThread::RecThread(QObject *parent)
{

}

RecThread::~RecThread()
{
    serialClose(fd_usb0);
    serialClose(fd_usb1);
}

void RecThread::run()
{
    int i,j;
    int k0,k1;

    unsigned char currBit;
    int currBitVal;

    QString strSql;

    QHash<int,int> inputIO;  //1~60为开关   61~91 为传感器  92~120为开关

    int cnt_sensor_delay_light[150];
    unsigned char flag_sensor_delay_light[i];

    int doubleCon[6];
    //=========================
    QHash<int,int>  sensor_light;  //sensor_light表   IO_sensor /  IO_light
    QMultiMap<int,int> path_detail;  //path_detail表    IO_light_1 / IO_light_2
    int k;
    QHash<int,int> vicinitas_light;  //传感器相领灯的IO

    QHash<int,int> light_status_sensor; //传感器对应的灯的状态
    QHash<int,int> pre_light_status_sensor;

    QHash<int,int> pre_light_status_switch;  //开关上一次状态
    QHash<int,int> light_status_switch;  //开关对应的灯的状态

    QHash<int,int> pre_light_status_app;  //开关上一次状态
    QHash<int,int> light_status_app;  //APP开关对应的灯的状态

    QHash<int,int> pre_light_status;

    int tem_light_status[70];
    //=============================================

    QThread::msleep(2000);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("MJDB.db");
    if (!db.open())
    {
        qDebug() << "Database Error!";
    }

    QSqlQuery sql_query(db);

    strSql=  QString("select  IO_sensor, IO_light  from sensor_light ");
    sql_query.prepare(strSql);
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
    }
    else
    {
        while(sql_query.next())
        {
            sensor_light[sql_query.value(0).toInt()]=sql_query.value(1).toInt();  //IO_sensor /  IO_light
        }
    }
    //-----------------------------
    strSql=  QString("select  IO_light_1, IO_light_2  from path_detail ");
    sql_query.prepare(strSql);
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
    }
    else
    {
        k=0;
        while(sql_query.next())
        {
            path_detail.insert(sql_query.value(0).toInt(),sql_query.value(1).toInt());  //IO_light_1 / IO_light_2
        }
    }
    db.close();
    //======================================
    if(wiringPiSetup() < 0);
    qDebug()<<"wp  ini  error\n";


    if((fd_usb1 = serialOpen("/dev/ttyUSB1",9600)) < 0)   //bottm 4,5,6 jidianqi  switch input and switch_light out
        qDebug()<<"open usb0 error\n";
    if((fd_usb0 = serialOpen("/dev/ttyUSB0",9600)) < 0)   // top  6,7,8 sensor input  and switch input and switch_light out
        qDebug()<<"open usb0 error\n";

    SePintMode();

    for(i=1;i<120;i++)
    {
       pre_light_status_switch[i]=0;
       pre_light_status_sensor[i]=0;
       pre_light_status_app[i]=0;

       cnt_sensor_delay_light[i]=0;
       flag_sensor_delay_light[i]=0;
    }

    while(1)
    {
        //sensor input  and switch_light_2 out start ========================================
        k0=serialDataAvail (fd_usb0);
        //qDebug()<<"k0="<<k0;
        if(k0>17)
        {
            for(i=1;i<120;i++)
            {
                light_status_switch[i]=0;
                light_status_sensor[i]=0;
                light_status_app[i]=0;
            }

            for(i=0;i<3;i++)
            {
                tx0[i]=0;
                tx1[i]=0;
            }

            for(i=0;i<k0;i++)
            {
                rx0[i]=serialGetchar(fd_usb0);
            }
            for(i=0;i<k0;i++)
            {
                if(rx0[i]=='a')
                {
                    a_p_0=i;
                    break;
                }
            }

            //==============================usb1
            k1=serialDataAvail (fd_usb1);   //switch input
            while(k1<17)
                k1=serialDataAvail (fd_usb1);

            //qDebug()<<"k1=" << k1;
            for(i=0;i<k1;i++)
                rx1[i]=serialGetchar(fd_usb1);
                //     qDebug()<<"rx=" << rx1[i];

            for(i=0;i<k1;i++)
            {
                if(rx1[i]=='a')
                {
                    a_p_1=i;
                    break;
                }
            }
            //============================
            int io_light; //当前灯号
            for(i=0;i<8;i++)  //共120个输入
            {
                for(j=0;j<8;j++)
                {
                    currBit=i * 8 +j +1;
                    if(currBit<61)
                    {
                        currBitVal= (rx0[i+a_p_0+1] >>j ) & 0x01;
                        inputIO[currBit+60]=currBitVal;  //currBit =61~120     usb0  上面的板子

                        //switch input  usb1  下面板子全部为开关输入
                        currBitVal= (rx1[i+a_p_1+1] >>j ) & 0x01;
                        inputIO[currBit]=currBitVal;   //currBit=1~60    usb1  下面的板子
                    }
                }
            }

            //sensor input  usb0   上面板子前部分用作1~31 对应IO_sensor编号1+60~31+60 传感器输入
            //多个传感器对应一个灯
            if((inputIO.value(81) || inputIO.value(85) || inputIO.value(86))>0)
            {
                inputIO[81]=1;
                inputIO[85]=1;
                inputIO[86]=1;
            }
            if((inputIO.value(61) || inputIO.value(62) || inputIO.value(63))>0)
            {
                inputIO[61]=1;
                inputIO[62]=1;
                inputIO[63]=1;
            }
            if((inputIO.value(65) || inputIO.value(66) )>0)
            {
                inputIO[65]=1;
                inputIO[66]=1;
            }
            if((inputIO.value(72) || inputIO.value(73) )>0)
            {
                inputIO[72]=1;
                inputIO[73]=1;
            }
            if((inputIO.value(75) || inputIO.value(76) )>0)
            {
                inputIO[75]=1;
                inputIO[76]=1;
            }
            if((inputIO.value(77) || inputIO.value(78) )>0)
            {
                inputIO[77]=1;
                inputIO[78]=1;
            }
            if((inputIO.value(85) || inputIO.value(86) )>0)
            {
                inputIO[85]=1;
                inputIO[86]=1;
            }
            if((inputIO.value(87) || inputIO.value(88) || inputIO.value(89))>0)
            {
                inputIO[87]=1;
                inputIO[88]=1;
                inputIO[89]=1;
            }
            //=================================
            //处理双控
            if((inputIO.value(3) ^ inputIO.value(4))==0)
            {
                inputIO[3]=0;
                inputIO[4]=0;
            }
            else
            {
                inputIO[3]=1;
                inputIO[4]=1;
            }

            if((inputIO.value(53) ^ inputIO.value(54))==0)
            {
                 inputIO[53]=0;
                 inputIO[54]=0;
               //  qDebug()<<"open 48";
            }
            else
            {
                inputIO[53]=1;
                inputIO[54]=1;
               // qDebug()<<"close 48";
            }

            if((inputIO.value(13) ^ inputIO.value(14))==0)
            {
                inputIO[13]=0;
                inputIO[14]=0;
            }
            else
            {
                inputIO[13]=1;
                inputIO[14]=1;
            }

            if((inputIO.value(22) ^ inputIO.value(23))==0)
             {
                inputIO[22]=0;
                inputIO[23]=0;
            }
            else
            {
                inputIO[22]=1;
                inputIO[23]=1;
            }

            if((inputIO.value(38) ^ inputIO.value(39))==0)
            {
                inputIO[38]=0;
                inputIO[39]=0;
            }
            else
            {
                inputIO[38]=1;
                inputIO[39]=1;
            }

            if((inputIO.value(47) ^ inputIO.value(48))==0)
            {
                inputIO[47]=0;
                inputIO[48]=0;
            }
            else
            {
                inputIO[47]=1;
                inputIO[48]=1;
            }
            //==============================

            for(i=1;i<120;i++) //sensor input range :61~91  light: 41~59
            {
                io_light=0;
                QList<int> vals;
                vals.clear();
                io_light=sensor_light.value(i);  //light:41~59
                //================================
                //开关输入 start
                //=================================
                if((i>0) && (i<61))
                {
                    if(inputIO.value(i)==0)  //开关低电平开灯
                         light_status_switch[io_light]= 1; //开灯
                }
                // 开关输入 end

                //============================
                //传感器输入 start
                if((i>60) && (i<92))
                {
                    if(inputIO.value(i)>0)  //sensor active,传感器高电平开灯
                    {
                        vals = path_detail.values(io_light);
                        for(int p1=0;p1<vals.size();p1++)  //查找相领灯
                            light_status_sensor[vals.at(i)]=1; //传感器相邻灯点亮
                        //传感器控制的当前灯点亮
                        light_status_sensor[io_light]=1;
                    }
                }
                //传感器输入 end
                //=======================================

                // switch input  usb0  上面块板子的后部分IO 用作开关输入
                if((i>91) && (i<121))  //switch input
                {
                    if(inputIO.value(i)==0 )  //开关打开
                        light_status_switch[io_light]=1;
                }
                //=============================
            }  //end 1-120 loop


            for(i=1;i<121;i++)  //switch input 1~61
            {
                io_light=0;
                io_light=sensor_light.value(i);
                //===========================
                //手机控制
                if(pre_light_status_app.value(i) ^ light_status_app.value(i)==1  )
                 {
                    if(light_status_app.value(i)==1)
                       light_status[i]=1;
                    else
                       light_status[i]=0;
                 }
                //=================================
                //传感器控制

                if(pre_light_status_sensor.value(i) ^ light_status_sensor.value(i)==1  )
                 {
                    if(light_status_sensor.value(i)==1)
                       light_status[i]=1;
                    else
                       light_status[i]=0;
                    //传感器控制的灯由开变关，需要延时
                    if((pre_light_status_sensor.value(i)==1) && (light_status_sensor.value(i)==0))
                    {
                        cnt_sensor_delay_light[i]=0;
                        flag_sensor_delay_light[i]=0x01;
                    }
                 }
              /*
                 if(flag_sensor_delay_light[i]==0x01)
                 {
                     cnt_sensor_delay_light[i]=cnt_sensor_delay_light[i]+1;
                     light_status[i]=1;
                   //  qDebug()<<"cnt_sensor_delay_light="<<cnt_sensor_delay_light;
                 }

                 if(cnt_sensor_delay_light[i]==100)
                 {
                      cnt_sensor_delay_light[i]=0;
                      flag_sensor_delay_light[i]=0x00;
                      light_status[i]=0;
                 }
                */
                //=================================
                //开关控制
                if(pre_light_status_switch.value(i) ^ light_status_switch.value(i)==1  )
                 {
                    if(light_status_switch.value(i)==1)
                       light_status[i]=1;
                    else
                       light_status[i]=0;
                 }
            } //end 1-120 loop
            //============================

            for(int i=1;i<61;i++)
            {
                //开关控制的灯 修改数据
                SetLightBit(i,light_status.value(i));
            }
            //=======================================

            for(i=1;i<60;i++)  //保存上一次灯的状态
            {
                pre_light_status_switch[i]= light_status_switch.value(i);
                pre_light_status_sensor[i]= light_status_sensor.value(i);
                pre_light_status_app[i]= light_status_app.value(i);
                pre_light_status[i]= light_status.value(i);
            }
            io_45=light_status.value(45);
            io_41=light_status.value(41);
            io_43=light_status.value(43);
            io_51=light_status.value(51);
            io_52=light_status.value(52);
            io_55=light_status.value(55);
            io_54=light_status.value(54);

            io_1=(tx1[0]) & 0x01;  //1
            io_7= (tx1[0]>>6) & 0x01 ;  //7
            io_15=(tx1[1]>>6) & 0x01; //15

            io_28=(tx0[0]>>7) & 0x01; //28
            io_30=(tx0[1]>>1) & 0x01; //30
//qDebug()<<"io_30="<<io_30;

            //灯带的状态发生了变化，发信号给菲利浦的控制器

            if((pre_io_1 ^ io_1==1) || (pre_io_7 ^ io_7==1) || (pre_io_15 ^ io_15==1) || (pre_io_28 ^ io_28==1) || (pre_io_30 ^ io_30==1) || ((pre_io_41 ^ io_41) ==1) || ((pre_io_43 ^ io_43) ==1) || ((pre_io_45 ^ io_45) ==1)|| ((pre_io_51 ^ io_51) ==1) || ((pre_io_52 ^ io_52) ==1) || ((pre_io_54 ^ io_54) ==1) || ((pre_io_55 ^ io_55) ==1) )
            {
                emit UpdateSignal(0,0,5);
               // qDebug()<<"send to ph =";
            }

            pre_io_7=io_7;
            pre_io_15=io_15;
            pre_io_1=io_1;
            pre_io_28= io_28 ;//茶室墙2
            pre_io_30= io_30 ;

            pre_io_45= io_45 ;//西厨感应
            pre_io_41= io_41 ;//老人房1
            pre_io_43=io_43 ;//一卫1
            pre_io_51= io_51 ;//次卫1
            pre_io_52= io_52 ;//次卧灯2
            pre_io_55= io_55 ;//主卫灯2
            pre_io_54= io_54 ;//主卧灯2

            //给灯带通电
            SetLightBit(7,1);  //书房4
            SetLightBit(15,1); //二走廊4
            SetLightBit(1,1); //大厅灯5
            SetLightBit(28,1); //
            SetLightBit(30,1); //

            //================================
            serialPutchar(fd_usb1,'a');
            serialPutchar(fd_usb1,'b');
            serialPutchar(fd_usb1,tx1[0]);
            serialPutchar(fd_usb1,tx1[1]);
            serialPutchar(fd_usb1,tx1[2]);
            serialPutchar(fd_usb1,'c');

            serialPutchar(fd_usb0,'a');
            serialPutchar(fd_usb0,'b');
            serialPutchar(fd_usb0,tx0[0]);
            serialPutchar(fd_usb0,tx0[1]);
            serialPutchar(fd_usb0,tx0[2]);
            serialPutchar(fd_usb0,'c');

            for(i=41;i<60;i++)  //置灯的状态
            {
               tem_light_status[i]=light_status.value(i);
            }

            //给灯带通电
            tem_light_status[45]=1 ;//西厨感应
            tem_light_status[41]=1 ;//老人房1
            tem_light_status[43]=1 ;//一卫1
            tem_light_status[51]=1 ;//次卫1
            tem_light_status[52]=1 ;//次卧灯2
            tem_light_status[55]=1 ;//主卫灯2
            tem_light_status[54]=1 ;//主卧灯2

            for(i=41;i<60;i++)  //置灯的状态
            {
              //   sensor_light_status[i]=0;
                SetSensorLightBit(i,tem_light_status[i]);
            }

            emit UpdateSignal(0,0,3);
        }  //end if

        QThread::msleep(10);
    }  //end while

}

void RecThread::bit_set(unsigned char *p_data, unsigned char position,int flag)
{
    int a = 1<<(position-1);

    if (flag)
    {
        *p_data |= a;  //set 1
    }
    else
    {
        *p_data &= ~a;  //set 0
    }
}

void RecThread::ResetSlot()
{

}


void RecThread::SePintMode()
{
    pinMode(out1, OUTPUT);
    pinMode(out2, OUTPUT);
    pinMode(out3, OUTPUT);
    pinMode(out4, OUTPUT);
    pinMode(out5, OUTPUT);
    pinMode(out6, OUTPUT);
    pinMode(out7, OUTPUT);
    pinMode(out8, OUTPUT);
    pinMode(out9, OUTPUT);
    pinMode(out10, OUTPUT);
    pinMode(out11, OUTPUT);
    pinMode(out12, OUTPUT);
    pinMode(out13, OUTPUT);
    pinMode(out14, OUTPUT);
    pinMode(out15, OUTPUT);
    pinMode(out16, OUTPUT);
    pinMode(out17, OUTPUT);
    pinMode(out18, OUTPUT);
    pinMode(out19, OUTPUT);
    pinMode(out20, OUTPUT);

    pinMode(in_1,INPUT);
}

void RecThread::SetSensorLightBit(int p,int val)
{
    switch(p-40)
    {
    case 1:
        if(val==0)
            digitalWrite(out1, LOW);
        else
            digitalWrite(out1, HIGH);
    case 2:
        if(val==0)
            digitalWrite(out2, LOW);
        else
            digitalWrite(out2, HIGH);
    case 3:
        if(val==0)
            digitalWrite(out3, LOW);
        else
            digitalWrite(out3, HIGH);
    case 4:
        if(val==0)
            digitalWrite(out4, LOW);
        else
            digitalWrite(out4, HIGH);
    case 5:
        if(val==0)
            digitalWrite(out5, LOW);
        else
            digitalWrite(out5, HIGH);
    case 6:
        if(val==0)
            digitalWrite(out6, LOW);
        else
            digitalWrite(out6, HIGH);
    case 7:
        if(val==0)
            digitalWrite(out7, LOW);
        else
            digitalWrite(out7, HIGH);
    case 8:
        if(val==0)
            digitalWrite(out8, LOW);
        else
            digitalWrite(out8, HIGH);
    case 9:
        if(val==0)
            digitalWrite(out9, LOW);
        else
            digitalWrite(out9, HIGH);
    case 10:
        if(val==0)
            digitalWrite(out10, LOW);
        else
            digitalWrite(out10, HIGH);
    case 11:
        if(val==0)
            digitalWrite(out11, LOW);
        else
            digitalWrite(out11, HIGH);
    case 12:
        if(val==0)
            digitalWrite(out12, LOW);
        else
            digitalWrite(out12, HIGH);
    case 13:
        if(val==0)
            digitalWrite(out13, LOW);
        else
            digitalWrite(out13, HIGH);
    case 14:
        if(val==0)
            digitalWrite(out14, LOW);
        else
            digitalWrite(out14, HIGH);
    case 15:
        if(val==0)
            digitalWrite(out15, LOW);
        else
            digitalWrite(out15, HIGH);
    case 16:
        if(val==0)
            digitalWrite(out16, LOW);
        else
            digitalWrite(out16, HIGH);
    case 17:
        if(val==0)
            digitalWrite(out17, LOW);
        else
            digitalWrite(out17, HIGH);
    case 18:
        if(val==0)
            digitalWrite(out18, LOW);
        else
            digitalWrite(out18,HIGH);
    case 19:
        if(val==0)
            digitalWrite(out19, LOW);
        else
            digitalWrite(out19, HIGH);
    case 20:
        if(val==0)
            digitalWrite(out20, LOW);
        else
            digitalWrite(out20, HIGH);
    default:
    {
    }
    }
}

void RecThread::SetLightBit(int io_light,int val)
{
    // if((io_light>40) && (io_light <61))
    // {
    //     sensor_light_status[io_light-40]=1;
    //   qDebug()<<"io_light"<<io_light-40 << "val=" << sensor_light_status[io_light-40];
    // }


    if((io_light<21) && (io_light>0) ) //4,5,6 jidianqi  usb1
    {
        //当前灯点亮
        if(io_light<9)
            bit_set(&tx1[0], io_light,val);
        if((io_light>8) && (io_light<17))
            bit_set(&tx1[1], io_light-8,val);
        if(io_light>16)
            bit_set(&tx1[2], io_light-16,val);
    }
    if((io_light>20) && (io_light<41))    // 6,7,8 jidianqi  usb0
    {
        io_light=io_light-20;
        //当前灯点亮
        if(io_light<9)
            bit_set(&tx0[0], io_light,val);
        if((io_light>8) && (io_light<17))
            bit_set(&tx0[1], io_light-8,val);
        if(io_light>16)
            bit_set(&tx0[2], io_light-16,val);
    }
}
