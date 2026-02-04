
#ifndef __SAVE_FLASH_H__
#define __SAVE_FLASH_H__

#include "led_strand_effect.h"
#include "led_strip_drive.h"
typedef enum
{
    FRIST_BYTE,    // 第一次上电标志
    LED_LEDGTH_MS, // 灯带长度高8位
    LED_LEDGTH_LS, // 灯带长度低8位
} FLASH_BYTE_FLAG;

#pragma pack(1)
typedef struct
{
    unsigned char header; // 头部
    fc_effect_t fc_save;
} save_flash_t;

#pragma pack()

// 需要保存数据时，延时保存的时间：（单位：ms）
#define DELAY_SAVE_FLASH_TIMES ((u16)3000)

void read_flash_device_status_init(void);
void save_user_data_area3(void);

void save_user_data_enable(void); 

void save_user_data_time_count_down(void);
void save_user_data_handle(void);

#endif