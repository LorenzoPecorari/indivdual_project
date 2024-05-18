#include "adc_fft_utils.h"

void app_main(void){
    init_adc();
    get_adc_values();
        
    fft_init();
    fft();

    printf("current frequency: %f - current duration: %f\n", (SAMPLES / duration), duration);
    max_freq_calc();

    printf("new frequency: %f - new duration: %f\n", (SAMPLES / duration), duration);

    float k = aggregate_over_window(5.0);

}