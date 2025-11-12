#include "rf24g_key_event_handle.h"
#include "rf24g_key.h"
#include "../../../apps/user_app/rf24g_key/rf24g_key.h"
#include "../../../apps/user_app/one_wire/one_wire.h" // 包含电机的驱动程序

#include "../../../apps/user_app/led_strip/led_strand_effect.h"              // 包含 fc_effect 的声明
#include "../../../apps/user_app/ws2812-fx-lib/WS2812FX_C/ws2812fx_effect.h" // 包含部分写好的动画
#include "../../../apps/user_app/ws2812-fx-lib/WS2812FX_C/WS2812FX.H"        // 包含 ws2812的部分函数接口
#include "../../../apps/user_app/one_wire/one_wire.h"                        // 包含电机的驱动程序
#include "../../../apps/user_app/save_flash/save_flash.h"

// 流星灯尾焰长度索引列表
const u8 meteor_tail_len_buff[] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 12};

// =========================================================
// key 1 目前表示控制带有流星灯的遥控器

void rf24g_key_1_event_r1c1_click_handle(void)
{
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

        // printf("fc_effect.ls_b %u\n", (u16)fc_effect.ls_b);
        // printf("fc_effect.app_b %u\n", (u16)fc_effect.app_b);
        printf("fc_effect.b %u\n", (u16)fc_effect.b);
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

void rf24g_key_1_event_r1c2_click_handle(void)
{
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

        // printf("fc_effect.ls_b %u\n", (u16)fc_effect.ls_b);
        // printf("fc_effect.app_b %u\n", (u16)fc_effect.app_b);
        printf("fc_effect.b %u\n", (u16)fc_effect.b);
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

void rf24g_key_1_event_r1c3_click_handle(void)
{
    soft_turn_off_lights();
}

void rf24g_key_1_event_r1c4_click_handle(void)
{
    if (DEVICE_ON ==  fc_effect.on_off_flag )
    {
        // 如果已经开机，不响应该按键事件
        return;
    }

    soft_turn_on_the_light(); // 打开设备
    // save_user_data_area3(); //
}

void rf24g_key_1_event_r2c1_click_handle(void)
{
    color_t color_structure = {0};
    color_structure.r = 0xFF;
    color_structure.g = 0x00;
    color_structure.b = 0x00;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_1_event_r2c2_click_handle(void)
{
    color_t color_structure = {0};
    color_structure.r = 0x00;
    color_structure.g = 0xFF;
    color_structure.b = 0x00;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_1_event_r2c3_click_handle(void)
{
    color_t color_structure = {0};
    color_structure.r = 0x00;
    color_structure.g = 0x00;
    color_structure.b = 0xFF;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_1_event_r2c4_click_handle(void)
{
    color_t color_structure = {0};
    color_structure.r = 0x00;
    color_structure.g = 0x00;
    color_structure.b = 0x00;
    color_structure.w = 0xFF;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_1_event_r3c1_click_handle(void)
{
    color_t color_structure = {0};
    color_structure.r = 0xFF;
    color_structure.g = 0xFF / 2;
    color_structure.b = 0x00;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_1_event_r3c2_click_handle(void)
{
    color_t color_structure = {0};
    color_structure.r = 0x00;
    color_structure.g = 0xFF;
    color_structure.b = 0xFF / 2;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_1_event_r3c3_click_handle(void)
{
    color_t color_structure = {0};
    color_structure.r = 0x00;
    color_structure.g = 0xFF;
    color_structure.b = 0xFF;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_1_event_r3c4_click_handle(void)
{
    color_t color_structure = {0};
    color_structure.r = 0xFF;
    color_structure.g = 0xFF;
    color_structure.b = 0xFF;
    color_structure.w = 0xFF;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_1_event_r4c1_click_handle(void)
{
    color_t color_structure = {0};
    color_structure.r = 0xFF;
    color_structure.g = 0xFF;
    color_structure.b = 0x00;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_1_event_r4c2_click_handle(void)
{
    color_t color_structure = {0};
    color_structure.r = 0xFF;
    color_structure.g = 0x00;
    color_structure.b = 0xFF / 2;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_1_event_r4c3_click_handle(void)
{
    color_t color_structure = {0};
    color_structure.r = 0xFF;
    color_structure.g = 0x00;
    color_structure.b = 0xFF;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_1_event_r4c4_click_handle(void)
{
    // 流星灯开关

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

        
    }
    
    fd_meteor_on_off(); // 向app反馈流星开关的状态
}

void rf24g_key_1_event_r5c1_click_handle(void)
{
    // 七彩频闪

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

void rf24g_key_1_event_r5c2_click_handle(void)
{
    // 七色跳变

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

void rf24g_key_1_event_r5c3_click_handle(void)
{
    // 七彩渐变

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

void rf24g_key_1_event_r5c4_click_handle(void)
{
    /*
        切换流星灯模式

        - 正常流星（慢）模式
        - 正常流星（中速）模式
        - 正常流星（快速）模式
        - 流星乱闪模式（类似呼吸）
        - 流星灯中同时只有随机1~2个灯呼吸的模式（暂时没有做它的功能）
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
    // if (fc_effect.star_index >= STAR_INDEX_METEOR_RANDOM_BREATH_2) // 测试时使用
    if (fc_effect.star_index > STAR_INDEX_METEOR_MUSIC_CONTROL)
    {
        fc_effect.star_index = STAR_INDEX_METEOR_NORMAL_SLOW; // 默认为正常流星（慢速）
    }

    ls_meteor_stat_effect();
}

void rf24g_key_1_event_r6c1_click_handle(void)
{
    // AUTO

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

void rf24g_key_1_event_r6c2_click_handle(void)
{
    // 声控模式

    ls_set_music_mode();
}

void rf24g_key_1_event_r6c3_click_handle(void)
{
    // 呼吸模式，1，先按单色键，再按呼吸键，则为单色呼吸，2，先按变色键，再按呼吸，则为变色呼吸

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

void rf24g_key_1_event_r6c4_click_handle(void)
{
    if (DEVICE_OFF == fc_effect.star_on_off)
    {
        return;
    }

    // 限制范围：0 ~ 9 ，总共10种索引
    if (fc_effect.meteor_speed_index < ARRAY_SIZE(meteor_tail_len_buff))
    {
        fc_effect.meteor_speed_index++;
    }

    // 成功调整参数后，需要重新跑流星灯动画

    // 正常流星模式下，增加尾焰长度
    if (STAR_INDEX_METEOR_NORMAL_SLOW == fc_effect.star_index ||
        STAR_INDEX_METEOR_NORMAL_MIDDLE == fc_effect.star_index ||
        STAR_INDEX_METEOR_NORMAL_FAST == fc_effect.star_index)
    {
        meteor_tail_len = meteor_tail_len_buff[fc_effect.meteor_speed_index];
    }
    else if (STAR_INDEX_METEOR_RANDOM_BREATH == fc_effect.star_index)
    {
        // 流星灯乱闪模式下，调节速度
        // USER_TO_DO 
    }
    else if (STAR_INDEX_METEOR_MUSIC_CONTROL <= fc_effect.star_index ||
             STAR_INDEX_METEOR_MUSIC_CONTROL_3 >= fc_effect.star_index)
    {
        // 带声控的流星灯模式下，调节灵敏度
        meteor_lights_sound_sensitivity_add();
    }

    // 让流星灯动画重新开始跑
    WS2812FX_resetSegmentRuntime(1); //
    WS2812FX_running_flag_set();
}

void rf24g_key_1_event_r7c1_click_handle(void)
{
    // 电机开关

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

void rf24g_key_1_event_r7c2_click_handle(void)
{
    // 电机转速 加
    if (DEVICE_OFF == fc_effect.motor_on_off)
    {
        // 电机没有启动，不调节电机转速
        return;
    }

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
    printf("motor speed index %u\n", (u16)fc_effect.motor_speed_index);
    os_taskq_post("msg_task", 1, MSG_SEQUENCER_ONE_WIRE_SEND_INFO);
    fb_motor_speed(); // 向app反馈电机的转速

    // printf("motor index = %u \n", (u16)index);
}

void rf24g_key_1_event_r7c3_click_handle(void)
{
    // 电机转速 减

    if (DEVICE_OFF == fc_effect.motor_on_off)
    {
        // 电机没有启动，不调节电机转速
        return;
    }

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
    printf("motor speed index %u\n", (u16)fc_effect.motor_speed_index);
    os_taskq_post("msg_task", 1, MSG_SEQUENCER_ONE_WIRE_SEND_INFO);
    fb_motor_speed(); // 向app反馈电机的转速

    // printf("motor index = %u \n", (u16)index);
}

void rf24g_key_1_event_r7c4_click_handle(void)
{
    if (DEVICE_OFF == fc_effect.star_on_off)
    {
        return;
    }

    // 目前 fc_effect.meteor_speed_index 范围：0 ~ 9 ，总共10种索引
    if (fc_effect.meteor_speed_index > 0)
    {
        fc_effect.meteor_speed_index--;
    }

    // 成功调整参数后，需要重新跑流星灯动画

    // 正常流星模式下，增加尾焰长度
    if (STAR_INDEX_METEOR_NORMAL_SLOW == fc_effect.star_index ||
        STAR_INDEX_METEOR_NORMAL_MIDDLE == fc_effect.star_index ||
        STAR_INDEX_METEOR_NORMAL_FAST == fc_effect.star_index)
    {
        meteor_tail_len = meteor_tail_len_buff[fc_effect.meteor_speed_index];
    }
    else if (STAR_INDEX_METEOR_RANDOM_BREATH == fc_effect.star_index)
    {
        // 流星灯乱闪模式下，调节速度
        // USER_TO_DO 
    }
    else if (STAR_INDEX_METEOR_MUSIC_CONTROL <= fc_effect.star_index ||
             STAR_INDEX_METEOR_MUSIC_CONTROL_3 >= fc_effect.star_index)
    {
        // 带声控的流星灯模式下，调节灵敏度
        meteor_lights_sound_sensitivity_sub();
    }

    // 让流星灯动画重新开始跑
    WS2812FX_resetSegmentRuntime(1); //
    WS2812FX_running_flag_set();
}

// =================================================================================
// key 2 目前表示【控制没有流星灯】的设备的遥控器
void rf24g_key_2_event_r3c1_click_handle(void)
{
    // 红100% 绿50%
    // printf("key 2 r3c1 click\n");

    color_t color_structure = {0};
    color_structure.r = 0xFF;
    color_structure.g = 0xFF / 2;
    color_structure.b = 0x00;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_2_event_r3c2_click_handle(void)
{
    // 绿100% 红10%
    color_t color_structure = {0};
    color_structure.r = (u8)((u16)0xFF * 10 / 100); // 10%分量
    color_structure.g = 0xFF / 2;
    color_structure.b = 0x00;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_2_event_r3c3_click_handle(void)
{
    // 绿100% 蓝100%
    color_t color_structure = {0};
    color_structure.r = 0x00;
    color_structure.g = 0xFF;
    color_structure.b = 0xFF;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_2_event_r4c1_click_handle(void)
{
    // 红100% 绿100%
    color_t color_structure = {0};
    color_structure.r = 0xFF;
    color_structure.g = 0xFF;
    color_structure.b = 0x00;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_2_event_r4c2_click_handle(void)
{
    // 绿100% 红10% 蓝10%
    color_t color_structure = {0};
    // color_structure.r = (u8)((u16)0xFF * 10 / 100); // 10%分量
    color_structure.r = (u8)((u16)0xFF * 20 / 100); //  %分量
    color_structure.g = 0xFF;
    // color_structure.b = (u8)((u16)0xFF * 10 / 100); // 10%分量
    color_structure.b = (u8)((u16)0xFF * 20 / 100); //  %分量
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_2_event_r4c3_click_handle(void)
{
    // 绿100% 蓝10%
    color_t color_structure = {0};
    color_structure.r = 0x00;
    color_structure.g = 0xFF;
    color_structure.b = (u8)((u16)0xFF * 10 / 100); // 10%分量
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_2_event_r5c1_click_handle(void)
{
    // 红100% 蓝50%
    color_t color_structure = {0};
    color_structure.r = 0xFF;
    color_structure.g = 0x00;
    color_structure.b = 0xFF / 2; // 50% 分量
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_2_event_r5c2_click_handle(void)
{
    // 红100% 蓝100%
    color_t color_structure = {0};
    color_structure.r = 0xFF;
    color_structure.g = 0x00;
    color_structure.b = 0xFF;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_2_event_r5c3_click_handle(void)
{
    // 红20% 蓝100%
    color_t color_structure = {0};
    color_structure.r = (u8)((u16)0xFF * 20 / 100); // 20% 分量
    color_structure.g = 0x00;
    color_structure.b = 0xFF;
    color_structure.w = 0x00;
    colorful_lights_set_static_mode(color_structure);
}

void rf24g_key_2_event_r5c4_click_handle(void)
{
    // 电机模式切换
    // 1.匀速转，2，正转+反转，3，带暂停的转动，4，速度可变模式，5，音频节奏转动
    // USER_TO_DO 可能要先套用现有的模式
    // 测试发现电机一直是同一个方向，没有切换模式

    if (DEVICE_OFF == fc_effect.motor_on_off)
    {
        // 电机没有启动，不调节电机模式
        return;
    }

    fc_effect.base_ins.mode = (fc_effect.base_ins.mode + 1) % 6;

    // 没有观察到电机反转
    // if (fc_effect.base_ins.mode == 2)
    // {
    //     fc_effect.base_ins.dir = 1;
    // }
    // else
    // {
    //     fc_effect.base_ins.dir = 0;
    // }

    printf("motor mode %u \n", (u16)fc_effect.base_ins.mode);
    os_taskq_post("msg_task", 1, MSG_SEQUENCER_ONE_WIRE_SEND_INFO);
}

const rf24_key_handle_func_t rf24_key_handle_func_buff[RF24G_KEY_EVENT_MAX] = {

    // =================================================================================
    // key 1 目前表示控制带有流星灯的遥控器
    [RF24G_WHITE_KEY_1_EVENT_R1C1_CLICK] = rf24g_key_1_event_r1c1_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R1C1_LONG] = rf24g_key_1_event_r1c1_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R1C2_CLICK] = rf24g_key_1_event_r1c2_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R1C2_LONG] = rf24g_key_1_event_r1c2_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R1C3_CLICK] = rf24g_key_1_event_r1c3_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R1C3_LONG] = rf24g_key_1_event_r1c3_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R1C4_CLICK] = rf24g_key_1_event_r1c4_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R1C4_LONG] = rf24g_key_1_event_r1c4_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R2C1_CLICK] = rf24g_key_1_event_r2c1_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R2C1_LONG] = rf24g_key_1_event_r2c1_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R2C2_CLICK] = rf24g_key_1_event_r2c2_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R2C2_LONG] = rf24g_key_1_event_r2c2_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R2C3_CLICK] = rf24g_key_1_event_r2c3_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R2C3_LONG] = rf24g_key_1_event_r2c3_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R2C4_CLICK] = rf24g_key_1_event_r2c4_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R2C4_LONG] = rf24g_key_1_event_r2c4_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R3C1_CLICK] = rf24g_key_1_event_r3c1_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R3C1_LONG] = rf24g_key_1_event_r3c1_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R3C2_CLICK] = rf24g_key_1_event_r3c2_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R3C2_LONG] = rf24g_key_1_event_r3c2_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R3C3_CLICK] = rf24g_key_1_event_r3c3_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R3C3_LONG] = rf24g_key_1_event_r3c3_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R3C4_CLICK] = rf24g_key_1_event_r3c4_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R3C4_LONG] = rf24g_key_1_event_r3c4_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R4C1_CLICK] = rf24g_key_1_event_r4c1_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R4C1_LONG] = rf24g_key_1_event_r4c1_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R4C2_CLICK] = rf24g_key_1_event_r4c2_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R4C2_LONG] = rf24g_key_1_event_r4c2_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R4C3_CLICK] = rf24g_key_1_event_r4c3_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R4C3_LONG] = rf24g_key_1_event_r4c3_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R4C4_CLICK] = rf24g_key_1_event_r4c4_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R4C4_LONG] = rf24g_key_1_event_r4c4_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R5C1_CLICK] = rf24g_key_1_event_r5c1_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R5C1_LONG] = rf24g_key_1_event_r5c1_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R5C2_CLICK] = rf24g_key_1_event_r5c2_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R5C2_LONG] = rf24g_key_1_event_r5c2_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R5C3_CLICK] = rf24g_key_1_event_r5c3_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R5C3_LONG] = rf24g_key_1_event_r5c3_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R5C4_CLICK] = rf24g_key_1_event_r5c4_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R5C4_LONG] = rf24g_key_1_event_r5c4_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R6C1_CLICK] = rf24g_key_1_event_r6c1_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R6C1_LONG] = rf24g_key_1_event_r6c1_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R6C2_CLICK] = rf24g_key_1_event_r6c2_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R6C2_LONG] = rf24g_key_1_event_r6c2_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R6C3_CLICK] = rf24g_key_1_event_r6c3_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R6C3_LONG] = rf24g_key_1_event_r6c3_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R6C4_CLICK] = rf24g_key_1_event_r6c4_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R6C4_LONG] = rf24g_key_1_event_r6c4_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R7C1_CLICK] = rf24g_key_1_event_r7c1_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R7C1_LONG] = rf24g_key_1_event_r7c1_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R7C2_CLICK] = rf24g_key_1_event_r7c2_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R7C2_LONG] = rf24g_key_1_event_r7c2_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R7C3_CLICK] = rf24g_key_1_event_r7c3_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R7C3_LONG] = rf24g_key_1_event_r7c3_click_handle,

    [RF24G_WHITE_KEY_1_EVENT_R7C4_CLICK] = rf24g_key_1_event_r7c4_click_handle,
    [RF24G_WHITE_KEY_1_EVENT_R7C4_LONG] = rf24g_key_1_event_r7c4_click_handle,

    // =================================================================================
    // key 2 目前表示控制【没有流星灯】的遥控器
    [RF24G_WHITE_KEY_2_EVENT_R1C1_CLICK] = rf24g_key_1_event_r1c1_click_handle, // 跟 key 1 一样的功能
    [RF24G_WHITE_KEY_2_EVENT_R1C1_LONG] = rf24g_key_1_event_r1c1_click_handle,  // 跟 key 1 一样的功能
    // [RF24G_WHITE_KEY_2_EVENT_R1C1_HOLD] = rf24g_key_1_event_r1c1_click_handle,

    [RF24G_WHITE_KEY_2_EVENT_R1C2_CLICK] = rf24g_key_1_event_r1c2_click_handle, // 跟 key 1 一样的功能
    [RF24G_WHITE_KEY_2_EVENT_R1C2_LONG] = rf24g_key_1_event_r1c2_click_handle,  // 跟 key 1 一样的功能
    // [RF24G_WHITE_KEY_2_EVENT_R1C2_HOLD] = rf24g_key_1_event_r1c2_click_handle,  // 跟 key 1 一样的功能

    [RF24G_WHITE_KEY_2_EVENT_R1C3_CLICK] = rf24g_key_1_event_r1c3_click_handle, // 跟 key 1 一样的功能
    [RF24G_WHITE_KEY_2_EVENT_R1C3_LONG] = rf24g_key_1_event_r1c3_click_handle,  // 跟 key 1 一样的功能

    [RF24G_WHITE_KEY_2_EVENT_R1C4_CLICK] = rf24g_key_1_event_r1c4_click_handle, // 跟 key 1 一样的功能
    [RF24G_WHITE_KEY_2_EVENT_R1C4_LONG] = rf24g_key_1_event_r1c4_click_handle,  // 跟 key 1 一样的功能

    [RF24G_WHITE_KEY_2_EVENT_R2C1_CLICK] = rf24g_key_1_event_r2c1_click_handle, // 跟 key 1 一样的功能
    [RF24G_WHITE_KEY_2_EVENT_R2C1_LONG] = rf24g_key_1_event_r2c1_click_handle,  // 跟 key 1 一样的功能

    [RF24G_WHITE_KEY_2_EVENT_R2C2_CLICK] = rf24g_key_1_event_r2c2_click_handle, // 跟 key 1 一样的功能
    [RF24G_WHITE_KEY_2_EVENT_R2C2_LONG] = rf24g_key_1_event_r2c2_click_handle,  // 跟 key 1 一样的功能

    [RF24G_WHITE_KEY_2_EVENT_R2C3_CLICK] = rf24g_key_1_event_r2c3_click_handle, // 跟 key 1 一样的功能
    [RF24G_WHITE_KEY_2_EVENT_R2C3_LONG] = rf24g_key_1_event_r2c3_click_handle,  // 跟 key 1 一样的功能

    [RF24G_WHITE_KEY_2_EVENT_R2C4_CLICK] = rf24g_key_1_event_r2c4_click_handle, // 跟 key 1 一样的功能
    [RF24G_WHITE_KEY_2_EVENT_R2C4_LONG] = rf24g_key_1_event_r2c4_click_handle,  // 跟 key 1 一样的功能

    [RF24G_WHITE_KEY_2_EVENT_R3C1_CLICK] = rf24g_key_2_event_r3c1_click_handle,
    [RF24G_WHITE_KEY_2_EVENT_R3C1_LONG] = rf24g_key_2_event_r3c1_click_handle,

    [RF24G_WHITE_KEY_2_EVENT_R3C2_CLICK] = rf24g_key_2_event_r3c2_click_handle,
    [RF24G_WHITE_KEY_2_EVENT_R3C2_LONG] = rf24g_key_2_event_r3c2_click_handle,

    [RF24G_WHITE_KEY_2_EVENT_R3C3_CLICK] = rf24g_key_2_event_r3c3_click_handle,
    [RF24G_WHITE_KEY_2_EVENT_R3C3_LONG] = rf24g_key_2_event_r3c3_click_handle,

    [RF24G_WHITE_KEY_2_EVENT_R3C4_CLICK] = rf24g_key_1_event_r3c4_click_handle, // 跟 key 1 一样的功能
    [RF24G_WHITE_KEY_2_EVENT_R3C4_LONG] = rf24g_key_1_event_r3c4_click_handle,  // 跟 key 1 一样的功能

    [RF24G_WHITE_KEY_2_EVENT_R4C1_CLICK] = rf24g_key_2_event_r4c1_click_handle,
    [RF24G_WHITE_KEY_2_EVENT_R4C1_LONG] = rf24g_key_2_event_r4c1_click_handle,

    [RF24G_WHITE_KEY_2_EVENT_R4C2_CLICK] = rf24g_key_2_event_r4c2_click_handle,
    [RF24G_WHITE_KEY_2_EVENT_R4C2_LONG] = rf24g_key_2_event_r4c2_click_handle,

    [RF24G_WHITE_KEY_2_EVENT_R4C3_CLICK] = rf24g_key_2_event_r4c3_click_handle,
    [RF24G_WHITE_KEY_2_EVENT_R4C3_LONG] = rf24g_key_2_event_r4c3_click_handle,

    [RF24G_WHITE_KEY_2_EVENT_R4C4_CLICK] = rf24g_key_1_event_r7c1_click_handle, // 电机开关，对应key 1 的 r7c1 click 功能
    [RF24G_WHITE_KEY_2_EVENT_R4C4_LONG] = rf24g_key_1_event_r7c1_click_handle,  // 电机开关，对应key 1 的 r7c1 click 功能

    [RF24G_WHITE_KEY_2_EVENT_R5C1_CLICK] = rf24g_key_2_event_r5c1_click_handle,
    [RF24G_WHITE_KEY_2_EVENT_R5C1_LONG] = rf24g_key_2_event_r5c1_click_handle,

    [RF24G_WHITE_KEY_2_EVENT_R5C2_CLICK] = rf24g_key_2_event_r5c2_click_handle,
    [RF24G_WHITE_KEY_2_EVENT_R5C2_LONG] = rf24g_key_2_event_r5c2_click_handle,

    [RF24G_WHITE_KEY_2_EVENT_R5C3_CLICK] = rf24g_key_2_event_r5c3_click_handle,
    [RF24G_WHITE_KEY_2_EVENT_R5C3_LONG] = rf24g_key_2_event_r5c3_click_handle,

    [RF24G_WHITE_KEY_2_EVENT_R5C4_CLICK] = rf24g_key_2_event_r5c4_click_handle, // 电机模式 切换
    [RF24G_WHITE_KEY_2_EVENT_R5C4_LONG] = rf24g_key_2_event_r5c4_click_handle,  // 电机模式 切换

    [RF24G_WHITE_KEY_2_EVENT_R6C1_CLICK] = rf24g_key_1_event_r5c1_click_handle, // FLASH ，对应 key 1 的 r5c1 click 功能
    [RF24G_WHITE_KEY_2_EVENT_R6C1_LONG] = rf24g_key_1_event_r5c1_click_handle,  // FLASH ，对应 key 1 的 r5c1 click 功能

    [RF24G_WHITE_KEY_2_EVENT_R6C2_CLICK] = rf24g_key_1_event_r5c2_click_handle, // JUMP，对应 key 1 的 r5c2 click 功能
    [RF24G_WHITE_KEY_2_EVENT_R6C2_LONG] = rf24g_key_1_event_r5c2_click_handle,  // JUMP，对应 key 1 的 r5c2 click 功能

    [RF24G_WHITE_KEY_2_EVENT_R6C3_CLICK] = rf24g_key_1_event_r5c3_click_handle, // FADE，对应 key 1 的 r5c3 click 功能
    [RF24G_WHITE_KEY_2_EVENT_R6C3_LONG] = rf24g_key_1_event_r5c3_click_handle,  // FADE，对应 key 1 的 r5c3 click 功能

    [RF24G_WHITE_KEY_2_EVENT_R6C4_CLICK] = rf24g_key_1_event_r7c2_click_handle, // 电机速度 加，对应 key 1 的 r7c2 click 功能
    [RF24G_WHITE_KEY_2_EVENT_R6C4_LONG] = rf24g_key_1_event_r7c2_click_handle,  // 电机速度 加，对应 key 1 的 r7c2 click 功能

    [RF24G_WHITE_KEY_2_EVENT_R7C1_CLICK] = rf24g_key_1_event_r6c1_click_handle, // AUTO，对应 key 1 的 r6c1 click 功能
    [RF24G_WHITE_KEY_2_EVENT_R7C1_LONG] = rf24g_key_1_event_r6c1_click_handle,  // AUTO，对应 key 1 的 r6c1 click 功能

    [RF24G_WHITE_KEY_2_EVENT_R7C2_CLICK] = rf24g_key_1_event_r6c2_click_handle, // 声控模式，对应 key 1 的 r6c2 click 功能
    [RF24G_WHITE_KEY_2_EVENT_R7C2_LONG] = rf24g_key_1_event_r6c2_click_handle,  // 声控模式，对应 key 1 的 r6c2 click 功能

    [RF24G_WHITE_KEY_2_EVENT_R7C3_CLICK] = rf24g_key_1_event_r6c3_click_handle, // BREATH，对应 key 1 的 r6c3 click 功能
    [RF24G_WHITE_KEY_2_EVENT_R7C3_LONG] = rf24g_key_1_event_r6c3_click_handle,  // BREATH，对应 key 1 的 r6c3 click 功能

    [RF24G_WHITE_KEY_2_EVENT_R7C4_CLICK] = rf24g_key_1_event_r7c3_click_handle, // 电机速度 减，对应 key 1 的 r7c3 click 功能
    [RF24G_WHITE_KEY_2_EVENT_R7C4_LONG] = rf24g_key_1_event_r7c3_click_handle,  // 电机速度 减，对应 key 1 的 r7c3 click 功能

    [RF24G_KEY_EVENT_NONE] = NULL,
};