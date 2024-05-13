#include "wifi_main.h"
#include "mqtt_utils.h"

void app_main(void){
    ESP_LOGI(APP_NAME_MQTT, "Main execution...");
    
    esp_netif_init();
    wifi_init_main();
    ESP_LOGI(APP_NAME, "Ciao!");
    start_mqtt();
}