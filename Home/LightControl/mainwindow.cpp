#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#define  IN1_1 29
#define  IN1_2 28
#define  IN1_3 27
#define  IN1_4 26

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

     readIni();

     flag_run=0;

     if((fd_usb2 = serialOpen("/dev/ttyUSB2",9600)) < 0)   //null rs485
         qDebug()<<"open usb0 error\n";

     myThread_1 = new RecThread;
     connect(myThread_1, SIGNAL(UpdateSignal(int,int,int)), this, SLOT(UpdateSlot(int,int,int)));
     myThread_1->start(QThread::InheritPriority);
     //=======================================
     bool retBind;
     udpSocket_5000 = new QUdpSocket;

     udpSocket_5000->setReadBufferSize(512);   //现场
     retBind=udpSocket_5000-> bind(5000,QUdpSocket::ShareAddress);
     if(false==retBind)
     {
        qDebug()<<"bind port:5000 error";
        return;
     }
     connect(udpSocket_5000, SIGNAL(readyRead()), this, SLOT(udp_rec_5000()));
     send_cnt=0;
     //=======================================
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::udp_rec_5000()
{
    QByteArray arr;
    while(udpSocket_5000->hasPendingDatagrams())
    {
        arr.resize(udpSocket_5000->pendingDatagramSize());
        udpSocket_5000->readDatagram(arr.data(), arr.size());
    }
   // IP_sensor=udpSocket_5000->peerAddress(); //获取数据包地址

    this->ui->label->setText(arr.toHex());
}


void MainWindow::UpdateSlot(int num1,int num2,int s)
{
    if(s==5) //灯带的状态发生了变化，发信号给菲利浦的控制器
    {
        QByteArray arr0;
        arr0.resize(13);
        arr0[0]=io_41; //老人房1
        arr0[1]=io_43; //一卫1
        arr0[2]=io_30; //中厨3
        arr0[3]=io_28; //茶室墙2
        arr0[4]=io_51; //次卫1
        arr0[5]=io_52; //次卧灯2
        arr0[6]=io_55; //主卫灯2
        arr0[7]=io_54; //主卧灯2
        arr0[8]=io_15; //二走廊4
        arr0[9]=io_7; //书房4
        arr0[10]=io_1; //大厅灯5
        arr0[11]=io_45; //西厨感应

        ui->label_6->setText(arr0.toHex());
        dIP.setAddress("192.168.31.134");
        int rt= udpSocket_5000->writeDatagram(arr0,12,dIP,6000);
        if(rt<0)
        {
            qDebug()<<"send ph  error";
            return;
        }
    }
   // send_cnt++;
    if(s==3)
    {
        //send_cnt=0;
        QByteArray arr;
        arr.resize(17);
        arr[0]=(rx1[2+a_p_1]>>(10-9)) & 0x01 ;//10  客卫生间 调光玻璃开关
        arr[1]=(rx1[7+a_p_1]>>(55-49)) & 0x01;   //西厨2  55 调光玻璃开关
        arr[2]=1;   //西厨1  调光玻璃开关  待查
        arr[3]=1;   //西厨1  调光玻璃开关  待查

        arr[4]=(rx1[8+a_p_1]>>(56-49)) & 0x01; //二层主厕  56	调光玻璃开关
        arr[5]=(rx0[8+a_p_0]>>(59-57)) & 0x01; //茶室上 119 调光玻璃开关
        arr[6]=(rx0[8+a_p_0]>>(58-57)) & 0x01;  //二层北卧窗台 118	窗户关闭检测
        arr[7]=(rx0[8+a_p_0]>>(57-57)) & 0x01;  // 二层南卧窗台 117	窗户关闭检测
        arr[8]=(rx0[8+a_p_0]>>(56-49)) & 0x01; //茶室窗台	116	窗户关闭检测
        arr[9]=(rx0[8+a_p_0]>>(55-49)) & 0x01; //一层南卧窗台	115	窗户关闭检测
        arr[10]=(rx0[7+a_p_0]>>(54-49)) & 0x01; //主厕屋顶	114	主厕所门关闭检测
        arr[11]=(rx0[7+a_p_0]>>(53-49)) & 0x01; //次厕所屋顶 113	次厕所门关闭检测
        arr[12]=(rx0[7+a_p_0]>>(52-49)) & 0x01; //一层厕所	 112	一层厕所门关闭检测

        arr[13]=(rx0[2+a_p_0]>>(10-9)) & 0x01;//茶室感应
        arr[14]=0;//(rx0[8+a_p_0]>>(57-57)) & 0x01;
        arr[15]=0;//(rx0[8+a_p_0]>>(57-57)) & 0x01;

        arr[16]=light_status[46];  //预留给蜡烛灯
        ui->label_2->setText(arr.toHex());
        dIP.setAddress("192.168.31.134");
        qint64 rt ;
        rt= udpSocket_5000->writeDatagram(arr,17,dIP,5003);
        if(rt<0)
        {
            qDebug()<<"open window error";
            return;
        }
        //================================
        QByteArray arr2;
        arr2.resize(1);
        arr2[0]=arr[11]; //次厕所屋顶 111	次厕所门关闭检测
        dIP.setAddress("192.168.31.111");
        rt= udpSocket_5000->writeDatagram(arr2,1,dIP,5002);
        if(rt<0)
        {
            qDebug()<<"open window error";
            return;
        }
        //================================
        QByteArray arr4;
        arr4.resize(2);
        arr4[0]=0x02; //大门顶 110  大门口的灯开关检测

        if (((rx1[2+a_p_1]>>2) & 0x01) ==1)
            doorLightSwitch=0x00;   //close light
        else
            doorLightSwitch=0x01; //open light

        if((pre_doorLightSwitch ^ doorLightSwitch)==1)
        {
            arr4[1]=doorLightSwitch;
            dIP.setAddress("192.168.31.110");
            rt= udpSocket_5000->writeDatagram(arr4,2,dIP,5004);
            if(rt<0)
            {
                qDebug()<<"open window error";
                return;
            }
        }

        pre_doorLightSwitch = doorLightSwitch;
        //================================

    //=================================
    QString ss1,ss2,ss3,ss4,ss5,ss6,ss7,ss8,ss9,ss10,ss11;
    int i;
     //=====================================================
     ss1="";
     ss2="";
     ss3="";
     ss4="";
     ss5="";
     ss6="";
     ss7="";
     ss8="";
     for(i=0;i<8;i++)
     {
         ss1.append(QString::number(i+1));
         ss1.append("-");
         ss1.append(QString::number((rx1[1+a_p_1]>>i) & 0x01 ));
         ss1.append(" ");

         ss2.append(QString::number(i+1+8));
         ss2.append("-");
         ss2.append(QString::number((rx1[2+a_p_1]>>i) & 0x01 ));
         ss2.append(" ");

         ss3.append(QString::number(i+1+16));
         ss3.append("-");
         ss3.append(QString::number((rx1[3+a_p_1]>>i) & 0x01 ));
         ss3.append(" ");

         ss4.append(QString::number(i+1+24));
         ss4.append("-");
         ss4.append(QString::number((rx1[4+a_p_1]>>i) & 0x01 ));
         ss4.append(" ");

         ss5.append(QString::number(i+1+32));
         ss5.append("-");
         ss5.append(QString::number((rx1[5+a_p_1]>>i) & 0x01 ));
         ss5.append(" ");

         ss6.append(QString::number(i+1+40));
         ss6.append("-");
         ss6.append(QString::number((rx1[6+a_p_1]>>i) & 0x01 ));
         ss6.append(" ");

         ss7.append(QString::number(i+1+48));
         ss7.append("-");
         ss7.append(QString::number((rx1[7+a_p_1]>>i) & 0x01 ));
         ss7.append(" ");

         ss8.append(QString::number(i+1+56));
         ss8.append("-");
         ss8.append(QString::number((rx1[8+a_p_1]>>i) & 0x01 ));
         ss8.append(" ");
     }

      ui->label_8->setText(ss1 + ss2 +ss3);  //input 0
      ui->label_9->setText(ss4 +ss5 +ss6) ;  //input 1
      ui->label_10->setText(ss7 +ss8.mid(0,20)) ;       //input 2

     //===================================================
      ss1="";
      ss2="";
      ss3="";
      ss4="";
      ss5="";
      ss6="";
      ss7="";
      ss8="";
     for(i=0;i<8;i++)
     {
         ss1.append(QString::number(i+1));
         ss1.append("-");
         ss1.append(QString::number((rx0[1+a_p_0]>>i) & 0x01 ));
         ss1.append(" ");

         ss2.append(QString::number(i+1+8));
         ss2.append("-");
         ss2.append(QString::number((rx0[2+a_p_0]>>i) & 0x01 ));
         ss2.append(" ");

         ss3.append(QString::number(i+1+16));
         ss3.append("-");
         ss3.append(QString::number((rx0[3+a_p_0]>>i) & 0x01 ));
         ss3.append(" ");

         ss4.append(QString::number(i+1+24));
         ss4.append("-");
         ss4.append(QString::number((rx0[4+a_p_0]>>i) & 0x01 ));
         ss4.append(" ");

         ss5.append(QString::number(i+1+32));
         ss5.append("-");
         ss5.append(QString::number((rx0[5+a_p_0]>>i) & 0x01 ));
         ss5.append(" ");

         ss6.append(QString::number(i+1+40));
         ss6.append("-");
         ss6.append(QString::number((rx0[6+a_p_0]>>i) & 0x01 ));
         ss6.append(" ");

         ss7.append(QString::number(i+1+48));
         ss7.append("-");
         ss7.append(QString::number((rx0[7+a_p_0]>>i) & 0x01 ));
         ss7.append(" ");

         ss8.append(QString::number(i+1+56));
         ss8.append("-");
         ss8.append(QString::number((rx0[8+a_p_0]>>i) & 0x01 ));
         ss8.append(" ");
     }

      ui->label_12->setText(ss1 + ss2 +ss3);  //input 0
      ui->label_13->setText(ss4 +ss5 +ss6) ;  //input 1
      ui->label_14->setText(ss7 +ss8.mid(0,20)) ;       //input 2


    ss9="";
    for(i=1;i<21;i++)
    {
        //==========================
        ss9.append(QString::number(i));
        ss9.append("-");
        ss9.append(QString::number(light_status.value(i+40)));
        ss9.append(" ");
       // qDebug()<<"i" <<i<<sensor_light_status.value(i);
    }
    ui->label_5->setText(ss9);  //output   sensor

    ss9="";
    ss10="";
    ss11="";
    for(i=0;i<8;i++)
    {
        //==========================
        ss9.append(QString::number(i+1));
        ss9.append("-");
        ss9.append(QString::number((tx1[0]>>i) & 0x01 ));
        ss9.append(" ");

        ss10.append(QString::number(i+1+8));
        ss10.append("-");
        ss10.append(QString::number((tx1[1]>>i) & 0x01 ));
        ss10.append(" ");

        ss11.append(QString::number(i+1+16));
        ss11.append("-");
        ss11.append(QString::number((tx1[2]>>i) & 0x01 ));
        ss11.append(" ");

    }
     ui->label_11->setText(ss9 + ss10 + ss11.mid(0,20));  //output
     ss9="";
     ss10="";
     ss11="";
     for(i=0;i<8;i++)
     {
         //==========================
         ss9.append(QString::number(i+1));
         ss9.append("-");
         ss9.append(QString::number((tx0[0]>>i) & 0x01 ));
         ss9.append(" ");

         ss10.append(QString::number(i+1+8));
         ss10.append("-");
         ss10.append(QString::number((tx0[1]>>i) & 0x01 ));
         ss10.append(" ");

         ss11.append(QString::number(i+1+16));
         ss11.append("-");
         ss11.append(QString::number((tx0[2]>>i) & 0x01 ));
         ss11.append(" ");
     }

     ui->label_15->setText(ss9 + ss10 + ss11.mid(0,20));  //output
    }
}

void MainWindow::setIni()
{
    QString f1=QCoreApplication::applicationDirPath();
    f1.append("/my.ini");


    QSettings *configIniWrite = new QSettings(f1, QSettings::IniFormat);

    configIniWrite->setValue("/DAQ/IP", ui->lineEdit_IP->text());

    delete configIniWrite;
}

void MainWindow::readIni()
{
    QString iniPath=QCoreApplication::applicationDirPath();
    iniPath.append("/my.ini");

    QSettings *configIniRead = new QSettings(iniPath, QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型

    ui->lineEdit_IP->setText(configIniRead->value("/DAQ/IP").toString());

    delete configIniRead;

}


void MainWindow::on_lineEdit_IP_textEdited(const QString &arg1)
{
    setIni();
}

/*
            QHash<int, int>::iterator v_light;
            for( v_light=vicinitas_light.begin(); v_light!=vicinitas_light.end(); ++v_light)
            {
                if(v_light.value()>40)
                {   //点亮
                    if(flag_kk_switch[v_light.value()-40]==0)
                        sensor_light_status[v_light.value()-40]=1;
                }
            }
            */

//发送数据到192.168.31.134 调光玻璃
void MainWindow::on_pushButton_clicked()
{
    QByteArray arr;
    arr.resize(2);
    arr[0]=0x02;  //第1字节01为打开，第2字节为上下窗户区分， 01开下面窗户 ，02 开上面窗户，
    arr[1]=0x01;

    dIP.setAddress("192.168.31.134");
    qint64 rt ;
    rt= udpSocket_5000->writeDatagram(arr,2,dIP,5001);
    if(rt<0)
    {
        qDebug()<<"open window error";
        return;
    }
}
