#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "bh1750.h"
#include "i2c_handler.h"
#include "sen55.h"
#include "Ina219.h"



void app_main(void)
{
    // Initialize I2C bus and sensors
    i2c_init();
    sen55_init();
    bh1750_init();
    ina219_init();

    // sen55 sensor setup
    sen55_soft_reset();
    sen55_start_measurement();


    // Create tasks for reading sensor data
    xTaskCreatePinnedToCore(&vTask_sen55_read_measurement, "SEN55 Read Task", 4096, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(&vTask_bh1750_read_light_level, "BH1750 Read Task", 2048, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(&vTask_ina219_read_current, "INA219 Read Task", 2048, NULL, 5, NULL, 1);

    
  


}
    


