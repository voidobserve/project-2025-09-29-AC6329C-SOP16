#include "led_strip_voice.h"
#include "asm/adc_api.h"
#include "led_strip_drive.h"

#define MAX_SOUND 10
struct MUSIC_VOICE_T
{
    u8 sound_trg;
    u8 meteor_trg;
    u32 adc_sum;
    u32 adc_sum_n;
    int sound_buf[MAX_SOUND];
    u8 sound_cnt;
    int c_v;
    int v;
    u8 valid;
};

struct MUSIC_VOICE_T music_voic = {

    .sound_trg = 0,
    .meteor_trg = 0,
    .adc_sum = 0,
    .adc_sum_n = 0,
    .sound_cnt = 0,
    .valid = 0,
    .v = 0,
    .c_v = 0,
};

// 获取声控结果
// 触发条件：（（当前声音大小 - 平均值）* 100 ）/ 平均值 > 灵敏度（0~100）
// 0:没触发
// 1:触发
u8 get_sound_result(void)
{
    u8 p_trg;
    p_trg = music_voic.sound_trg;
    music_voic.sound_trg = 0;
    return p_trg;
}

u8 get_meteor_result(void)
{
    u8 p_metemor_trg;
    p_metemor_trg = music_voic.meteor_trg;
    music_voic.meteor_trg = 0;
    return p_metemor_trg;
}

void sound_handle(void)
{
    u16 adc;
    u8 i;
    // 记录adc值
#if 1
    // if (fc_effect.on_off_flag == DEVICE_ON &&
    //     (fc_effect.Now_state == IS_light_music ||
    //      fc_effect.base_ins.mode == 0x05 ||
    //      fc_effect.star_index == 17 ||
    //      fc_effect.star_index == 18))

    if (fc_effect.star_index == 99) // 测试时使用
    {

        music_voic.sound_buf[music_voic.sound_cnt] = check_mic_adc();
        music_voic.c_v = music_voic.sound_buf[music_voic.sound_cnt]; // 记录当前值
        music_voic.sound_cnt++;

        if (music_voic.sound_cnt > (MAX_SOUND - 1))
        {
            music_voic.sound_cnt = 0;
            music_voic.valid = 1;
            music_voic.v = 0;
            for (i = 0; i < MAX_SOUND; i++)
            {
                music_voic.v += music_voic.sound_buf[i];
            }
            music_voic.v = music_voic.v / MAX_SOUND; // 计算平均值
        }

        if (music_voic.valid)
        {

            if (music_voic.c_v > music_voic.v)
            {
                if ((music_voic.c_v - music_voic.v) * 100 / music_voic.v > fc_effect.music.s) // 很灵敏
                {
                    music_voic.sound_trg = 1;  // 七彩声控
                    music_voic.meteor_trg = 1; // 流星声控

                    // USER_TO_DO 需要再这里也加一层限制
                    WS2812FX_trigger();        // 让主循环扫描到立刻更新动画（注意不能在非声控模式使用，否则一检测到有声控，就会立即触发动画切换）
                }
            }
        }
    }
    else
    {
        music_voic.valid = 0;
    }

#endif
}
