#ifndef SEN55_H
#define SEN55_H

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "i2c_handler.h"
#include "esp_err.h"
#include "data_handler.h"



#define SEN55_I2C_ADDRESS 0x69  // I2C address for SEN55 sensor
#define SEN55_CMD_START_MEASUREMENT 0x0021 // Command to start measurement wait 50ms
#define SEN55_CMD_STOP_MEASUREMENT 0x0104 // Command to stop measurement wait 200ms
#define SEN55_START_MEASUREMENT_RHT_GAS 0x0037 // Command to start measurement of RHT and Gas wait 20ms
#define SEN55_DATA_READY_FLAG 0x0202 // Command to check if data is ready wait 1ms
#define SEN55_READ_MEASUREMENT 0x03C4 // Command to read measurement data wait 1ms

#define SEN55_START_FAN_CLEANING 0x5607 // Command to start fan cleaning wait 25000ms
#define SEN55_READ_DEVICE_STATUS 0xD206 // Command to read device status wait 1ms


typedef struct sen55_sensor_res_t {
    uint16_t pm1_0;      // PM1.0 concentration in µg/m³
    uint16_t pm2_5;      // PM2.5 concentration in µg/m³
    uint16_t pm4_0;      // PM4.0 concentration in µg/m³
    uint16_t pm10_0;     // PM10 concentration in µg/m³
    uint16_t RH;         // Relative Humidity in %RH
    uint16_t T;          // Temperature in °C
    uint16_t VOC;        // Volatile Organic Compounds in ppb
    uint16_t NOx;        // Nitrogen Oxides in ppb
 } sen55_sensor_res_t;




/*
@brief Initialize the SEN55 sensor.
@param sen55_dev Pointer to the I2C device handle for the SEN55 sensor.
@return ESP_OK on success, or an error code on failure.
*/

esp_err_t sen55_init();


esp_err_t sen55_start_measurement(void);

esp_err_t sen55_stop_measurement(void);

void vTask_sen55_read_measurement(void *pvParameters);

esp_err_t sen55_check_data_ready(void);

esp_err_t sen55_start_fan_cleaning(void);

esp_err_t sen55_read_device_status(void);

esp_err_t sen55_soft_reset(void);


#endif // SEN55_H
