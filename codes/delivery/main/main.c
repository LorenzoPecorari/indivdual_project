#include "adc_fft_utils.h"
#include "wifi_utils.h"
#include "mqtt_utils.h"

void app_main(void){
    wifi_init_main();
    start_mqtt();

    subscribe_to_topic();

    init_adc();
    get_adc_values();
        
    fft_init();
    fft();

    float k = aggregate_over_window(5.0);
    send_value_to_broker(k, "(oversampling) : ");
    
    printf("current frequency: %f - current duration: %f\n", (SAMPLES / duration), duration);
    max_freq_calc();
    printf("new frequency: %f - new duration: %f\n", (SAMPLES / duration), duration);

    k = aggregate_over_window(5.0);
    send_value_to_broker(k, "(optimal) : ");
    
    unsubscribe_to_topic();
    vTaskDelay(2500 / portTICK_PERIOD_MS);
    wifi_termination();

    // CODE FOR ENERGY CONSUMPTION
    // init_adc();
    // fft_init();
    
    // for(int i = 0; i < 20; i++)
    //     get_adc_values();

    // vTaskDelay(2500 / portTICK_PERIOD_MS);
    // fft();
    // max_freq_calc();

    // vTaskDelay(2500 / portTICK_PERIOD_MS);
    // for(int i = 0; i < 20; i++)
    //     get_adc_values();

}