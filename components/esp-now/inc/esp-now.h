#ifndef ESP_NOW_TX_H
#define ESP_NOW_TX_H

#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include "esp_err.h"
#include "esp_check.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "wifi_event.h"
#include "data_handler.h"  // measurements_t


esp_err_t esp_now_tx_init(void);

//esp_err_t espnow_tx_add_peer(const uint8_t *peer_addr, bool is_encrypt);

esp_err_t espnow_tx_add_peer(const uint8_t peer_mac[6]);

esp_err_t esp_now_del_peer(const uint8_t *peer_addr);

esp_err_t esp_now_send_raw(const uint8_t *peer_addr,
                    const uint8_t *data, size_t len);

esp_err_t espnow_tx_send_struct(const uint8_t peer_mac[6],
                                const void *packed_struct,
                                size_t struct_size);

void espnow_tx_register_send_cb(esp_now_send_cb_t cb);


esp_err_t espnow_simple_send(const uint8_t peer_mac[6], const uint8_t *data, int len);

esp_err_t esp_now_send_measurements(const uint8_t peer_mac[6]);


esp_err_t esp_now_tx_init_once(void);

#endif // ESP_NOW_H