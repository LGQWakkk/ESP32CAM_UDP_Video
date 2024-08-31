#include "wifi_sta.h"
#include "esp_log.h"


//WIFI_NAME
#define DEFAULT_SSID        "D7"
//WIFI_PASSWD
#define DEFAULT_PWD         "12345678"

/* 事件标志 */
static EventGroupHandle_t   wifi_event;
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

network_connet_info network_connet;
static const char *TAG = "wifi_sta";
char lcd_buff[100] = {0};

#define WIFICONFIG()   {                            \
    .sta = {                                        \
        .ssid = DEFAULT_SSID,                       \
        .password = DEFAULT_PWD,                    \
        .threshold.authmode = WIFI_AUTH_WPA2_PSK,   \
    },                                              \
}

void connet_display(uint8_t flag)
{
    if((flag & 0x80) == 0x80)//WIFI_EVENT_STA_CONNECTED
    {
        ESP_LOGI(TAG, "wifi connected");
        ESP_LOGI(TAG, "ssid:%s   password:%s",DEFAULT_SSID,DEFAULT_PWD);
    
        // lcd_fill(0,90,320,240,WHITE);
        // sprintf(lcd_buff, "ssid:%s",DEFAULT_SSID);
        // lcd_show_string(0, 90, 240, 16, 16, lcd_buff, BLUE);
        // sprintf(lcd_buff, "psw:%s",DEFAULT_PWD);
        // lcd_show_string(0, 110, 240, 16, 16, lcd_buff, BLUE);
        // lcd_show_string(0, 130, 200, 16, 16, "KEY0:Send data", MAGENTA);
    }
    else if ((flag & 0x04) == 0x04)//WIFI_EVENT_STA_START
    {
        ESP_LOGI(TAG, "wifi connecting......");
        // lcd_show_string(0, 90, 240, 16, 16, "wifi connecting......", BLUE);
    }
    else if ((flag & 0x02) == 0x02)//WIFI_EVENT_STA_DISCONNECTED
    {
        ESP_LOGI(TAG, "wifi connecting fail");
        // lcd_show_string(0, 90, 240, 16, 16, "wifi connecting fail", BLUE);
    }
    else if ((flag & 0x01) == 0x01)
    {
        ESP_LOGI(TAG,"IP: %s",(char*)network_connet.ip_buf);
        // lcd_show_string(0, 150, 200, 16, 16, (char*)network_connet.ip_buf, MAGENTA);
    }
    network_connet.connet_state &= 0x00;
}

//WIFI事件回调处理函数
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    static int s_retry_num = 0;

    /* 扫描到要连接的WIFI事件 */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        network_connet.connet_state |= 0x04;
        network_connet.fun(network_connet.connet_state);
        esp_wifi_connect();
    }
    /* 连接WIFI事件 */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        network_connet.connet_state |= 0x80;
        network_connet.fun(network_connet.connet_state);
    }
    /* 连接WIFI失败事件 */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        network_connet.connet_state |= 0x02;
        /* 尝试连接 */
        if (s_retry_num < 20)
        {
            esp_wifi_connect();
            s_retry_num ++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(wifi_event, WIFI_FAIL_BIT);
            network_connet.fun(network_connet.connet_state);
        }

        ESP_LOGI(TAG,"connect to the AP fail");
    }
    /* 工作站从连接的AP获得IP */
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        network_connet.connet_state |= 0x01;
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        sprintf(network_connet.ip_buf, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        network_connet.fun(network_connet.connet_state);
        xEventGroupSetBits(wifi_event, WIFI_CONNECTED_BIT);
    }
}

//WIFI_INIT
void wifi_sta_init(void)
{
    static esp_netif_t *sta_netif = NULL;
    network_connet.connet_state = 0x00;
    network_connet.fun = connet_display;

    wifi_event= xEventGroupCreate();    /* 创建一个事件标志组 */
    /* 网卡初始化 */
    ESP_ERROR_CHECK(esp_netif_init());
    /* 创建新的事件循环 */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif= esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL) );
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));    
    wifi_config_t  wifi_config = WIFICONFIG();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start());

    /* 等待链接成功后、ip生成 */
    EventBits_t bits = xEventGroupWaitBits( wifi_event,
                                            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                            pdFALSE,
                                            pdFALSE,
                                            portMAX_DELAY);

    /* 判断连接事件 */
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 DEFAULT_SSID, DEFAULT_PWD);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 DEFAULT_SSID, DEFAULT_PWD);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
    vEventGroupDelete(wifi_event);
}
