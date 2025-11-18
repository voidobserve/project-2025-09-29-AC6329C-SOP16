#ifndef led_srtip_sys_h
#define led_srtip_sys_h

// 控灯系统属性
#include "cpu.h"
/******************************************************************系统cfg******************************************************************/
/*定义数据总线大端/小端，二选一*/
#define SYS_LITTLE_END 0
#define SYS_BIG_END 1
// 定义协议大小端
#define PROTOCOL_END SYS_BIG_END
#define SYS_BUS_END SYS_LITTLE_END

#if (PROTOCOL_END == SYS_BUS_END)
#define __SWP16(n) n
#define __SWP32(n) n
#endif
#if (PROTOCOL_END != SYS_BUS_END)
#define __SWP16(n) ((((u16)(n) & 0xff00) >> 8) | (((u16)(n) & 0x00FF) << 8))
#define __SWP32(n) (((u32)(n) & 0xff000000 >> 24) | \
                    ((u32)(n) & 0x00ff0000 >> 8) |  \
                    ((u32)(n) & 0x0000ff00 << 8) |  \
                    ((u32)(n) & 0x000000ff << 24))

#endif
/******************************************************************灯具配置 cfg******************************************************************/

#define TYPE_Fiber_optic_lights (1) // 光纤灯
#define TYPE_Magic_lights (2)       // 幻彩灯
#define LED_STRIP_TYPE TYPE_Fiber_optic_lights

/* 定义灯珠颜色，五选1*/
// #define LED_STRIP_R
// #define LED_STRIP_RG
// #define LED_STRIP_RGB
#define LED_STRIP_RGBW 1
// #define LED_STRIP_RGBCW

/* 定义灯珠通道,取值1~5*/
#define LED_STRIP_CH 3

/******************************************************************common*****************************************************************/

#define MAX_BRIGHT_RANK 10 // 七彩灯的最大亮度等级
#define MAX_SPEED_RANK 10  // 七彩灯的最大速度等级

typedef struct _HSV_COLOUR_DAT
{
    u32 h_val;
    u32 s_val;
    u32 v_val;
} HSV_COLOUR_DAT;

typedef struct
{
    u16 h_val; // h的数值，色相 0~360
    u16 s_val; // s的数值，饱和度 0~360
    u16 v_val; // v的数值，亮度0~1000
} hsv_t;

typedef struct
{
#ifdef LED_STRIP_R
    u8 r;
#endif
#ifdef LED_STRIP_RG
    u8 r;
    u8 g;
#endif
#ifdef LED_STRIP_RGB
    u8 r;
    u8 g;
    u8 b;
#endif
#ifdef LED_STRIP_RGBW
    u8 r;
    u8 g;
    u8 b;
    u8 w;
#endif
#ifdef LED_STRIP_RGBCW
    u8 r;
    u8 g;
    u8 b;
    u8 c;
    u8 w;
#endif
} color_t;

typedef enum
{
    DEVICE_OFF, // 关机
    DEVICE_ON,  // 开机
} ON_OFF_FLAG;

typedef enum
{
    IS_AUTO,
    IS_PAUSE,
} _AUTO_T;

typedef enum
{
    PHONE_MIC,    // 手机麦克风
    EXTERIOR_MIC, // 外部麦克风
} MIC_TYPE_T;

typedef enum
{
    IR_TIMER_NO = 0, // 无定时
    IR_TIMER_30MIN = 30 * 60 * 1000,
    IR_TIMER_60MIN = 60 * 60 * 1000,
    IR_TIMER_90MIN = 90 * 60 * 1000,
    IR_TIMER_120MIN = 120 * 60 * 1000,
} AUTO_TIME_T;

enum
{
    MSG_SEQUENCER_NONE = 0x00,
    MSG_SEQUENCER_ONE_WIRE_SEND_INFO, // 使能单线发送
    MSG_METEOR_LIGHTS_ON,             // 流星灯开启

    MSG_USER_SAVE_INFO, // 将数据写入flash
};

extern const u8 led_b_array[MAX_BRIGHT_RANK];
extern const u16 led_speed_array[MAX_SPEED_RANK]; // 存放旧版的七彩灯动态模式下的所有速度值
extern const u16 colorful_lights_speed_array[MAX_SPEED_RANK]; // 存放七彩灯动态模式下的所有速度值

void ls_add_bright(void); // 增加亮度
void ls_sub_bright(void); // 减少亮度

void ls_add_sensitive(void); // 遥控加灵敏度
void ls_sub_sensitive(void); // 遥控减灵敏度

void ls_add_star_speed(void); // 还未测试
void ls_sub_star_speed(void); // 还未测试

void set_static_mode(u8 r, u8 g, u8 b);
void colorful_lights_set_static_mode(color_t colors_structure); // 七彩灯设置为静态模式，颜色值由传参设定

void ls_set_music_mode(void); // 通过遥控器控制七彩灯的音乐模式切换

void OpenMortor(void);
void CloseMotor(void);

void app_set_bright(u8 tp_b);
void app_set_sensitive(u8 tp_s);   // 通过app设置灵敏度
void app_set_mereor_mode(u8 tp_m); // 通过app设置流星模式
void app_set_meteor_pro(u8 tp_p);  // 通过app设置流星周期时间

void soft_turn_on_the_light(void); // 软件打开设备
void soft_turn_off_lights(void);   // 软件关闭设备

#endif
