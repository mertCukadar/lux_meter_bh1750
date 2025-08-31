#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "bh1750.h"
#include "i2c_handler.h"
#include "sen55.h"



void app_main(void)
{

    i2c_init();

    sen55_init();

    sen55_soft_reset();

    sen55_start_measurement();
    ESP_LOGI("MAIN", "Measurement started successfully.");


    while(1){
        vTaskDelay(pdMS_TO_TICKS(5000)); // Read every 5 seconds
        esp_err_t ret = sen55_read_measurement();
        if (ret != ESP_OK) {
            ESP_LOGE("MAIN", "Failed to read measurement data: %s", esp_err_to_name(ret));
        }
}
    


}
