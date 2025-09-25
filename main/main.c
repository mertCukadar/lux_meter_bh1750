#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "bh1750.h"
#include "i2c_handler.h"
#include "sen55.h"
#include "Ina219.h"
#include "wifi_event.h"   // wifi_init_sta için
#include "esp-now.h"     // esp_now_tx_init için
#include "esp_sleep.h"



static const char *TAG = "MAIN";
static const uint8_t PEER_MAC[6] = {0xCC,0xBA,0x97,0x12,0x51,0x40};

// 5 dk’da bir gonderip uyutan supervisor
static void vTask_supervisor(void *pv) {
    // ESPNOW init + peer ekle (bir kere)
    esp_err_t e = esp_now_tx_init_once();
    if (e != ESP_OK) {
        ESP_LOGE("SUP", "esp_now_tx_init_once: %s", esp_err_to_name(e));
        vTaskDelete(NULL);
    }
    e = espnow_tx_add_peer(PEER_MAC);
    if (e != ESP_OK && e != ESP_ERR_ESPNOW_EXIST) ESP_ERROR_CHECK(e);

    // ilk boot’ta sensörlerin değer üretmesi için kısa bir ısınma
    vTaskDelay(pdMS_TO_TICKS(6000));

    for (;;) {
        // 1) snapshot al (yırtılmayı önlemek için kopya alıyoruz)
        measurements_t snap = *data_handler_get();   // senin getter’in pointer döndürüyor

        // 2) gonder
        ESP_ERROR_CHECK(esp_now_send_measurements(PEER_MAC)); 
        // Not: Yukarıdaki fonksiyon global state’i gönderiyor.
        // İstersen bunun bir "snapshot" alan versiyonunu da yazabilirsin:
        // esp_now_send_measurements_snapshot(PEER_MAC, &snap);

        // 3) ESPNOW airtime/callback icin minicik bekleme
        vTaskDelay(pdMS_TO_TICKS(50));

        // 4) 5 dakika deep sleep
        const uint64_t uS_5MIN = 1ULL * 60ULL * 1000000ULL;
        ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(uS_5MIN));
        ESP_LOGI(TAG, "sleeping for 5 min...");
        esp_deep_sleep_start();
        // buradan sonrası calismaz; uyaninca tum sistem yeniden baslar
    }
}

void initialize_sensors() {
    i2c_init();

    esp_err_t ret = sen55_init();
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "SEN55 initialization failed");
    } else {
        sen55_soft_reset();
        sen55_start_measurement();
    }

    bh1750_init();
    ina219_init();
}


void app_main(void)
{
    

    i2c_init();


    // // 1. Sensör init
    sen55_init();

    bh1750_init();
    ina219_init();

    sen55_soft_reset();
    sen55_start_measurement();

    // // 2. Task’lar
    xTaskCreate(&vTask_sen55_read_measurement, "SEN55 Read Task", 4096, NULL , 5, NULL);  
    xTaskCreate(&vTask_bh1750_read_light_level, "BH1750 Read Task", 4096, NULL , 5, NULL); 
    xTaskCreate(&vTask_ina219_read_current, "INA219 Read Task", 4096, NULL , 5 , NULL);

    // 3) supervisor
    xTaskCreate(vTask_supervisor, "SUP", 4096 * 2, NULL, 6, NULL);



}
