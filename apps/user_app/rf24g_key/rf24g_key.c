#include "rf24g_key.h"

#include "../../../apps/user_app/one_wire/one_wire.h" // 包含电机的驱动程序

#include "../../../apps/user_app/led_strip/led_strand_effect.h"              // 包含 fc_effect 的声明
#include "../../../apps/user_app/ws2812-fx-lib/WS2812FX_C/ws2812fx_effect.h" // 包含部分写好的动画
#include "../../../apps/user_app/ws2812-fx-lib/WS2812FX_C/WS2812FX.H"        // 包含 ws2812的部分函数接口

#if 1

static volatile u8 rf24g_rx_flag = 0;               // 是否收到了新的数据
static volatile u8 rf24g_recved_key_val = 0;        // 存放接收到的按键键值
static volatile u8 rf24g_dynamic_code_last = 0;     // 存放动态码，长按--动态码会一直变化，短按--动态码不变
static volatile u8 rf24g_dynamic_code_cur = 0;      // 存放动态码，长按--动态码会一直变化，短按--动态码不变
static volatile u8 rf24g_dynamic_code_same_cnt = 0; // 存放动态码连续相同次数

// volatile rf24g_recv_info_t rf24g_recv_info;  // 存放接收到的数据包
// volatile u8 chromatic_circle_val = 0;        // 存放色环按键对应的数值，范围：0x00~0xFF

const u8 rf24g_key_event_table[][RF34G_KEY_EVENT_MAX + 1] = {

    // ==============================
    // 白色面板遥控器按键：
    {RF24G_WHITE_KEY_VAL_R1C1, RF24G_WHITE_KEY_EVENT_R1C1_CLICK, RF24G_WHITE_KEY_EVENT_R1C1_HOLD, RF24G_WHITE_KEY_EVENT_R1C1_LOOSE},
    {RF24G_WHITE_KEY_VAL_R1C2, RF24G_WHITE_KEY_EVENT_R1C2_CLICK, RF24G_WHITE_KEY_EVENT_R1C2_HOLD, RF24G_WHITE_KEY_EVENT_R1C2_LOOSE},
    {RF24G_WHITE_KEY_VAL_R1C3, RF24G_WHITE_KEY_EVENT_R1C3_CLICK, RF24G_WHITE_KEY_EVENT_R1C3_HOLD, RF24G_WHITE_KEY_EVENT_R1C3_LOOSE},
    {RF24G_WHITE_KEY_VAL_R1C4, RF24G_WHITE_KEY_EVENT_R1C4_CLICK, RF24G_WHITE_KEY_EVENT_R1C4_HOLD, RF24G_WHITE_KEY_EVENT_R1C4_LOOSE},

    {RF24G_WHITE_KEY_VAL_R2C1, RF24G_WHITE_KEY_EVENT_R2C1_CLICK, RF24G_WHITE_KEY_EVENT_R2C1_HOLD, RF24G_WHITE_KEY_EVENT_R2C1_LOOSE},
    {RF24G_WHITE_KEY_VAL_R2C2, RF24G_WHITE_KEY_EVENT_R2C2_CLICK, RF24G_WHITE_KEY_EVENT_R2C2_HOLD, RF24G_WHITE_KEY_EVENT_R2C2_LOOSE},
    {RF24G_WHITE_KEY_VAL_R2C3, RF24G_WHITE_KEY_EVENT_R2C3_CLICK, RF24G_WHITE_KEY_EVENT_R2C3_HOLD, RF24G_WHITE_KEY_EVENT_R2C3_LOOSE},
    {RF24G_WHITE_KEY_VAL_R2C4, RF24G_WHITE_KEY_EVENT_R2C4_CLICK, RF24G_WHITE_KEY_EVENT_R2C4_HOLD, RF24G_WHITE_KEY_EVENT_R2C4_LOOSE},

    {RF24G_WHITE_KEY_VAL_R3C1, RF24G_WHITE_KEY_EVENT_R3C1_CLICK, RF24G_WHITE_KEY_EVENT_R3C1_HOLD, RF24G_WHITE_KEY_EVENT_R3C1_LOOSE},
    {RF24G_WHITE_KEY_VAL_R3C2, RF24G_WHITE_KEY_EVENT_R3C2_CLICK, RF24G_WHITE_KEY_EVENT_R3C2_HOLD, RF24G_WHITE_KEY_EVENT_R3C2_LOOSE},
    {RF24G_WHITE_KEY_VAL_R3C3, RF24G_WHITE_KEY_EVENT_R3C3_CLICK, RF24G_WHITE_KEY_EVENT_R3C3_HOLD, RF24G_WHITE_KEY_EVENT_R3C3_LOOSE},
    {RF24G_WHITE_KEY_VAL_R3C4, RF24G_WHITE_KEY_EVENT_R3C4_CLICK, RF24G_WHITE_KEY_EVENT_R3C4_HOLD, RF24G_WHITE_KEY_EVENT_R3C4_LOOSE},

    {RF24G_WHITE_KEY_VAL_R4C1, RF24G_WHITE_KEY_EVENT_R4C1_CLICK, RF24G_WHITE_KEY_EVENT_R4C1_HOLD, RF24G_WHITE_KEY_EVENT_R4C1_LOOSE},
    {RF24G_WHITE_KEY_VAL_R4C2, RF24G_WHITE_KEY_EVENT_R4C2_CLICK, RF24G_WHITE_KEY_EVENT_R4C2_HOLD, RF24G_WHITE_KEY_EVENT_R4C2_LOOSE},
    {RF24G_WHITE_KEY_VAL_R4C3, RF24G_WHITE_KEY_EVENT_R4C3_CLICK, RF24G_WHITE_KEY_EVENT_R4C3_HOLD, RF24G_WHITE_KEY_EVENT_R4C3_LOOSE},
    {RF24G_WHITE_KEY_VAL_R4C4, RF24G_WHITE_KEY_EVENT_R4C4_CLICK, RF24G_WHITE_KEY_EVENT_R4C4_HOLD, RF24G_WHITE_KEY_EVENT_R4C4_LOOSE},

    {RF24G_WHITE_KEY_VAL_R5C1, RF24G_WHITE_KEY_EVENT_R5C1_CLICK, RF24G_WHITE_KEY_EVENT_R5C1_HOLD, RF24G_WHITE_KEY_EVENT_R5C1_LOOSE},
    {RF24G_WHITE_KEY_VAL_R5C2, RF24G_WHITE_KEY_EVENT_R5C2_CLICK, RF24G_WHITE_KEY_EVENT_R5C2_HOLD, RF24G_WHITE_KEY_EVENT_R5C2_LOOSE},
    {RF24G_WHITE_KEY_VAL_R5C3, RF24G_WHITE_KEY_EVENT_R5C3_CLICK, RF24G_WHITE_KEY_EVENT_R5C3_HOLD, RF24G_WHITE_KEY_EVENT_R5C3_LOOSE},
    {RF24G_WHITE_KEY_VAL_R5C4, RF24G_WHITE_KEY_EVENT_R5C4_CLICK, RF24G_WHITE_KEY_EVENT_R5C4_HOLD, RF24G_WHITE_KEY_EVENT_R5C4_LOOSE},

    {RF24G_WHITE_KEY_VAL_R6C1, RF24G_WHITE_KEY_EVENT_R6C1_CLICK, RF24G_WHITE_KEY_EVENT_R6C1_HOLD, RF24G_WHITE_KEY_EVENT_R6C1_LOOSE},
    {RF24G_WHITE_KEY_VAL_R6C2, RF24G_WHITE_KEY_EVENT_R6C2_CLICK, RF24G_WHITE_KEY_EVENT_R6C2_HOLD, RF24G_WHITE_KEY_EVENT_R6C2_LOOSE},
    {RF24G_WHITE_KEY_VAL_R6C3, RF24G_WHITE_KEY_EVENT_R6C3_CLICK, RF24G_WHITE_KEY_EVENT_R6C3_HOLD, RF24G_WHITE_KEY_EVENT_R6C3_LOOSE},
    {RF24G_WHITE_KEY_VAL_R6C4, RF24G_WHITE_KEY_EVENT_R6C4_CLICK, RF24G_WHITE_KEY_EVENT_R6C4_HOLD, RF24G_WHITE_KEY_EVENT_R6C4_LOOSE},

};

volatile u8 rf24g_key_driver_event = 0; // 由key_driver_scan() 更新
volatile u8 rf24g_key_driver_value = 0; // 由key_driver_scan() 更新

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
    {
        // printf("recv: \n");
        // printf_buf(p, sizeof(rf24g_recv_info_t)); // 打印接收到的数据包

        // 直接接收键值
        // rf24g_recv_info = *p; // 结构体变量赋值
        rf24g_recved_key_val = p->key;
        rf24g_dynamic_code_cur = p->dynamic_code_1;

        // 打印键值：
        // printf("key 0x %x\n", (u16)rf24g_recved_key_val);

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
    else if (KEY_EVENT_LONG == key_driver_event || KEY_EVENT_HOLD == key_driver_event)
    {
        // long和hold都当作hold处理
        key_event_index = 2;
    }
    else if (KEY_EVENT_UP == key_driver_event)
    {
        // 长按后松手
        key_event_index = 3;
    }

    if (0 == key_event_index || NO_KEY == key_value)
    {
        // 按键事件与上面的事件都不匹配
        // 得到的键值是无效键值
        return RF24G_KEY_EVENT_NONE;
    }

    for (u8 i = 0; i < sizeof(rf24g_key_event_table) / sizeof(rf24g_key_event_table[0]); i++)
    {
        if (key_value == rf24g_key_event_table[i][0])
        {
            return rf24g_key_event_table[i][key_event_index];
        }
    }

    // 如果运行到这里，都没有找到对应的按键，返回无效按键事件
    return RF24G_KEY_EVENT_NONE;
}

void rf24_key_handle(void)
{
    u8 rf24g_key_event = 0;
    color_t color_structure = {0};

    if (NO_KEY == rf24g_key_driver_value)
        return;

    rf24g_key_event = rf24g_convert_key_event(rf24g_key_driver_value, rf24g_key_driver_event);
    rf24g_key_driver_value = NO_KEY;

    /*
        fc_effect.dream_scene.speed 最大应该是2000
        最小应该是
    */
    // fc_effect.dream_scene.speed = 200; // 测试时使用
    fc_effect.dream_scene.speed = 2000; // 测试时使用

    switch (rf24g_key_event)
    {
        // 收到短按 再执行对应的功能 ；不处理长按的事件

    case RF24G_WHITE_KEY_EVENT_R1C1_CLICK: // +
    {                                      // 亮度加、速度加
        if (IS_STATIC == fc_effect.Now_state)
        {
            // 单色模式下，调节亮度
            // fc_effect.b = ;
            // ls_add_bright();
            if (fc_effect.ls_b < (MAX_BRIGHT_RANK - 1))
                fc_effect.ls_b++;
            fc_effect.app_b = (fc_effect.ls_b + 1) * 10;
            fc_effect.b = led_b_array[fc_effect.ls_b];
            fb_bright();
        }
        else if (MODO_COLORFUL_LIGHTS_FLASH == fc_effect.Now_state ||
                 MODE_COLORFUL_LIGHTS_BREATH == fc_effect.Now_state ||
                 MODE_COLORFUL_LIGHTS_GRADUAL == fc_effect.Now_state ||
                 MODE_COLORFUL_LIGHTS_JUMP == fc_effect.Now_state ||
                 MODE_COLORFUL_LIGHTS_AUTO == fc_effect.Now_state)
        {
            // 七彩灯动态模式下，调节速度
            // fc_effect.dream_scene.speed = ;
        }

        // 调节完成后，可能要反馈给app
    }
    break;
    case RF24G_WHITE_KEY_EVENT_R1C2_CLICK: // -
    {                                      // 亮度减、速度减
        if (IS_STATIC == fc_effect.Now_state)
        {
            // 单色模式下，调节亮度
            // fc_effect.b = ;
            // ls_sub_bright();
            if (fc_effect.ls_b > 0)
                fc_effect.ls_b--;
            fc_effect.app_b = (fc_effect.ls_b + 1) * 10;
            fc_effect.b = led_b_array[fc_effect.ls_b];
            fb_bright();
        }
        else if (MODO_COLORFUL_LIGHTS_FLASH == fc_effect.Now_state ||
                 MODE_COLORFUL_LIGHTS_BREATH == fc_effect.Now_state ||
                 MODE_COLORFUL_LIGHTS_GRADUAL == fc_effect.Now_state ||
                 MODE_COLORFUL_LIGHTS_JUMP == fc_effect.Now_state ||
                 MODE_COLORFUL_LIGHTS_AUTO == fc_effect.Now_state)
        {
            // 七彩灯动态模式下，调节速度
            // fc_effect.dream_scene.speed = ;
        }

        // 调节完成后，可能要反馈给app
    }
    break;

    case RF24G_WHITE_KEY_EVENT_R1C3_CLICK: // OFF
    {
    }
    break;

    case RF24G_WHITE_KEY_EVENT_R1C4_CLICK: // ON
    {
    }
    break;

    case RF24G_WHITE_KEY_EVENT_R2C1_CLICK: // R
        // printf("key event R2C1 click\n");
        color_structure.r = 0xFF;
        color_structure.g = 0x00;
        color_structure.b = 0x00;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_EVENT_R2C2_CLICK: // G
        // printf("key event R2C2 click\n");
        color_structure.r = 0x00;
        color_structure.g = 0xFF;
        color_structure.b = 0x00;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);

        break;
    case RF24G_WHITE_KEY_EVENT_R2C3_CLICK: // B
        // printf("key event R2C3 click\n");

        color_structure.r = 0x00;
        color_structure.g = 0x00;
        color_structure.b = 0xFF;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);

        break;

    case RF24G_WHITE_KEY_EVENT_R2C4_CLICK:

        color_structure.r = 0x00;
        color_structure.g = 0x00;
        color_structure.b = 0x00;
        color_structure.w = 0xFF;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_EVENT_R3C1_CLICK:

        color_structure.r = 0xFF;
        color_structure.g = 0xFF / 2;
        color_structure.b = 0x00;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_EVENT_R3C2_CLICK:

        color_structure.r = 0x00;
        color_structure.g = 0xFF;
        color_structure.b = 0xFF / 2;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_EVENT_R3C3_CLICK:

        color_structure.r = 0x00;
        color_structure.g = 0xFF;
        color_structure.b = 0xFF;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_EVENT_R3C4_CLICK:
        color_structure.r = 0xFF;
        color_structure.g = 0xFF;
        color_structure.b = 0xFF;
        color_structure.w = 0xFF;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_EVENT_R4C1_CLICK:
        color_structure.r = 0xFF;
        color_structure.g = 0xFF;
        color_structure.b = 0x00;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_EVENT_R4C2_CLICK:
        color_structure.r = 0xFF;
        color_structure.g = 0x00;
        color_structure.b = 0xFF / 2;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_EVENT_R4C3_CLICK:
        color_structure.r = 0xFF;
        color_structure.g = 0x00;
        color_structure.b = 0xFF;
        color_structure.w = 0x00;
        colorful_lights_set_static_mode(color_structure);
        break;

    case RF24G_WHITE_KEY_EVENT_R5C1_CLICK: // 七彩频闪

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

        break;

    case RF24G_WHITE_KEY_EVENT_R5C2_CLICK: // 七色跳变

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

        break;

    case RF24G_WHITE_KEY_EVENT_R5C3_CLICK: // 七彩渐变

        // printf("key event R5C3 click\n");

        u8 color_nums = 0; // 存放颜色数量
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
        fc_effect.dream_scene.c_n = color_nums; // 颜色数量
        fc_effect.Now_state = IS_light_scene;
        set_fc_effect();

        break;

    case RF24G_WHITE_KEY_EVENT_R6C1_CLICK: // AUTO

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

        break;

    case RF24G_WHITE_KEY_EVENT_R6C2_CLICK:

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

        fc_effect.music.s = 40; // 测试时使用，灵敏度

        ls_set_music_mode();
        // printf("fc_effect.music.m = %u\n", (u16)fc_effect.music.m);

        break;

    case RF24G_WHITE_KEY_EVENT_R6C3_CLICK: // 呼吸模式，1，先按单色键，再按呼吸键，则为单色呼吸，2，先按变色键，再按呼吸，则为变色呼吸
    {
        u8 color_nums = 0; // 存放颜色数量

        if (MODE_COLORFUL_LIGHTS_BREATH == fc_effect.Now_state)
        { // 如果本来就是呼吸模式，不响应该事件
        }
        else if (fc_effect.Now_state == IS_STATIC)
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
        }
        else
        {
            /*
                如果不是从静态模式进入呼吸模式，
                变成变色呼吸
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
        }

        if ((fc_effect.dream_scene.change_type != MODE_COLORFUL_LIGHTS_BREATH) ||
            (fc_effect.Now_state != IS_light_scene))
        {
            /*
                如果之前不是七彩灯的呼吸模式，
                清空灯光动画运行时使用的数据，让动画重新开始跑
            */
            WS2812FX_resetSegmentRuntime(0); //
        }

        fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
        fc_effect.dream_scene.c_n = color_nums; // 颜色数量
        fc_effect.Now_state = IS_light_scene;
        set_fc_effect();
    }
    break;

    case RF24G_WHITE_KEY_EVENT_R5C4_CLICK:
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
        mode_ptr *animation_ptr = NULL;
        printf("meteor mode change\n");
        fc_effect.star_index++;
        if (fc_effect.star_index >= STAR_INDEX_METEOR_MAX) // 防止溢出
        {
            fc_effect.star_index = STAR_INDEX_METEOR_NORMAL_SLOW; // 默认为正常流星（慢速）
        }

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

            // case STAR_INDEX_METEOR_RANDOM_BREATH:
            // animation_ptr = meteor_light_random_breath;
            // break;

        default:
            return; // 出错，直接返回
        }

        WS2812FX_stop();
        WS2812FX_setSegment_colorOptions(
            1,                     // 第0段
            1,                     // 起始位置
            fc_effect.led_num - 1, // 结束位置
            animation_ptr,         // 动画效果
            WHITE,                 // 颜色，WS2812FX_setColors设置
            0,                     // 速度，对于样机的正常流星模式，速度这一属性无效
            NO_OPTIONS);           // 选项
        WS2812FX_start();

        break;
    } // switch (rf24g_key_event)
}
#endif
