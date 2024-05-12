#include "adc_utils.h"

#define N 1024

void app_main(void){
    array = (float*) malloc(sizeof(float) * N);
    
    if(!array)
        ESP_LOGE(APP_NAME_ADC, "Error during malloc!");
    else{
        duration = (float) (0.01 / portTICK_PERIOD_MS);
        n = N;
        printf("Sampling frequency : %f\n", (float) (1 / duration));

        characterize_adc1_ch_0();
        get_values_from_adc();

        free(array);
    }
}