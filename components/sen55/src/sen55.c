#include "sen55.h"
#include "esp_err.h"

static const char *TAG = "SEN55";

i2c_master_dev_handle_t sen55_handle;
sen55_sensor_res_t sen55_data_inc_pm;

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

esp_err_t sen55_soft_reset(void){
    uint8_t cmd[2] = { 0xD3, 0x04 }; // Soft reset command
    esp_err_t ret = i2c_master_transmit(sen55_handle , cmd , sizeof(cmd) , -1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to perform soft reset: %s", esp_err_to_name(ret));
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(200)); // Wait for 200ms after reset
    ESP_LOGI(TAG, "Soft reset performed successfully.");
    return ESP_OK;
}
    
esp_err_t sen55_read_device_status(void){
    uint8_t status_data[4];
    uint8_t cmd[2] = { (SEN55_READ_DEVICE_STATUS >> 8) & 0xFF, SEN55_READ_DEVICE_STATUS & 0xFF };
    esp_err_t ret = i2c_master_transmit(sen55_handle , cmd , sizeof(cmd) , -1);

    vTaskDelay(pdMS_TO_TICKS(20)); // Wait for 10ms


    ret = i2c_master_receive(sen55_handle , status_data , sizeof(status_data) , -1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read device status: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}



esp_err_t sen55_start_measurement(void){

    uint8_t cmd[2] = { (SEN55_CMD_START_MEASUREMENT >> 8) & 0xFF, SEN55_CMD_START_MEASUREMENT & 0xFF };
    esp_err_t ret = i2c_master_transmit(sen55_handle , cmd , sizeof(cmd) , -1);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start measurement: %s", esp_err_to_name(ret));
        return ret;
    }

    // Wait for the measurement to complete
    vTaskDelay(pdMS_TO_TICKS(50)); // Wait for 50ms as

    ESP_LOGI(TAG, "Measurement started successfully.");
    return ESP_OK;
}

esp_err_t sen55_stop_measurement(void){
    uint8_t cmd[2] = { (SEN55_CMD_STOP_MEASUREMENT >> 8) & 0xFF, SEN55_CMD_STOP_MEASUREMENT & 0xFF };
    esp_err_t ret = i2c_master_transmit(sen55_handle , cmd , sizeof(cmd) , -1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop measurement: %s", esp_err_to_name(ret));
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(200)); // Wait for 200ms after stopping measurement
    ESP_LOGI(TAG, "Measurement stopped successfully.");
    return ESP_OK;
}




void vTask_sen55_read_measurement(void *pvParameters){
    measurements_t *meas = data_handler_get();

    uint8_t measurement_data[24]; 
    uint8_t cmd[2] = { (SEN55_READ_MEASUREMENT >> 8) & 0xFF, SEN55_READ_MEASUREMENT & 0xFF };
    esp_err_t ret;
    


    while(1){
        vTaskDelay(pdMS_TO_TICKS(5000)); // Read every 5 seconds
        ret= i2c_master_transmit(sen55_handle , cmd , sizeof(cmd) , -1);

        vTaskDelay(pdMS_TO_TICKS(20)); // Wait for 20ms

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to send read measurement command: %s", esp_err_to_name(ret));
        }

        ret = i2c_master_receive(sen55_handle , measurement_data , sizeof(measurement_data) , -1);



        sen55_data_inc_pm.pm1_0 = (measurement_data[0] << 8 | measurement_data[1]) / 10;
        sen55_data_inc_pm.pm2_5 = (measurement_data[3] << 8 | measurement_data[4])/10;
        sen55_data_inc_pm.pm4_0 = (measurement_data[6] << 8 | measurement_data[7])/10;
        sen55_data_inc_pm.pm10_0 = (measurement_data[9] << 8 | measurement_data[10])/10;
        sen55_data_inc_pm.RH = (measurement_data[12] << 8 | measurement_data[13])/100;
        sen55_data_inc_pm.T = (measurement_data[15] << 8 | measurement_data[16])/200;
        sen55_data_inc_pm.VOC = (measurement_data[18] << 8 | measurement_data[19])/10;
        sen55_data_inc_pm.NOx = (measurement_data[21] << 8 | measurement_data[22])/10;

        meas->pm2_5 = sen55_data_inc_pm.pm2_5;
        meas->pm10 = sen55_data_inc_pm.pm10_0;
        meas->voc = sen55_data_inc_pm.VOC;
        meas->temperature = sen55_data_inc_pm.T;
        meas->humidity = sen55_data_inc_pm.RH;

        
        ESP_LOGI(TAG, "PM1.0: %d µg/m³", sen55_data_inc_pm.pm1_0);
        ESP_LOGI(TAG, "PM2.5: %d µg/m³", sen55_data_inc_pm.pm2_5);
        ESP_LOGI(TAG, "PM4.0: %d µg/m³", sen55_data_inc_pm.pm4_0);
        ESP_LOGI(TAG, "PM10: %d µg/m³", sen55_data_inc_pm.pm10_0);
        ESP_LOGI(TAG, "Relative Humidity: %d %%RH", sen55_data_inc_pm.RH);
        ESP_LOGI(TAG, "Temperature: %d °C", sen55_data_inc_pm.T);
        ESP_LOGI(TAG, "VOC: %d ppb", sen55_data_inc_pm.VOC);
        ESP_LOGI(TAG, "NOx: %d ppb", sen55_data_inc_pm.NOx);


    }

}


