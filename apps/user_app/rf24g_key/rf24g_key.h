#ifndef __RF24G_KEY_H
#define __RF24G_KEY_H

#include "includes.h"

typedef struct
{
    u8 header1;        // 0x55
    u8 header2;        // 0xAA
    u8 fixed_val_1;    // 固定值 0x02
    u8 dynamic_code_1; // 动态码
    u8 fixed_val_2;    // 固定值 0x24
    u8 fixed_val_3;    // 固定值 0xBE
    u8 dynamic_code_2; // 动态码
    u8 fixed_val_4;    // 固定值 0xC0
    u8 fixed_val_5;    // 固定值 0x34
    u8 fixed_val_6;    // 固定值 0x04
    u8 key;          // 键值
    u8 fixed_val_7;    // 固定值 0x00
    u8 crc;            // 校验码
} rf24g_recv_info_t;   // 指令数据

// 定义按键键值：（不是真是的按键键值）
enum
{
    RF24G_KEY_ON_OFF = 0x01, // 开关机

    RF24G_KEY_CHROMATIC_CIRCLE = 0x51, // 色环

    RF24G_KEY_BRIGHTNESS_ADD = 0x03, // 亮度加
    RF24G_KEY_BRIGHTNESS_SUB = 0x06, // 亮度减

    RF24G_KEY_DYNAMIC_SPEED_ADD = 0x04, // 动态速度加
    RF24G_KEY_DYNAMIC_SPEED_SUB = 0x07, // 动态速度减

    RF24G_KEY_MODE_ADD = 0x05, // 模式加
    RF24G_KEY_MODE_SUB = 0x08, // 模式减

    RF24G_KEY_COLD_WHITE_BRIGHTNESS_ADD = 0x0A, // 冷白亮度加，同时暖白亮度减
    RF24G_KEY_COLD_WHITE_BRIGHTNESS_SUB = 0x0D, // 冷白亮度减，同时暖白亮度加

    RF24G_KEY_MOTOR_SPEED_ADD = 0x0B, // 电机速度加
    RF24G_KEY_MOTOR_SPEED_SUB = 0x0E, // 电机速度减

    RF24G_KEY_METEOR_SPEED_ADD = 0x0C, // 流星速度加
    RF24G_KEY_METEOR_SPEED_SUB = 0x0F, // 流星速度减

    RF24G_KEY_NONE, // 无按键

    RF24G_KEY_VAL_R1C1 = 0x4C,
};

// 定义按键事件
enum
{
    RF24G_KEY_EVENT_NONE = 0x00,

    RF24G_KEY_EVENT_R1C1_CLICK,
    RF24G_KEY_EVENT_R1C1_HOLD,
    RF24G_KEY_EVENT_R1C1_LOOSE,

    RF24G_KEY_EVENT_ON_OFF_CLICK,
    RF24G_KEY_EVENT_ON_OFF_HOLD,
    RF24G_KEY_EVENT_ON_OFF_LOOSE,

    RF24G_KEY_EVENT_MODE_ADD_CLICK,
    RF24G_KEY_EVENT_MODE_ADD_HOLD,
    RF24G_KEY_EVENT_MODE_ADD_LOOSE,

    RF24G_KEY_EVENT_MODE_SUB_CLICK,
    RF24G_KEY_EVENT_MODE_SUB_HOLD,
    RF24G_KEY_EVENT_MODE_SUB_LOOSE,

    RF24G_KEY_EVENT_SPEED_ADD_CLICK,
    RF24G_KEY_EVENT_SPEED_ADD_HOLD,
    RF24G_KEY_EVENT_SPEED_ADD_LOOSE,

    RF24G_KEY_EVENT_SPEED_SUB_CLICK,
    RF24G_KEY_EVENT_SPEED_SUB_HOLD,
    RF24G_KEY_EVENT_SPEED_SUB_LOOSE,

    RF24G_KEY_EVENT_DEMO_CLICK,
    RF24G_KEY_EVENT_DEMO_HOLD,
    RF24G_KEY_EVENT_DEMO_LOOSE,

    RF24G_KEY_EVENT_COLOR_ADD_CLICK,
    RF24G_KEY_EVENT_COLOR_ADD_HOLD,
    RF24G_KEY_EVENT_COLOR_ADD_LOOSE,

    RF24G_KEY_EVENT_COLOR_SUB_CLICK,
    RF24G_KEY_EVENT_COLOR_SUB_HOLD,
    RF24G_KEY_EVENT_COLOR_SUB_LOOSE,

    RF24G_KEY_EVENT_BRIGHT_ADD_CLICK,
    RF24G_KEY_EVENT_BRIGHT_ADD_HOLD,
    RF24G_KEY_EVENT_BRIGHT_ADD_LOOSE,

    RF24G_KEY_EVENT_BRIGHT_SUB_CLICK,
    RF24G_KEY_EVENT_BRIGHT_SUB_HOLD,
    RF24G_KEY_EVENT_BRIGHT_SUB_LOOSE,

    // 字母B
    RF24G_KEY_EVENT_B_CLICK,
    RF24G_KEY_EVENT_B_HOLD,
    RF24G_KEY_EVENT_B_LOOSE,

    // 字母F
    RF24G_KEY_EVENT_F_CLICK,
    RF24G_KEY_EVENT_F_HOLD,
    RF24G_KEY_EVENT_F_LOOSE,
    // 字母G
    RF24G_KEY_EVENT_G_CLICK,
    RF24G_KEY_EVENT_G_HOLD,
    RF24G_KEY_EVENT_G_LOOSE,
};
#define RF34G_KEY_EVENT_MAX (3) // 按键事件种类个数， 短按、持续hold、松开

#define RF24G_KEY_SCAN_TIME_MS (10)   // 2.4G遥控器按键扫描频率，单位：ms
#define RF24G_KEY_LONG_TIME_MS (1500) // 长按事件触发时间，要长一些，防止频繁短按而进入了长按判断中
#define RF24G_KEY_HOLD_TIME_MS (500)
#define RF24G_KEY_SCAN_CLICK_DELAY_TIME_MS (0) // 2.4G遥控器按键被抬起后等待连击延时数量
#define RF24G_KEY_SCAN_FILTER_TIME_MS (0)      // 2.4G遥控器按键消抖延时

#define RF24G_HEADER_1 0x55
#define RF24G_HEADER_2 0xAA

// 样机遥控器的格式：
// #define RF24G_HEADER_1 0x5E
// #define RF24G_HEADER_2 0x01

extern volatile struct key_driver_para rf24g_scan_para;

extern void rf24g_scan(void *recv_buff);
extern void rf24_key_handle(void);

#endif // end of file
