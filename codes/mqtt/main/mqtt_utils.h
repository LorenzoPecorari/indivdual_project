
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

int RESPONSE_BIT = 0;


void response_to_msg(esp_mqtt_client_handle_t client){
    if(!RESPONSE_BIT){
        const char* message = "Ping pong!";
        int id_message = esp_mqtt_client_publish(client, TOPIC, message, 0, QOS, 0);
        ESP_LOGI(APP_NAME_MQTT, "Response: %s - id_message: %d", message, id_message);
    }

    RESPONSE_BIT = !RESPONSE_BIT;
}

void mqtt_event_handler(void* args, esp_event_base_t base, int32_t id, void* data){
    ESP_LOGI(APP_NAME_MQTT, "Triggered event - id: %" PRIi32 "\t base: %s", id, base);
    
    esp_mqtt_event_handle_t event = data;
    esp_mqtt_client_handle_t client = event->client;

    int id_message = 0;

    if(id == MQTT_EVENT_CONNECTED){
        id_message = esp_mqtt_client_subscribe(client, TOPIC, QOS);
        ESP_LOGI(APP_NAME_MQTT, "Subscription - id_message: %d", id_message);

        //id_message = esp_mqtt_client_unsubscribe(client, TOPIC);
        //ESP_LOGI(APP_NAME_MQTT, "Unsubscription - id_message: %d", id_message);
    }
    else if(id == MQTT_EVENT_DISCONNECTED){
        ESP_LOGI(APP_NAME_MQTT, "Disconnected");
    }
    else if(id == MQTT_EVENT_SUBSCRIBED){
        id_message = esp_mqtt_client_publish(client, TOPIC, "ESP32-S3 subscribed!\0", 0, QOS, 0);
        ESP_LOGI(APP_NAME_MQTT, "Publish - id_message: %d", id_message);
    }
    else if(id == MQTT_EVENT_UNSUBSCRIBED){
        ESP_LOGI(APP_NAME_MQTT, "Unsubscribed - id_message* : %d", event->msg_id);
    }
    else if(id == MQTT_EVENT_PUBLISHED){
        ESP_LOGI(APP_NAME_MQTT, "Published %s- id_message* : %d", event->data, event->msg_id);
    }
    else if(id == MQTT_EVENT_DATA){
        event->data[event->data_len] = '\0';
        ESP_LOGI(APP_NAME_MQTT, "Topic: %s - Data: %s", event->topic, event->data);

        // ESP_LOGI(APP_NAME_MQTT, "Data received -Topic: %s\n\t\tData : %s\n", event->topic, event->data);
        response_to_msg(client);
    }
    else if(id == MQTT_EVENT_ERROR){
        ESP_LOGI(APP_NAME_MQTT, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGI(APP_NAME_MQTT, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(APP_NAME_MQTT, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(APP_NAME_MQTT, "Last captured errno : %d (%s)",  event->error_handle->esp_transport_sock_errno, strerror(event->error_handle->esp_transport_sock_errno));
        } 
        else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGI(APP_NAME_MQTT, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        }
    }
    else{
        ESP_LOGI(APP_NAME_MQTT, "Other event, %ld", id);
    }
}

void start_mqtt(void){
    ESP_LOGI(APP_NAME_MQTT, "Starting mqtt configuration...");

    esp_mqtt_client_config_t conf = {
        .broker = {
            .address.uri = URI,
            .verification.certificate = (const char*) broker_cert_pem,
            //.verification.certificate = 0 // CHECK IT!!
        },
        
        .credentials.username = CREDENTIAL_USERNAME,
        .credentials.authentication.password = CREDENTIAL_PWD,
    };

    ESP_LOGI(APP_NAME_MQTT, "Configuration structure variable populated");

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&conf);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    
    ESP_LOGI(APP_NAME_MQTT, "Client handler registered");

    esp_mqtt_client_start(client);
}

