#include "sen55.h"
#include "esp_err.h"

static const char *TAG = "SEN55";

i2c_master_dev_handle_t sen55_handle;

esp_err_t sen55_init(){
    ESP_LOGI(TAG , "Initializing SEN55 sensor...");

        i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SEN55_I2C_ADDRESS,
        .scl_speed_hz = 100000,
    };

        // Add the SEN55 device to the I2C bus
        esp_err_t ret = i2c_add_bus_device(&sen55_handle, &dev_cfg);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to add SEN55 device to I2C bus: %s", esp_err_to_name(ret));
            return ret;
        }

        ESP_LOGI(TAG, "SEN55 sensor initialized successfully.");
    return ESP_OK;
}

    
esp_err_t sen55_read_device_status(void){
    uint8_t status_data[4];
    uint8_t cmd[2] = { (SEN55_READ_DEVICE_STATUS >> 8) & 0xFF, SEN55_READ_DEVICE_STATUS & 0xFF };
    esp_err_t ret = i2c_master_transmit_receive(sen55_handle , cmd , sizeof(cmd) , status_data , 4 , 1000);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read device status: %s", esp_err_to_name(ret));
        return ret;
    }

    uint32_t status = (status_data[0] << 24) | (status_data[1] << 16) | (status_data[2] << 8) | status_data[3];
    ESP_LOGI(TAG, "Device Status: 0x%08X", status);
    return ESP_OK;


}

