/* 
 * Skittles AP code for telemetry
 * based on the esp-idf example code for soft AP and http server 
 * removed extra features and logging to make it more lightweight
 * 
 * last modified: 04/02/24
 * 
 * Michail Kasmeridis 
 * 
 * EE579 G5
*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "driver/gpio.h"

#include "esp_err.h"
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>

#define LED GPIO_NUM_2

static esp_err_t telemetry_handler(httpd_req_t *req){
	esp_err_t error;
	const char *response = (const char *) req->user_ctx;
	error = httpd_resp_send(req, response, strlen(response));
	return error;
}

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = telemetry_handler,
    .user_ctx  = "<!DOCTYPE html>\
<html lang=\"en\">\
<head>\
    <meta charset=\"UTF-8\">\
    <meta name=\"viewport\" content=\"width=device-width, height=device-height, initial-scale=1.0\">\
    <title>Skittles Telemetry G5</title>\
    <center>\
    <h1>Skittles Telemetry G5</h1>\
    </center>\
    <style>\
        body {\
            display: flex;\
            flex-direction: column;\
            align-items: center;\
            min-height: 100vh;\
            margin: auto;\
            background: #333; \
            color: #fff; \
        }\
        .container {\
        	display : table;\
            height:200vh;\
            position: flex;\
            text-align: center;\
            margin-top: 200px; \
        }\
        .textbox {\
            position: absolute;\
            width: 100px;\
            height: 15px;\
            text-align: center;\
            background: white; \
            border: 15px solid #00000; \
            border-radius: 25px; \
            padding: 5px; \
        }\
        }\
    </style>\
</head>\
<body>\
<div class=\"container\">\
<center>\
    <svg \
    xmlns=\"http://www.w3.org/2000/svg\" \
    width=\"7em\"\
    style=\"top:50%\"\
    margin= auto\
    display=flex\
    viewBox=\"0 0 15 15\">\
	<path fill=#3444db d=\"M13.84 6.852L12.6 5.7l-1.1-2.2a1.05 1.05 0 0 0-.9-.5H4.4a1.05 1.05 0 0 0-.9.5L2.4 5.7L1.16 6.852a.5.5 0 0 0-.16.367V11.5a.5.5 0 0 0 .5.5h2c.2 0 .5-.2.5-.4V11h7v.5c0 .2.2.5.4.5h2.1a.5.5 0 0 0 .5-.5V7.219a.5.5 0 0 0-.16-.367M4.5 4h6l1 2h-8ZM5 8.6c0 .2-.3.4-.5.4H2.4c-.2 0-.4-.3-.4-.5V7.4c.1-.3.3-.5.6-.4l2 .4c.2 0 .4.3.4.5Zm8-.1c0 .2-.2.5-.4.5h-2.1c-.2 0-.5-.2-.5-.4v-.7c0-.2.2-.5.4-.5l2-.4c.3-.1.5.1.6.4Z\" />\
</svg>\
    <input type=\"text\" class=\"textbox top\" placeholder=\"Front\" style=\"top:20%; left: 50%;\" margin=auto display= flex>\
    <input type=\"text\" class=\"textbox bottom\" placeholder=\"Back\" style=\"top: 80%; left: 50%;\" margin=auto display= flex>\
    <input type=\"text\" class=\"textbox left\" placeholder=\"Left\" style=\"left: 20%; top:50%;\"  margin=auto display= flex>\
    <input type=\"text\" class=\"textbox right\" placeholder=\"Right\" style=\"left: 80%; top:50%;\" margin=flex>\
</center>\
</div>\
</body>\
</html>"
};

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err){
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

static httpd_handle_t start_webserver(void){
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &root);
        return server;
    }
    return NULL;
}

static void stop_webserver(httpd_handle_t server){httpd_stop(server);}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        *server = start_webserver();
    }
}

// http server code end

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
    }
}


void wifi_init_softap(void){
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "skittles_telemetry_g5",
            .ssid_len = strlen("skittles_telemetry_g5"),
            .channel = 1,
            .authmode = WIFI_AUTH_OPEN,
            .max_connection = 4,
            .pmf_cfg = {
                    .required = true,
            },
        },
    };
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();
}



void app_main(void){
	static httpd_handle_t server = NULL;
    //Initialize NVS
    nvs_flash_init();
    wifi_init_softap();
    esp_netif_init();
    esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server);
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server);
}