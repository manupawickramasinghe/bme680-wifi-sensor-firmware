idf_component_register(SRCS "smartconfig.c" "bme680_example.c"
                    INCLUDE_DIRS "."
                    REQUIRES "bme680" "esp_wifi" "nvs_flash" "esp_netif" "esp_event" "wpa_supplicant")