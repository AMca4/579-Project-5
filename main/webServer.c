/* 579 Skittles Autonomous Car Project G5
 *
 * Web Server App
 * 
 * Web Server code that will be used primarily for debugging and telemetry data gathering/analysis.
 * 
 * This is a new version of original HTTP Server, now updated to a Websocket server allowing asynchronous data pushes from
 * server to the client saving memory.
 *
 * Andrew McAllister
 * 
 */

// Websocket Server responsive to GET request

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
#include "webServer.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b)) // Define MIN macro
const char *TAG = "Websocket Server: ";

char message_received[100] = "";
bool startStopSignal = false;

// HTTP SERVER Code

// http server code end

void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
    }
}


// EXAMPLE CODE ON WEBSOCKET SERVER


void wifi_connection()
{
    nvs_flash_init();
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL);
     wifi_config_t wifi_config = {
        .ap = {
            .ssid = "SKITTLES TELEMETRY G5",
            .ssid_len = strlen("SKITTLES TELEMETRY G5"),
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
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}


// The asynchronous response
void generate_async_resp(void *arg)
{
    // Data format to be sent from the server as a response to the client
    char http_string[250];
    char *data_string = "Hello from ESP32 websocket server ...";
    sprintf(http_string, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", strlen(data_string));

    // Initialize asynchronous response data structure
    struct async_resp_arg *resp_arg = (struct async_resp_arg *)arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;

    // Send data to the client
    ESP_LOGI(TAG, "Executing queued work fd : %d", fd);
    httpd_socket_send(hd, fd, http_string, strlen(http_string), 0);
    httpd_socket_send(hd, fd, data_string, strlen(data_string), 0);

    free(arg);
}

// Initialize a queue for asynchronous communication
esp_err_t async_get_handler(httpd_req_t *req)
{
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    ESP_LOGI(TAG, "Queuing work fd : %d", resp_arg->fd);
    httpd_queue_work(req->handle, generate_async_resp, resp_arg);
    return ESP_OK;
}

// URI Handlers


esp_err_t test_handler(httpd_req_t *req)
{
  esp_err_t error;
	const char *response = (const char *) req->user_ctx;
	error = httpd_resp_send(req, response, strlen(response));
	return error;
}


esp_err_t homePage_handler(httpd_req_t *req){
	esp_err_t error;
	const char *response = (const char *) req->user_ctx;
	error = httpd_resp_send(req, response, strlen(response));
	return error;
}


/* HTTP POST handler */
esp_err_t message_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;
    while (remaining > 0) {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                httpd_resp_send_408(req);
            }
            return ESP_FAIL;
        }
        remaining -= ret;
    }
    httpd_resp_send(req, "Message received successfully", HTTPD_RESP_USE_STRLEN);
    ESP_LOGI(TAG, "Received message: %.*s", req->content_len, buf);
    snprintf(message_received, sizeof(message_received), "%.*s", req->content_len, buf); // Store received message
     if (strcmp(message_received, "Start") == 0) {
        ESP_LOGI(TAG, "Received message: Start");
        setFlag(true);
    } else if (strcmp(message_received, "Stop") == 0) {
        ESP_LOGI(TAG, "Received message: Stop");
        setFlag(false);
    } else {
        ESP_LOGI(TAG, "Received message: %s", message_received);
    }
    return ESP_OK;
}


void setFlag(bool arg){
    startStopSignal = arg;
}

bool readFlag(){

    return startStopSignal;

}

// Create URI (Uniform Resource Identifier) 
// for the server which is added to default gateway


// URI Pages

const httpd_uri_t homePage = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = homePage_handler,
    .user_ctx  = "<!DOCTYPE html>\
<html>\
\
<body>\
\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
<style>\
body {\
  font-family: \"Lato\", sans-serif;\
}\
\
.sidenav {\
  height: 100%;\
  width: 0;\
  position: fixed;\
  z-index: 1;\
  top: 0;\
  left: 0;\
  background-color: #111;\
  overflow-x: hidden;\
  transition: 0.5s;\
  padding-top: 60px;\
}\
\
.sidenav a {\
  padding: 8px 8px 8px 32px;\
  text-decoration: none;\
  font-size: 25px;\
  color: #818181;\
  display: block;\
  transition: 0.3s;\
}\
\
.sidenav a:hover {\
  color: #f1f1f1;\
}\
\
.sidenav .closebtn {\
  position: absolute;\
  top: 0;\
  right: 25px;\
  font-size: 36px;\
  margin-left: 50px;\
}\
\
@media screen and (max-height: 450px) {\
  .sidenav {padding-top: 15px;}\
  .sidenav a {font-size: 18px;}\
}\
</style>\
</head>\
<body>\
\
<div id=\"mySidenav\" class=\"sidenav\">\
  <a href=\"javascript:void(0)\" class=\"closebtn\" onclick=\"closeNav()\">&times;</a>\
  <a href=\"/s\">Home</a>\
  <a href=\"/carIgnition\">Live Telemetry</a>\
  <a href=\"/config\">Configuration</a>\
\
</div>\
<span style=\"font-size:30px;cursor:pointer\" onclick=\"openNav()\">&#9776;Skittles Autonomous Car App</span>\
\
<h1>Home Page</h1>\
\
<script>\
function openNav() {\
  document.getElementById(\"mySidenav\").style.width = \"250px\";\
}\
\
function closeNav() {\
  document.getElementById(\"mySidenav\").style.width = \"0\";\
}\
</script>\
<script>\
        function sendStartMessage() {\
            var xhr = new XMLHttpRequest();\
            xhr.open(\"POST\", \"/message\", true);\
            xhr.setRequestHeader(\"Content-Type\", \"text/plain\");\
            xhr.send(\"Start\");\
            xhr.onload = function () {\
                if (xhr.status == 200) {\
                    alert(\"Message sent successfully\");\
                } else {\
                    alert(\"Failed to send message\");\
                }\
            };\
        }\
        function sendStopMessage() {\
            var xhrStop = new XMLHttpRequest();\
            xhrStop.open(\"POST\", \"/message\", true);\
            xhrStop.setRequestHeader(\"Content-Type\", \"text/plain\");\
            xhrStop.send(\"Stop\");\
            xhrStop.onload = function () {\
                if (xhrStop.status == 200) {\
                    alert(\"Message sent successfully\");\
                } else {\
                    alert(\"Failed to send message\");\
                }\
            };\
        }\
    </script>\
\
<p> Welcome to the hompage of Group 5's Telemtry Data Application. This server contains useful tools in order to debug, perform data analysis and troubleshoot\
potential errors with the Skittles Autonomous Car Project! </p>\
<button onclick=\"sendStartMessage()\">Start Car</button>\
<button onclick=\"sendStopMessage()\">Stop Car</button>\
</body>\
</html>"
};

/* HTTP server config */
httpd_uri_t message_post = {
    .uri       = "/message",
    .method    = HTTP_POST,
    .handler   = message_post_handler,
    .user_ctx  = "<!DOCTYPE html>\
<html>\
\
<body>\
\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
<style>\
body {\
  font-family: \"Lato\", sans-serif;\
}\
\
.sidenav {\
  height: 100%;\
  width: 0;\
  position: fixed;\
  z-index: 1;\
  top: 0;\
  left: 0;\
  background-color: #111;\
  overflow-x: hidden;\
  transition: 0.5s;\
  padding-top: 60px;\
}\
\
.sidenav a {\
  padding: 8px 8px 8px 32px;\
  text-decoration: none;\
  font-size: 25px;\
  color: #818181;\
  display: block;\
  transition: 0.3s;\
}\
\
.sidenav a:hover {\
  color: #f1f1f1;\
}\
\
.sidenav .closebtn {\
  position: absolute;\
  top: 0;\
  right: 25px;\
  font-size: 36px;\
  margin-left: 50px;\
}\
\
@media screen and (max-height: 450px) {\
  .sidenav {padding-top: 15px;}\
  .sidenav a {font-size: 18px;}\
}\
</style>\
</head>\
<body>\
\
<div id=\"mySidenav\" class=\"sidenav\">\
  <a href=\"javascript:void(0)\" class=\"closebtn\" onclick=\"closeNav()\">&times;</a>\
  <a href=\"/s\">Home</a>\
  <a href=\"/carIgnition\">Live Telemetry</a>\
  <a href=\"/config\">Configuration</a>\
\
</div>\
<span style=\"font-size:30px;cursor:pointer\" onclick=\"openNav()\">&#9776;Skittles Autonomous Car App</span>\
\
<h1>Home Page</h1>\
\
<script>\
function openNav() {\
  document.getElementById(\"mySidenav\").style.width = \"250px\";\
}\
\
function closeNav() {\
  document.getElementById(\"mySidenav\").style.width = \"0\";\
}\
</script>\
<script>\
        function sendStartMessage() {\
            var xhr = new XMLHttpRequest();\
            xhr.open(\"POST\", \"/message\", true);\
            xhr.setRequestHeader(\"Content-Type\", \"text/plain\");\
            xhr.send(\"Start\");\
            xhr.onload = function () {\
                if (xhr.status == 200) {\
                    alert(\"Message sent successfully\");\
                } else {\
                    alert(\"Failed to send message\");\
                }\
            };\
        }\
         function sendStopMessage() {\
            var xhrStop = new XMLHttpRequest();\
            xhrStop.open(\"POST\", \"/message\", true);\
            xhrStop.setRequestHeader(\"Content-Type\", \"text/plain\");\
            xhrStop.send(\"Stop\");\
            xhrStop.onload = function () {\
                if (xhrStop.status == 200) {\
                    alert(\"Message sent successfully\");\
                } else {\
                    alert(\"Failed to send message\");\
                }\
            };\
        }\
    </script>\
\
<p> Welcome to the hompage of Group 5's Telemtry Data Application. This server contains useful tools in order to debug, perform data analysis and troubleshoot\
potential errors with the Skittles Autonomous Car Project! </p>\
<button onclick=\"sendStartMessage()\">Start Car</button>\
<button onclick=\"sendStopMessage()\">Stop Car</button>\
</body>\
</html>"
};

void websocket_app_start(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Registering the uri_handler
        ESP_LOGI(TAG, "Registering URI handler");
        httpd_register_uri_handler(server, &homePage);
        httpd_register_uri_handler(server, &message_post);

    }
}


// MAIN FUNC

/*
void app_main(void)
{

    wifi_connection();
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    websocket_app_start();
}*/