#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <esp_err.h>
#include <stdint.h>


typedef struct {
    float voc , co2 , temperature, humidity, pm2_5, pm10;
    float lux;
    float current;
} measurements_t;

/**
 * @brief Ölçüm değerlerini set et
 * @param new_data struct pointer
 * @return ESP_OK veya hata
 */
esp_err_t data_handler_set(const measurements_t *new_data);

/**
 * @brief Ölçüm değerlerini al
 * @return measurements_t pointer (dahili struct’a işaret eder)
 */
measurements_t* data_handler_get(void);

#endif // DATA_HANDLER_H