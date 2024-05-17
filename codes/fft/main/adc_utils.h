// include section
#include "../../common/common.h"
#include "stdlib.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include <string.h>
#include <math.h>
#include <inttypes.h>

// define section
#define APP_NAME_ADC "[ADC] "

#define SAMPLES 1024
#define FINAL_SIZE (SAMPLES / 2)

#define ATTENUATION ADC_ATTEN_DB_12
#define WIDTH ADC_WIDTH_BIT_DEFAULT

// global vars
static esp_adc_cal_characteristics_t adc1_chars;

float duration = 1.0;
int n = SAMPLES;

// initialization of adc1 ch0
void characterize_adc1_ch_0(){
    ESP_LOGI(APP_NAME_ADC, "Initializing ADC_1 CH_0...");

    esp_adc_cal_characterize(ADC_UNIT_1, ATTENUATION, WIDTH, 0, &adc1_chars);
    ESP_LOGI(APP_NAME_ADC, "Characterization ADC_1 CH_0 : done");
    
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));
    ESP_LOGI(APP_NAME_ADC, "Channel width and configuration done");
}


// gets values from the adc and puts
// them into a buffer passed as argument of the function
void get_values_from_adc(){
    if(!array)
        ESP_LOGE(APP_NAME_ADC, "Invalid pointer!");
    else if(!n)
        ESP_LOGE(APP_NAME_ADC, "No samples!");
    else{
        int period = pdMS_TO_TICKS(duration);

        for(int i = 0; i < n; i++){
            array[i] = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);

            vTaskDelay(period);
        }

        ESP_LOGI(APP_NAME_ADC, "Sampling completed");
    }
}

// initialization of array and ADC_1 with channel 0
void adc_init(){
    array = (float*) malloc(sizeof(float) * SAMPLES);
    if(!array)
        ESP_LOGE(APP_NAME_ADC, "Array memory allocation error");
    else{
        characterize_adc1_ch_0();
    }
}
