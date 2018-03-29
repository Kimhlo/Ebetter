#include <global.h>

//车子的运行状态
int flag_run;
//车子的动作状态
int curr_status=0;
/*********************************************
 * 发给上位机的状态数据说明：
 * 每次间隔500ms上报一次状态
 * 0：车子的状态，0x01启动，0x00休眠
 * 1：车子当前状态，0x01前进，0x02后退，0x03投食，0x04停止，0x00未启动
 * 2：车子的当前位置，0x00,在起点，0x01在投食的路上，0x02在返回的路上
 * 3：预留
*********************************************/
char sendData[4]={0x00,0x00,0x00,0x00};
/*********************************************
 * 收到上位机的指令说明：
 * 0：控制车子的状态指令，0x01启动，0x00休眠,0x03手动
 * 1：控制车子的动作指令，0x01前进，0x02后退，0x03投食，0x04停止
*********************************************/
char recData[64];
