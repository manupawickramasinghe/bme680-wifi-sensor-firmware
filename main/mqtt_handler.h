#ifndef _MQTT_HANDLER_H_
#define _MQTT_HANDLER_H_

#include "esp_system.h"
#include "mqtt_client.h"
#include "esp_timer.h"

// Function declarations
void mqtt_init(void);
esp_mqtt_client_handle_t get_mqtt_client(void);

#endif /* _MQTT_HANDLER_H_ */