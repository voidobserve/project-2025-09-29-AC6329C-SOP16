#include "system/includes.h"
#include "led_strand_effect.h"
#include "WS2812FX.H"
#include "ws2812fx_effect.h"
#include "Adafruit_NeoPixel.H"
#include "led_strip_drive.h"
#include "app_main.h"
#include "asm/mcpwm.h"

volatile fc_effect_t fc_effect; // 幻彩灯串效果数据
void set_fc_effect(void);

// FADE_SLOW：12颗
// FADE_MEDIUM：6颗
// FADE_FAST：5颗灯
// FADE_XFAST:3颗灯
const u8 fade_type[3] = {
    FADE_XFAST, FADE_FAST, FADE_MEDIUM //,FADE_SLOW
};

// 流星灯尾焰长度索引列表
// const u8 meteor_tail_len_buff[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12};

#define segment_num 1
#define _0_seg_start 0
#define _0_seg_stop 0

extern void WS2812FX_setSegment_colorOptions(uint8_t n, uint16_t start, uint16_t stop, mode_ptr mode, uint32_t color, uint16_t speed, uint8_t options);

/**
 * @brief 设置段的颜色
 *
 * @param n
 * @param c
 */
void ls_set_colors(uint8_t n, color_t *c)
{
    uint32_t colors[MAX_NUM_COLORS];
    uint8_t i;

#if LED_STRIP_RGBW

    for (i = 0; i < n; i++)
    {
        colors[i] = c[i].w << 24 | c[i].r << 16 | c[i].g << 8 | c[i].b;
    }

#elif LED_STRIP_RGB

    for (i = 0; i < n; i++)
    {
        colors[i] = c[i].r << 16 | c[i].g << 8 | c[i].b;
    }

#endif

    WS2812FX_setColors(0, colors);
}

// 设置fc_effect.dream_scene.rgb的颜色池
// n:0-MAX_NUM_COLORS
// c:WS2812FX颜色系，R<<16,G<<8,B在低8位
void ls_set_color(uint8_t n, uint32_t c)
{
    if (n < MAX_NUM_COLORS)
    {

#if LED_STRIP_RGBW
        fc_effect.dream_scene.rgb[n].w = (c >> 24) & 0xff;
        fc_effect.dream_scene.rgb[n].r = (c >> 16) & 0xff;
        fc_effect.dream_scene.rgb[n].g = (c >> 8) & 0xff;
        fc_effect.dream_scene.rgb[n].b = c & 0xff;
#elif LED_STRIP_RGB
        fc_effect.dream_scene.rgb[n].r = (c >> 16) & 0xff;
        fc_effect.dream_scene.rgb[n].g = (c >> 8) & 0xff;
        fc_effect.dream_scene.rgb[n].b = c & 0xff;

#endif
    }
}

//====================================================================================================
//====================================================================================================
//====================================================================================================

/*----------------------------------静态色效果----------------------------------*/
static void static_mode(void)
{
    extern uint16_t WS2812FX_mode_static(void);

    WS2812FX_setSegment_colorOptions( // 设置一段颜色的效果
        0,                            // 第0段
        0,                            // 起始位置
        0,                            // 结束位置
        // &WS2812FX_mode_static,                          // 效果
        &colorful_lights_static,
        0,                                              // 颜色，WS2812FX_setColors设置
        100,                                            // 速度
        0);                                             // 选项，这里像素点大小：1
    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n); // 设置颜色数量  0：第0段   fc_effect.dream_scene.c_n  颜色数量，一个颜色包含（RGB）
    ls_set_colors(1, &fc_effect.rgb);                   // 1:1个颜色    &fc_effect.rgb 这个颜色是什么色
    WS2812FX_resetSegmentRuntime(0);                    // 清除指定段的显示缓存
    WS2812FX_running_flag_set();                        // 置位运行标志
    // WS2812FX_triggered_by_colorful_lights();
}

/*----------------------------------彩虹效果----------------------------------*/
static void strand_rainbow(void)
{
    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                           // 第0段
        0, 0,                        // 起始位置，结束位置
        &WS2812FX_mode_mutil_fade,   // 效果
        0,                           // 颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed, // 速度
        SIZE_SMALL);                 // 选项，这里像素点大小：1

    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

/*----------------------------------跳变效果----------------------------------*/
void strand_jump_change(void)
{
    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                                // 第0段
        0, 0,                             // 起始位置，结束位置
        &WS2812FX_mode_single_block_scan, // 效果
        0,                                // 颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,      // 速度
        SIZE_SMALL);                      // 选项，这里像素点大小：1
    // SIZE_MEDIUM);                     // 选项，这里像素点大小：3

    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();
}
/*----------------------------------呼吸系列效果----------------------------------*/
void strand_breath(void)
{
    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                           // 第0段
        0, 0,                        // 起始位置，结束位置
        &WS2812FX_mode_mutil_breath, // 效果
        0,                           // 颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed, // 速度
        SIZE_MEDIUM                  // 选项，这里像素点大小：3
    );

    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

void single_c_breath(void)
{

    WS2812FX_setSegment_colorOptions(
        0,                           // 第0段
        0, 0,                        // 起始位置，结束位置
        &WS2812FX_mode_breath,       // 效果
        0,                           // 颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed, // 速 度
        SIZE_MEDIUM                  // 选项，这里像素点大小：3
    );

    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

/*----------------------------------闪烁效果----------------------------------*/
void strand_twihkle(void)
{

    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                               // 第0段
        0, 0,                            // 起始位置，结束位置
        &WS2812FX_mode_mutil_twihkle,    // 效果
        0,                               // 颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed * 4, // 速度
        SIZE_SMALL);                     // 选项，这里像素点大小：1
    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

// 多颜色频闪
void ls_strobe(void)
{

    WS2812FX_setSegment_colorOptions(
        0,                               // 第0段
        0, 0,                            // 起始位置，结束位置
        &WS2812FX_mutil_strobe,          // 效果
        0,                               // 颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed * 5, // 速度
        0);                              // 选项，这里像素点大小：3

    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

/*----------------------------------流水效果----------------------------------*/
void strand_flow_water(void)
{
    uint8_t option;
    // 正向
    if (fc_effect.dream_scene.direction == IS_forward)
    {
        option = SIZE_MEDIUM | 0;
    }
    else
    {
        option = SIZE_MEDIUM | REVERSE;
    }

    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                               // 第0段
        0, 0,                            // 起始位置，结束位置
        &WS2812FX_mode_multi_block_scan, // 效果
        0,                               // 颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,     // 速度
        option);                         // 选项，这里像素点大小：3,反向/反向
    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

/*----------------------------------追光效果----------------------------------*/
void strand_chas_light(void)
{

    WS2812FX_stop();
    // 正向
    if (fc_effect.dream_scene.direction == IS_forward)
    {
        WS2812FX_setSegment_colorOptions(
            0,                                 // 第0段
            0, 0,                              // 起始位置，结束位置
            &WS2812FX_mode_multi_forward_same, // 效果
            0,                                 // 颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed,       // 速度
            0);                                // 选项
    }
    else
    {
        WS2812FX_setSegment_colorOptions(
            0,                              // 第0段
            0, 0,                           // 起始位置，结束位置
            &WS2812FX_mode_multi_back_same, // 效果
            0,                              // 颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed,    // 速度
            0);
    }
    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

/*----------------------------------炫彩效果----------------------------------*/
void strand_colorful(void)
{
    uint8_t option;
    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                               // 第0段
        0, 0,                            // 起始位置，结束位置
        &WS2812FX_mode_multi_block_scan, // 效果
        0,                               // 颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed,     // 速度
        SIZE_SMALL);                     // 选项，这里像素点大小：1
    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

/*----------------------------------渐变系列效果----------------------------------*/
void strand_grandual(void)
{
    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                           // 第0段
        0, 0,                        // 起始位置，结束位置
        &WS2812FX_mode_mutil_fade,   // 效果
        0,                           // 颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed, // 速度
        SIZE_MEDIUM);                // 选项，这里像素点大小：3,反向/反向

    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}
// 整条灯带渐变，支持多种颜色之间切换
// 颜色池：fc_effect.dream_scene.rgb[]
// 颜色数量fc_effect.dream_scene.c_n
void mutil_c_grandual(void)
{
    extern uint16_t WS2812FX_mutil_c_gradual(void);
    WS2812FX_setSegment_colorOptions(
        0,                           // 第0段
        0, 0,                        // 起始位置，结束位置
        &WS2812FX_mutil_c_gradual,   // 效果
        0,                           // 颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed, // 速度
        SIZE_MEDIUM);                // 选项，这里像素点大小：3,反向/反向

    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

// 纯白色渐变
void w_grandual(void)
{

    extern uint16_t breath_w(void);

    WS2812FX_setSegment_colorOptions(
        0,                           // 第0段
        0, 0,                        // 起始位置，结束位置
        &breath_w,                   // 效果
        WHITE,                       // 颜色，WS2812FX_setColors设置
        fc_effect.dream_scene.speed, // 速度
        0);                          // 选项，这里像素点大小：3,反向/反向

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

/*----------------------------------跳变效果----------------------------------*/
void standard_jump(void)
{
    extern uint16_t WS2812FX_mutil_c_jump(void);
    // WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        0,                      // 第0段
        0, 0,                   // 起始位置，结束位置
        &WS2812FX_mutil_c_jump, // 效果
        0,                      // 颜色， WS2812FX_setColors 设置
        // (fc_effect.dream_scene.speed * 40), // 速度
        (fc_effect.dream_scene.speed), // 速度
        0);                            // 选项，这里像素点大小：3

    WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
    ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
    // WS2812FX_start();
    // WS2812FX_running_flag_set();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

//====================================================================================================
//====================================================================================================
//====================================================================================================

void strand_meteor(u8 index)
{

    uint8_t option;
    // 正向
    if (fc_effect.dream_scene.direction == IS_forward)
    {
        option = 0;
    }
    else
    {
        option = REVERSE;
    }

    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        1,                               // 第0段
        1, fc_effect.led_num,            // 起始位置，结束位置
        &WS2812FX_mode_comet_1,          // 效果
        WHITE,                           // 颜色，WS2812FX_setColors设置
        fc_effect.star_speed,            // 速度
        fade_type[index - 19] | option); // 选项，这里像素点大小：3,反向/反向
                                         // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0);     // 清除指定段的显示缓存
    WS2812FX_running_flag_set();         // 置位运行标志
}

void double_meteor(void)
{

    extern uint16_t fc_double_meteor(void);
    uint8_t option;
    // 正向
    if (fc_effect.dream_scene.direction == IS_forward)
    {
        option = 0;
    }
    else
    {
        option = REVERSE;
    }

    WS2812FX_stop();
    WS2812FX_setSegment_colorOptions(
        1,                    // 第0段
        1, fc_effect.led_num, // 起始位置，结束位置
        &fc_double_meteor,    // 效果
        WHITE,                // 颜色，WS2812FX_setColors设置
        fc_effect.star_speed, // 速度
        option);              // 选项，这里像素点大小：3,反向/反向

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();     // 置位运行标志
}

//====================================================================================================
//====================================================================================================
//====================================================================================================

/**
 * @brief APP模式中，基本的七彩动态效果集合
 *
 * @param tp_num
 */
void base_Dynamic_Effect(u8 tp_num)
{
    switch (tp_num)
    {
    case 0x07:
    { // 3色跳变
        ls_set_color(0, BLUE);
        ls_set_color(1, GREEN);
        ls_set_color(2, RED);
        fc_effect.dream_scene.change_type = MODE_JUMP;
        fc_effect.dream_scene.c_n = 3;
        fc_effect.Now_state = IS_light_scene;
    }
    break;

    case 0x08:
    { // 7色跳变
        ls_set_color(0, BLUE);
        ls_set_color(1, GREEN);
        ls_set_color(2, RED);
        ls_set_color(3, WHITE);
        ls_set_color(4, YELLOW);
        ls_set_color(5, CYAN);
        ls_set_color(6, PURPLE);
        fc_effect.dream_scene.change_type = MODE_JUMP;
        fc_effect.dream_scene.c_n = 7;
        fc_effect.Now_state = IS_light_scene;
    }
    break;

    case 0x09:
        // { // 3色渐变
        //     ls_set_color(0, BLUE);
        //     ls_set_color(1, GREEN);
        //     ls_set_color(2, RED);
        //     fc_effect.dream_scene.change_type = MODE_MUTIL_C_GRADUAL;
        //     fc_effect.dream_scene.c_n = 3;
        //     fc_effect.Now_state = IS_light_scene;
        // }

        { // 三色呼吸
            ls_set_color(0, BLUE);
            ls_set_color(1, GREEN);
            ls_set_color(2, RED);
            fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
            fc_effect.dream_scene.c_n = 3;
            fc_effect.Now_state = IS_light_scene;
        } // 三色呼吸
        break;

    case 0x0A:
        // { // 七彩渐变
        //     ls_set_color(0, BLUE);
        //     ls_set_color(1, GREEN);
        //     ls_set_color(2, RED);
        //     ls_set_color(3, WHITE);
        //     ls_set_color(4, YELLOW);
        //     ls_set_color(5, CYAN);
        //     ls_set_color(6, PURPLE);
        //     fc_effect.dream_scene.change_type = MODE_MUTIL_C_GRADUAL;
        //     fc_effect.dream_scene.c_n = 7;
        //     fc_effect.Now_state = IS_light_scene;
        // }

        { // 七色呼吸
            ls_set_color(0, BLUE);
            ls_set_color(1, GREEN);
            ls_set_color(2, RED);
            ls_set_color(3, WHITE);
            ls_set_color(4, YELLOW);
            ls_set_color(5, CYAN);
            ls_set_color(6, PURPLE);
            fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
            fc_effect.dream_scene.c_n = 7;
            fc_effect.Now_state = IS_light_scene;
        } // 七色呼吸
        break;

    case 0x0B:
        // { // 红色呼吸
        //     ls_set_color(0, RED);
        //     ls_set_color(1, BLACK);
        //     fc_effect.dream_scene.change_type = MODE_SINGLE_C_BREATH;
        //     fc_effect.dream_scene.c_n = 2;
        //     fc_effect.Now_state = IS_light_scene;
        // }

        /*
            MODE_SINGLE_C_BREATH 和 MODE_COLORFUL_LIGHTS_BREATH
            两者的所需的速度值不一致，现在统一使用 MODE_COLORFUL_LIGHTS_BREATH，使用 fc_effect.dream_scene.speed 作为速度值，单位：ms
        */
        { // 红色呼吸
            ls_set_color(0, RED);
            fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
            fc_effect.dream_scene.c_n = 1;
            fc_effect.Now_state = IS_light_scene;
        }
        break;

    case 0x0c:
        // { // 蓝色呼吸
        //     ls_set_color(0, BLUE);
        //     ls_set_color(1, BLACK);
        //     fc_effect.dream_scene.change_type = MODE_SINGLE_C_BREATH;
        //     fc_effect.dream_scene.c_n = 2;
        //     fc_effect.Now_state = IS_light_scene;
        // }

        /*
            MODE_SINGLE_C_BREATH 和 MODE_COLORFUL_LIGHTS_BREATH
            两者的所需的速度值不一致，现在统一使用 MODE_COLORFUL_LIGHTS_BREATH，使用 fc_effect.dream_scene.speed 作为速度值，单位：ms
        */
        { // 蓝色呼吸
            ls_set_color(0, BLUE);
            fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
            fc_effect.dream_scene.c_n = 1;
            fc_effect.Now_state = IS_light_scene;
        }
        break;

    case 0x0D:
        // { // 绿色呼吸
        //     ls_set_color(0, GREEN);
        //     ls_set_color(1, BLACK);
        //     fc_effect.dream_scene.change_type = MODE_SINGLE_C_BREATH;
        //     fc_effect.dream_scene.c_n = 2;
        //     fc_effect.Now_state = IS_light_scene;
        // }

        /*
            MODE_SINGLE_C_BREATH 和 MODE_COLORFUL_LIGHTS_BREATH
            两者的所需的速度值不一致，现在统一使用 MODE_COLORFUL_LIGHTS_BREATH，使用 fc_effect.dream_scene.speed 作为速度值，单位：ms
        */
        { // 绿色呼吸
            ls_set_color(0, GREEN);
            fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
            fc_effect.dream_scene.c_n = 1;
            fc_effect.Now_state = IS_light_scene;
        }
        break;

    case 0x0E:
        // { // 青色呼吸
        //     ls_set_color(0, CYAN);
        //     ls_set_color(1, BLACK);
        //     fc_effect.dream_scene.change_type = MODE_SINGLE_C_BREATH;
        //     fc_effect.dream_scene.c_n = 2;
        //     fc_effect.Now_state = IS_light_scene;
        // }

        { // 青色呼吸
            ls_set_color(0, CYAN);
            fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
            fc_effect.dream_scene.c_n = 1;
            fc_effect.Now_state = IS_light_scene;
        }
        break;

    case 0x0F:
        // { // 黄色呼吸
        //     ls_set_color(0, YELLOW);
        //     ls_set_color(1, BLACK);
        //     fc_effect.dream_scene.change_type = MODE_SINGLE_C_BREATH;
        //     fc_effect.dream_scene.c_n = 2;
        //     fc_effect.Now_state = IS_light_scene;
        // }

        { // 黄色呼吸
            ls_set_color(0, YELLOW);
            fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
            fc_effect.dream_scene.c_n = 1;
            fc_effect.Now_state = IS_light_scene;
        }
        break;

    case 0x10:
    { // 紫色呼吸
        ls_set_color(0, PURPLE);
        fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
        fc_effect.dream_scene.c_n = 1;
        fc_effect.Now_state = IS_light_scene;
    }
    break;

    case 0x11:
    { // 混白色呼吸
        ls_set_color(0, WHITE);
        fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
        fc_effect.dream_scene.c_n = 1;
        fc_effect.Now_state = IS_light_scene;
    }
    break;

    case 0x12:
        // { // 纯白色渐变 （纯白色呼吸）
        //     ls_set_color(0, WHITE);
        //     ls_set_color(1, BLACK);
        //     fc_effect.dream_scene.change_type = MODE_BREATH_W;
        //     fc_effect.dream_scene.c_n = 2;
        //     fc_effect.Now_state = IS_light_scene;
        // }

        { // 纯白色呼吸
            fc_effect.dream_scene.rgb[0].w = 0xFF;
            fc_effect.dream_scene.rgb[0].r = 0x00;
            fc_effect.dream_scene.rgb[0].g = 0x00;
            fc_effect.dream_scene.rgb[0].b = 0x00;
            fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
            fc_effect.dream_scene.c_n = 1;
            fc_effect.Now_state = IS_light_scene;
        }
        break;

    case 0x13:
        // { // 红绿渐变
        //     ls_set_color(0, RED);
        //     ls_set_color(1, GREEN);
        //     fc_effect.dream_scene.change_type = MODE_MUTIL_C_GRADUAL;
        //     fc_effect.dream_scene.c_n = 2;
        //     fc_effect.Now_state = IS_light_scene;
        // } // 红绿渐变

        { // 红绿呼吸
            ls_set_color(0, RED);
            ls_set_color(1, GREEN);
            fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
            fc_effect.dream_scene.c_n = 2;
            fc_effect.Now_state = IS_light_scene;
        } // 红绿呼吸
        break;

    case 0x14:
        // { // 红蓝渐变
        //     ls_set_color(0, BLUE);
        //     ls_set_color(1, RED);
        //     fc_effect.dream_scene.change_type = MODE_MUTIL_C_GRADUAL;
        //     fc_effect.dream_scene.c_n = 2;
        //     fc_effect.Now_state = IS_light_scene;
        // }// 红蓝渐变

        { // 红蓝呼吸
            ls_set_color(0, BLUE);
            ls_set_color(1, RED);
            fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
            fc_effect.dream_scene.c_n = 2;
            fc_effect.Now_state = IS_light_scene;
        }
        break;

    case 0x15:
        // { // 绿蓝渐变
        //     ls_set_color(0, GREEN);
        //     ls_set_color(1, BLUE);
        //     fc_effect.dream_scene.change_type = MODE_MUTIL_C_GRADUAL;
        //     fc_effect.dream_scene.c_n = 2;
        //     fc_effect.Now_state = IS_light_scene;
        // }// 绿蓝渐变

        { // 绿蓝呼吸
            ls_set_color(0, GREEN);
            ls_set_color(1, BLUE);
            fc_effect.dream_scene.change_type = MODE_COLORFUL_LIGHTS_BREATH;
            fc_effect.dream_scene.c_n = 2;
            fc_effect.Now_state = IS_light_scene;
        } // 绿蓝呼吸
        break;

    case 0x16:
    { // 七色频闪
        ls_set_color(0, BLUE);
        ls_set_color(1, GREEN);
        ls_set_color(2, RED);
        ls_set_color(3, WHITE);
        ls_set_color(4, YELLOW);
        ls_set_color(5, CYAN);
        ls_set_color(6, PURPLE);

        // fc_effect.dream_scene.change_type = MODE_STROBE;
        fc_effect.dream_scene.change_type = MODO_COLORFUL_LIGHTS_FLASH;
        fc_effect.dream_scene.c_n = 7;
        fc_effect.Now_state = IS_light_scene;
    }
    break;

    case 0x17:
    { // 红色频闪
        ls_set_color(0, RED);
        // fc_effect.dream_scene.change_type = MODE_STROBE;
        fc_effect.dream_scene.change_type = MODO_COLORFUL_LIGHTS_FLASH;
        fc_effect.dream_scene.c_n = 1;
        fc_effect.Now_state = IS_light_scene;
    }
    break;

    case 0x18:
    { // 蓝色频闪
        ls_set_color(0, BLUE);
        // fc_effect.dream_scene.change_type = MODE_STROBE;
        fc_effect.dream_scene.change_type = MODO_COLORFUL_LIGHTS_FLASH;
        fc_effect.dream_scene.c_n = 1;
        fc_effect.Now_state = IS_light_scene;
    }

    break;

    case 0x19:
    { // 绿色频闪
        ls_set_color(0, GREEN);
        // fc_effect.dream_scene.change_type = MODE_STROBE;
        fc_effect.dream_scene.change_type = MODO_COLORFUL_LIGHTS_FLASH;
        fc_effect.dream_scene.c_n = 1;
        fc_effect.Now_state = IS_light_scene;
    }
    break;

    case 0x1a:
    { // 青色频闪
        ls_set_color(0, CYAN);
        // fc_effect.dream_scene.change_type = MODE_STROBE;
        fc_effect.dream_scene.change_type = MODO_COLORFUL_LIGHTS_FLASH;
        fc_effect.dream_scene.c_n = 1;
        fc_effect.Now_state = IS_light_scene;
    }
    break;

    case 0x1b:
    { // 黄色频闪
        ls_set_color(0, YELLOW);
        // fc_effect.dream_scene.change_type = MODE_STROBE;
        fc_effect.dream_scene.change_type = MODO_COLORFUL_LIGHTS_FLASH;
        fc_effect.dream_scene.c_n = 1;
        fc_effect.Now_state = IS_light_scene;
    }
    break;

    case 0x1c:
    { // 紫色频闪
        ls_set_color(0, PURPLE);
        // fc_effect.dream_scene.change_type = MODE_STROBE;
        fc_effect.dream_scene.change_type = MODO_COLORFUL_LIGHTS_FLASH;
        fc_effect.dream_scene.c_n = 1;
        fc_effect.Now_state = IS_light_scene;
    }
    break;

    case 0x1e:
    { // 混白色频闪
        ls_set_color(0, WHITE);
        // fc_effect.dream_scene.change_type = MODE_STROBE;
        fc_effect.dream_scene.change_type = MODO_COLORFUL_LIGHTS_FLASH;
        fc_effect.dream_scene.c_n = 1;
        fc_effect.Now_state = IS_light_scene;
    }
    break;
    }

    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存（调用相应的动画前，需要重新开始跑）
    set_fc_effect();
}

/**
 * @brief 情景效果集合
 *
 */
static void ls_scene_effect(void)
{
    // if (fc_effect.dream_scene.change_type >= MODO_COLORFUL_LIGHTS_FLASH &&
    //     fc_effect.dream_scene.change_type <= MODE_COLORFUL_LIGHTS_AUTO)
    // {
    //     fc_effect.dream_scene.speed = colorful_lights_speed_array[fc_effect.ls_speed]; // 根据速度等级进行查表，得到速度值
    // }
    // else
    // {
    //     fc_effect.dream_scene.speed = led_speed_array[fc_effect.ls_speed];
    // }

    // app_set_bright(100); // 设置为最大亮度
    switch (fc_effect.dream_scene.change_type)
    {

    case MODE_MUTIL_RAINBOW: // 彩虹
        strand_rainbow();
        break;

    case MODE_MUTIL_JUMP: // 跳变模式
        strand_jump_change();
        break;

    case MODE_MUTIL_BRAETH: // 呼吸模式
        strand_breath();
        break;

    case MODE_MUTIL_TWIHKLE: // 闪烁模式
        strand_twihkle();
        break;

    case MODE_MUTIL_FLOW_WATER: // 流水模式
        strand_flow_water();
        break;

    case MODE_CHAS_LIGHT: // 追光模式
        strand_chas_light();
        break;

    case MODE_MUTIL_COLORFUL: // 炫彩模式
        strand_colorful();
        break;

    case MODE_MUTIL_SEG_GRADUAL: // 渐变模式
        strand_grandual();
        break;

    case MODE_JUMP: // 标准跳变
        standard_jump();
        break;

    case MODE_MUTIL_C_GRADUAL: // 多段同时渐变
        mutil_c_grandual();
        break;

    case MODE_BREATH_W: // 白色渐变
        w_grandual();
        break;

    case MODE_STROBE: // 标准频闪
        ls_strobe();
        break;

    case MODE_SINGLE_C_BREATH:
        single_c_breath();
        break;

    case MODO_COLORFUL_LIGHTS_FLASH: // 七彩灯频闪

        WS2812FX_setSegment_colorOptions(
            0,                           // 第0段
            0,                           // 起始位置
            0,                           // 结束位置
            &colorful_lights_flash,      // 效果  // 七彩灯渐变
            0,                           // 颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed, // 速 度
            NO_OPTIONS                   // 选项
        );

        WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n); // 设置颜色数量
        ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
        WS2812FX_running_flag_set();

        break;

    case MODE_COLORFUL_LIGHTS_JUMP: // 七彩灯跳变

        WS2812FX_setSegment_colorOptions(
            0,                           // 第0段
            0,                           // 起始位置
            0,                           // 结束位置
            &colorful_lights_jump,       // 效果  // 七彩灯渐变
            0,                           // 颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed, // 速 度
            NO_OPTIONS                   // 选项
        );

        WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n); // 设置颜色数量
        ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
        WS2812FX_running_flag_set();

        break;

    case MODE_COLORFUL_LIGHTS_GRADUAL: // 七彩灯渐变

        // 注意，提供的颜色数量至少要有两个，否则函数内部会越界访问

        WS2812FX_setSegment_colorOptions(
            0,                           // 第0段
            0,                           // 起始位置
            0,                           // 结束位置
            &colorful_lights_gradual,    // 效果  // 七彩灯渐变
            0,                           // 颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed, // 速 度
            NO_OPTIONS                   // 选项
        );

        // WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n);
        // ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
        WS2812FX_running_flag_set();
        break;

    case MODE_COLORFUL_LIGHTS_BREATH: // 七彩灯呼吸
    {
        /*
            注意，进入这里不会重新开始跑动画（样机对应的七彩灯呼吸在调节速度不会重新开始跑动画）

            如果需要重新开始跑动画，在进入这里之前，需要先调用 WS2812FX_resetSegmentRuntime(0);
        */
        WS2812FX_setSegment_colorOptions(
            0,                           // 第0段
            0,                           // 起始位置
            0,                           // 结束位置
            &colorful_lights_breathing,  // 效果  // 七彩灯呼吸
            0,                           // 颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed, // 速 度
            NO_OPTIONS                   // 选项
        );

        WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n); // 设置颜色数量
        ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
        WS2812FX_running_flag_set();
    }
    break;

    case MODE_COLORFUL_LIGHTS_AUTO: // 七彩灯的自动模式

        WS2812FX_setSegment_colorOptions(
            0,                           // 第0段
            0,                           // 起始位置
            0,                           // 结束位置
            &colorful_lights_auto,       // 效果  // 七彩灯自动（颜色和颜色数量在函数内部设置，这里不用再设置）
            0,                           // 颜色，WS2812FX_setColors设置
            fc_effect.dream_scene.speed, // 速 度
            NO_OPTIONS                   // 选项
        );

        // WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n); // 设置颜色数量
        // ls_set_colors(fc_effect.dream_scene.c_n, &fc_effect.dream_scene.rgb);
        WS2812FX_running_flag_set();
        break;

    default:
        break;
    }
}

/**
 * @brief 涂鸦的配对效果
 *
 */
static void ls_ty_pair_effect(void)
{
}

/**
 * @brief 自定义效果集合
 *
 */
static void ls_custom_effect(void)
{
}

/**
 * @brief 光纤灯的流星集合
 *
 */
void ls_meteor_stat_effect(void)
{
    if (fc_effect.star_on_off == DEVICE_OFF)
    {
        return;
    }

    // fc_effect.period_cnt = 0;              // app调节的流星灯效果使用的参数
    mode_ptr meteor_func_ptr = NULL;       // 函数指针，流星灯动画
    u8 meteor_effect_options = NO_OPTIONS; // 流星灯动画的选项

    u16 meteor_effect_speed = 0; // 流星灯动画的速度值
    meteor_effect_speed = fc_effect.star_speed;

    printf("fc_effect.star_index = %u\n", (u16)fc_effect.star_index);
    // printf("fc_effect.star_speed = %d\n", fc_effect.star_speed);
    // printf("fc_effect.meteor_period = %d\n", fc_effect.meteor_period);
    // printf("fc_effect.period_cnt = %d\n", fc_effect.period_cnt);

    // 流星效果
#if 1
    if (fc_effect.star_index == 1) // 单流星
    {
        meteor_func_ptr = WS2812FX_mode_comet_1;
        meteor_effect_options = fade_type[0];
    }
    else if (fc_effect.star_index == 2) // 单流星 （反向）
    {
        meteor_func_ptr = WS2812FX_mode_comet_1;
        meteor_effect_options = fade_type[0] | REVERSE;
    }
    else if (fc_effect.star_index == 3) // 双流星
    {
        meteor_func_ptr = fc_double_meteor;
        meteor_effect_options = fade_type[0];
    }
    else if (fc_effect.star_index == 4) // 双流星 （反向）
    {
        meteor_func_ptr = fc_double_meteor;
        meteor_effect_options = fade_type[0] | REVERSE;
    }
    else if (fc_effect.star_index == 5) // 频闪效果
    {
        meteor_func_ptr = WS2812FX_mode_comet_3;
        meteor_effect_options = NO_OPTIONS;
    }
    else if (fc_effect.star_index == 6) // 频闪效果 （反向）
    {
        meteor_func_ptr = WS2812FX_mode_comet_3;
        meteor_effect_options = REVERSE;
    }
    else if (fc_effect.star_index == 7)
    {
        // 3个灯流水，长度为5个灯，然后另外5个灯随机闪动
        meteor_func_ptr = meteor_effect_G;
        meteor_effect_options = NO_OPTIONS;
    }
    else if (fc_effect.star_index == 8)
    {
        // 3个灯流水，长度为5个灯，两次流水，然后另外5个灯随机闪
        meteor_func_ptr = meteor_effect_H;
        meteor_effect_options = NO_OPTIONS;
    }
    else if (fc_effect.star_index == 9) // 堆积流水
    {
        meteor_func_ptr = WS2812FX_mode_comet_4;
        meteor_effect_options = NO_OPTIONS;
    }
    else if (fc_effect.star_index == 10) // 堆积流水 （反向）
    {
        meteor_func_ptr = WS2812FX_mode_comet_4;
        meteor_effect_options = REVERSE;
    }
    else if (fc_effect.star_index == 11) // 逐点流水
    {
        meteor_func_ptr = WS2812FX_mode_comet_5;
        meteor_effect_options = NO_OPTIONS;
    }
    else if (fc_effect.star_index == 12) // 逐点流水 （反向）
    {
        meteor_func_ptr = WS2812FX_mode_comet_5;
        meteor_effect_options = REVERSE;
    }
    else if (fc_effect.star_index == 13) // 两段渐变灭灯流星  从中心靠拢或发散
    {
        meteor_func_ptr = WS2812FX_mode_comet_2;
        meteor_effect_options = fade_type[0];
    }
    else if (fc_effect.star_index == 14) // 两段渐变灭灯流星  从中心靠拢或发散
    {
        meteor_func_ptr = WS2812FX_mode_comet_2;
        meteor_effect_options = fade_type[0] | REVERSE;
    }
    else if (fc_effect.star_index == 15) // 追逐流水
    {
        meteor_func_ptr = WS2812FX_mode_comet_6;
        meteor_effect_options = NO_OPTIONS;
    }
    else if (fc_effect.star_index == 16) // 追逐流水 （反向）
    {
        meteor_func_ptr = WS2812FX_mode_comet_6;
        meteor_effect_options = REVERSE;
    }
    else if (fc_effect.star_index == 17) // 音乐律动1
    {
        meteor_func_ptr = meteor;
        meteor_effect_options = NO_OPTIONS;
    }
    else if (fc_effect.star_index == 18) // 音乐律动2
    {
        meteor_func_ptr = music_meteor3;
        meteor_effect_options = NO_OPTIONS;
    }
    /* == 目前没有 index 在 19 ~ 22 的模式 == */
    // else if (fc_effect.star_index == 19 ||
    //          fc_effect.star_index == 20 ||
    //          fc_effect.star_index == 21) //
    // {
    //     if (fc_effect.dream_scene.direction == IS_forward)
    //     {
    //         meteor_effect_options = NO_OPTIONS;
    //     }
    //     else
    //     {
    //         meteor_effect_options = REVERSE;
    //     }

    //     meteor_func_ptr = WS2812FX_mode_comet_1;
    //     meteor_effect_options |= fade_type[fc_effect.star_index - 19];
    // }
    // else if (fc_effect.star_index == 22)
    // {
    //     if (fc_effect.dream_scene.direction == IS_forward)
    //     {
    //         meteor_effect_options = NO_OPTIONS;
    //     }
    //     else
    //     {
    //         meteor_effect_options = REVERSE;
    //     }

    //     meteor_func_ptr = fc_double_meteor;
    // }
#endif
    // ====
    // 注意，以下的流星灯速度，需要根据实际来修改 USER_TO_DO
    // 目前还剩乱闪模式的速度不好修改调节
    else if (STAR_INDEX_METEOR_NORMAL_SLOW == fc_effect.star_index)
    {
        meteor_func_ptr = meteor_effect_slow;
    }
    else if (STAR_INDEX_METEOR_NORMAL_MIDDLE == fc_effect.star_index)
    {
        meteor_func_ptr = meteor_effect_middle;
    }
    else if (STAR_INDEX_METEOR_NORMAL_FAST == fc_effect.star_index)
    {
        meteor_func_ptr = meteor_effect_fast;
    }
    else if (STAR_INDEX_METEOR_RANDOM_BREATH == fc_effect.star_index)
    {
        meteor_func_ptr = meteor_effect_random_breath;
    }
    else if (STAR_INDEX_METEOR_MUSIC_CONTROL == fc_effect.star_index)
    {
        // 带声控的流星灯模式，
        /*
            单点流水
            单路的均衡器
            双路的均衡器
            ...
            依次循环
        */
        meteor_func_ptr = meteor_light_single_point_flow; // 声控模式每次都从流星灯单点流水开始
    }
    else if (STAR_INDEX_METEOR_MUSIC_CONTROL_2 == fc_effect.star_index)
    {
        meteor_func_ptr = meteor_light_sigle_channel_equalizer_effect;
    }
    else if (STAR_INDEX_METEOR_MUSIC_CONTROL_3 == fc_effect.star_index)
    {
        meteor_func_ptr = meteor_light_two_channel_equalizer_effect;
    }
    else
    {
        return;
    }

    WS2812FX_setSegment_colorOptions(
        1,                     // 第x段
        1,                     // 起始位置
        fc_effect.led_num - 1, // 结束位置
        meteor_func_ptr,
        WHITE,                  // 颜色，WS2812FX_setColors设置
        fc_effect.star_speed,   // 速度
        meteor_effect_options); // 选项

    WS2812FX_resetSegmentRuntime(1); // 重置流星灯所在的段运行时参数（这里发现清除不掉显示残留）
    WS2812FX_show();                 // 更新显示（这里发现清除不掉显示残留）
    WS2812FX_running_flag_set();
}

/**
 * @brief 音乐效果集合
 *
 */
static void ls_music_effect(void)
{

    // extern uint16_t fc_music_gradual(void);
    // extern uint16_t fc_music_breath(void);
    // extern uint16_t fc_music_static(void);
    // extern uint16_t fc_music_twinkle(void);

    // void *music_effect_addr = &fc_music_gradual; // 避免出现地址空，导致不断复位
    void *music_effect_addr = NULL;

    // app_set_bright(100); // 设置为最大亮度
    // WS2812FX_setBrightness(255); // 设置为最大亮度，但不改变 fc_effect.b 的值
#if (LED_STRIP_TYPE == TYPE_Fiber_optic_lights)

#if 0
    switch (fc_effect.music.m)
    {
    case 0:
        music_effect_addr = &fc_music_gradual;
        break;
    case 1:
        music_effect_addr = &fc_music_breath;
        break;
    case 2:
        music_effect_addr = &fc_music_static;
        break;
    case 3:
        music_effect_addr = &fc_music_twinkle;
        break;
    default:
        break;
    }
#endif

    switch (fc_effect.music.m)
    {
    case 0:
        music_effect_addr = &colorful_lights_sound_gradual_max_brightness;
        break;
    case 1:
        music_effect_addr = &colorful_lights_sound_breath_max_brightness;
        break;
    case 2:
        music_effect_addr = &colorful_lights_sound_static_max_brightness;
        break;
    case 3:
        music_effect_addr = &colorful_lights_sound_twinkle_max_brightness;
        break;
    default:
        break;
    }

    WS2812FX_setSegment_colorOptions(
        0,                       // 第0段
        0,                       // 起始位置
        0,                       // 结束位置
        music_effect_addr,       // 效果
        WHITE,                   // 颜色，WS2812FX_setColors设置
        100,                     // 速度
        SIZE_MEDIUM | FADE_XSLOW // 选项，这里像素点大小：3,反向/反向
    );

#elif (LED_STRIP_TYPE == TYPE_Magic_lights)

#endif

    // WS2812FX_start();
    WS2812FX_resetSegmentRuntime(0); // 清除指定段的显示缓存
    WS2812FX_running_flag_set();
}

/**
 * @brief 涂抹效果集合
 *
 */
static void ls_smear_adjust_effect(void)
{
}

// 设置流星灯的动画效果
// void meteor_lights_effect_set(void)
// {
//     mode_ptr *animation_ptr = NULL;
//     switch (fc_effect.star_index)
//     {
//     case STAR_INDEX_METEOR_NORMAL_SLOW:
//         animation_ptr = meteor_effect_slow;
//         break;

//     case STAR_INDEX_METEOR_NORMAL_MIDDLE:
//         animation_ptr = meteor_effect_middle;
//         break;

//     case STAR_INDEX_METEOR_NORMAL_FAST:
//         animation_ptr = meteor_effect_fast;
//         break;

//     case STAR_INDEX_METEOR_RANDOM_BREATH:
//         animation_ptr = meteor_effect_random_breath;
//         break;

//     case STAR_INDEX_METEOR_RANDOM_BREATH_2:
//         break;

//     case STAR_INDEX_METEOR_MUSIC_CONTROL: // 带声控的流星灯模式

//         break;

//     default:
//         return; // 出错，直接返回
//     }

//     WS2812FX_setSegment_colorOptions(
//         1,                     // 第x段
//         1,                     // 起始位置
//         fc_effect.led_num - 1, // 结束位置
//         animation_ptr,         // 动画效果
//         WHITE,                 // 颜色，WS2812FX_setColors设置
//         0,                     // 速度，对于样机的正常流星模式，速度这一属性无效
//         NO_OPTIONS);           // 选项

//     WS2812FX_resetSegmentRuntime(1); //
//     WS2812FX_running_flag_set();
// }

/**
 * @brief 静态效果集合
 *
 */
static void ls_static_effect(void)
{
    extern void static_mode(void);
    static_mode();
}

//====================================================================================================
//====================================================================================================
//====================================================================================================

/**
 * @brief 灯光模式总调度
 *
 */
void set_fc_effect(void)
{
    if (fc_effect.on_off_flag == DEVICE_OFF)
    {
        return;
    }

    switch (fc_effect.Now_state)
    {
    // 幻彩场景
    case IS_light_scene:
        ls_scene_effect();
        break;

    // 配对模式
    case ACT_TY_PAIR:
        // ls_ty_pair_effect();
        break;

    // 自定义效果模式
    case ACT_CUSTOM:
        // ls_custom_effect();
        break;

    // 音乐模式
    case IS_light_music:
        ls_music_effect();
        break;

    // 涂抹模式
    case IS_smear_adjust:
        // ls_smear_adjust_effect();
        break;

    // 静态模式
    case IS_STATIC:
        ls_static_effect();
        break;

    case IS_IN_MODE_PHONE_MIC:
    {
        WS2812FX_setSegment_colorOptions( // 设置一段颜色的效果
            0,                            // 第0段
            0,                            // 起始位置
            0,                            // 结束位置
            &colorful_lights_static_max_brightness,
            0,                                              // 颜色，WS2812FX_setColors设置
            100,                                            // 速度
            0);                                             // 选项，这里像素点大小：1
        WS2812FX_set_coloQty(0, fc_effect.dream_scene.c_n); // 设置颜色数量  0：第0段   fc_effect.dream_scene.c_n  颜色数量，一个颜色包含（RGB）
        ls_set_colors(1, &fc_effect.rgb);                   // 1:1个颜色    &fc_effect.rgb 这个颜色是什么色
        WS2812FX_resetSegmentRuntime(0);                    // 清除指定段的显示缓存
        WS2812FX_running_flag_set();                        // 置位运行标志
    }
    break;
    default:
        break;
    }
}
