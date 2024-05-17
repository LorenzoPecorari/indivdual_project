// include section
#include "../../common/common.h"
#include "adc_utils.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "esp_system.h"
#include "esp_dsp.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/uart.h"

#include "soc/gpio_struct.h"
#include "soc/uart_struct.h"

// define section
#define APP_NAME_FFT "[FFT] "

// test purposes defines only
#define WAVE_AMPLITUDE 1.0
#define WAVE_FREQUENCY 0.16
#define WAVE_PHASE 0.0

// global vars
float* input;
float output[2 * SAMPLES];

float final[FINAL_SIZE];

int max_idx = 0;
float max_freq = 0.0;

// variables for z-score calculation
float mean = 0.0;
float std_dev = 0.0;
float threshold = 3.0;

// calculates standard deviation of the sampled values
void std_dev_calc(){
    for(int i = 0; i < FINAL_SIZE; i++)
        std_dev += pow(final[i] - mean, 2);

    std_dev = sqrt(std_dev / FINAL_SIZE);
}

// using z-score, it computes if the value is an outlier
int outlier_calc(float value){
    float z = (value - mean) / std_dev;

    if(z > threshold)
        return 1;
    else
        return 0;
}

int max_outlier_finder(){
    int max_idx = 0;

    for(int i = 0; i < FINAL_SIZE; i++){
        if(outlier_calc(final[i])){
            max_idx = i;
        }
    }

    return max_idx;
}

// generates hann window and multiplies it for the input signal;
// only the first part of the "output" is being populated
// for handling one input signal

void window_gen(){
    
    float* window = malloc(sizeof(float) * SAMPLES);
    dsps_wind_hann_f32(window, SAMPLES);

    for(int i = 0; i < SAMPLES; i++){
        output[i * 2 + 0] = input[i] * window[i];
        output[i * 2 + 1] = 0.0;
    }
    
    free(window);

    ESP_LOGI(APP_NAME_FFT, "Hann window generated");
}

void input_populating(){
    dsps_tone_gen_f32(input, SAMPLES, WAVE_AMPLITUDE, WAVE_FREQUENCY, WAVE_PHASE);
    ESP_LOGI(APP_NAME_FFT, "Input test wave generated");
}


// computes the fft of the signal and plots it through the shell
void fft(){

    dsps_fft2r_fc32(output, SAMPLES);
    
    // reverses bits and generates a single array
    // of values from the two comples generated
    dsps_bit_rev_fc32(output, SAMPLES);
    dsps_cplx2reC_fc32(output, SAMPLES);

    float temp_1 = 0.0;
    float temp_2 = 0.0;
 
    int j = 0;
    
    for(int i = 0; i < FINAL_SIZE; i++){
        j = i + FINAL_SIZE;

        // power spectrum of the output coming from the fft (dB_10)
        temp_1 = 10 * log10f(((output[(i * 2) + 0] * output[(i * 2) + 0]) + (output[(i * 2) + 1] * output[(i * 2) + 1])) / SAMPLES);
        final[i] = temp_1;

        mean = mean + final[i];

    }
    
    mean = mean / FINAL_SIZE;
    dsps_view(final, FINAL_SIZE, 64, 10, -10, 90, '|');

    std_dev_calc();
    max_idx = max_outlier_finder();

    max_freq = (max_idx * 20 / duration);
    ESP_LOGI(APP_NAME_FFT, "max_frequency: %f\n", max_freq);

}

void fft_init(){
    input = array;
    
    if(dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE) != ESP_OK){
        ESP_LOGE(APP_NAME_FFT, "Not possible to initialize FFT, error");
        return;
    }

    window_gen();
}
