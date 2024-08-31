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

/* WIFI�豸��Ϣ */
typedef struct _network_connet_info_t
{
    uint8_t         connet_state;    /* ��������״̬ */
    char            ip_buf[100];     /* �����IP��ַ */
    char            mac_buf[100];    /* �������� */
    void (*fun)(uint8_t x);          /* ����ָ�� */
} network_connet_info;

extern network_connet_info network_connet;

/* �������� */
void wifi_sta_init(void);

#endif
