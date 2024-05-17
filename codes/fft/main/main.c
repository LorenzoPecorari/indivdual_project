#include "fft_utils.h"

void app_main(void){

    // ADC part
    adc_init();
    //test_init_array();
    get_values_from_adc();

    // FFT part
    fft_init();
    //input_populating(); // TEST PURPOSES ONLY FUNCTION!
 
    fft();
}