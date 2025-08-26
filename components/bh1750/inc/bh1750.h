#ifndef BH1750_H
#define BH1750_H

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_handler.h"


#define BH1750_I2C_ADDRESS 0x23  // Default I2C address for BH1750
#define BH1750_CMD_POWER_ON 0x01
#define BH1750_CMD_POWER_OFF 0x00
#define BH1750_CMD_H_RES_MODE 0x10



esp_err_t bh1750_init();

esp_err_t bh1750_read_light_level(float *lux);





#endif // BH1750_H