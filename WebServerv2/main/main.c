#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "driver/gpio.h"
#include "esp_timer.h"
//#include "esp_websocket_client.h"

// Define HC-SR04 pins
#define TRIGGER_PIN GPIO_NUM_12
#define ECHO_PIN    GPIO_NUM_13

// Function prototypes
static esp_err_t event_handler(void *ctx, esp_event_base_t event_base, int32_t event_id, void *event_data);
esp_err_t ultrasonic_init();
float ultrasonic_measure();
void websocket_task(void *pvParameters);
void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

// Global variables
esp_websocket_client_handle_t ws_client = NULL;

// Initialize ultrasonic sensor
esp_err_t ultrasonic_init()
{
    gpio_config_t io_conf;
    // Configure trigger pin as output
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << TRIGGER_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    // Configure echo pin as input
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << ECHO_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    return ESP_OK;
}

// Measure distance using ultrasonic sensor
float ultrasonic_measure()
{
    // Trigger pulse
    gpio_set_level(TRIGGER_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(TRIGGER_PIN, 0);

    // Wait for echo response
    uint32_t start = esp_timer_get_time();
    while (!gpio_get_level(ECHO_PIN)) {
        if ((esp_timer_get_time() - start) > 10000) // Timeout after 10ms
            return -1.0;
    }
    uint32_t echo_start = esp_timer_get_time();

    while (gpio_get_level(ECHO_PIN)) {
        if ((esp_timer_get_time() - echo_start) > 50000) // Timeout after 50ms
            return -1.0;
    }
    uint32_t echo_end = esp_timer_get_time();

    // Calculate distance
    float distance = (echo_end - echo_start) * 0.034 / 2.0; // Speed of sound is 34 cm/ms
    return distance;
}

// Task to handle Websocket connection
void websocket_task(void *pvParameters)
{
    esp_websocket_client_config_t websocket_cfg = {
        .uri = "ws://your_websocket_server_address", // Replace with your websocket server address
    };
    ws_client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(ws_client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)ws_client);
    esp_websocket_client_start(ws_client);
    vTaskDelete(NULL);
}

// Websocket event handler
void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI("websocket", "connected to server");
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI("websocket", "disconnected from server");
            break;
        case WEBSOCKET_EVENT_DATA:
            ESP_LOGI("websocket", "received data: %.*s", data->data_len, (char *)data->data_ptr);
            break;
        default:
            break;
    }
}

void app_main()
{
    nvs_flash_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "YOUR_WIFI_SSID",
            .password = "YOUR_WIFI_PASSWORD",
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ultrasonic_init();

    xTaskCreate(&websocket_task, "websocket_task", 4096, NULL, 5, NULL);
}

// WiFi event handler
static esp_err_t event_handler(void *ctx, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
    }
    return ESP_OK;
}
