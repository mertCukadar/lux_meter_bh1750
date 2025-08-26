#include "bh1750.h"
#include "esp_err.h"
#include "esp_log.h"


static const char *TAG = "BH1750";

i2c_master_dev_handle_t bh1750_handle;

esp_err_t bh1750_init(){
ESP_LOGI(TAG , "Initializing BH1750 sensor...");

    i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = BH1750_I2C_ADDRESS,
    .scl_speed_hz = 100000,
};


    // Add the BH1750 device to the I2C bus
    esp_err_t ret = i2c_add_bus_device(&bh1750_handle, &dev_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add BH1750 device to I2C bus: %s", esp_err_to_name(ret));
        return ret;
    }

    uint8_t power_on_cmd = BH1750_CMD_POWER_ON;
    ret = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit(bh1750_handle, &power_on_cmd, 1, -1));

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to power on BH1750 sensor: %s", esp_err_to_name(ret));
        return ret;
    }


    ESP_LOGI(TAG, "BH1750 sensor initialized successfully.");
    return ESP_OK;

}


esp_err_t bh1750_read_light_level(float *lux) {
    if (lux == NULL) {
        ESP_LOGE(TAG, "Invalid argument: light_level pointer is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t cmd = BH1750_CMD_H_RES_MODE;
    esp_err_t ret = i2c_master_transmit(bh1750_handle, &cmd, 1, -1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send measurement command to BH1750: %s", esp_err_to_name(ret));
        return ret;
    }

    uint8_t data[2];
    ret = i2c_master_receive(bh1750_handle, data, 2, -1);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read data from BH1750: %s", esp_err_to_name(ret));
        return ret;
    }

    // print data to terminal for debugging
    ESP_LOGI(TAG, "Raw data from BH1750: 0x%02X 0x%02X", data[0], data[1]);


    uint16_t raw_value = (data[0] << 8) | data[1];
    
    *lux = ((float)(raw_value)) / 1.2; // Convert to lux





    return ESP_OK;
}