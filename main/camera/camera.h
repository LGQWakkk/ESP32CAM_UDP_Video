#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_sntp.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "esp_camera.h"

//camera.c
extern TaskHandle_t cameraTaskHandle;
//udp.c
extern int sock;
extern struct sockaddr_in dest_addr;

//61440 Bytes
#define UDP_SEND_MAX_LEN ( 60 * 1024 ) /* 设置UDP一包发送的最大数量 */

//camera library
#define CAM_PIN_PWDN    (32)
#define CAM_PIN_RESET   (2) //not used!!!
#define CAM_PIN_XCLK    (0)
#define CAM_PIN_SIOD    (26)
#define CAM_PIN_SIOC    (27)

#define CAM_PIN_D7      (35)
#define CAM_PIN_D6      (34)
#define CAM_PIN_D5      (39)
#define CAM_PIN_D4      (36)
#define CAM_PIN_D3      (21)
#define CAM_PIN_D2      (19)
#define CAM_PIN_D1      (18)
#define CAM_PIN_D0      (5)

#define CAM_PIN_VSYNC   (25)
#define CAM_PIN_HREF    (23)
#define CAM_PIN_PCLK    (22)

static const char *camera = "camera";

//basical config
static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 22.5 * 1000 * 1000,//22.5MHz
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG,     //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_VGA,       //QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.
    .jpeg_quality = 20,                  //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 2,                      //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

void camera_task(void *pvParameters);

#endif /* __CAMERA_H__ */
