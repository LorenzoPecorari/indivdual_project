#include "adc_fft_utils.h"

void app_main(void){
    get_adc_values();
    fft();

    printf("current frequency: %f - current duration: %f\n", (SAMPLES / duration), duration);
    max_freq_calc();

    printf("current frequency: %f - current duration: %f\n", (SAMPLES / duration), duration);
    fft();
    //max_freq_calc();

    float k = aggregate_over_window(5.0);

}