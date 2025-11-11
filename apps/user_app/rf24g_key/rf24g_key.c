#include "rf24g_key.h"

#include "../../../apps/user_app/one_wire/one_wire.h" // 包含电机的驱动程序

#include "../../../apps/user_app/led_strip/led_strand_effect.h"              // 包含 fc_effect 的声明
#include "../../../apps/user_app/ws2812-fx-lib/WS2812FX_C/ws2812fx_effect.h" // 包含部分写好的动画
#include "../../../apps/user_app/ws2812-fx-lib/WS2812FX_C/WS2812FX.H"        // 包含 ws2812的部分函数接口
#include "../../../apps/user_app/one_wire/one_wire.h"                        // 包含电机的驱动程序
#include "../../../apps/user_app/save_flash/save_flash.h"                    // 包含读写flash的接口

#include "../../../apps/user_app/rf24g_key/rf24g_key_event_handle.h"

#if 1

static volatile u8 rf24g_rx_flag = 0;               // 是否收到了新的数据
static volatile u8 rf24g_recved_key_val = 0;        // 存放接收到的按键键值
static volatile u8 rf24g_dynamic_code_last = 0;     // 存放动态码，长按--动态码会一直变化，短按--动态码不变
static volatile u8 rf24g_dynamic_code_cur = 0;      // 存放动态码，长按--动态码会一直变化，短按--动态码不变
static volatile u8 rf24g_dynamic_code_same_cnt = 0; // 存放动态码连续相同次数
static volatile u8 rf24g_remote_type = RF24G_REMOTE_TYPE_NONE;

volatile u8 rf24g_key_driver_event = 0; // 由key_driver_scan() 更新
volatile u8 rf24g_key_driver_value = 0; // 由key_driver_scan() 更新

// volatile rf24g_recv_info_t rf24g_recv_info;  // 存放接收到的数据包
// volatile u8 chromatic_circle_val = 0;        // 存放色环按键对应的数值，范围：0x00~0xFF

const u8 rf24g_key_event_table[][RF34G_KEY_EVENT_MAX + 1] = {
    // ==============================
    // 白色面板1，2.4G遥控器按键：
    {RF24G_WHITE_KEY_1_VAL_R1C1, RF24G_WHITE_KEY_1_EVENT_R1C1_CLICK, RF24G_WHITE_KEY_1_EVENT_R1C1_LONG, RF24G_WHITE_KEY_1_EVENT_R1C1_HOLD, RF24G_WHITE_KEY_1_EVENT_R1C1_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R1C2, RF24G_WHITE_KEY_1_EVENT_R1C2_CLICK, RF24G_WHITE_KEY_1_EVENT_R1C2_LONG, RF24G_WHITE_KEY_1_EVENT_R1C2_HOLD, RF24G_WHITE_KEY_1_EVENT_R1C2_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R1C3, RF24G_WHITE_KEY_1_EVENT_R1C3_CLICK, RF24G_WHITE_KEY_1_EVENT_R1C3_LONG, RF24G_WHITE_KEY_1_EVENT_R1C3_HOLD, RF24G_WHITE_KEY_1_EVENT_R1C3_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R1C4, RF24G_WHITE_KEY_1_EVENT_R1C4_CLICK, RF24G_WHITE_KEY_1_EVENT_R1C4_LONG, RF24G_WHITE_KEY_1_EVENT_R1C4_HOLD, RF24G_WHITE_KEY_1_EVENT_R1C4_LOOSE},

    {RF24G_WHITE_KEY_1_VAL_R2C1, RF24G_WHITE_KEY_1_EVENT_R2C1_CLICK, RF24G_WHITE_KEY_1_EVENT_R2C1_LONG, RF24G_WHITE_KEY_1_EVENT_R2C1_HOLD, RF24G_WHITE_KEY_1_EVENT_R2C1_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R2C2, RF24G_WHITE_KEY_1_EVENT_R2C2_CLICK, RF24G_WHITE_KEY_1_EVENT_R2C2_LONG, RF24G_WHITE_KEY_1_EVENT_R2C2_HOLD, RF24G_WHITE_KEY_1_EVENT_R2C2_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R2C3, RF24G_WHITE_KEY_1_EVENT_R2C3_CLICK, RF24G_WHITE_KEY_1_EVENT_R2C3_LONG, RF24G_WHITE_KEY_1_EVENT_R2C3_HOLD, RF24G_WHITE_KEY_1_EVENT_R2C3_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R2C4, RF24G_WHITE_KEY_1_EVENT_R2C4_CLICK, RF24G_WHITE_KEY_1_EVENT_R2C4_LONG, RF24G_WHITE_KEY_1_EVENT_R2C4_HOLD, RF24G_WHITE_KEY_1_EVENT_R2C4_LOOSE},

    {RF24G_WHITE_KEY_1_VAL_R3C1, RF24G_WHITE_KEY_1_EVENT_R3C1_CLICK, RF24G_WHITE_KEY_1_EVENT_R3C1_LONG, RF24G_WHITE_KEY_1_EVENT_R3C1_HOLD, RF24G_WHITE_KEY_1_EVENT_R3C1_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R3C2, RF24G_WHITE_KEY_1_EVENT_R3C2_CLICK, RF24G_WHITE_KEY_1_EVENT_R3C2_LONG, RF24G_WHITE_KEY_1_EVENT_R3C2_HOLD, RF24G_WHITE_KEY_1_EVENT_R3C2_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R3C3, RF24G_WHITE_KEY_1_EVENT_R3C3_CLICK, RF24G_WHITE_KEY_1_EVENT_R3C3_LONG, RF24G_WHITE_KEY_1_EVENT_R3C3_HOLD, RF24G_WHITE_KEY_1_EVENT_R3C3_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R3C4, RF24G_WHITE_KEY_1_EVENT_R3C4_CLICK, RF24G_WHITE_KEY_1_EVENT_R3C4_LONG, RF24G_WHITE_KEY_1_EVENT_R3C4_HOLD, RF24G_WHITE_KEY_1_EVENT_R3C4_LOOSE},

    {RF24G_WHITE_KEY_1_VAL_R4C1, RF24G_WHITE_KEY_1_EVENT_R4C1_CLICK, RF24G_WHITE_KEY_1_EVENT_R4C1_LONG, RF24G_WHITE_KEY_1_EVENT_R4C1_HOLD, RF24G_WHITE_KEY_1_EVENT_R4C1_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R4C2, RF24G_WHITE_KEY_1_EVENT_R4C2_CLICK, RF24G_WHITE_KEY_1_EVENT_R4C2_LONG, RF24G_WHITE_KEY_1_EVENT_R4C2_HOLD, RF24G_WHITE_KEY_1_EVENT_R4C2_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R4C3, RF24G_WHITE_KEY_1_EVENT_R4C3_CLICK, RF24G_WHITE_KEY_1_EVENT_R4C3_LONG, RF24G_WHITE_KEY_1_EVENT_R4C3_HOLD, RF24G_WHITE_KEY_1_EVENT_R4C3_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R4C4, RF24G_WHITE_KEY_1_EVENT_R4C4_CLICK, RF24G_WHITE_KEY_1_EVENT_R4C4_LONG, RF24G_WHITE_KEY_1_EVENT_R4C4_HOLD, RF24G_WHITE_KEY_1_EVENT_R4C4_LOOSE},

    {RF24G_WHITE_KEY_1_VAL_R5C1, RF24G_WHITE_KEY_1_EVENT_R5C1_CLICK, RF24G_WHITE_KEY_1_EVENT_R5C1_LONG, RF24G_WHITE_KEY_1_EVENT_R5C1_HOLD, RF24G_WHITE_KEY_1_EVENT_R5C1_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R5C2, RF24G_WHITE_KEY_1_EVENT_R5C2_CLICK, RF24G_WHITE_KEY_1_EVENT_R5C2_LONG, RF24G_WHITE_KEY_1_EVENT_R5C2_HOLD, RF24G_WHITE_KEY_1_EVENT_R5C2_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R5C3, RF24G_WHITE_KEY_1_EVENT_R5C3_CLICK, RF24G_WHITE_KEY_1_EVENT_R5C3_LONG, RF24G_WHITE_KEY_1_EVENT_R5C3_HOLD, RF24G_WHITE_KEY_1_EVENT_R5C3_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R5C4, RF24G_WHITE_KEY_1_EVENT_R5C4_CLICK, RF24G_WHITE_KEY_1_EVENT_R5C4_LONG, RF24G_WHITE_KEY_1_EVENT_R5C4_HOLD, RF24G_WHITE_KEY_1_EVENT_R5C4_LOOSE},

    {RF24G_WHITE_KEY_1_VAL_R6C1, RF24G_WHITE_KEY_1_EVENT_R6C1_CLICK, RF24G_WHITE_KEY_1_EVENT_R6C1_LONG, RF24G_WHITE_KEY_1_EVENT_R6C1_HOLD, RF24G_WHITE_KEY_1_EVENT_R6C1_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R6C2, RF24G_WHITE_KEY_1_EVENT_R6C2_CLICK, RF24G_WHITE_KEY_1_EVENT_R6C2_LONG, RF24G_WHITE_KEY_1_EVENT_R6C2_HOLD, RF24G_WHITE_KEY_1_EVENT_R6C2_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R6C3, RF24G_WHITE_KEY_1_EVENT_R6C3_CLICK, RF24G_WHITE_KEY_1_EVENT_R6C3_LONG, RF24G_WHITE_KEY_1_EVENT_R6C3_HOLD, RF24G_WHITE_KEY_1_EVENT_R6C3_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R6C4, RF24G_WHITE_KEY_1_EVENT_R6C4_CLICK, RF24G_WHITE_KEY_1_EVENT_R6C4_LONG, RF24G_WHITE_KEY_1_EVENT_R6C4_HOLD, RF24G_WHITE_KEY_1_EVENT_R6C4_LOOSE},

    {RF24G_WHITE_KEY_1_VAL_R7C1, RF24G_WHITE_KEY_1_EVENT_R7C1_CLICK, RF24G_WHITE_KEY_1_EVENT_R7C1_LONG, RF24G_WHITE_KEY_1_EVENT_R7C1_HOLD, RF24G_WHITE_KEY_1_EVENT_R7C1_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R7C2, RF24G_WHITE_KEY_1_EVENT_R7C2_CLICK, RF24G_WHITE_KEY_1_EVENT_R7C2_LONG, RF24G_WHITE_KEY_1_EVENT_R7C2_HOLD, RF24G_WHITE_KEY_1_EVENT_R7C2_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R7C3, RF24G_WHITE_KEY_1_EVENT_R7C3_CLICK, RF24G_WHITE_KEY_1_EVENT_R7C3_LONG, RF24G_WHITE_KEY_1_EVENT_R7C3_HOLD, RF24G_WHITE_KEY_1_EVENT_R7C3_LOOSE},
    {RF24G_WHITE_KEY_1_VAL_R7C4, RF24G_WHITE_KEY_1_EVENT_R7C4_CLICK, RF24G_WHITE_KEY_1_EVENT_R7C4_LONG, RF24G_WHITE_KEY_1_EVENT_R7C4_HOLD, RF24G_WHITE_KEY_1_EVENT_R7C4_LOOSE},
};

const u8 rf24g_key_2_event_table[][RF34G_KEY_EVENT_MAX + 1] = {
    // ==============================
    // 白色面板2，2.4G遥控器按键：
    {RF24G_WHITE_KEY_2_VAL_R1C1, RF24G_WHITE_KEY_2_EVENT_R1C1_CLICK, RF24G_WHITE_KEY_2_EVENT_R1C1_LONG, RF24G_WHITE_KEY_2_EVENT_R1C1_HOLD, RF24G_WHITE_KEY_2_EVENT_R1C1_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R1C2, RF24G_WHITE_KEY_2_EVENT_R1C2_CLICK, RF24G_WHITE_KEY_2_EVENT_R1C2_LONG, RF24G_WHITE_KEY_2_EVENT_R1C2_HOLD, RF24G_WHITE_KEY_2_EVENT_R1C2_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R1C3, RF24G_WHITE_KEY_2_EVENT_R1C3_CLICK, RF24G_WHITE_KEY_2_EVENT_R1C3_LONG, RF24G_WHITE_KEY_2_EVENT_R1C3_HOLD, RF24G_WHITE_KEY_2_EVENT_R1C3_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R1C4, RF24G_WHITE_KEY_2_EVENT_R1C4_CLICK, RF24G_WHITE_KEY_2_EVENT_R1C4_LONG, RF24G_WHITE_KEY_2_EVENT_R1C4_HOLD, RF24G_WHITE_KEY_2_EVENT_R1C4_LOOSE},

    {RF24G_WHITE_KEY_2_VAL_R2C1, RF24G_WHITE_KEY_2_EVENT_R2C1_CLICK, RF24G_WHITE_KEY_2_EVENT_R2C1_LONG, RF24G_WHITE_KEY_2_EVENT_R2C1_HOLD, RF24G_WHITE_KEY_2_EVENT_R2C1_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R2C2, RF24G_WHITE_KEY_2_EVENT_R2C2_CLICK, RF24G_WHITE_KEY_2_EVENT_R2C2_LONG, RF24G_WHITE_KEY_2_EVENT_R2C2_HOLD, RF24G_WHITE_KEY_2_EVENT_R2C2_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R2C3, RF24G_WHITE_KEY_2_EVENT_R2C3_CLICK, RF24G_WHITE_KEY_2_EVENT_R2C3_LONG, RF24G_WHITE_KEY_2_EVENT_R2C3_HOLD, RF24G_WHITE_KEY_2_EVENT_R2C3_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R2C4, RF24G_WHITE_KEY_2_EVENT_R2C4_CLICK, RF24G_WHITE_KEY_2_EVENT_R2C4_LONG, RF24G_WHITE_KEY_2_EVENT_R2C4_HOLD, RF24G_WHITE_KEY_2_EVENT_R2C4_LOOSE},

    {RF24G_WHITE_KEY_2_VAL_R3C1, RF24G_WHITE_KEY_2_EVENT_R3C1_CLICK, RF24G_WHITE_KEY_2_EVENT_R3C1_LONG, RF24G_WHITE_KEY_2_EVENT_R3C1_HOLD, RF24G_WHITE_KEY_2_EVENT_R3C1_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R3C2, RF24G_WHITE_KEY_2_EVENT_R3C2_CLICK, RF24G_WHITE_KEY_2_EVENT_R3C2_LONG, RF24G_WHITE_KEY_2_EVENT_R3C2_HOLD, RF24G_WHITE_KEY_2_EVENT_R3C2_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R3C3, RF24G_WHITE_KEY_2_EVENT_R3C3_CLICK, RF24G_WHITE_KEY_2_EVENT_R3C3_LONG, RF24G_WHITE_KEY_2_EVENT_R3C3_HOLD, RF24G_WHITE_KEY_2_EVENT_R3C3_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R3C4, RF24G_WHITE_KEY_2_EVENT_R3C4_CLICK, RF24G_WHITE_KEY_2_EVENT_R3C4_LONG, RF24G_WHITE_KEY_2_EVENT_R3C4_HOLD, RF24G_WHITE_KEY_2_EVENT_R3C4_LOOSE},

    {RF24G_WHITE_KEY_2_VAL_R4C1, RF24G_WHITE_KEY_2_EVENT_R4C1_CLICK, RF24G_WHITE_KEY_2_EVENT_R4C1_LONG, RF24G_WHITE_KEY_2_EVENT_R4C1_HOLD, RF24G_WHITE_KEY_2_EVENT_R4C1_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R4C2, RF24G_WHITE_KEY_2_EVENT_R4C2_CLICK, RF24G_WHITE_KEY_2_EVENT_R4C2_LONG, RF24G_WHITE_KEY_2_EVENT_R4C2_HOLD, RF24G_WHITE_KEY_2_EVENT_R4C2_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R4C3, RF24G_WHITE_KEY_2_EVENT_R4C3_CLICK, RF24G_WHITE_KEY_2_EVENT_R4C3_LONG, RF24G_WHITE_KEY_2_EVENT_R4C3_HOLD, RF24G_WHITE_KEY_2_EVENT_R4C3_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R4C4, RF24G_WHITE_KEY_2_EVENT_R4C4_CLICK, RF24G_WHITE_KEY_2_EVENT_R4C4_LONG, RF24G_WHITE_KEY_2_EVENT_R4C4_HOLD, RF24G_WHITE_KEY_2_EVENT_R4C4_LOOSE},

    {RF24G_WHITE_KEY_2_VAL_R5C1, RF24G_WHITE_KEY_2_EVENT_R5C1_CLICK, RF24G_WHITE_KEY_2_EVENT_R5C1_LONG, RF24G_WHITE_KEY_2_EVENT_R5C1_HOLD, RF24G_WHITE_KEY_2_EVENT_R5C1_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R5C2, RF24G_WHITE_KEY_2_EVENT_R5C2_CLICK, RF24G_WHITE_KEY_2_EVENT_R5C2_LONG, RF24G_WHITE_KEY_2_EVENT_R5C2_HOLD, RF24G_WHITE_KEY_2_EVENT_R5C2_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R5C3, RF24G_WHITE_KEY_2_EVENT_R5C3_CLICK, RF24G_WHITE_KEY_2_EVENT_R5C3_LONG, RF24G_WHITE_KEY_2_EVENT_R5C3_HOLD, RF24G_WHITE_KEY_2_EVENT_R5C3_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R5C4, RF24G_WHITE_KEY_2_EVENT_R5C4_CLICK, RF24G_WHITE_KEY_2_EVENT_R5C4_LONG, RF24G_WHITE_KEY_2_EVENT_R5C4_HOLD, RF24G_WHITE_KEY_2_EVENT_R5C4_LOOSE},

    {RF24G_WHITE_KEY_2_VAL_R6C1, RF24G_WHITE_KEY_2_EVENT_R6C1_CLICK, RF24G_WHITE_KEY_2_EVENT_R6C1_LONG, RF24G_WHITE_KEY_2_EVENT_R6C1_HOLD, RF24G_WHITE_KEY_2_EVENT_R6C1_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R6C2, RF24G_WHITE_KEY_2_EVENT_R6C2_CLICK, RF24G_WHITE_KEY_2_EVENT_R6C2_LONG, RF24G_WHITE_KEY_2_EVENT_R6C2_HOLD, RF24G_WHITE_KEY_2_EVENT_R6C2_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R6C3, RF24G_WHITE_KEY_2_EVENT_R6C3_CLICK, RF24G_WHITE_KEY_2_EVENT_R6C3_LONG, RF24G_WHITE_KEY_2_EVENT_R6C3_HOLD, RF24G_WHITE_KEY_2_EVENT_R6C3_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R6C4, RF24G_WHITE_KEY_2_EVENT_R6C4_CLICK, RF24G_WHITE_KEY_2_EVENT_R6C4_LONG, RF24G_WHITE_KEY_2_EVENT_R6C4_HOLD, RF24G_WHITE_KEY_2_EVENT_R6C4_LOOSE},

    {RF24G_WHITE_KEY_2_VAL_R7C1, RF24G_WHITE_KEY_2_EVENT_R7C1_CLICK, RF24G_WHITE_KEY_2_EVENT_R7C1_LONG, RF24G_WHITE_KEY_2_EVENT_R7C1_HOLD, RF24G_WHITE_KEY_2_EVENT_R7C1_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R7C2, RF24G_WHITE_KEY_2_EVENT_R7C2_CLICK, RF24G_WHITE_KEY_2_EVENT_R7C2_LONG, RF24G_WHITE_KEY_2_EVENT_R7C2_HOLD, RF24G_WHITE_KEY_2_EVENT_R7C2_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R7C3, RF24G_WHITE_KEY_2_EVENT_R7C3_CLICK, RF24G_WHITE_KEY_2_EVENT_R7C3_LONG, RF24G_WHITE_KEY_2_EVENT_R7C3_HOLD, RF24G_WHITE_KEY_2_EVENT_R7C3_LOOSE},
    {RF24G_WHITE_KEY_2_VAL_R7C4, RF24G_WHITE_KEY_2_EVENT_R7C4_CLICK, RF24G_WHITE_KEY_2_EVENT_R7C4_LONG, RF24G_WHITE_KEY_2_EVENT_R7C4_HOLD, RF24G_WHITE_KEY_2_EVENT_R7C4_LOOSE},
};

static u8 rf24g_get_key_value(void); // 获取按键键值的函数声明
volatile struct key_driver_para rf24g_scan_para = {
    .scan_time = RF24G_KEY_SCAN_TIME_MS,                                                     // 按键扫描频率, 单位: ms
    .last_key = NO_KEY,                                                                      // 上一次get_value按键值, 初始化为NO_KEY;
    .filter_time = RF24G_KEY_SCAN_FILTER_TIME_MS,                                            // 按键消抖延时;
    .long_time = RF24G_KEY_LONG_TIME_MS / RF24G_KEY_SCAN_TIME_MS,                            // 按键判定长按数量
    .hold_time = (RF24G_KEY_LONG_TIME_MS + RF24G_KEY_HOLD_TIME_MS) / RF24G_KEY_SCAN_TIME_MS, // 按键判定HOLD数量
    .click_delay_time = RF24G_KEY_SCAN_CLICK_DELAY_TIME_MS,                                  // 按键被抬起后等待连击延时数量
    .key_type = KEY_DRIVER_TYPE_RF24GKEY,
    .get_value = rf24g_get_key_value,
};

// 底层按键扫描，由 __resolve_adv_report() 调用
void rf24g_scan(void *recv_buff)
{
    volatile rf24g_recv_info_t *p = (rf24g_recv_info_t *)recv_buff;
    if (p->header1 == RF24G_HEADER_1 && p->header2 == RF24G_HEADER_2)
    // if (p->header1 == 0xFF && p->header2 == 0xFF)
    {
        // printf("recv: \n");
        // printf_buf(p, sizeof(rf24g_recv_info_t)); // 打印接收到的数据包

        if (0x54 == p->fixed_val_6)
        {
            // 控制带流星灯的设备的遥控器
            rf24g_remote_type = RF24G_REMOTE_TYPE_WITH_METEOR_REMOTE;
        }
        // else if (0x55 == p->fixed_val_6)
        else
        {
            // 控制不带流星灯的设备的遥控器
            rf24g_remote_type = RF24G_REMOTE_TYPE_WITHOUT_METEOR_REMOTE;
        }

        // 直接接收键值
        // rf24g_recv_info = *p; // 结构体变量赋值
        rf24g_recved_key_val = p->key;
        rf24g_dynamic_code_cur = p->dynamic_code_1;

        // 打印键值：
        // printf("key 0x %x\n", (u16)rf24g_recved_key_val);

        // 根据类型，判断是控制带流星灯的遥控器还是不带流星灯的遥控器

        rf24g_rx_flag = 1;
    }
}

// 解码只适用于只响应短按的操作，如果要检测长按，响应短按的时间会变长，现在检测长按会先识别到一次短按，然后才识别到长按
static u8 rf24g_get_key_value(void)
{
    u8 key_value = NO_KEY;
    static volatile u8 time_out_cnt = 0; // 加入超时，防止丢包（超时时间与按键扫描时间有关）
    static u8 last_key_value = NO_KEY;

    if (rf24g_rx_flag == 1) // 收到2.4G广播
    {
        rf24g_rx_flag = 0;

        key_value = rf24g_recved_key_val;

        // 接收到数据包，就更新一次超时计时：
        time_out_cnt = 30; // 2.4G接收可能会丢失100~200ms的数据包（响应会慢一些）(容易识别到多次短按)

        if (rf24g_dynamic_code_last != rf24g_dynamic_code_cur) // 如果动态码不一样，说明可能是长按
        {
            rf24g_dynamic_code_same_cnt = 0;
            rf24g_dynamic_code_last = rf24g_dynamic_code_cur; // 更新记录的动态码
        }
        else
        {
            rf24g_dynamic_code_same_cnt++;
            if (rf24g_dynamic_code_same_cnt >= 3) // 动态码连续相同多次，才认为有短按
            {
                time_out_cnt = 0;
                last_key_value = NO_KEY;
                // printf("%d\n", __LINE__);
                return NO_KEY;
            }

            // printf("%d\n", __LINE__);
        }

        // printf("%d\n", __LINE__);
        last_key_value = key_value;
        return last_key_value;
    }

    if (time_out_cnt > 0)
    {
        // 还在长按的超时延迟中时，返回上一次按键键值
        time_out_cnt--;
        return last_key_value;
    }

    // 如果没有收到数据，也不在长按的超时中：
    return NO_KEY;
}

// 根据按键键值和key_driver_scan得到的事件值，转换为对应的按键事件
u8 rf24g_convert_key_event(u8 key_value, u8 key_driver_event)
{
    // 将key_driver_scan得到的key_event转换成自定义的key_event对应的索引
    // 索引对应 rf24g_key_event_table[][] 中的索引
    u8 key_event_index = 0; // 默认为0，0对应无效索引
    if (KEY_EVENT_CLICK == key_driver_event)
    {
        key_event_index = 1;
    }
    else if (KEY_EVENT_LONG == key_driver_event)
    {
        // long
        key_event_index = 2;
    }
    else if (KEY_EVENT_HOLD == key_driver_event)
    {
        // hold
        key_event_index = 3;
    }
    else if (KEY_EVENT_UP == key_driver_event)
    {
        // 长按后松手
        key_event_index = 4;
    }

    if (0 == key_event_index || NO_KEY == key_value)
    {
        // 按键事件与上面的事件都不匹配
        // 得到的键值是无效键值
        return RF24G_KEY_EVENT_NONE;
    }

    if (RF24G_REMOTE_TYPE_WITH_METEOR_REMOTE == rf24g_remote_type)
    {
        // printf("key type with meteor\n");
        for (u8 i = 0; i < ARRAY_SIZE(rf24g_key_event_table); i++)
        {
            if (key_value == rf24g_key_event_table[i][0])
            {
                return rf24g_key_event_table[i][key_event_index];
            }
        }
    }
    // else if (RF24G_REMOTE_TYPE_WITHOUT_METEOR_REMOTE == rf24g_remote_type)
    else
    {
        // printf("key type without meteor\n");
        for (u8 i = 0; i < ARRAY_SIZE(rf24g_key_2_event_table); i++)
        {
            if (key_value == rf24g_key_2_event_table[i][0])
            {
                return rf24g_key_2_event_table[i][key_event_index];
            }
        }
    }

    // 如果运行到这里，都没有找到对应的按键，返回无效按键事件
    return RF24G_KEY_EVENT_NONE;
}

void rf24_key_handle(void)
{
    u8 rf24g_key_event = 0;

    if (NO_KEY == rf24g_key_driver_value)
        return;

    rf24g_key_event = rf24g_convert_key_event(rf24g_key_driver_value, rf24g_key_driver_event);
    rf24g_key_driver_value = NO_KEY;

    // printf("rf24g_key_event: %u\n", (u16)rf24g_key_event);

    /*
        fc_effect.dream_scene.speed 最大应该是2000
        最小应该是 200
    */
    // fc_effect.dream_scene.speed = 200; // 测试时使用
    // fc_effect.dream_scene.speed = 2000; // 测试时使用

    if (RF24G_KEY_EVENT_NONE == rf24g_key_event)
    {
        return;
    }

    if (DEVICE_OFF == get_on_off_state())
    { // 如果设备没有启动，只对开关按键做处理
        if (
            RF24G_WHITE_KEY_1_EVENT_R1C4_CLICK == rf24g_key_event ||
            RF24G_WHITE_KEY_1_EVENT_R1C4_LONG == rf24g_key_event ||

            RF24G_WHITE_KEY_2_EVENT_R1C4_CLICK == rf24g_key_event ||
            RF24G_WHITE_KEY_2_EVENT_R1C4_LONG == rf24g_key_event)
        {
            soft_turn_on_the_light(); // 打开设备
            // save_user_data_area3();
            os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
            // save_user_data_enable();
        }

        return;
    }

    // 运行到这里，说明设备已经启动
    rf24_key_handle_func_t rf24g_key_handle_func_ptr = rf24_key_handle_func_buff[rf24g_key_event];
    // if (NULL == rf24g_key_handle_func_ptr ||
    //     0 == rf24g_key_handle_func_ptr)
    if (NULL == rf24g_key_handle_func_ptr)
    {
        return;
    }
    rf24g_key_handle_func_ptr();

    // save_user_data_area3();
    os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
    // save_user_data_enable();
}
#endif
