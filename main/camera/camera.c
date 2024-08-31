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
    esp_camera_deinit();                                /* ��λ����ͷ��Ӳ������ */
    esp_err_t err = esp_camera_init(&camera_config);    /* ���ռȶ�������������ͷ */
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
        //20240610 ���Ӷ��ڻ�ȡͼ������ʱ���̽��
        // ע��˺������������� �����ڲ������˳�ʱʱ��4000ms
        uint64_t startTime = esp_timer_get_time(); // ��ȡ��ʼʱ��
        pic = esp_camera_fb_get();//��ȡһ֡ͼ��
        uint64_t endTime = esp_timer_get_time(); // ��ȡ����ʱ��
        uint64_t elapsedTime = endTime - startTime; // ����ʱ����������������ʱ��
        ESP_LOGI(camera,"Elapsed time: %lld us!\n", elapsedTime);

        if( pic != NULL)//������������
        {
            //pic->width, pic->height, pic->format, pic->buf

			//send by udpSock
			unsigned int preSendPicLen = pic->len;//��ȡ֡����
            unsigned char * startPalce =  pic->buf;//buffer��ַ

            if(preSendPicLen < UDP_SEND_MAX_LEN)
            {
                //int err = sendto(sock, payload, strlen(payload), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

                //VOFA: // �ȷ���ǰ��֡  "image:IMG_ID, IMG_SIZE, IMG_WIDTH, IMG_HEIGHT, IMG_FORMAT\n"
                // int err = sendto(sock,pre_frame_buf,strlen(pre_frame_buf), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

                int err = sendto(sock,startPalce,preSendPicLen, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                // vTaskDelay(60 / portTICK_PERIOD_MS);  //�����ʱ��ȫ���Բ���� ��������֡��
                if (err < 0) {
                    ESP_LOGE("camera", "Error occurred during sending: errno %d", errno);
                    break;
                }
                //ESP_LOGI("camera", "Image sent!");
            }

			// use pic->buf to access the image
			//ESP_LOGI(camera, "Picture taken! Its size was: %zu bytes,width: %zu,height: %zu", pic->len,pic->width,pic->height);
			
            esp_camera_fb_return(pic);//�ͷſռ�

            uint64_t endTime = esp_timer_get_time(); // ��ȡUDP���ͽ���ʱ��
            uint64_t elapsedTime = endTime - startTime; // ����ʱ���� ��֡����
            ESP_LOGI(camera,"frame time: %d ms!\n", (int)((float)elapsedTime/1000.0));
		}
		else
        {
            ESP_LOGI("camera","error");
        }
    }
}
