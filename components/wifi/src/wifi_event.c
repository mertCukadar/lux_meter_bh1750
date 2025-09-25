#include "wifi_event.h"

static const char *TAG = "WIFI_EVENT";

// Event handler fonksiyonu
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data) {
    // Wi-Fi event’leri
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "Wi-Fi STA started, trying to connect...");
                esp_wifi_connect();   // bağlanmayı deniyoruz
                break;

            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "Connected to AP");
                break;

            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGW(TAG, "Disconnected, retrying...");
                esp_wifi_connect();   // tekrar bağlan
                break;

            default:
                ESP_LOGI(TAG, "Other Wi-Fi event: %d", event_id);
                break;
        }
    }

    // IP event’leri
    else if (event_base == IP_EVENT) {
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP: {
                ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
                ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
                // Burada MQTT başlatabilirsin
                break;
            }

            default:
                ESP_LOGI(TAG, "Other IP event: %d", event_id);
                break;
        }
    }
}

void wifi_init_sta(void) {
    // 1. NVS init (Wi-Fi driver kalıcı ayarları buraya yazar)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // 2. Netif ve Event loop init
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    // 3. Wi-Fi driver init
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // 4. Event handler register
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                        &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler, NULL, NULL);

    // 5. Wi-Fi ayarlarını yap
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,       // menuconfig’den gelir
            .password = CONFIG_WIFI_PASSWORD,   // menuconfig’den gelir
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

    // 6. Wi-Fi başlat
    esp_wifi_start();
    ESP_LOGI(TAG, "wifi_init_sta finished.");

}

// wifi_event.c
esp_err_t wifi_init_sta_fixed_channel()
{
    esp_err_t err;

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK) return err;

    err = esp_netif_init();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return err;

    // default event loop zaten varsa INVALID_STATE doner → sorun değil
    err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return err;

    // STA netif'i bir kere oluştur
    static bool s_netif_created = false;
    if (!s_netif_created) {
        esp_netif_create_default_wifi_sta();
        s_netif_created = true;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (err != ESP_OK && err != ESP_ERR_WIFI_INIT_STATE) return err;

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) return err;

    // wifi_start ikinci kez cagirilirsa ESP_ERR_WIFI_CONN exist gelmez, guvenle cagirilir
    err = esp_wifi_start();
    if (err != ESP_OK && err != ESP_ERR_WIFI_CONN) return err;

    // kanalı start'tan sonra sabitle
    err = esp_wifi_set_channel(CONFIG_ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
    if (err != ESP_OK) return err;

    uint8_t self[6];
    esp_read_mac(self, ESP_MAC_WIFI_STA);
    ESP_LOGI(TAG, "RX STA MAC: %02X:%02X:%02X:%02X:%02X:%02X (ch=%d)",
             self[0], self[1], self[2], self[3], self[4], self[5], CONFIG_ESP_NOW_CHANNEL);
    return ESP_OK;
}
