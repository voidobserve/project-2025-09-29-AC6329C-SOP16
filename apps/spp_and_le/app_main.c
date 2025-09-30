/*********************************************************************************************
    *   Filename        : app_main.c

    *   Description     :

    *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
*********************************************************************************************/
#include "system/includes.h"
#include "app_config.h"
#include "app_action.h"
#include "app_main.h"
#include "update.h"
#include "update_loader_download.h"
#include "app_charge.h"
#include "app_power_manage.h"
#include "asm/charge.h"

#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
#include "jl_kws/jl_kws_api.h"
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */

#define LOG_TAG_CONST APP
#define LOG_TAG "[APP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#include "../../../apps/user_app/rf24g_key/rf24g_key.h"

OS_SEM LED_TASK_SEM;

/*任务列表   */
const struct task_info task_info_table[] = {
#if CONFIG_APP_FINDMY
    {"app_core", 1, 0, 640 * 2, 128},
#else
    {"app_core", 1, 0, 640, 128},
#endif

    {"sys_event", 7, 0, 256, 0},
    {"btctrler", 4, 0, 512, 256},
    {"btencry", 1, 0, 512, 128},
    {"btstack", 3, 0, 768, 256},
    {"systimer", 7, 0, 128, 0},
    {"update", 1, 0, 512, 0},
    {"dw_update", 2, 0, 256, 128},
#if (RCSP_BTMATE_EN)
    {"rcsp_task", 2, 0, 640, 0},
#endif
#if (USER_UART_UPDATE_ENABLE)
    {"uart_update", 1, 0, 256, 128},
#endif
#if (XM_MMA_EN)
    {"xm_mma", 2, 0, 640, 256},
#endif
    {"usb_msd", 1, 0, 512, 128},
#if TCFG_AUDIO_ENABLE
    {"audio_dec", 3, 0, 768, 128},
    {"audio_enc", 4, 0, 512, 128},
#endif /*TCFG_AUDIO_ENABLE*/
#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
    {"kws", 2, 0, 256, 64},
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */
#if (TUYA_DEMO_EN)
    {"user_deal", 2, 0, 512, 512}, // 定义线程 tuya任务调度
#endif
#if (CONFIG_APP_HILINK)
    {"hilink_task", 2, 0, 1024, 0}, // 定义线程 hilink任务调度
#endif

    {"led_task", 2, 0, 512, 512}, // 灯光
    // {"led_task", 3, 0, 512, 512}, // 灯光
    {0, 0},
};

APP_VAR app_var;

void app_var_init(void)
{
    app_var.play_poweron_tone = 1;

    app_var.auto_off_time = TCFG_AUTO_SHUT_DOWN_TIME;
    app_var.warning_tone_v = 340;
    app_var.poweroff_tone_v = 330;
}

__attribute__((weak))
u8
get_charge_online_flag(void)
{
    return 0;
}

void clr_wdt(void);
void check_power_on_key(void)
{
#if TCFG_POWER_ON_NEED_KEY

    u32 delay_10ms_cnt = 0;
    while (1)
    {
        clr_wdt();
        os_time_dly(1);

        extern u8 get_power_on_status(void);
        if (get_power_on_status())
        {
            log_info("+");
            delay_10ms_cnt++;
            if (delay_10ms_cnt > 70)
            {
                /* extern void set_key_poweron_flag(u8 flag); */
                /* set_key_poweron_flag(1); */
                return;
            }
        }
        else
        {
            log_info("-");
            delay_10ms_cnt = 0;
            log_info("enter softpoweroff\n");
            power_set_soft_poweroff();
        }
    }
#endif
}

void app_main()
{
    struct intent it;

    if (!UPDATE_SUPPORT_DEV_IS_NULL())
    {
        int update = 0;
        update = update_result_deal();
    }

    printf(">>>>>>>>>>>>>>>>>app_main...\n");
    printf(">>> v220,2022-11-23 >>>\n");

    if (get_charge_online_flag())
    {
#if (TCFG_SYS_LVD_EN == 1)
        vbat_check_init();
#endif
    }
    else
    {
        check_power_on_voltage();
    }

#if TCFG_POWER_ON_NEED_KEY
    check_power_on_key();
#endif

#if TCFG_AUDIO_ENABLE
    extern int audio_dec_init();
    extern int audio_enc_init();
    audio_dec_init();
    audio_enc_init();
#endif /*TCFG_AUDIO_ENABLE*/

#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
    jl_kws_main_user_demo();
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */

    init_intent(&it);

#if CONFIG_APP_SPP_LE
    it.name = "spp_le";
    it.action = ACTION_SPPLE_MAIN;

#elif CONFIG_APP_AT_COM || CONFIG_APP_AT_CHAR_COM
    it.name = "at_com";
    it.action = ACTION_AT_COM;

#elif CONFIG_APP_DONGLE
    it.name = "dongle";
    it.action = ACTION_DONGLE_MAIN;

#elif CONFIG_APP_MULTI
    it.name = "multi_conn";
    it.action = ACTION_MULTI_MAIN;

#elif CONFIG_APP_NONCONN_24G
    it.name = "nonconn_24g";
    it.action = ACTION_NOCONN_24G_MAIN;

#elif CONFIG_APP_HILINK
    it.name = "hilink";
    it.action = ACTION_HILINK_MAIN;

#elif CONFIG_APP_LL_SYNC
    it.name = "ll_sync";
    it.action = ACTION_LL_SYNC;

#elif CONFIG_APP_TUYA
    it.name = "tuya";
    it.action = ACTION_TUYA;

#elif CONFIG_APP_CENTRAL
    it.name = "central";
    it.action = ACTION_CENTRAL_MAIN;

#elif CONFIG_APP_DONGLE
    it.name = "dongle";
    it.action = ACTION_DONGLE_MAIN;

#elif CONFIG_APP_BEACON
    it.name = "beacon";
    it.action = ACTION_BEACON_MAIN;

#elif CONFIG_APP_IDLE
    it.name = "idle";
    it.action = ACTION_IDLE_MAIN;

#elif CONFIG_APP_CONN_24G
    it.name = "conn_24g";
    it.action = ACTION_CONN_24G_MAIN;

#elif CONFIG_APP_FINDMY
    it.name = "findmy";
    it.action = ACTION_FINDMY;

#else
    while (1)
    {
        printf("no app!!!");
    }
#endif

    log_info("run app>>> %s", it.name);
    log_info("%s,%s", __DATE__, __TIME__);

    start_app(&it);

#if TCFG_CHARGE_ENABLE
    set_charge_event_flag(1);
#endif
}

/*
 * app模式切换
 */
void app_switch(const char *name, int action)
{
    struct intent it;
    struct application *app;

    log_info("app_exit\n");

    init_intent(&it);
    app = get_current_app();
    if (app)
    {
        /*
         * 退出当前app, 会执行state_machine()函数中APP_STA_STOP 和 APP_STA_DESTORY
         */
        it.name = app->name;
        it.action = ACTION_BACK;
        start_app(&it);
    }

    /*
     * 切换到app (name)并执行action分支
     */
    it.name = name;
    it.action = action;
    start_app(&it);
}

int eSystemConfirmStopStatus(void)
{
    /* 系统进入在未来时间里，无任务超时唤醒，可根据用户选择系统停止，或者系统定时唤醒(100ms) */
    // 1:Endless Sleep
    // 0:100 ms wakeup
    /* log_info("100ms wakeup"); */
    return 1;
}

__attribute__((used)) int *__errno()
{
    static int err;
    return &err;
}

#if 1

// --------------------------------------------------------------------------定时器
static const u16 timer_div[] = {
    /*0000*/ 1,
    /*0001*/ 4,
    /*0010*/ 16,
    /*0011*/ 64,
    /*0100*/ 2,
    /*0101*/ 8,
    /*0110*/ 32,
    /*0111*/ 128,
    /*1000*/ 256,
    /*1001*/ 4 * 256,
    /*1010*/ 16 * 256,
    /*1011*/ 64 * 256,
    /*1100*/ 2 * 256,
    /*1101*/ 8 * 256,
    /*1110*/ 32 * 256,
    /*1111*/ 128 * 256,
};
#define APP_TIMER_CLK (CONFIG_BT_NORMAL_HZ / 2) // clk_get("timer")
#define MAX_TIME_CNT 0x7fff
#define MIN_TIME_CNT 0x100
#define TIMER_UNIT 1

#define TIMER_CON JL_TIMER2->CON
#define TIMER_CNT JL_TIMER2->CNT
#define TIMER_PRD JL_TIMER2->PRD
#define TIMER_VETOR IRQ_TIME2_IDX

___interrupt
    AT_VOLATILE_RAM_CODE void
    user_timer_isr(void) // 50us
{

    TIMER_CON |= BIT(14);

    extern void one_wire_send(void);
    one_wire_send(); // steomotor
}

void user_timer_init(void)
{
    u32 prd_cnt;
    u8 index;

    //	printf("********* user_timer_init **********\n");
    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++)
    {
        prd_cnt = TIMER_UNIT * (APP_TIMER_CLK / 8000) / timer_div[index];
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT)
        {
            break;
        }
    }

    TIMER_CNT = 0;
    TIMER_PRD = prd_cnt;
    request_irq(TIMER_VETOR, 0, user_timer_isr, 0);
    TIMER_CON = (index << 4) | BIT(0) | BIT(3);
}
__initcall(user_timer_init);

#endif

#include "../../apps/user_app/led_strip/led_strip_drive.h"
#include "../../apps/user_app/led_strip/led_strand_effect.h"
#include "hardware.h"

void main_while(void)
{

    while (1)
    {
        effect_stepmotor(); // 声控，电机的音乐效果
        stepmotor();        // 无霍尔时，电机停止指令计时

        // power_motor_Init();  // 电机

        meteor_period_sub(); // 流星周期控制

        

        rf24_key_handle();
        // printf("main circle\n"); // 主循环约10ms

        // printf("sizeof  fc_effect_t %d\n", sizeof(fc_effect_t)); // 打印是 276，存放到flash中会有问题

        os_time_dly(1);
    }
}

void WS2812_circle_task(void)
{
    sound_handle();
    run_tick_per_10ms();
    WS2812FX_service();

    // printf("cycle\n"); // 测试调用的周期
}

void my_main(void)
{
    led_gpio_init(); // 七彩灯控制引脚初始化
    led_pwm_init();  // 七彩灯控制引脚对应的PWM初始化
    mic_gpio_init();
    // fan_gpio_init();
    led_state_init();
    mcu_com_init(); // 电机一线通信

    read_flash_device_status_init();
    full_color_init();

    // os_sem_create(&LED_TASK_SEM, 0);

    sys_s_hi_timer_add(NULL, WS2812_circle_task, 10); // 10ms

    task_create(main_while, NULL, "led_task");
}
