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
    int volume_oversampling = send_value_to_broker(k, "(oversampling) : ");
    printf("Latency %lld ms\n", end_time - start_time);

    printf("current frequency: %f - current duration: %f\n", (SAMPLES / duration), duration);
    max_freq_calc();
    printf("new frequency: %f - new duration: %f\n", (SAMPLES / duration), duration);

    k = aggregate_over_window(5.0);
    int volume_optimal = send_value_to_broker(k, "(optimal) : ");
    
    printf(" Volume with oversampling frequency: %d B \n Volume with optimal frequency: %d B \n Delta optimal-oversampling: %d B \n", volume_oversampling, volume_optimal, (volume_optimal - volume_oversampling));

    unsubscribe_to_topic();
    vTaskDelay(2500 / portTICK_PERIOD_MS);
    wifi_termination();

    // CODE FOR ENERGY CONSUMPTION
    /*
    init_adc();
    fft_init();
    
    for(int i = 0; i < 20; i++)
        get_adc_values();

    vTaskDelay(2500 / portTICK_PERIOD_MS);
    fft();
    max_freq_calc();

    vTaskDelay(2500 / portTICK_PERIOD_MS);
    for(int i = 0; i < 20; i++)
        get_adc_values();

    */

}