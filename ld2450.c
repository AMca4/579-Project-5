/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
//#include <macros.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <esp_err.h>
#include <portmacro.h>

/**
 * This is an example which echos any data it receives on configured UART back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 */

#define ECHO_TEST_TXD (GPIO_NUM_43)
#define ECHO_TEST_RXD (GPIO_NUM_44)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (UART_NUM_1)
#define ECHO_UART_BAUD_RATE     115200
#define ECHO_TASK_STACK_SIZE    (4096)
#define portTICK_PERIOD_MS      (1) 

static const char *TAG = "UART TEST";

#define BUF_SIZE (1024)
#define STATE_SIZE 8

uint32_t lastTime = millis();

uint16_t twoByteToUint(char first, char second) {
    return (uint16_t)(second << 8) + first;
}

int16_t twoByteToInt(char first, char second) {
    return (int16_t)(second << 8) | first;
}

void reportTargetInfo(int object, char *raw) {
    printf("\n*****\tExecuting Report Target Information\t*****\n");
    int16_t newX, newY, newSpeed;
    uint16_t newResolution;

    ESP_LOGV(TAG, "Will reporting taget %d", target);

    switch (object) {
      case 0:
        newX = twoByteToUint(raw[0], raw[1] & 0x7F);
        printf("\nNewX = %d", newX);
        if (raw[1] >> 7 != 0x1)
          newX = 0 - newX / 10;
        else
          newX = newX / 10;
        printf("\nNewX = %d", newX);
        newY = twoByteToUint(raw[2], raw[3] & 0x7F);
        printf("\nNewY = %d", newY);
        if (raw[3] >> 7 != 0x1)
          newY = 0 - newY / 10;
        else
          newY = newY / 10;
        printf("\nNewY = %d", newY);
        newSpeed = twoByteToUint(raw[4], raw[5] & 0x7F);
        printf("\nNewSpeed = %d", newSpeed);
        if (raw[5] >> 7 != 0x1)
          newSpeed = 0 - newSpeed;
        printf("\nNewSpeed = %d", newSpeed);
        newResolution = twoByteToUint(raw[6], raw[7]);
        printf("\nNewRes = %d", newResolution);
        break;
      case 1:
        newX = twoByteToUint(raw[0], raw[1] & 0x7F);
        printf("\nNewX = %d", newX);
        if (raw[1] >> 7 != 0x1)
          newX = 0 - newX / 10;
        else
          newX = newX / 10;
        printf("\nNewX = %d", newX);
        newY = twoByteToUint(raw[2], raw[3] & 0x7F);
        if (raw[3] >> 7 != 0x1)
          newY = 0 - newY / 10;
        else
          newY = newY / 10;
        printf("\nNewY = %d", newY);
        newSpeed = twoByteToUint(raw[4], raw[5] & 0x7F);
        printf("\nNewSpeed = %d", newSpeed);
        if (raw[5] >> 7 != 0x1)
          newSpeed = 0 - newSpeed;
        printf("\nNewSpeed = %d", newSpeed);
        newResolution = twoByteToUint(raw[6], raw[7]);
        printf("\nNewRes = %d", newResolution);
        break;
      case 2:
        newX = twoByteToUint(raw[0], raw[1] & 0x7F);
        printf("\nNewX = %d", newX);
        if (raw[1] >> 7 != 0x1)
          newX = 0 - newX / 10;
        else
          newX = newX / 10;
        printf("\nNewX = %d", newX);
        newY = twoByteToUint(raw[2], raw[3] & 0x7F);
        printf("\nNewY = %d", newY);
        if (raw[3] >> 7 != 0x1)
          newY = 0 - newY / 10;
        else
          newY = newY / 10;
        printf("\nNewY = %d", newY);
        newSpeed = twoByteToUint(raw[4], raw[5] & 0x7F);
        printf("\nNewSpeed = %d", newSpeed);
        if (raw[5] >> 7 != 0x1)
          newSpeed = 0 - newSpeed;
        printf("\nNewSpeed = %d", newSpeed);
        newResolution = twoByteToUint(raw[6], raw[7]);
        printf("\nNewRes = %d", newResolution);
        break;
    }
}

void handlePeriodicData(char *buffer, int length) {
    printf("\n******\tExecuting Handling periodic data Function\t*****\n");
    if (length < 29)
      return;  // 4 frame start bytes + 2 length bytes + 1 data end byte + 1 crc byte + 4 frame end bytes
    if (buffer[0] != 0xAA || buffer[1] != 0xFF || buffer[2] != 0x03 || buffer[3] != 0x00)
      return;  // check 4 frame start bytes
    if (buffer[len - 2] != 0x55 || buffer[len - 1] != 0xCC)
      return;  //  data end=0x55, 0xcc
    char stateBytes[STATE_SIZE];

    int currentTime = millis();
    if (currentTime - lastTime < 1000)
      return;
      printf("\nCurrent time - last Time = %d - %d = %d", currentTime, lastTime, (currentTime - lastTime));
    lastTime = currentTime;
    for (int i = 0; i < TARGETS; i++) {
      memcpy(stateBytes, &buffer[4 + i * STATE_SIZE], STATE_SIZE);
      printf("\nState Bytes = %s", stateBytes);
      reportTargetInfo(i, stateBytes);
    }
}

void readline(int readch, char *buffer, int length) {
    printf("\n******\tExecuting Readline FUNCTION\t*****\n");
    static int position;
    printf("\nPosition = %d", pos);
    if (readch >= 0) {
      if (position < len - 1) {
        buffer[position++] = readch;
        buffer[position] = 0;
      } else {
        pos = 0;
      }
      printf("\nBuffer = ");
      for(int i=0; i<len; i++)
        printf("%c ", buffer[i]);
      if ((buffer[position - 2] == 0x55 && buffer[position - 1] == 0xCC) && position>29) {
          handlePeriodicData(buffer, pos);
          position = 0;  // Reset position index ready for next time
      } 
    }
    return;
}

static void echo_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    /*const char *init_cmds[] = {

    }*/

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    #if CONFIG_UART_ISR_IN_IRAM
        intr_alloc_flags = ESP_INTR_FLAG_IRAM;
    #endif

    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

    // Configure a temporary buffer for the incoming data
    int *data = (int *) malloc(BUF_SIZE);
    int iter = 0;
    const int max_line_length = 160;
    static char buffer[max_line_length];
    int lastTime = 0;

    while (1){

        // Read data from the UART
        int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        printf("\nData = %d\tIteration = %d", data[iter], iter);
        
        readline(data[iter], buffer, max_line_length);

        // Write data back to the UART
        uart_write_bytes(ECHO_UART_PORT_NUM, (const char *) data, len);
        if (len) {
            data[len] = '\0';
            //ESP_LOGI(TAG, "Recv str: %s", (char *) data);
        }
        iter++;
        //free(data);
    }
}

void app_main(void){
    xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
}
