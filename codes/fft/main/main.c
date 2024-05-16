#include "fft_utils.h"

void app_main(void){
    adc_init();
    test_init_array();
    //get_values_from_adc();

    fft_init();
    //input_populating(); // TEST PURPOSES ONLY FUNCTION!
    hann_multiplication();
    fft();
}