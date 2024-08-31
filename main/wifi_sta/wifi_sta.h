#ifndef __WIFI_STA_h
#define __WIFI_STA_h

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include <netdb.h>

/* WIFI设备信息 */
typedef struct _network_connet_info_t
{
    uint8_t         connet_state;    /* 网络连接状态 */
    char            ip_buf[100];     /* 分配的IP地址 */
    char            mac_buf[100];    /* 子网掩码 */
    void (*fun)(uint8_t x);          /* 函数指针 */
} network_connet_info;

extern network_connet_info network_connet;

/* 声明函数 */
void wifi_sta_init(void);

#endif
