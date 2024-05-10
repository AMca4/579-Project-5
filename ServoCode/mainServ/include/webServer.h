#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <stdio.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include <esp_http_server.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b)) // Define MIN macro

extern const char *TAG;
extern char message_received[100];

// HTTP SERVER Code

esp_err_t message_post_handler(httpd_req_t *req);
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void wifi_connection(void);
void setFlag(bool arg);
bool readFlag();

// EXAMPLE CODE ON WEBSOCKET SERVER

struct async_resp_arg
{
    httpd_handle_t hd; // Server instance
    int fd;            // Session socket file descriptor
};

void websocket_app_start(void);

#endif /* WEBSOCKET_SERVER_H */
#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <stdio.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include <esp_http_server.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b)) // Define MIN macro

extern const char *TAG;
extern char message_received[100];

// HTTP SERVER Code

esp_err_t message_post_handler(httpd_req_t *req);
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void wifi_connection(void);
void setFlag(bool arg);
bool readFlag();

// EXAMPLE CODE ON WEBSOCKET SERVER

struct async_resp_arg
{
    httpd_handle_t hd; // Server instance
    int fd;            // Session socket file descriptor
};

void websocket_app_start(void);

#endif /* WEBSOCKET_SERVER_H */