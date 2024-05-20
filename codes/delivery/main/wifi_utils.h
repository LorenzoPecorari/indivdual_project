// include section
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "nvs_flash.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

// define section
#define APP_NAME "[WI-FI] "

#define SSID "S10 di Lorenzo"
#define PASSWORD "1W1llH4ckY0ur4cc0unt"

// #define SSID "Vodafone-A76372035"
// #define PASSWORD "bhhuy6w9v82zyds4"

#define CONN_BIT BIT0
#define FAIL_BIT BIT1

// global vars
EventGroupHandle_t group;

// function for (mainly) handling wifi events 
void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){

    ESP_LOGI(APP_NAME, "Event: %ld", event_id);

    // checks if the event is a wifi-related one
    if(event_base == WIFI_EVENT){

        // if event is about the starting of station session and tries to conenct to AP 
        if (event_id == WIFI_EVENT_STA_START){
            ESP_LOGI(APP_NAME, "Connecting to wifi...");
            if (esp_wifi_connect() == ESP_OK)
                ESP_LOGI(APP_NAME, "Connection success!");
        }

        // if event is related to a disconnection it will try to reconnect to it
        else if(event_id == WIFI_EVENT_STA_DISCONNECTED){
            ESP_LOGE(APP_NAME, "Disconnected, reconnection will be tried");
            ESP_LOGI(APP_NAME, "Reconnecting to wifi...");
            if (esp_wifi_connect() == ESP_OK)
                ESP_LOGI(APP_NAME, "Connection success!");
        }
    }

    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(APP_NAME, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(group, CONN_BIT);
    }
}

void wifi_init(){
    // variable for collecting all events
    group = xEventGroupCreate();
    
    // initialization of network interfaces
    ESP_ERROR_CHECK(esp_netif_init());
    
    // initialization of default event loop for handling events,
    // generation of queue, registration of event handlers
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // setting up network interface for station mode
    esp_netif_create_default_wifi_sta();

    // structure for initialize wifi module + drivers initialization
    wifi_init_config_t conf = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&conf));

    // variable for storing handler related to any wifi event
    esp_event_handler_instance_t any_id;

    //variable for storing handler related to IP address obtainment
    esp_event_handler_instance_t got_ip;
    
    // registration of handlers for these types of events
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &got_ip));

    // structure for wifi specs of use
    wifi_config_t wifi_conf = {
        .sta = {
            .ssid = SSID,
            .password = PASSWORD,
        },
    };

    // setting station mode for wifi,
    // configuration using previously defined structure,
    // starting up the module with that configuration
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_conf) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    // structure for reacting to a triggered bit among the defined ones as argoument of the function
    EventBits_t notification_bits = xEventGroupWaitBits(group, CONN_BIT | FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    // checking for these bits, each one is referred to a certain event
    if(notification_bits & CONN_BIT)
        ESP_LOGI(APP_NAME, "Connected to AP");
    else if (notification_bits & FAIL_BIT)
        ESP_LOGE(APP_NAME, "Failed to connect to AP");
    else
        ESP_LOGE(APP_NAME, "Unknown event");

    ESP_LOGI(APP_NAME, "Init completed");
}

void wifi_init_main(void) {
    ESP_LOGI(APP_NAME, "Testing the wifi module");

    // initialization of non-volatile storage memory    
    esp_err_t error = nvs_flash_init();

    while(error != ESP_OK){
        nvs_flash_erase();
        error = nvs_flash_init();
    }

    // execution of wifi initialization function previously defined
    wifi_init();
}
