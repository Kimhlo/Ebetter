#include <global.h>


char recData[64];

int flag_run;

int curr_status=0;

unsigned char rx0[500];
unsigned char tx0[10];
int a_p_0;

unsigned char rx1[500];
unsigned char tx1[10];
int a_p_1;

unsigned char rx2[500];
unsigned char tx2[10];
int a_p_2;

unsigned char tx2_1[3];
unsigned char tx2_0[3];

unsigned char tx1_2[3];
unsigned char tx1_0[3];

unsigned char tx0_2[3];
unsigned char tx0_1[3];

int io_7;
int io_15;
int io_1;
int io_28 ;//茶室墙2

int pre_io_7;
int pre_io_15;
int pre_io_1;


int io_41 ;//老人房1
int io_43 ;//一卫1
int io_30;//中厨3

int io_51 ;//次卫1
int io_52 ;//次卧灯2
int io_55 ;//主卫灯2
int io_54 ;//主卧灯2
int io_45; //西厨感应

int pre_io_41 ;//老人房1
int pre_io_43 ;//一卫1
int pre_io_30;//中厨3
int pre_io_28 ;//茶室墙2

int pre_io_51 ;//次卫1
int pre_io_52 ;//次卧灯2
int pre_io_55 ;//主卫灯2
int pre_io_54 ;//主卧灯2
int pre_io_45; //西厨感应

QHash<int,int> light_status;  //灯的状态
