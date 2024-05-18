#include <stdint.h>
#include <stddef.h>
#include <sys/param.h>

#include "esp_partition.h"
#include "esp_netif.h"
#include "esp_tls.h"

#include "mqtt_client.h"

#include "broker_cert.h"

#define APP_NAME_MQTT "[MQTT] "

#define URI "mqtts://54c7f0618968409f95ff78c304b7b78e.s1.eu.hivemq.cloud:8883"
#define TOPIC "/topic/individual_project"

#define CREDENTIAL_USERNAME "lorenzo_tester"
#define CREDENTIAL_PWD "HiveMQ1_credentials"

#define QOS 1

esp_mqtt_client_handle_t client = {0};

void mqtt_event_handler(void* args, esp_event_base_t base, int32_t id, void* data){

    esp_mqtt_event_handle_t event = data;
    client = event->client;

    if(id == MQTT_EVENT_CONNECTED){
        ESP_LOGI(APP_NAME_MQTT, "Connected to HiveMQ broker");
    }
    else if(id == MQTT_EVENT_DISCONNECTED){
        ESP_LOGI(APP_NAME_MQTT, "Disconnected");
    }
    else if(id == MQTT_EVENT_SUBSCRIBED){
        //id_message = esp_mqtt_client_publish(client, TOPIC, "ESP32-S3 subscribed!\0", 0, QOS, 0);
        //ESP_LOGI(APP_NAME_MQTT, "Publish - id_message: %d", id_message);
        ESP_LOGI(APP_NAME_MQTT, "Subscribed to %s", TOPIC);
    }
    else if(id == MQTT_EVENT_UNSUBSCRIBED){
        ESP_LOGI(APP_NAME_MQTT, "Unsubscribed \t id_message* : %d", event->msg_id);
    }
    else if(id == MQTT_EVENT_PUBLISHED){
        ESP_LOGI(APP_NAME_MQTT, "Published %s \t id_message* : %d", event->data, event->msg_id);
        esp_mqtt_client_unsubscribe(client, TOPIC);
    }
    else if(id == MQTT_EVENT_DATA){
        event->data[event->data_len] = '\0';
        ESP_LOGI(APP_NAME_MQTT, "Topic: %s \t Data: %s", event->topic, event->data);
    }
    else if(id == MQTT_EVENT_ERROR){
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
            ESP_LOGI(APP_NAME_MQTT, "TCP transport event");
            
        else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) 
            ESP_LOGI(APP_NAME_MQTT, "Connection refused error");

    }
    else{
        ESP_LOGI(APP_NAME_MQTT, "Other event, %ld", id);
    }
}

void start_mqtt(void){

    esp_mqtt_client_config_t conf = {
        .broker = {
            .address.uri = URI,
            .verification.certificate = (const char*) broker_cert_pem,
            //.verification.certificate = 0 // CHECK IT!!
        },
        
        .credentials.username = CREDENTIAL_USERNAME,
        .credentials.authentication.password = CREDENTIAL_PWD,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&conf);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    
    ESP_LOGI(APP_NAME_MQTT, "Initialization completed");

    esp_mqtt_client_start(client);
}

void subscribe_to_topic(){
    esp_mqtt_client_subscribe(client, TOPIC, QOS);
}

void send_value_to_broker(float message){

    char str[32];
    char* prefix = "Computed mean: ";
    int offset = strlen(prefix);
    
    memset(str, '\0', 32);
    sprintf(str, "%s", prefix);
    sprintf(str + offset, "%.2f", message);
    
    esp_mqtt_client_publish(client, TOPIC, str, 0, QOS, 0);
}
