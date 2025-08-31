#ifndef INA219_H_
#define INA219_H_

#include <stdint.h>

#include <math.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c_types.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "i2c_handler.h"

/* Register addresses */
static const uint8_t INA219_ADDR        = 0x40;
static const uint8_t INA219_REG_CONFIG  = 0x00;
static const uint8_t INA219_REG_SHUNT_V = 0x01;
static const uint8_t INA219_REG_BUS_V   = 0x02;
static const uint8_t INA219_REG_POWER   = 0x03;
static const uint8_t INA219_REG_CURRENT = 0x04;
static const uint8_t INA219_REG_CALIB   = 0x05;

/* Config bits */
static const uint16_t CFG_BRNG_32V          = (1u << 13);
static const uint16_t CFG_PGA_320MV         = (3u << 11);  // gain /8, 320mV
static const uint16_t INA219_CFG_BADC_12BIT = (0x9u << 7);
static const uint16_t INA219_CFG_SADC_12BIT = (0x9u << 3);
static const uint16_t CFG_MODE_SHUNT_BUS_CONT = 0x07u;

static const uint16_t INA219_CONFIG_VALUE =
    (CFG_BRNG_32V | CFG_PGA_320MV |
     INA219_CFG_BADC_12BIT | INA219_CFG_SADC_12BIT |
     CFG_MODE_SHUNT_BUS_CONT);

/* Calibration */
// static const float SHUNT_RES_OHMS = 0.01f;

// static uint16_t CALIB_VALUE;


static const float SHUNT_RES_OHMS = 0.01f;
static const float CURRENT_LSB_A  = 0.001f; // 1mA
// static const float CURRENT_LSB_A = 0.04096f / (CALIB_VALUE * SHUNT_RES_OHMS);
static uint16_t CALIB_VALUE;  

// static const float CALIB_VALUE =
//     0.04096f / (CURRENT_LSB_A * SHUNT_RES_OHMS);




/* Prototypes */
esp_err_t write_u16(i2c_master_dev_handle_t dev, uint8_t reg, uint16_t val_be);
esp_err_t read_u16(i2c_master_dev_handle_t dev, uint8_t reg, uint16_t *out);
esp_err_t ina219_init(void);
esp_err_t ina219_read_current(float *current);






#endif /* INA219_H_ */