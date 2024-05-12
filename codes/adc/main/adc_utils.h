#include "../../common/common.h"
#include "stdlib.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include <math.h>
#include <inttypes.h>

#define APP_NAME_ADC "[ADC] "

#define ATTENUATION ADC_ATTEN_DB_11
#define WIDTH ADC_WIDTH_BIT_DEFAULT

static esp_adc_cal_characteristics_t adc1_chars;

float* array;
int duration;
int n;

float* outliers;

float max = 0.0;
float mean = 0.0;
float std_dev = 0.0;
float threshold = 3.0;

// initialization of adc1 ch0
void characterize_adc1_ch_0(){
    ESP_LOGI(APP_NAME_ADC, "Initializing ADC_1 CH_0...");

    esp_adc_cal_characterize(ADC_UNIT_1, ATTENUATION, WIDTH, 0, &adc1_chars);
    ESP_LOGI(APP_NAME_ADC, "Characterization ADC_1 CH_0 : done");
    
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));
    ESP_LOGI(APP_NAME_ADC, "Channel width and configuration done");
}

// calculates standard deviation of the sampled values
float std_dev_calc(){
    float std_dev = 0;

    for(int i = 0; i < n; i++){
        std_dev += pow(array[i] - mean, 2);
    }

    std_dev = sqrt(std_dev / n);

    return std_dev;
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
            //ESP_LOGI(APP_NAME_ADC, "Sampled: %f mV at index %d", array[i], i);
         
            mean += array[i];

            if(array[i] > max)
                max = array[i];
            
            vTaskDelay(period);
            // delay to define in order to achieve a different sampling frequency    
        }

        mean = (float) (mean / n);
        std_dev = std_dev_calc();
        printf("Max: %f - Average : %f - Stadnard dev: %f - threshold; %f\n", max, mean, std_dev, threshold);

        ESP_LOGI(APP_NAME_ADC, "Sampling completed");
    }
}

void outliers_calc(){
    outliers = (float*) malloc(sizeof(float) * (n / 2));

    if(!outliers)
        ESP_LOGE(APP_NAME_ADC, "Outliers memory allocation error");
    
    int idx = 0;
    float z = 0.0;

    for(int i = 0; i < n && idx < (n / 2); i++){
        z = (array[i] - mean) / std_dev;

        if((z > threshold) || (z < (-threshold))){
            outliers[idx] = array[i];
            ESP_LOGI(APP_NAME_ADC, "Outlier %d : %f", idx, outliers[idx]);
            idx++;
        }
    }

    free(outliers);
}


/*
    === NOTES FOR FFT ===

# New duration of delay:
    Since duration gives the information on how many ticks are needed to wait for sampling with a certain frequency,
    the sampling frequency to use for a more accurate approximation of the signal have to be 2 * f_max.
    The new duration to pass to the adc function for sampling the signal have to be duration = 1 / f_sampling =
    = 1 / (2 * f_max) = 1 / (2 * idx * n / (n * duration)) = 1 / (2 * idx / duration) = duration / (2 * duration)

# Trashold value:
    Obtaining a relevant value for the calculation of the max frequency to use for finding the new sampling frequqncy
    can be achieved through the z-score; calculating the z-score of a value tells if it is an outlier or not.

    z = (value - mean) / std_dev -> if z > threshold so value is an outlier

    Theshold value is defined as the mean value plus/minus 3 scale order of standard deviation as base point (statistical approach).
*/