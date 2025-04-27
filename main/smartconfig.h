#ifndef _SMARTCONFIG_H_
#define _SMARTCONFIG_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"
#include "nvs_flash.h"
#include "smartconfig_ack.h"

void initialise_wifi(void);

#endif /* _SMARTCONFIG_H_ */
