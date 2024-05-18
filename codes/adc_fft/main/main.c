#include "adc_fft_utils.h"

void app_main(void){
    get_adc_values();
    fft();
    max_freq_calc();
}