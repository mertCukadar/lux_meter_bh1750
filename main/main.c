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
float light_level = 0.0;

i2c_init();

// bh1750_init();

//     while (1) {
//         if (bh1750_read_light_level(&light_level) == ESP_OK) {
//             ESP_LOGI("MAIN", "Light Level: %f lx", light_level);
//         } else {
//             ESP_LOGE("MAIN", "Failed to read light level");
//         }
//         vTaskDelay(pdMS_TO_TICKS(2000)); // Delay for 2 seconds
//     }


sen55_init();
sen55_read_device_status();

}