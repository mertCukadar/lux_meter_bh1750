set(SRC_DIRS src)
set(INCLUDE_DIRS inc .)
set(REQUIRES i2c_handler driver esp_driver_i2c esp_wifi esp_event nvs_flash)

idf_component_register(SRC_DIRS ${SRC_DIRS}
                    INCLUDE_DIRS ${INCLUDE_DIRS}
                    REQUIRES ${REQUIRES})
