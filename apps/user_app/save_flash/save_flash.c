
#include "system/includes.h"
#include "syscfg_id.h"
#include "save_flash.h"
#define CFG_USER_LED_LEDGTH_DATA 3

// const u8 frist_mode[] = {0x3D, 0x00, 0x00, 0x0B, 0x00, 0x01, 0x00, 0x14, 0x00, 0x00, 0x01, 0x03, 0xE8, 0x03, 0xE8}; // 第一次上电默认模式

#define FLASH_CRC_DATA 0xC5

static volatile u16 timer_id = 0;
static volatile u16 time_count_down = 0;
static volatile u8 flag_is_enable_count_down = 0;
static volatile u8 flag_is_enable_to_save = 0; // 标志位，是否使能了保存

volatile save_flash_t save_data;

/*******************************************************************************************************
**函数名：上电读取FLASH里保存的指令数据
**输  出：
**输  入：读取 CFG_USER_COMMAND_BUF_DATA 里保存的最后一条接收到的指令，
**描  述：读取 CFG_USER_LED_LEDGTH_DATA 里保存的第一次上电标志，灯带长度，顺序是：：第1字节：第一次上电标志位，第2、3字节：灯带长度
**说  明：
**版  本：
**修改日期：
*******************************************************************************************************/
void read_flash_device_status_init(void)
{
    int ret = 0;
    local_irq_disable(); // 禁用中断
    ret = syscfg_read(CFG_USER_LED_LEDGTH_DATA, (u8 *)(&save_data), sizeof(save_flash_t));
    local_irq_enable(); // 使能中断
    if (ret != sizeof(save_flash_t))
    {
        // 如果读取到的数据个数不一致
        // printf("read save info error \n");
        memset((u8 *)&save_data, 0, sizeof(save_flash_t));
    }

    if (save_data.header != FLASH_CRC_DATA) // 第一次上电
    {
        save_data.header = FLASH_CRC_DATA;
        fc_data_init();
        // save_user_data_area3(); // 将初始化后的数据写回flash
        os_taskq_post("msg_task", 1, MSG_USER_SAVE_INFO);
        // printf("is first power on\n");

        // save_user_data_enable();
    }
    else
    {
        memcpy((u8 *)(&fc_effect), (u8 *)(&save_data.fc_save), sizeof(fc_effect_t));
        // printf("is not first power on\n");
    }
}

// 写入flash时间倒计时
void save_data_time_count_down(void *p)
{
#if 0
    if (time_count_down > 0)
    {
        time_count_down--;
    }
    else
    {
        if (timer_id)
        {
            sys_timer_del(timer_id);
            timer_id = 0;
            // printf("timer del\n");
        }

        save_user_data_area3();
    }
#endif

    // if (0 == flag_is_enable_to_save)
    // {
    //     return;
    // }

    if (0 == flag_is_enable_count_down)
    {
        return;
    }

    if (time_count_down > 0)
    {
        time_count_down--;
    }

    if (0 == time_count_down)
    {
        flag_is_enable_count_down = 0;
        flag_is_enable_to_save = 1;
        // save_user_data_area3();
    }
}

// 把用户数据写到区域3
void save_user_data_area3(void)
{
    int ret = 0;

    save_data.header = FLASH_CRC_DATA; // 表示数据有效

    memcpy((u8 *)(&save_data.fc_save), (u8 *)(&fc_effect), sizeof(fc_effect_t));
    local_irq_disable(); // 禁用中断
    ret = syscfg_write(CFG_USER_LED_LEDGTH_DATA, (u8 *)(&save_data), sizeof(save_flash_t));
    local_irq_enable(); // 使能中断

    flag_is_enable_to_save = 0;

    printf("save info done \n");
}

void save_user_data_enable(void)
{
#if 0
    if (timer_id)
    {
        // 如果已经创建了定时器，删除它
        sys_timer_del(timer_id);
        timer_id = 0;
    }

    time_count_down = 30;                                           // 30 * 100 ms定时器，实现 3000 ms延时
    timer_id = sys_timer_add(NULL, save_data_time_count_down, 100); // 创建 100ms 的定时
#endif

    flag_is_enable_count_down = 0;
    time_count_down = 30; // 30 * 100 ms定时器，实现 3000 ms延时
    flag_is_enable_count_down = 1;
    if (0 == timer_id)
    {
        timer_id = sys_timer_add(NULL, save_data_time_count_down, 100); // 创建 100ms 的定时
    }

    // printf("timer create success\n");
    // printf("timer id %u\n", (u16)timer_id);
}

u8 save_user_data_status_get(void)
{
    return flag_is_enable_to_save;
}


