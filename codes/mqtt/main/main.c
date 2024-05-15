#include "wifi_main.h"
#include "mqtt_utils.h"

void app_main(void){
    ESP_LOGI(APP_NAME_MQTT, "Main execution...");
    
    wifi_init_main();
    start_mqtt();

    // do some stuff...
}