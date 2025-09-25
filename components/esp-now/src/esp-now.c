#include "esp-now.h"


static const char *TAG = "espnow_tx";
static bool is_init = false;

static esp_now_send_cb_t send_cb = NULL;

static void _default_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (!mac_addr) return;
    ESP_LOGI(TAG, "send -> %02X:%02X:%02X:%02X:%02X:%02X %s",
             mac_addr[0], mac_addr[1], mac_addr[2],
             mac_addr[3], mac_addr[4], mac_addr[5],
             status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

esp_err_t esp_now_tx_init(void){
    esp_err_t err;
    if (is_init) {
        ESP_LOGW(TAG, "ESP-NOW is already initialized");
        return ESP_OK;
    }
    err = wifi_init_sta_fixed_channel();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WiFi in station mode with fixed channel: %s", esp_err_to_name(err));
        return err;
    }

    ESP_RETURN_ON_ERROR(esp_now_init(), TAG, "Failed to initialize ESP-NOW");

    esp_now_register_send_cb((esp_now_send_cb_t)_default_send_cb);
    return ESP_OK;



}


esp_err_t espnow_tx_add_peer(const uint8_t peer_mac[6]) {
    if (esp_now_is_peer_exist(peer_mac)) return ESP_OK;

    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, peer_mac, 6);
    peer.channel = CONFIG_ESP_NOW_CHANNEL;
    peer.ifidx   = WIFI_IF_STA;
    peer.encrypt = false;   // şifreleme yok!

    return esp_now_add_peer(&peer);
}

esp_err_t espnow_simple_send(const uint8_t peer_mac[6], const uint8_t *data, int len) {
    if (!peer_mac || !data || len <= 0) return ESP_ERR_INVALID_ARG;
    if (len > 250) return ESP_ERR_INVALID_SIZE;  // limit

    return esp_now_send(peer_mac, data, len);
}

esp_err_t esp_now_send_measurements(const uint8_t peer_mac[6]) {
    if (!peer_mac) return ESP_ERR_INVALID_ARG;

    const measurements_t *m = data_handler_get();   // global state pointer
    if (!m) return ESP_ERR_INVALID_STATE;

    static bool s_inited = false;
    if (!s_inited) {
        esp_err_t err = esp_now_tx_init();          // idempotent olmasına dikkat
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_now init fail: %s", esp_err_to_name(err));
            return err;
        }
        s_inited = true;
    }

    // Peer ekli değilse ekle (ekli ise tekrar eklemek ESP_ERR_ESPNOW_EXIST döndürebilir, sorun değil)
    esp_err_t err = espnow_tx_add_peer(peer_mac);
    if (err != ESP_OK && err != ESP_ERR_ESPNOW_EXIST) {
        ESP_LOGE(TAG, "add peer fail: %s", esp_err_to_name(err));
        return err;
    }

    // Binary ölçüm blokunu doğrudan gönder
    return espnow_simple_send(peer_mac, (const uint8_t*)m, sizeof(*m));
}

esp_err_t esp_now_tx_init_once()
{
    static bool s_inited = false;
    if (s_inited) return ESP_OK;

    // Wi-Fi (STA + channel) hazırla (idempotent sarmalayıcı)
    ESP_RETURN_ON_ERROR(wifi_init_sta_fixed_channel(), TAG, "wifi init fail");

    esp_err_t e = esp_now_init();
    if (e != ESP_OK && e != ESP_ERR_ESPNOW_EXIST) return e;

    s_inited = true;
    return ESP_OK;
}