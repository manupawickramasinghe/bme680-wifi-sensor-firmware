idf_component_register(SRCS "bme680.c" "bme680_platform.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp8266_wrapper)