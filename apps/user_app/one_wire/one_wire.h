#ifndef _ONE_WIRE_H
#define _ONE_WIRE_H

#include "system/includes.h"


typedef struct
{
    // 000:回正
    // 001:区域1摇摆
    // 010:区域2摇摆
    // 011:区域1和区域2摇摆
    // 100:360°正转
    // 101:音乐律动
    u8 mode; //电机模式 
    /*
        000:  8S 旋转一周用8s
        001:  13S; 
        010:  18S ;
        011:  21S ;
        100:  26S  //转速  
              35s 
    */
    u8 period;          
    u8 dir;            //1:反转 0:正转  仅音乐律动模式有效
    u8 music_mode;     //音乐律动下的转动模式
}base_ins_t;


extern const u8 motor_period[6]; // u8 motor_period[6] = {8, 13, 18, 21, 26, 35}; // 转速  app指令，需要将8 13 18 21 26 转换成相应的16进制
void one_wire_set_mode(u8 m); // 设置电机模式
void one_wire_set_period(u8 p); // 设置电机转速 

#endif



