#include "adc_utils.h"

#define N 1024

void app_main(void){
    array = (float*) malloc(sizeof(float) * N);
    
    if(!array)
        ESP_LOGE(APP_NAME_ADC, "Error during malloc!");
    else{
        // duration = 1000 / (float) portTICK_PERIOD_MS;
        // printf("Duration: %f - %fs\n", (duration / 100), (float) portTICK_PERIOD_MS);
        // printf("Sampling frequency : %f Hz\n", (float) (N * 10 / duration));

        duration = 1;
        printf("Duration: %d (ticks) - Sampling frequency : idk (1 / ticks)\n", duration);
        
        n = N;

        characterize_adc1_ch_0();
        get_values_from_adc();

        outliers_calc();

        free(array);
    }
}