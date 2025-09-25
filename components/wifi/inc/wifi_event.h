#ifndef WIFI_EVENT_H
#define WIFI_EVENT_H

#include <string.h>
#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_mac.h"



/*
@brief Initialize WiFi in station mode
    * @note Initialize WiFi in station mode
    * @param ssid: SSID of the WiFi network
    * @param password: Password of the WiFi network
    * @return None
*/

void wifi_init_sta(void);



/*@brief Deinitialize WiFi
    * @note Deinitialize WiFi
    * @return None
    * */
void wifi_deinit(void);



/*
@brief wifi statition init with fixed channel
    * @note Initialize WiFi in station mode with fixed channel
    * @return None

*/

esp_err_t wifi_init_sta_fixed_channel(void);

#endif // WIFI_EVENT_H