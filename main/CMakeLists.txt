idf_component_register(SRCS "smartconfig.c" "bme680_example.c" "mqtt_handler.c"
                    INCLUDE_DIRS "."
                    REQUIRES "bme680" "esp_wifi" "nvs_flash" "esp_netif" "driver" "mqtt")