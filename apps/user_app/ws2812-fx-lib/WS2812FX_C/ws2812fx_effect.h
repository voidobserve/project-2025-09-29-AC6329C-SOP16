#ifndef ws2812fx_effect_h
#define ws2812fx_effect_h

#include "includes.h"

uint16_t WS2812FX_multiColor_wipe(uint8_t is_reverse, uint8_t rev);

// builtin modes
uint16_t
WS2812FX_mode_static(void),
    WS2812FX_mode_blink(void),
    WS2812FX_mode_blink_rainbow(void),
    WS2812FX_mode_strobe(void),
    WS2812FX_mode_strobe_rainbow(void),
    WS2812FX_mode_color_wipe(void),
    WS2812FX_mode_color_wipe_inv(void),
    WS2812FX_mode_color_wipe_rev(void),
    WS2812FX_mode_color_wipe_rev_inv(void),
    WS2812FX_mode_color_wipe_random(void),
    WS2812FX_mode_color_sweep_random(void),
    WS2812FX_mode_random_color(void),
    WS2812FX_mode_single_dynamic(void),
    WS2812FX_mode_multi_dynamic(void),
    WS2812FX_mode_breath(void),
    WS2812FX_mode_fade(void),
    WS2812FX_mode_scan(void),
    WS2812FX_mode_dual_scan(void),
    WS2812FX_mode_theater_chase(void),
    WS2812FX_mode_theater_chase_rainbow(void),
    WS2812FX_mode_rainbow(void),
    WS2812FX_mode_rainbow_cycle(void),
    WS2812FX_mode_running_lights(void),
    WS2812FX_mode_twinkle(void),
    WS2812FX_mode_twinkle_random(void),
    WS2812FX_mode_twinkle_fade(void),
    WS2812FX_mode_twinkle_fade_random(void),
    WS2812FX_mode_sparkle(void),
    WS2812FX_mode_flash_sparkle(void),
    WS2812FX_mode_hyper_sparkle(void),
    WS2812FX_mode_multi_strobe(void),
    WS2812FX_mode_chase_white(void),
    WS2812FX_mode_chase_color(void),
    WS2812FX_mode_chase_random(void),
    WS2812FX_mode_chase_rainbow(void),
    WS2812FX_mode_chase_flash(void),
    WS2812FX_mode_chase_flash_random(void),
    WS2812FX_mode_chase_rainbow_white(void),
    WS2812FX_mode_chase_blackout(void),
    WS2812FX_mode_chase_blackout_rainbow(void),
    WS2812FX_mode_running_color(void),
    WS2812FX_mode_running_red_blue(void),
    WS2812FX_mode_running_random(void),
    WS2812FX_mode_larson_scanner(void),
    WS2812FX_mode_comet(void),
    WS2812FX_mode_fireworks(void),
    WS2812FX_mode_fireworks_random(void),
    WS2812FX_mode_merry_christmas(void),
    WS2812FX_mode_halloween(void),
    WS2812FX_mode_fire_flicker(void),
    WS2812FX_mode_fire_flicker_soft(void),
    WS2812FX_mode_fire_flicker_intense(void),
    WS2812FX_mode_circus_combustus(void),
    WS2812FX_mode_bicolor_chase(void),
    WS2812FX_mode_tricolor_chase(void),
    WS2812FX_mode_custom_0(void),
    WS2812FX_mode_custom_1(void),
    WS2812FX_mode_custom_2(void),
    WS2812FX_mode_custom_3(void),
    WS2812FX_mode_custom_4(void),
    WS2812FX_mode_custom_5(void),
    WS2812FX_mode_custom_6(void),
    WS2812FX_mode_custom_7(void),
    WS2812FX_mode_fade_single(void),
    WS2812FX_smear_adjust_effect(void);

uint16_t WS2812FX_mode_fade_each_led(void);
void set_seg_forward_out(uint8_t s, uint16_t ms);
uint16_t WS2812FX_mode_single_block_scan(void);
uint16_t WS2812FX_mode_mutil_fade(void);
uint16_t WS2812FX_mode_multi_block_scan(void);
uint16_t WS2812FX_mode_mutil_breath(void);
uint16_t WS2812FX_mode_mutil_twihkle(void);
uint16_t WS2812FX_mode_multi_forward_same(void);
uint16_t WS2812FX_mode_multi_back_same(void);
uint16_t WS2812FX_adj_rgb_sequence(void);
uint16_t WS2812FX_mutil_c_jump(void);
uint16_t WS2812FX_mutil_c_gradual(void);
uint16_t breath_w(void);
uint16_t WS2812FX_mutil_strobe(void);
uint16_t breath_rgb(void);

/*************** 光纤满天星流星效果*****************/

uint16_t WS2812FX_mode_comet_1(void);
uint16_t WS2812FX_mode_comet_2(void);
uint16_t WS2812FX_mode_comet_3(void);
uint16_t meteor_effect_G(void);
uint16_t meteor_effect_H(void);
uint16_t WS2812FX_mode_comet_4(void);
uint16_t WS2812FX_mode_comet_5(void);
uint16_t WS2812FX_mode_comet_6(void);

uint16_t meteor(void);
uint16_t music_meteor3(void);

uint16_t fc_double_meteor(void);
void close_metemor(void);

// ==============================================================

u16 colorful_lights_static_max_brightness(void);

// 定义在七彩灯自动模式下的各个子模式步骤：
enum
{
    COLORFUL_LIGHTS_NONE = 0,
    COLORFUL_LIGHTS_FLASH_BEGIN,
    COLORFUL_LIGHTS_FLASH_END,
    COLORFUL_LIGHTS_JUMP_BEGIN,
    COLORFUL_LIGHTS_JUMP_END,
    COLORFUL_LIGHTS_GRADUAL_BEGIN,
    COLORFUL_LIGHTS_GRADUAL_END,
    COLORFUL_LIGHTS_BREATHING_BEGIN,
    COLORFUL_LIGHTS_BREATHING_END,
};
// 七彩灯动画
u16 colorful_lights_static(void); // 七彩灯的静态效果
u16 colorful_lights_flash(void);  // 七彩灯的频闪效果
u16 colorful_lights_jump(void);   // 七彩灯跳变动画
u16 colorful_lights_gradual(void);
u16 colorful_lights_breathing(void);
u16 colorful_lights_auto(void); // 七彩灯的自动模式

u16 colorful_lights_effect_close(void); // 熄灭七彩灯

// 七彩灯声控模式下，对应的动画
u16 colorful_lights_sound_gradual_max_brightness(void); // 七彩灯的 声控渐变 效果
u16 colorful_lights_sound_breath_max_brightness(void); // 七彩灯的 声控呼吸 效果
u16 colorful_lights_sound_static_max_brightness(void); // 七彩灯的 声控静态 效果
u16 colorful_lights_sound_twinkle_max_brightness(void); // 七彩灯的 声控跳变 效果



// ==============================================================
/*
    meteor_effect_slow()、meteor_effect_middle()、
    meteor_effect_fast()动画中使用到的流星灯尾焰长度
    meteor_tail_len == 1，整个流星灯长度为1个灯珠
    meteor_tail_len == 4，整个流星灯长度为4个灯珠，尾焰3个灯珠
*/
extern volatile u8 meteor_tail_len;
/*
    meteor_effect_random_breath()动画中的速度和动画时间索引
*/
extern volatile u8 random_breath_index;

// 快速流星效果
// 在每次开启流星灯时使用
// u16 meteor_fast_effect(void);
u16 meteor_effect_when_pwr_on(void);
 

u16 meteor_effect_slow(void);                          // 样机的正常流星（慢速）模式
u16 meteor_effect_middle(void);                        // 样机的正常流星（中速）模式
u16 meteor_effect_fast(void);                          // 样机的正常流星（快速）模式
u16 meteor_effect_random_breath(void);                 // 流星动画，对应样机的乱闪效果
u16 meteor_light_single_point_flow(void);              // 声控模式下，流星灯单点流水
u16 meteor_light_sigle_channel_equalizer_effect(void); // 飙升（相当于声控模式下的单路均衡器效果）
u16 meteor_light_two_channel_equalizer_effect(void);   // 双路并行均衡器效果

#endif