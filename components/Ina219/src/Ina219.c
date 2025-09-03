#include "ina219.h"
#include "i2c_handler.h"

i2c_master_dev_handle_t ina_dev;
// device configuration
i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = INA219_ADDR,
    .scl_speed_hz = 100000,
};




static const char *TAG = "INA219";

esp_err_t write_u16(i2c_master_dev_handle_t dev, uint8_t reg, uint16_t val_be)
{
    uint8_t buf[3];
    buf[0] = reg;
    buf[1] = (uint8_t)(val_be >> 8); // MSB
    buf[2] = (uint8_t)(val_be & 0xFF); // LSB
    return i2c_master_transmit(dev, buf, sizeof(buf),
                            1000 / portTICK_PERIOD_MS);
}

esp_err_t read_u16(i2c_master_dev_handle_t dev, uint8_t reg, uint16_t *out)
{
    esp_err_t err = i2c_master_transmit(dev, &reg, 1,
                            1000 / portTICK_PERIOD_MS);

    if (err != ESP_OK) return err;
    uint8_t rx[2];
    err = i2c_master_receive(dev, rx, 2, 1000 / portTICK_PERIOD_MS);
    if (err != ESP_OK) return err;
    *out = ((uint16_t)rx[0] << 8) | rx[1]; // MSB:rx[0], LSB:rx[1]
    return ESP_OK;
}

esp_err_t ina219_init(void)
{
    CALIB_VALUE = (uint16_t)trunc(0.04096f / (CURRENT_LSB_A * SHUNT_RES_OHMS));

    esp_err_t ret = i2c_add_bus_device(&ina_dev, &dev_cfg);
    if (ret != ESP_OK) return ret;


    ESP_LOGI(TAG, "CALIB_VALUE: %u", CALIB_VALUE);

    // Kalibrasyonu yaz
    ret = write_u16(ina_dev, INA219_REG_CALIB, CALIB_VALUE);
    if (ret != ESP_OK) return ret;

    // Config yaz
    ret = write_u16(ina_dev, INA219_REG_CONFIG, INA219_CONFIG_VALUE);
    if (ret != ESP_OK) return ret;

    return ESP_OK;
}


void vTask_ina219_read_current(void *pvParameters)
{
    float *current = (float *)pvParameters;
    uint16_t raw_current;
    esp_err_t ret;
    int16_t signed_current;
    
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 saniye bekle

        ret = read_u16(ina_dev, INA219_REG_CURRENT, &raw_current);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read current: %s", esp_err_to_name(ret));
            continue;
        }

        signed_current = (int16_t)raw_current; 
        
        // Hesapla
        // 1 bit = 100 µA, CALIB_VALUE = 0.04096 / (CURRENT_LSB_A * SHUNT_RES_OHMS)
        // 100 µA = 0.0001 A

        *current = (float)signed_current /(1000*100);
        ESP_LOGI(TAG, "Current: %.6f A", *current);
    }
 

}

