#ifndef GLOBAL_H
#define GLOBAL_H

#endif // GLOBAL_H
#include <QHash>

extern char recData[64];

extern int flag_run;

extern int curr_status;

 extern unsigned char rx0[500];
extern unsigned char tx0[10];
extern int a_p_0;

extern unsigned char rx1[500];
extern unsigned char tx1[10];
extern int a_p_1;

extern unsigned char rx2[500];
extern unsigned char tx2[10];
extern int a_p_2;

extern unsigned char tx2_1[3];
extern unsigned char tx2_0[3];

extern unsigned char tx1_2[3];
extern unsigned char tx1_0[3];

extern unsigned char tx0_2[3];
extern unsigned char tx0_1[3];

extern int io_7;
extern int io_15;
extern int io_1;

extern int pre_io_7;
extern int pre_io_15;
extern int pre_io_1;
extern int io_28 ;//茶室墙2

extern int io_41 ;//老人房1
extern int io_43 ;//一卫1
extern int io_30;//中厨3
extern int pre_io_30;//中厨3
extern int pre_io_28 ;//茶室墙2

extern int io_51 ;//次卫1
extern int io_52 ;//次卧灯2
extern int io_55 ;//主卫灯2
extern int io_54 ;//主卧灯2
extern int io_45; //西厨感应

extern int pre_io_41 ;//老人房1
extern int pre_io_43 ;//一卫1
extern int pre_io_45; //西厨感应
extern int pre_io_51 ;//次卫1
extern int pre_io_52 ;//次卧灯2
extern int pre_io_55 ;//主卫灯2
extern int pre_io_54 ;//主卧灯2

extern QHash<int,int> light_status;  //灯的状态
