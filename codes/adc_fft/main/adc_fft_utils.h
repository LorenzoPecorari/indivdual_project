#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#include "esp_dsp.h"


#define APP_NAME_ADC "[ADC] "
#define APP_NAME_FFT "[FFT] "

#define ATTENUATION ADC_ATTEN_DB_12
#define WIDTH ADC_WIDTH_BIT_DEFAULT

#define SAMPLES 1024
#define FINAL_SIZE (SAMPLES/2)

float duration = 1.0;
int period = pdMS_TO_TICKS(1.0);

float mean = 0.0;
float std_dev = 0.0;
float t = 3.0;

int N = SAMPLES;

float input[SAMPLES];

float window[SAMPLES];
float output[SAMPLES * 2];

float final[SAMPLES / 2];

static esp_adc_cal_characteristics_t adc1_chars;

// gets values from ADC using unit 1 and channel 0
void get_adc_values(){
    ESP_LOGI(APP_NAME_ADC, "Initializing ADC_1 CH_0...");

    esp_adc_cal_characterize(ADC_UNIT_1, ATTENUATION, WIDTH, 0, &adc1_chars);
    ESP_LOGI(APP_NAME_ADC, "Characterization ADC_1 CH_0 : done");
    
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_DEFAULT));
    ESP_ERROR_CHECK(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11));
    ESP_LOGI(APP_NAME_ADC, "Channel width and configuration done");

    for(int i = 0; i < SAMPLES; i++){
        input[i] = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc1_chars);
        //printf("Value %d: %f\n", i, input[i]);
        vTaskDelay(period);
    }

}

// computes mean and standard deviation,
// preparation for z-score
void mean_and_std_dev(){
    for(int i = 0; i < FINAL_SIZE; i++){
        mean = mean + final[i];
    }

    mean = mean / FINAL_SIZE;

    for(int i = 0; i < FINAL_SIZE; i++){
        std_dev = std_dev + pow(final[i] - mean, 2);
    }

    std_dev = sqrt(std_dev / FINAL_SIZE);
}

// returns if a value is an outlier or not
int is_outlier(float v){
    float z = (v - mean) / std_dev;

    if(z > t)
        return 1;

    return 0;
}

// return the index with max frequency and relevant magnitude
int max_f_idx(){
    int idx = 0;

    for(int i = 0; i < FINAL_SIZE; i++){
        if(is_outlier(final[i])){
            //printf("outlier %d - value %f\n", i, final[i]);
            idx = i;
        }
    }

    return idx;
}

// initializes variables for computing fft
void fft_init(){
    if (dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE) != ESP_OK) {
        ESP_LOGE(APP_NAME_FFT, "FFT init error");
        return;
    }
}

// main fft function, multiplies for window, computes fft and power spectrum
void fft(){
    
    fft_init();
    ESP_LOGI(APP_NAME_FFT, "Initialization done");

    dsps_wind_hann_f32(window, SAMPLES);
    ESP_LOGI(APP_NAME_FFT, "Hann window generated");

    get_adc_values();
    ESP_LOGW(APP_NAME_ADC, "Values sampled");

    for (int i = 0 ; i < SAMPLES ; i++) {
        output[i * 2 + 0] = input[i] * window[i];
        output[i * 2 + 1] = 0.0;
    }

    dsps_fft2r_fc32(output, SAMPLES);
    ESP_LOGI(APP_NAME_FFT, "Intermediate result computed");

    dsps_bit_rev_fc32(output, SAMPLES);
    dsps_cplx2reC_fc32(output, SAMPLES);

    float temp_1 = 0.0;
    float temp_2 = 0.0;
    int j = 0;

    // power spectrum computation
    for (int i = 0 ; i < FINAL_SIZE; i++) {
        j = (i + SAMPLES);
        temp_1 = 10 * log10f((output[i * 2 + 0] * output[i * 2 + 0] + output[i * 2 + 1] * output[i * 2 + 1]) / SAMPLES);
        temp_2 = 10 * log10f((output[j * 2 + 0] * output[j * 2 + 0] + output[j * 2 + 1] * output[j * 2 + 1]) / SAMPLES);

        //printf("y1: %f - y2: %f\n", y1_cf[i], y2_cf[i]);
        // Simple way to show two power spectrums as one plot
        final[i] = fmax(temp_1, temp_2);
    }

    ESP_LOGW(APP_NAME_FFT, "Final result computed");

    // shows the final result
    dsps_view(final, FINAL_SIZE, 64, 10,  -60, 120, '|');
}

// calculates maximum frequency and sampling frequqncy (sampling theorem application)
void max_freq_calc(){
    mean_and_std_dev();
    ESP_LOGI(APP_NAME_FFT, "Mean: %f - Standard dev: %f", mean, std_dev);

    int max_idx = max_f_idx();

    float max_f = max_idx / duration; // (maxi_idx / SAMPLES) * (SAMPLES / duration) = max_idx / duration 
    
    ESP_LOGW(APP_NAME_FFT, "Index: %d - Max frequency: %f -> New ampling frequency : %f", max_idx, max_f, (2 * max_f));
    ESP_LOGI(APP_NAME_FFT, "Secure sampling frequency: %f", max_f * 2.1);
}