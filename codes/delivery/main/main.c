#include "adc_fft_utils.h"
#include "wifi_utils.h"
#include "mqtt_utils.h"

void app_main(void){
    init_adc();
    get_adc_values();
        
    fft_init();
    fft();

    printf("current frequency: %f - current duration: %f\n", (SAMPLES / duration), duration);
    max_freq_calc();

    printf("new frequency: %f - new duration: %f\n", (SAMPLES / duration), duration);

    float k = aggregate_over_window(5.0);

    wifi_init_main();
    start_mqtt();

    subscribe_to_topic();
    send_value_to_broker(k);
}