# BME680 WiFi Sensor Firmware

This repository contains the firmware for an ESP32-based environmental sensor utilizing the Bosch BME680 sensor. The device connects to a WiFi network and publishes sensor data (temperature, humidity, pressure, and gas resistance) to an MQTT broker. It also subscribes to a command topic to receive instructions.

## Features

-   **Environmental Sensing**: Accurately measures ambient temperature, relative humidity, barometric pressure, and volatile organic compounds (VOCs) via the BME680 sensor.
-   **WiFi Connectivity**: Connects to your local WiFi network using saved credentials, ensuring seamless integration into your smart home or IoT setup.
-   **MQTT Data Publishing**: Publishes real-time sensor readings to a configurable MQTT broker. Data is formatted as JSON for easy parsing by other applications or dashboards.
-   **MQTT Command Subscription**: Subscribes to a specific MQTT topic to receive commands, allowing for remote control or configuration updates (e.g., changing sampling rate, triggering actions).
-   **Real-time Sensor Readings**: Provides continuous updates of environmental parameters, enabling dynamic monitoring and automation.
-   **Configurable Sampling Rate**: The frequency of sensor readings and MQTT publishing can be adjusted to optimize power consumption and data granularity.
-   **Error Handling & Reconnection**: Robust handling for WiFi and MQTT connection errors, including automatic reconnection attempts to maintain continuous operation.

## Hardware Requirements

To build and run this project, you will need the following hardware components:

-   **ESP32 Development Board**: Any standard ESP32 development board (e.g., ESP32-DevKitC, NodeMCU-32S) with sufficient GPIO pins for I2C communication.
-   **BME680 Sensor Module**: A BME680 breakout board. Ensure it has pull-up resistors on the I2C lines if your development board does not provide them.
-   **Power Supply**: A 5V power supply for the ESP32 board (e.g., USB cable, external power adapter).
-   **Connecting Wires**: Jumper wires to connect the BME680 sensor to the ESP32 board via I2C.

### Wiring Diagram (Example)

| BME680 Pin | ESP32 Pin (Typical) |
| :--------- | :------------------ |
| VCC        | 3.3V                |
| GND        | GND                 |
| SDA        | GPIO 21             |
| SCL        | GPIO 22             |

*Note: GPIO pins for SDA/SCL might vary depending on your ESP32 board. Refer to your board's pinout diagram.*

## Software Dependencies

This project is built using the ESP-IDF framework. Ensure you have the following installed:

-   **ESP-IDF Framework**: Version 4.x or newer is recommended. Follow the official [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) for installation instructions.
-   **BME680 Driver**: This project integrates a BME680 driver. Ensure it's compatible with your ESP-IDF version.
-   **MQTT Client Library**: The ESP-IDF includes an MQTT client component (`esp-mqtt`).

## Getting Started

Follow these steps to get your BME680 WiFi Sensor up and running:

1.  **Clone this repository**:
    ```bash
    git clone https://github.com/manupawickramasinghe/bme680-wifi-sensor-firmware.git
    cd bme680-wifi-sensor-firmware
    ```

2.  **Install ESP-IDF**:
    If you haven't already, install the ESP-IDF framework by following the instructions in the [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html). This includes setting up the toolchain and environment variables.

3.  **Configure WiFi and MQTT credentials**:
    Navigate to the project directory and run the ESP-IDF configuration tool:
    ```bash
    idf.py menuconfig
    ```
    In `menuconfig`:
    *   Go to `Component config` -> `Wi-Fi` and configure your WiFi SSID and Password.
    *   Go to `Component config` -> `MQTT` and configure your MQTT broker URI (e.g., `mqtt://broker.hivemq.com:1883`).
    *   You might also find options to configure sensor-specific parameters or MQTT topics under custom component configurations if added.

4.  **Build the firmware**:
    ```bash
    idf.py build
    ```

5.  **Flash the firmware to your ESP32**:
    Connect your ESP32 board to your computer via USB. Ensure you know the serial port name (e.g., `/dev/ttyUSB0` on Linux/macOS, `COMx` on Windows).
    ```bash
    idf.py -p <PORT> flash
    ```
    Replace `<PORT>` with your ESP32's serial port.

6.  **Monitor sensor data via MQTT**:
    After flashing, the ESP32 will connect to WiFi and start publishing data. You can monitor the serial output for logs:
    ```bash
    idf.py -p <PORT> monitor
    ```
    To view the MQTT data, use an MQTT client (e.g., MQTT Explorer, Mosquitto client) and subscribe to the configured topics.

## Configuration

The project's configuration is primarily handled via `idf.py menuconfig`. Key parameters include:

-   **WiFi Credentials**: SSID and password for your network.
-   **MQTT Broker URI**: The address of your MQTT broker (e.g., `mqtt://broker.hivemq.com:1883`).
-   **MQTT Topics**:
    *   **Publish Topic**: `/zektopic/sensor/bme680/data` (default, can be changed in code)
    *   **Subscribe Topic**: `/zektopic/sensor/bme680/command` (default, can be changed in code)
-   **Sensor Reading Interval**: The delay between sensor readings and data publishing (e.g., 5 seconds). This is typically defined in `main.c` or a configuration header.

## MQTT Topics and Data Format

### Published Data Topic

The sensor publishes data to `/zektopic/sensor/bme680/data` (or a similar configured topic). The data is sent as a JSON string.

**Example JSON Payload:**

```json
{
  "temperature_c": 25.50,
  "humidity_percent": 60.25,
  "pressure_hpa": 1012.34,
  "gas_resistance_ohm": 150000.00
}
```

### Subscribed Command Topic

The device subscribes to `/zektopic/sensor/bme680/command` to receive commands. The expected command format is a simple string.

**Example Command Payload:**

```
REBOOT
```
*Note: The current firmware might only log received commands. Extend `mqtt_event_handler` to implement specific actions based on commands.*

## Troubleshooting

-   **"Host is unreachable" / DNS errors**:
    *   Ensure your ESP32 has a stable WiFi connection and receives an IP address.
    *   Verify your MQTT broker URI is correct, including the port.
    *   Try using the IP address of the MQTT broker instead of the hostname (e.g., `mqtt://91.121.93.94:1883` for `test.mosquitto.org`).
    *   Check your network's DNS settings.
-   **"Failed to open a new connection"**:
    *   The MQTT broker might be down or unreachable from your network.
    *   Firewall issues on the broker side or your local network.
    *   Incorrect port number in the MQTT URI.
-   **No data published**:
    *   Verify the MQTT client is connected (check serial logs for `MQTT_EVENT_CONNECTED`).
    *   Ensure the publish topic is correct and your MQTT client is subscribed to it.
    *   Check sensor initialization and reading functions for errors.
-   **BME680 reading 0.00 Ohm for gas resistance**:
    *   The BME680 gas sensor requires a burn-in period (typically 20-30 minutes) after power-on to stabilize. Initial readings might be inaccurate or zero.
    *   Ensure the BME680 driver is correctly initialized and calibrated.

## Contributing

Contributions are welcome! If you have suggestions for improvements, bug fixes, or new features, please open an issue or submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
