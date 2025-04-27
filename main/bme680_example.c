#include "bme680.h"
#include "smartconfig.h"
#include "mqtt_handler.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_timer.h"

#define TASK_STACK_DEPTH 2048

#define I2C_MASTER_SCL_IO           7      // GPIO 7 for I2C SCL
#define I2C_MASTER_SDA_IO           6      // GPIO 6 for I2C SDA
#define I2C_MASTER_NUM              0      // I2C master i2c port number
#define I2C_MASTER_FREQ_HZ          100000 // I2C master clock frequency
#define I2C_MASTER_TX_BUF_DISABLE   0      // I2C master doesn't need buffer
#define I2C_MASTER_RX_BUF_DISABLE   0      // I2C master doesn't need buffer

static bme680_sensor_t* sensor = 0;

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

/*
 * User task that triggers measurements of sensor every seconds. It uses
 * function *vTaskDelay* to wait for measurement results. Busy wating
 * alternative is shown in comments
 */
void user_task(void *pvParameters)
{
    bme680_values_float_t values;
    char mqtt_data[128];
    TickType_t last_wakeup = xTaskGetTickCount();
    uint32_t duration = bme680_get_measurement_duration(sensor);
    esp_mqtt_client_handle_t mqtt_client = get_mqtt_client();

    while (1)
    {
        if (bme680_force_measurement(sensor))
        {
            vTaskDelay(duration);

            if (bme680_get_results_float(sensor, &values)) {
                // Format sensor data as JSON
                snprintf(mqtt_data, sizeof(mqtt_data), 
                        "{\"temperature\":%.2f,\"humidity\":%.2f,\"pressure\":%.2f,\"gas\":%.2f}",
                        values.temperature, values.humidity, values.pressure, values.gas_resistance);
                
                // Publish to MQTT if client is available
                if (mqtt_client != NULL) {
                    esp_mqtt_client_publish(mqtt_client, "/sensor/bme680", mqtt_data, 0, 1, 0);
                }
                
                // Also print to console
                printf("%.3f BME680 Sensor: %.2f °C, %.2f %%, %.2f hPa, %.2f Ohm\n",
                       (double)esp_timer_get_time()*1e-6,
                       values.temperature, values.humidity,
                       values.pressure, values.gas_resistance);
            }
        }
        vTaskDelayUntil(&last_wakeup, 1000 / portTICK_PERIOD_MS);
    }
}


void app_main(void)
{
    // Initialize UART
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
    
    /** -- MANDATORY PART -- */

    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();

    // Initialize MQTT after WiFi
    mqtt_init();

    // Initialize I2C
    i2c_master_init();

    // init the sensor with slave address BME680_I2C_ADDRESS_2
    sensor = bme680_init_sensor(I2C_MASTER_NUM, BME680_I2C_ADDRESS_2, 0);

    if (sensor)
    {
        /** -- SENSOR CONFIGURATION PART (optional) --- */

        // Changes the oversampling rates to 4x oversampling for temperature
        // and 2x oversampling for humidity. Pressure measurement is skipped.
        bme680_set_oversampling_rates(sensor, osr_4x, osr_4x, osr_2x);

        // Change the IIR filter size for temperature and pressure to 7.
        bme680_set_filter_size(sensor, iir_size_7);

        // Change the heater profile 0 to 200 degree Celcius for 100 ms.
        bme680_set_heater_profile (sensor, 0, 200, 100);
        bme680_use_heater_profile (sensor, 0);

        // Set ambient temperature to 25 degree Celsius
        bme680_set_ambient_temperature (sensor, 25);
            
        xTaskCreate(user_task, "user_task", TASK_STACK_DEPTH, NULL, 2, NULL);
    }
    else
    {
        ESP_LOGE("BME680", "Could not initialize BME680 sensor");
    }
}
