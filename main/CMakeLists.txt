idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES bh1750 i2c_handler esp_driver_i2c driver sen55)