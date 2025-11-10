#ifndef __RF24G_KEY_EVENT_HANDLE_H__
#define __RF24G_KEY_EVENT_HANDLE_H__

#include "includes.h"

#include "../../../apps/user_app/rf24g_key/rf24g_key.h"

// 给【指向遥控器按键事件处理函数的指针类型】起别名
typedef void (*rf24_key_handle_func_t)(void);
// typedef struct
// {
//     u8 key_event;
//     rf24_key_handle_func_t key_handle_func;
// } rf24_key_handle_info_t;

// void rf24g_key_2_event_r1c1_click_handle(void);
// void rf24g_key_2_event_r1c2_click_handle(void);
// void rf24g_key_2_event_r1c3_click_handle(void);
// void rf24g_key_2_event_r1c4_click_handle(void);

extern const rf24_key_handle_func_t rf24_key_handle_func_buff[RF24G_KEY_EVENT_MAX];

#endif