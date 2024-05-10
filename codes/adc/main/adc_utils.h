#include "../../common/common.h"
#include "stdlib.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <inttypes.h>

#define APP_NAME_ADC "[ADC] "

#define ATTENUATION ADC_ATTEN_DB_11
#define WIDTH ADC_WIDTH_BIT_DEFAULT

static esp_adc_cal_characteristics_t adc1_chars;

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
void get_values_from_adc(float* array, int n){
    if(!array)
        ESP_LOGE(APP_NAME_ADC, "Invalid pointer!");
    else if(!n)
        ESP_LOGE(APP_NAME_ADC, "No samples!");
    else{
        for(int i = 0; i < n; i++){
            array[i] = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);
            ESP_LOGI(APP_NAME_ADC, "Sampled: %f mV at index %d", array[i], i);
         
            vTaskDelay(1000/portTICK_PERIOD_MS);
            // delay to define in order to achieve a different sampling frequency    
        }
    }

}