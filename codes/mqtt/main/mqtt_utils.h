
#include <stdint.h>
#include <stddef.h>
#include <sys/param.h>

#include "esp_partition.h"
#include "esp_netif.h"
#include "esp_tls.h"

#include "mqtt_client.h"

#define APP_NAME_MQTT "[MQTT] "

void mqtt_event_handler(void* args, esp_event_base_t base, int32_t id, void* data){
    ESP_LOGI(APP_NAME_MQTT, "Triggered event -\n\t id: %" PRIi32 "\n\t base: %s\n", id, base);
    
    esp_mqtt_event_handle_t event = data;
    esp_mqtt_client_handle_t client = event->client;

    int id_message = 0;

    if(id == MQTT_EVENT_CONNECTED){
        id_message = esp_mqtt_client_subscribe(client, "/topic/1", 1);
        ESP_LOGI(APP_NAME_MQTT, "Subscription - id_message: %d", id_message);

        id_message = esp_mqtt_client_unsubscribe(client, "/topic/1");
        ESP_LOGI(APP_NAME_MQTT, "Unsubscription - id_message: %d", id_message);
    }
    else if(id == MQTT_EVENT_DISCONNECTED){
        ESP_LOGI(APP_NAME_MQTT, "Disconnected");
    }
    else if(id == MQTT_EVENT_SUBSCRIBED){
        id_message = esp_mqtt_client_publish(client, "/topic/1", "data", 0, 1, 0);
        ESP_LOGI(APP_NAME_MQTT, "Subscribed - id_message: %d", id_message);
    }
    else if(id == MQTT_EVENT_UNSUBSCRIBED){
        ESP_LOGI(APP_NAME_MQTT, "Disconnected - id_message* : %d", event->msg_id);
    }
    else if(id == MQTT_EVENT_PUBLISHED){
        ESP_LOGI(APP_NAME_MQTT, "Published - id_message* : %d", event->msg_id);
    }
    else if(id == MQTT_EVENT_DATA){
        ESP_LOGI(APP_NAME_MQTT, "Data received -\n\t Topic: %s\n\t Data: %s", event->topic, event->data);
    }
    else if(id == MQTT_EVENT_ERROR){
        ESP_LOGI(APP_NAME_MQTT, "Error, check later");
    }
    else{
        ESP_LOGI(APP_NAME_MQTT, "Other event, %ld", id);
    }
}

void start_mqtt(void){
    ESP_LOGI(APP_NAME_MQTT, "Starting mqtt configuration...");

    esp_mqtt_client_config_t conf = {
        .broker = {
            .address.uri = "54c7f0618968409f95ff78c304b7b78e.s1.eu.hivemq.cloud:8883",
            .verification.certificate = 0 // CHECK IT!!
        },
    };

    ESP_LOGI(APP_NAME_MQTT, "Configuration structure variable populated");

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&conf);
    esp_mqtt_client_register_event(client, MQTT_EVENT_CONNECTED, mqtt_event_handler, NULL);
    esp_mqtt_client_register_event(client, MQTT_EVENT_DISCONNECTED, mqtt_event_handler, NULL);
    esp_mqtt_client_register_event(client, MQTT_EVENT_SUBSCRIBED, mqtt_event_handler, NULL);
    esp_mqtt_client_register_event(client, MQTT_EVENT_UNSUBSCRIBED, mqtt_event_handler, NULL);
    esp_mqtt_client_register_event(client, MQTT_EVENT_PUBLISHED, mqtt_event_handler, NULL);
    esp_mqtt_client_register_event(client, MQTT_EVENT_DATA, mqtt_event_handler, NULL);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ERROR, mqtt_event_handler, NULL);
    
    ESP_LOGI(APP_NAME_MQTT, "Client handler registered");

    esp_mqtt_client_start(client);

    ESP_LOGI(APP_NAME_MQTT, "Client");
}
