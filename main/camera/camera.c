//20231204
#include "camera.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "string.h"

TaskHandle_t cameraTaskHandle;

void camera_task(void *pvParameters)
{
    camera_fb_t *pic;
    esp_camera_deinit();                                /* 复位摄像头的硬件设置 */
    esp_err_t err = esp_camera_init(&camera_config);    /* 按照既定参数设置摄像头 */
    if (err != ESP_OK)
    {
        ESP_LOGE(camera, "Camera Init Failed");
        return ;
    }
    if (sock < 0) {
        ESP_LOGE("camera","sock error!");
    }

    while(1)
    {
        //20240610 增加对于获取图像阻塞时间的探测
        // 注意此函数是阻塞函数 并且内部设置了超时时间4000ms
        uint64_t startTime = esp_timer_get_time(); // 获取起始时间
        pic = esp_camera_fb_get();//获取一帧图像
        uint64_t endTime = esp_timer_get_time(); // 获取结束时间
        uint64_t elapsedTime = endTime - startTime; // 计算时间间隔，即程序运行时间
        ESP_LOGI(camera,"Elapsed time: %lld us!\n", elapsedTime);

        if( pic != NULL)//接受数据正常
        {
            //pic->width, pic->height, pic->format, pic->buf

			//send by udpSock
			unsigned int preSendPicLen = pic->len;//获取帧长度
            unsigned char * startPalce =  pic->buf;//buffer地址

            if(preSendPicLen < UDP_SEND_MAX_LEN)
            {
                //int err = sendto(sock, payload, strlen(payload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

                //VOFA: // 先发送前导帧  "image:IMG_ID, IMG_SIZE, IMG_WIDTH, IMG_HEIGHT, IMG_FORMAT\n"
                // int err = sendto(sock,pre_frame_buf,strlen(pre_frame_buf), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

                int err = sendto(sock,startPalce,preSendPicLen, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                // vTaskDelay(60 / portTICK_PERIOD_MS);  //这个延时完全可以不添加 可以拉满帧率
                if (err < 0) {
                    ESP_LOGE("camera", "Error occurred during sending: errno %d", errno);
                    break;
                }
                //ESP_LOGI("camera", "Image sent!");
            }

			// use pic->buf to access the image
			//ESP_LOGI(camera, "Picture taken! Its size was: %zu bytes,width: %zu,height: %zu", pic->len,pic->width,pic->height);
			
            esp_camera_fb_return(pic);//释放空间

            uint64_t endTime = esp_timer_get_time(); // 获取UDP发送结束时间
            uint64_t elapsedTime = endTime - startTime; // 计算时间间隔 即帧周期
            ESP_LOGI(camera,"frame time: %d ms!\n", (int)((float)elapsedTime/1000.0));
		}
		else
        {
            ESP_LOGI("camera","error");
        }
    }
}
