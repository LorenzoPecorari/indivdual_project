#include "fft_utils.h"

void app_main(void){

    // configuration and initialization for fft radix two, allocation of needed memory
    if(dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE) != ESP_OK){
        ESP_LOGE(APP_NAME_FFT, "Not possible to initialize FFT. Error");
        return;
    }

    window_gen();
    input_populating();
    hann_multiplication();
    fft();
}