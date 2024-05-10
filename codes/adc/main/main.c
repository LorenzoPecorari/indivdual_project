#include "adc_utils.h"

#define N 1024

void app_main(void){
    float* wave = (float*) malloc(sizeof(float) * N);
    
    if(!wave)
        ESP_LOGE(APP_NAME_ADC, "Error during malloc!");
    else{
        characterize_adc1_ch_0();
        get_values_from_adc(wave, N);
        
        free(wave);
    }
}