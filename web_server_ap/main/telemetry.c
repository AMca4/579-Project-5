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
#include "my_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include <esp_http_server.h>

static const char *TAG = "Websocket Server: ";


// HTTP SERVER Code

// http server code end

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
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

// Asynchronous response data structure
struct async_resp_arg
{
    httpd_handle_t hd; // Server instance
    int fd;            // Session socket file descriptor
};

// The asynchronous response
static void generate_async_resp(void *arg)
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
static esp_err_t async_get_handler(httpd_req_t *req)
{
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    ESP_LOGI(TAG, "Queuing work fd : %d", resp_arg->fd);
    httpd_queue_work(req->handle, generate_async_resp, resp_arg);
    return ESP_OK;
}

// URI Handlers


static esp_err_t test_handler(httpd_req_t *req)
{
  esp_err_t error;
	const char *response = (const char *) req->user_ctx;
	error = httpd_resp_send(req, response, strlen(response));
	return error;
}

static esp_err_t telemetry_handler(httpd_req_t *req){
	esp_err_t error;
	const char *response = (const char *) req->user_ctx;
	error = httpd_resp_send(req, response, strlen(response));
	return error;
}

static esp_err_t homePage_handler(httpd_req_t *req){
	esp_err_t error;
	const char *response = (const char *) req->user_ctx;
	error = httpd_resp_send(req, response, strlen(response));
	return error;
}

static esp_err_t configPage_handler(httpd_req_t *req){
	esp_err_t error;
	const char *response = (const char *) req->user_ctx;
	error = httpd_resp_send(req, response, strlen(response));
	return error;
}


esp_err_t carIgnitionPage_handler(httpd_req_t *req)
{
  esp_err_t error;
	const char *response = (const char *) req->user_ctx;
	error = httpd_resp_send(req, response, strlen(response));
	return error;
    
}

esp_err_t ultrasonicAPage_handler(httpd_req_t *req)
{
  esp_err_t error;
	const char *response = (const char *) req->user_ctx;
	error = httpd_resp_send(req, response, strlen(response));
	return error;
    
}

esp_err_t killWebserverPage_handler(httpd_req_t *req)
{
  esp_err_t error;
  esp_err_t httpd_stop(httpd_handle_t handle);
	const char *response = (const char *) req->user_ctx;
	error = httpd_resp_send(req, response, strlen(response));
	return error;
    
}

// Create URI (Uniform Resource Identifier) 
// for the server which is added to default gateway


// URI Pages

httpd_uri_t carIgnitionPage = {
    .uri = "/carIgnition",
    .method = HTTP_GET,
    .handler = carIgnitionPage_handler,
    .user_ctx = "<!DOCTYPE html>\
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
  <a href=\"/\">Home</a>\
  <a href=\"/liveTelemetry\">Live Telemetry</a>\
  <a href=\"/config\">Configuration</a>\
\
</div>\
<span style=\"font-size:30px;cursor:pointer\" onclick=\"openNav()\">&#9776;Skittles Autonomous Car App</span>\
\
<h1>Ignition Selection</h1>\
\
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
<script>\
function openNav() {\
  document.getElementById(\"mySidenav\").style.width = \"250px\";\
}\
\
function closeNav() {\
  document.getElementById(\"mySidenav\").style.width = \"0\";\
}\
</script>\
\
<p> Select Ignition for car (with live telemetry or without live telemetry)</p>\
\
<form action=\"/liveTelemetry\">\
<button type=\"submit\" value=\"Telemetry Ignition\">\
Live Telemetry Ignition\
</button>\
</form>\
<form action=\"/killWebserver\">\
<button type=\"submit\" value=\"Raw Ignition\" onclick=\"sendStartMessage();\" />\
Raw Ignition\
</button>\
</form>\
</body>\
</html>"
};



static const httpd_uri_t liveTelemetryPage = {
    .uri       = "/liveTelemetry",
    .method    = HTTP_GET,
    .handler   = telemetry_handler,
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
#myDIV {\
  width: 100%;\
  padding: 50px 0;\
  text-align: center;\
  background-color: lightblue;\
  margin-top: 20px;\
}\
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
  <a href=\"/\">Home</a>\
  <a href=\"/liveTelemetry\">Live Telemetry</a>\
  <a href=\"/config\">Configuration</a>\
\
</div>\
<span style=\"font-size:30px;cursor:pointer\" onclick=\"openNav()\">&#9776;Skittles Autonomous Car App</span>\
\
<h1>Live Telemetry Data</h1>\
\
<script>\
function openNav() {\
  document.getElementById(\"mySidenav\").style.width = \"250px\";\
}\
\
function closeNav() {\
  document.getElementById(\"mySidenav\").style.width = \"0\";\
}\
\
  if (document.getElementById(\"SensorA\").checked == \"1\");\
    window.location.href=\'/liveTelemetry/UltrasonicA\';\
  }\
\
</script>\
\
<h3>Sensor Selection</h3>\
<form>\
    <input type=\"checkbox\" id=\"SensorA\" name=\"SensorA\" value=\"Ultrasonic\">\
    <label for=\"SensorA\"> Ultrasonic A</label><br>\
    <input type=\"checkbox\" id=\"SensorB\" name=\"SensorB\" value=\"Ultrasonic\">\
    <label for=\"SensorB\"> Ultrasonic B</label><br>\
    <input type=\"checkbox\" id=\"SensorC\" name=\"SensorC\" value=\"InfraRed\">\
    <label for=\"SensorC\"> InfraRed A</label><br>\
    <input type=\"checkbox\" id=\"SensorD\" name=\"SensorD\" value=\"InfraRed\">\
    <label for=\"SensorD\"> InfraRed B</label><br>\
    <input type=\"submit\" value=\"Display Data\" onclick=\"displaySensorData()\">\
</form>\
\
<div class=\"container\">\
  <center>\
      <svg\ 
      xmlns=\"http://www.w3.org/2000/svg\"\ 
      width=\"7em\"\
      style=\"top:50%\"\
      margin= auto\
      display=flex\
      viewBox=\"0 0 15 15\">\
      	<path fill=#3444db d=\"M13.84 6.852L12.6 5.7l-1.1-2.2a1.05 1.05 0 0 0-.9-.5H4.4a1.05 1.05 0 0 0-.9.5L2.4 5.7L1.16 6.852a.5.5 0 0 0-.16.367V11.5a.5.5 0 0 0 .5.5h2c.2 0 .5-.2.5-.4V11h7v.5c0 .2.2.5.4.5h2.1a.5.5 0 0 0 .5-.5V7.219a.5.5 0 0 0-.16-.367M4.5 4h6l1 2h-8ZM5 8.6c0 .2-.3.4-.5.4H2.4c-.2 0-.4-.3-.4-.5V7.4c.1-.3.3-.5.6-.4l2 .4c.2 0 .4.3.4.5Zm8-.1c0 .2-.2.5-.4.5h-2.1c-.2 0-.5-.2-.5-.4v-.7c0-.2.2-.5.4-.5l2-.4c.3-.1.5.1.6.4Z\" />\
  </svg>\
  </center>\
  </div>\
  \
</body>\
</html>"
};

static const httpd_uri_t homePage = {
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
\
<p> Welcome to the hompage of Group 5's Telemtry Data Application. This server contains useful tools in order to debug, perform data analysis and troubleshoot\
 potential errors with the Skittles Autonomous Car Project! </p>\
</body>\
</html>"
};

static const httpd_uri_t configPage = {
    .uri       = "/config",
    .method    = HTTP_GET,
    .handler   = configPage_handler,
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
  <a href=\"/\">Home</a>\
  <a href=\"/carIgnition\">Live Telemetry</a>\
  <a href=\"/config\">Configuration</a>\
\
</div>\
<span style=\"font-size:30px;cursor:pointer\" onclick=\"openNav()\">&#9776;Skittles Autonomous Car App</span>\
\
<h1>Configuration Page</h1>\
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
\
<p> Welcome to the Configuration page! This page contains important information regarding the Skittles Autonomous Car\
 parts, diagrams and construction.</p>\
</body>\
</html>"
};

static const httpd_uri_t ultrasonicAPage = {
    .uri       = "/liveTelemetry/UltrasonicA",
    .method    = HTTP_GET,
    .handler   = ultrasonicAPage_handler,
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
#myDIV {\
  width: 100%;\
  padding: 50px 0;\
  text-align: center;\
  background-color: lightblue;\
  margin-top: 20px;\
}\
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
  <a href=\"/\">Home</a>\
  <a href=\"/liveTelemetry\">Live Telemetry</a>\
  <a href=\"/config\">Configuration</a>\
\
</div>\
<span style=\"font-size:30px;cursor:pointer\" onclick=\"openNav()\">&#9776;Skittles Autonomous Car App</span>\
\
<h1>Live Telemetry Data</h1>\
\
<script>\
function openNav() {\
  document.getElementById(\"mySidenav\").style.width = \"250px\";\
}\
\
function closeNav() {\
  document.getElementById(\"mySidenav\").style.width = \"0\";\
}\
function displaySensorData() {\
\
  if (document.getElementById(\"SensorA\").style.width = \"1\");\
    window.location.href=\'/\';\
  }\
\
</script>\
\
<h3>Ultrasonic A Data</h3>\
<form>\
    <input type=\"checkbox\" id=\"SensorA\" name=\"SensorA\" value=\"Ultrasonic\">\
    <label for=\"SensorA\"> Ultrasonic A</label><br>\
    <input type=\"checkbox\" id=\"SensorB\" name=\"SensorB\" value=\"Ultrasonic\">\
    <label for=\"SensorB\"> Ultrasonic B</label><br>\
    <input type=\"checkbox\" id=\"SensorC\" name=\"SensorC\" value=\"InfraRed\">\
    <label for=\"SensorC\"> InfraRed A</label><br>\
    <input type=\"checkbox\" id=\"SensorD\" name=\"SensorD\" value=\"InfraRed\">\
    <label for=\"SensorD\"> InfraRed B</label><br>\
    <input type=\"submit\" value=\"Display Data\" onclick=\"displaySensorData()\">\
</form>\
\
<div class=\"container\">\
  <center>\
      <svg\ 
      xmlns=\"http://www.w3.org/2000/svg\"\ 
      width=\"7em\"\
      style=\"top:50%\"\
      margin= auto\
      display=flex\
      viewBox=\"0 0 15 15\">\
      	<path fill=#3444db d=\"M13.84 6.852L12.6 5.7l-1.1-2.2a1.05 1.05 0 0 0-.9-.5H4.4a1.05 1.05 0 0 0-.9.5L2.4 5.7L1.16 6.852a.5.5 0 0 0-.16.367V11.5a.5.5 0 0 0 .5.5h2c.2 0 .5-.2.5-.4V11h7v.5c0 .2.2.5.4.5h2.1a.5.5 0 0 0 .5-.5V7.219a.5.5 0 0 0-.16-.367M4.5 4h6l1 2h-8ZM5 8.6c0 .2-.3.4-.5.4H2.4c-.2 0-.4-.3-.4-.5V7.4c.1-.3.3-.5.6-.4l2 .4c.2 0 .4.3.4.5Zm8-.1c0 .2-.2.5-.4.5h-2.1c-.2 0-.5-.2-.5-.4v-.7c0-.2.2-.5.4-.5l2-.4c.3-.1.5.1.6.4Z\" />\
  </svg>\
  </center>\
  </div>\
  \
</body>\
</html>"
};

static const httpd_uri_t killWebserverPage = {
    .uri       = "/killWebserver",
    .method    = HTTP_GET,
    .handler   = killWebserverPage_handler,
    .user_ctx  = NULL
    };


static void websocket_app_start(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Registering the uri_handler
        ESP_LOGI(TAG, "Registering URI handler");
        httpd_register_uri_handler(server, &liveTelemetryPage);
        httpd_register_uri_handler(server, &homePage);
        httpd_register_uri_handler(server, &configPage);
        httpd_register_uri_handler(server, &carIgnitionPage);
        httpd_register_uri_handler(server, &killWebserverPage);
        httpd_register_uri_handler(server, &ultrasonicAPage);
    }
}



// MAIN FUNC


void app_main(void)
{

    wifi_connection();
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    websocket_app_start();
}