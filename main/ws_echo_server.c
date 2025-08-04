/* WebSocket Echo Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <esp_http_server.h>
#include <iot_servo.h>
#include "driver/gpio.h"  // Add GPIO driver
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char *TAG = "ws_echo_server";
static const char *TAG1 = "Servo";
const int stoptime = 600;
#define INPUT_PIN 34  // Define input pin number

// Initialize GPIO pin
static void gpio_init(void) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << INPUT_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    ESP_LOGI(TAG, "GPIO initialized: pin %d", INPUT_PIN);
}
static void stopRoll(void){
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, 90);
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 1, 90);
    ESP_LOGI(TAG1, "STOP STOP STOP STOP");
}
static void posRoll(void){
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, 0);
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 1, 180);
    ESP_LOGI(TAG1, "++++++++++++++");
    vTaskDelay(stoptime / portTICK_PERIOD_MS);
    stopRoll();
    
}

static void negRoll(void){
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, 180);
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 1, 0);
    ESP_LOGI(TAG1, "---------------");
    vTaskDelay(stoptime / portTICK_PERIOD_MS);
    stopRoll();
}



static void servo_init(void)
{
    

    // Configure the servo
    servo_config_t servo_cfg1 = {
        .max_angle = 180,
        .min_width_us = 500,
        .max_width_us = 2500,
        .freq = 50,
        .timer_number = LEDC_TIMER_0,
        .channels = {
            .servo_pin = {
                18,
            },
            .ch = {
                LEDC_CHANNEL_0,
            },
        },
        .channel_number = 1,
    };

    servo_config_t servo_cfg2 = {
        .max_angle = 180,
        .min_width_us = 500,
        .max_width_us = 2500,
        .freq = 50,
        .timer_number = LEDC_TIMER_1,
        .channels = {
            .servo_pin = {
                19,
            },
            .ch = {
                LEDC_CHANNEL_1,
            },
        },
        .channel_number = 1,
    };
    ESP_LOGI(TAG1, "Trying to servo");
    // Initialize the servo
    iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_cfg1);
    iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_cfg2);
    stopRoll();
    ESP_LOGI(TAG1, "Succeeded in initialize servo");

}

// Read pin and control servos task
static void pin_read_task(void *pvParameter) {
    int pin_value;
    int prev_pin_value = 0;  // Initialize to invalid value to ensure first read triggers action
    
    while (1) {
        pin_value = gpio_get_level(INPUT_PIN);
        ESP_LOGI(TAG, "Pin %d value: %d", INPUT_PIN, pin_value);
        
        // Only execute servo commands when pin value changes
        if (pin_value != prev_pin_value) {
            if (pin_value == 1) {
                posRoll();
            } else {
                negRoll();
            }
            prev_pin_value = pin_value;  // Update previous value
        }
        
        // Wait 0.5 seconds
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    // Initialize GPIO
    gpio_init();
    
    // Initialize servo
    servo_init();
    
    // Create task to read pin and control servos
    xTaskCreate(
        pin_read_task,     // Function that implements the task
        "pin_read_task",   // Text name for the task
        2048,              // Stack size in words
        NULL,              // Task input parameter
        5,                 // Priority of the task
        NULL               // Task handle
    );
    
    // Keep main task alive
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
