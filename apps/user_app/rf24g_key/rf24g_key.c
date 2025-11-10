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
            rf24g_remote_type = RF24G_REMOTE_TYPE_WITH_METEOR_REMOTE;
        }
        // else if (0x55 == p->fixed_val_6)
        else
        {
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

#if 0 // 根据 rf24g_key_event 进行按键事件处理
    color_t color_structure = {0};

    if (DEVICE_OFF == get_on_off_state())
    { // 如果设备没有启动，只对开关按键做处理
        if (
            // RF24G_WHITE_KEY_1_EVENT_R1C4_CLICK == rf24g_key_event ||
            RF24G_WHITE_KEY_1_EVENT_R1C4_LONG == rf24g_key_event ||

            // RF24G_WHITE_KEY_2_EVENT_R1C4_CLICK == rf24g_key_event ||
            RF24G_WHITE_KEY_2_EVENT_R1C4_LONG == rf24g_key_event)
        {
            soft_turn_on_the_light(); // 打开设备
            save_user_data_area3();
        }

        return;
    }

    switch (rf24g_key_event)
    {
        // 收到短按 再执行对应的功能 ；不处理长按的事件

    case RF24G_WHITE_KEY_1_EVENT_R1C1_CLICK: // +
    {                                        // 亮度加、速度加、灵敏度加
      // printf("rf24g_key_event_r1c1_click\n");
        if (IS_STATIC == fc_effect.Now_state)
        {
            /*
                单色模式下，调节亮度
                只修改七彩灯的亮度，不能通过 WS2812FX_setBrightness() 函数调节亮度，
                这样会连流星灯的亮度也修改
            */
            if (fc_effect.ls_b < (MAX_BRIGHT_RANK - 1))
            {
                fc_effect.ls_b++;
            }

            fc_effect.app_b = (fc_effect.ls_b + 1) * 10;
            fc_effect.b = led_b_array[fc_effect.ls_b];
            fb_bright();
        }
        else if (IS_light_scene == fc_effect.Now_state && // 七彩灯的动态模式
                 (MODO_COLORFUL_LIGHTS_FLASH == fc_effect.dream_scene.change_type ||
                  MODE_COLORFUL_LIGHTS_BREATH == fc_effect.dream_scene.change_type ||
                  MODE_COLORFUL_LIGHTS_GRADUAL == fc_effect.dream_scene.change_type ||
                  MODE_COLORFUL_LIGHTS_JUMP == fc_effect.dream_scene.change_type ||
                  MODE_COLORFUL_LIGHTS_AUTO == fc_effect.dream_scene.change_type))
        {
            // 七彩灯动态模式下，调节速度
            // 注意：速度值是越小越快
            // 得到速度等级：
            if (fc_effect.ls_speed > 0)
            {
                fc_effect.ls_speed--;
            }

            fc_effect.dream_scene.speed = colorful_lights_speed_array[fc_effect.ls_speed]; // 根据速度等级进行查表，得到速度值
            fc_effect.app_speed = 100 - (fc_effect.ls_speed) * 10;                         // 根据速度等级得到要反馈给app的速度值
            fb_speed();                                                                    // 给app反馈速度值
            WS2812FX_setSpeed_seg(0, fc_effect.dream_scene.speed);                         // 通过库提供的接口修改速度，而不是修改速度后再调用一次动画函数
            // printf("fc_effect.dream_scene.speed %u\n", (u16)fc_effect.dream_scene.speed);
        }
        else if (IS_light_music == fc_effect.Now_state)
        {
            // 七彩灯声控模式下，调节灵敏度
            colorful_lights_sound_sensitivity_add();
            fb_sensitive(); // 向app反馈灵敏度
        }
        else
        {
            // 其他模式，直接退出，不执行后续的读写flash操作
            return;
        }
    }
    break;
    case RF24G_WHITE_KEY_1_EVENT_R1C2_CLICK: // -
    {                                        // 亮度减、速度减、灵敏度减
        // printf("rf24g_key_event_r1c2_click\n");
        if (IS_STATIC == fc_effect.Now_state)
        {
            /*
                单色模式下，调节亮度
                只修改七彩灯的亮度，不能通过 WS2812FX_setBrightness() 函数调节亮度，
                这样会连流星灯的亮度也修改
            */
            if (fc_effect.ls_b > 0)
            {
                fc_effect.ls_b--;
            }
            fc_effect.app_b = (fc_effect.ls_b + 1) * 10;
            fc_effect.b = led_b_array[fc_effect.ls_b];
            fb_bright();
        }
        else if (IS_light_scene == fc_effect.Now_state &&
                 (MODO_COLORFUL_LIGHTS_FLASH == fc_effect.dream_scene.change_type ||
                  MODE_COLORFUL_LIGHTS_BREATH == fc_effect.dream_scene.change_type ||
                  MODE_COLORFUL_LIGHTS_GRADUAL == fc_effect.dream_scene.change_type ||
                  MODE_COLORFUL_LIGHTS_JUMP == fc_effect.dream_scene.change_type ||
                  MODE_COLORFUL_LIGHTS_AUTO == fc_effect.dream_scene.change_type))
        {
            // 七彩灯动态模式下，调节速度
            // 注意：速度值是越小越快
            // 得到速度等级：
            if (fc_effect.ls_speed < (MAX_SPEED_RANK - 1))
            {
                fc_effect.ls_speed++;
            }

            fc_effect.dream_scene.speed = colorful_lights_speed_array[fc_effect.ls_speed]; // 根据速度等级进行查表，得到速度值
            fc_effect.app_speed = 100 - (fc_effect.ls_speed) * 10;                         // 根据速度等级得到要反馈给app的速度值
            fb_speed();                                                                    // 给app反馈速度值
            WS2812FX_setSpeed_seg(0, fc_effect.dream_scene.speed);                         // 通过库提供的接口修改速度，而不是修改速度后再调用一次动画函数

            // printf("fc_effect.dream_scene.speed %u\n", (u16)fc_effect.dream_scene.speed);
        }
        else if (IS_light_music == fc_effect.Now_state)
        {
            // 七彩灯声控模式下，调节灵敏度
            // ls_sub_sensitive();
            colorful_lights_sound_sensitivity_sub();
            fb_sensitive(); // 向app反馈灵敏度
        }
        else
        {
            // 其他模式，直接退出，不执行后续的读写flash操作
            return;
        }
    }
    break;
    // case RF24G_WHITE_KEY_1_EVENT_R1C3_CLICK: // OFF
    case RF24G_WHITE_KEY_1_EVENT_R1C3_LONG:
    // case RF24G_WHITE_KEY_2_EVENT_R1C3_CLICK:
    case RF24G_WHITE_KEY_2_EVENT_R1C3_LONG:
    {
        // 关闭设备

        soft_turn_off_lights();
    }
    break;
    case RF24G_WHITE_KEY_1_EVENT_R1C4_CLICK: // ON
    {
    }
    break;
    case RF24G_WHITE_KEY_1_EVENT_R2C1_CLICK: // R
    {                                        // printf("key event R2C1 click\n");
        color_structure.r = 0xFF;
        color_structure.g = 0x00;
        color_structure.b = 0x00;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
    }
    break;
    case RF24G_WHITE_KEY_1_EVENT_R2C2_CLICK: // G
        // printf("key event R2C2 click\n");
        color_structure.r = 0x00;
        color_structure.g = 0xFF;
        color_structure.b = 0x00;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);

        break;
    case RF24G_WHITE_KEY_1_EVENT_R2C3_CLICK: // B
        // printf("key event R2C3 click\n");

        color_structure.r = 0x00;
        color_structure.g = 0x00;
        color_structure.b = 0xFF;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);

        break;

    case RF24G_WHITE_KEY_1_EVENT_R2C4_CLICK:

        color_structure.r = 0x00;
        color_structure.g = 0x00;
        color_structure.b = 0x00;
        color_structure.w = 0xFF;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_1_EVENT_R3C1_CLICK:

        color_structure.r = 0xFF;
        color_structure.g = 0xFF / 2;
        color_structure.b = 0x00;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_1_EVENT_R3C2_CLICK:

        color_structure.r = 0x00;
        color_structure.g = 0xFF;
        color_structure.b = 0xFF / 2;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_1_EVENT_R3C3_CLICK:

        color_structure.r = 0x00;
        color_structure.g = 0xFF;
        color_structure.b = 0xFF;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_1_EVENT_R3C4_CLICK:
        color_structure.r = 0xFF;
        color_structure.g = 0xFF;
        color_structure.b = 0xFF;
        color_structure.w = 0xFF;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_1_EVENT_R4C1_CLICK:
        color_structure.r = 0xFF;
        color_structure.g = 0xFF;
        color_structure.b = 0x00;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_1_EVENT_R4C2_CLICK:
        color_structure.r = 0xFF;
        color_structure.g = 0x00;
        color_structure.b = 0xFF / 2;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_1_EVENT_R4C3_CLICK:
        color_structure.r = 0xFF;
        color_structure.g = 0x00;
        color_structure.b = 0xFF;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_1_EVENT_R4C4_CLICK: // 流星灯开关
    {
        if (fc_effect.star_on_off == DEVICE_OFF)
        {
            fc_effect.star_on_off = DEVICE_ON;
            printf("meteor on\n");
        }
        else
        {
            fc_effect.star_on_off = DEVICE_OFF;
            printf("meteor off\n");
        }

        if (DEVICE_ON == fc_effect.star_on_off)
        {
            // 执行一次快速流星动画，在动画结束后，给处理消息的线程发送消息，让它切换流星灯动画
            WS2812FX_setSegment_colorOptions(
                1,                           // 第0段
                1,                           // 起始位置
                fc_effect.led_num - 1,       // 结束位置
                &meteor_effect_when_pwr_on,  // 效果
                0,                           // 颜色
                fc_effect.star_speed,        // 速度
                0);                          // 选项，这里像素点大小：3 REVERSE决定方向
            WS2812FX_resetSegmentRuntime(1); // 清除指定段的显示缓存
            WS2812FX_running_flag_set();

            fd_meteor_on_off(); // 向app反馈流星开关的状态
        }
        else
        {
            // 关闭流星灯，实际上是让流星灯一直熄灭
            // extern void close_metemor(void);
            // WS2812FX_stop();
            WS2812FX_setSegment_colorOptions(
                1,                           // 第0段
                1,                           // 起始位置
                fc_effect.led_num - 1,       // 结束位置
                &close_metemor,              // 效果
                0,                           // 颜色
                fc_effect.star_speed,        // 速度
                0);                          // 选项，这里像素点大小：3 REVERSE决定方向
            WS2812FX_resetSegmentRuntime(1); // 清除指定段的显示缓存
            WS2812FX_running_flag_set();
            fd_meteor_on_off(); // 向app反馈流星开关的状态
        }
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R5C1_CLICK: // 七彩频闪
    {
        ls_set_color(0, BLUE);
        ls_set_color(1, GREEN);
        ls_set_color(2, RED);
        ls_set_color(3, WHITE);
        ls_set_color(4, YELLOW);
        ls_set_color(5, CYAN);
        ls_set_color(6, PURPLE);

        if ((fc_effect.dream_scene.change_type != MODO_COLORFUL_LIGHTS_FLASH) ||
            (fc_effect.Now_state != IS_light_scene))
        {
            /*
                如果之前不是七彩灯的跳变模式，
                清空灯光动画运行时使用的数据，让动画重新开始跑
            */
            WS2812FX_resetSegmentRuntime(0); //
        }

        fc_effect.dream_scene.change_type = MODO_COLORFUL_LIGHTS_FLASH; //
        fc_effect.dream_scene.c_n = 7;                                  // 有效颜色数量
        fc_effect.Now_state = IS_light_scene;
        set_fc_effect();
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R5C2_CLICK: // 七色跳变
    {
        // printf("key event R5C2 click\n");

        ls_set_color(0, RED);
        ls_set_color(1, GREEN);
        ls_set_color(2, BLUE);
        ls_set_color(3, YELLOW);
        ls_set_color(4, CYAN);
        ls_set_color(5, PURPLE);
        ls_set_color(6, WHITE);

        if ((fc_effect.dream_scene.change_type != MODE_COLORFUL_LIGHTS_JUMP) ||
            (fc_effect.Now_state != IS_light_scene))
        {
            /*
                如果之前不是七彩灯的跳变模式，
                清空灯光动画运行时使用的数据，让动画重新开始跑
            */
            WS2812FX_resetSegmentRuntime(0); //
        }

        fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_JUMP; //
        fc_effect.dream_scene.c_n = 7;                                 // 有效颜色数量
        fc_effect.Now_state = IS_light_scene;
        set_fc_effect();
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R5C3_CLICK: // 七彩渐变
    {
        // printf("key event R5C3 click\n");

        if ((fc_effect.dream_scene.change_type != MODE_COLORFUL_LIGHTS_GRADUAL) ||
            (fc_effect.Now_state != IS_light_scene))
        {
            /*
                如果之前不是七彩灯的渐变模式，
                清空灯光动画运行时使用的数据，让动画重新开始跑
            */
            WS2812FX_resetSegmentRuntime(0); //
        }

        fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_GRADUAL;
        // fc_effect.dream_scene.c_n = color_nums; // 颜色数量
        fc_effect.Now_state = IS_light_scene;
        set_fc_effect();
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R5C4_CLICK:
    {
        /*
             切换流星灯模式

             - 正常流星（慢）模式
             - 正常流星（中速）模式
             - 正常流星（快速）模式
             - 流星乱闪模式（类似呼吸）
             - 流星灯中同时只有随机1~2个灯呼吸的模式
             - 带声控的流星灯模式
                 单点跑循环，动画时间25s
                 单路均衡器--1*10个灯组成，动画时间25s，
                 双路均衡器--2*5个灯组成，动画时间25s，
                 ......
                 依次循环

             每次按下切换模式的时候，不会跑一次快速流星模式，而是切换到下一个模式
        */

        if (DEVICE_OFF == fc_effect.star_on_off)
        {
            // 如果流星灯是关着的，不做响应
            return;
        }

        printf("meteor mode change\n");

        fc_effect.star_index++;
        // if (fc_effect.star_index >= STAR_INDEX_METEOR_MAX) // 防止溢出
        if (fc_effect.star_index >= STAR_INDEX_METEOR_RANDOM_BREATH_2) // 测试时使用
        {
            fc_effect.star_index = STAR_INDEX_METEOR_NORMAL_SLOW; // 默认为正常流星（慢速）
        }

        mode_ptr *animation_ptr = NULL;
        switch (fc_effect.star_index)
        {
        case STAR_INDEX_METEOR_NORMAL_SLOW:
            animation_ptr = meteor_effect_slow;
            break;

        case STAR_INDEX_METEOR_NORMAL_MIDDLE:
            animation_ptr = meteor_effect_middle;
            break;

        case STAR_INDEX_METEOR_NORMAL_FAST:
            animation_ptr = meteor_effect_fast;
            break;

        case STAR_INDEX_METEOR_RANDOM_BREATH:
            animation_ptr = meteor_effect_random_breath;
            break;

        case STAR_INDEX_METEOR_RANDOM_BREATH_2:
            break;

        case STAR_INDEX_METEOR_MUSIC_CONTROL: // 带声控的流星灯模式

            break;

        default:
            return; // 出错，直接返回
        }

        // WS2812FX_stop();
        WS2812FX_setSegment_colorOptions(
            1,                     // 第0段
            1,                     // 起始位置
            fc_effect.led_num - 1, // 结束位置
            animation_ptr,         // 动画效果
            WHITE,                 // 颜色，WS2812FX_setColors设置
            0,                     // 速度，对于样机的正常流星模式，速度这一属性无效
            NO_OPTIONS);           // 选项
        // WS2812FX_start();
        WS2812FX_resetSegmentRuntime(1); //
        WS2812FX_running_flag_set();
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R6C1_CLICK: // AUTO
    {

        if ((fc_effect.dream_scene.change_type != MODE_COLORFUL_LIGHTS_AUTO) ||
            (fc_effect.Now_state != IS_light_scene))
        {
            /*
                如果之前不是七彩灯的自动模式，
                清空灯光动画运行时使用的数据，让动画重新开始跑
            */
            WS2812FX_resetSegmentRuntime(0); //
        }

        fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_AUTO;
        fc_effect.Now_state = IS_light_scene;
        set_fc_effect();
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R6C2_CLICK:
    {

        ls_set_music_mode();
        // printf("fc_effect.music.m = %u\n", (u16)fc_effect.music.m);
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R6C3_CLICK: // 呼吸模式，1，先按单色键，再按呼吸键，则为单色呼吸，2，先按变色键，再按呼吸，则为变色呼吸
    {
        u8 color_nums = 0; // 存放颜色数量

        if (IS_light_scene == fc_effect.Now_state &&
            MODE_COLORFUL_LIGHTS_BREATH == fc_effect.dream_scene.change_type)
        { // 如果本来就是呼吸模式，不响应该事件
          // printf("__LINE__ %d\n", __LINE__);
        }
        else if (IS_STATIC == fc_effect.Now_state)
        {
            /*
                如果是从静态模式进入呼吸模式
                变成单色呼吸
            */
            color_nums = 1; // 颜色数量 -- 只有1个颜色
            fc_effect.dream_scene.rgb[0].r = fc_effect.rgb.r;
            fc_effect.dream_scene.rgb[0].g = fc_effect.rgb.g;
            fc_effect.dream_scene.rgb[0].b = fc_effect.rgb.b;
            fc_effect.dream_scene.rgb[0].w = fc_effect.rgb.w;
            // printf("__LINE__ %d\n", __LINE__);
        }
        else
        {
            /*
                如果不是从静态模式进入呼吸模式，
                变成变色呼吸（每轮呼吸完换一种颜色）
            */
            ls_set_color(color_nums++, RED);
            ls_set_color(color_nums++, ORANGE);
            ls_set_color(color_nums++, YELLOW);
            ls_set_color(color_nums++, GREEN);
            ls_set_color(color_nums++, CYAN);
            ls_set_color(color_nums++, BLUE);
            ls_set_color(color_nums++, PURPLE);
            ls_set_color(color_nums++, PINK);
            ls_set_color(color_nums++, MAGENTA); // 品红
            ls_set_color(color_nums++, WHITE);
            // printf("__LINE__ %d\n", __LINE__);
        }

        if ((fc_effect.dream_scene.change_type != MODE_COLORFUL_LIGHTS_BREATH) ||
            (fc_effect.Now_state != IS_light_scene))
        {
            /*
                如果之前不是七彩灯的呼吸模式，
                清空灯光动画运行时使用的数据，让动画重新开始跑
            */
            WS2812FX_resetSegmentRuntime(0); //
            // printf("__LINE__ %d\n", __LINE__);
        }

        fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
        fc_effect.dream_scene.c_n = color_nums; // 颜色数量
        fc_effect.Now_state = IS_light_scene;
        // printf("color_nums = %u\n", (u16)color_nums);
        set_fc_effect();
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R6C4_CLICK:
    {
#if 0
        /*
            流星灯声控模式下，增加灵敏度
            流星灯乱闪模式下，增加速度
            正常流星模式下，增加尾焰长度

            样机的在流星灯关闭时，也会生效，并且修改的参数作用于全部流星灯的模式
            例如，在声控模式下增加了灵敏度，切换回正常流星模式后，尾焰也会变长
        */
        printf("meteor param add\n");
#endif

        if (DEVICE_OFF == fc_effect.star_on_off)
        {
            return;
        }

        // 正常流星模式下，增加尾焰长度
        if (STAR_INDEX_METEOR_NORMAL_SLOW == fc_effect.star_index ||
            STAR_INDEX_METEOR_NORMAL_MIDDLE == fc_effect.star_index ||
            STAR_INDEX_METEOR_NORMAL_FAST == fc_effect.star_index)
        {
            // if (fc_effect.meteor_tail_len < 12)
            // {
            //     fc_effect.meteor_tail_len++;
            // }
        }
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R7C1_CLICK: // 电机开关
    {
        u8 motor_mode = 0x00; // 默认是关机
        if (fc_effect.motor_on_off == DEVICE_ON)
        {
            motor_mode = 0x06;                                      // 关机命令
            fc_effect.motor_speed_index = ARRAY_SIZE(motor_period); // 让索引值超出数组的索引范围，表示关闭电机，下一次重新上电让电机默认关闭
            fc_effect.motor_on_off = DEVICE_OFF;
        }
        else
        {
            motor_mode = 0x04; // 控制命令 -- 360度旋转
            fc_effect.motor_on_off = DEVICE_ON;
        }

        one_wire_set_mode(motor_mode); // 配置电机模式
        os_taskq_post("msg_task", 1, MSG_SEQUENCER_ONE_WIRE_SEND_INFO);

        fb_motor_mode(); // 向app反馈电机的状态
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R7C2_CLICK: // 电机转速 加
    {
        u8 index = 0;
        for (; index < ARRAY_SIZE(motor_period); index++) // 找到当前电机速度索引对应的下标
        {
            if (motor_period[index] == fc_effect.base_ins.period)
            {
                break;
            }
        }

        // 在 motor_period[] 中，索引值越小，电机速度越快
        if (index > 0)
        {
            index--;
            fc_effect.base_ins.period = motor_period[index];
        }

        one_wire_set_period(motor_period[index]);
        fc_effect.motor_speed_index = index;
        os_taskq_post("msg_task", 1, MSG_SEQUENCER_ONE_WIRE_SEND_INFO);
        fb_motor_speed(); // 向app反馈电机的转速

        // printf("motor index = %u \n", (u16)index);
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R7C3_CLICK: // 电机转速 减
    {
        u8 index = 0;
        for (; index < ARRAY_SIZE(motor_period); index++) // 找到当前电机速度索引对应的下标
        {
            if (motor_period[index] == fc_effect.base_ins.period)
            {
                break;
            }
        }

        // 在 motor_period[] 中，索引值越小，电机速度越快
        if (index < ARRAY_SIZE(motor_period) - 1)
        {
            index++;
            fc_effect.base_ins.period = motor_period[index];
        }

        one_wire_set_period(motor_period[index]);
        fc_effect.motor_speed_index = index;
        os_taskq_post("msg_task", 1, MSG_SEQUENCER_ONE_WIRE_SEND_INFO);
        fb_motor_speed(); // 向app反馈电机的转速

        // printf("motor index = %u \n", (u16)index);
    }
    break;

    case RF24G_WHITE_KEY_1_EVENT_R7C4_CLICK: //
    {
#if 0
        /*
            流星灯声控模式下，减少灵敏度
            流星灯乱闪模式下，减少速度
            正常流星模式下，减少尾焰长度

            样机的在流星灯关闭时，也会生效，并且修改的参数作用于全部流星灯的模式
            例如，在声控模式下增加了灵敏度，切换回正常流星模式后，尾焰也会变长
        */

        printf("meteor param sub\n");
#endif

        if (DEVICE_OFF == fc_effect.star_on_off)
        {
            return;
        }

        // 正常流星模式下，增加尾焰长度
        if (STAR_INDEX_METEOR_NORMAL_SLOW == fc_effect.star_index ||
            STAR_INDEX_METEOR_NORMAL_MIDDLE == fc_effect.star_index ||
            STAR_INDEX_METEOR_NORMAL_FAST == fc_effect.star_index)
        {
            // if (fc_effect.meteor_tail_len > 1)
            // {
            //     fc_effect.meteor_tail_len--;
            // }
        }
    }
    break;

    default:
    {
        // 如果不是对应的按键事件，直接返回，不执行之后的代码
        return;
    }
    break;

    } // switch (rf24g_key_event)

    save_user_data_area3();
#endif

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
            save_user_data_area3();
        }

        return;
    }

    // 运行到这里，说明设备已经启动
    rf24_key_handle_func_t rf24g_key_handle_func_ptr = rf24_key_handle_func_buff[rf24g_key_event];
    if (NULL == rf24g_key_handle_func_ptr) 
    {   
        return;
    }
    rf24g_key_handle_func_ptr();



    save_user_data_area3();
}
#endif
