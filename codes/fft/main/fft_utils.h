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
float window[SAMPLES];
float output[2 * SAMPLES];

float final[SUM_SIZE];

// generator of hann window
void window_gen(){
    dsps_wind_hann_f32(window, SAMPLES);
    ESP_LOGI(APP_NAME_FFT, "Hann window generated");
}

// test purposes function only
// generates a random signal
void input_populating(){
    dsps_tone_gen_f32(input, SAMPLES, WAVE_AMPLITUDE, WAVE_FREQUENCY, WAVE_PHASE);
    ESP_LOGI(APP_NAME_FFT, "Input test wave generated");
}

// multiplies the hann window for the input signal;
// only the first part of the "output" is being populated
// for handling one input signal
void hann_multiplication(){
    for(int i = 0; i < SAMPLES; i++){
        output[i * 2 + 0] = input[i] * window[i];
        output[i * 2 + 1] = 0.0;
    }
    
    ESP_LOGI(APP_NAME_FFT, "Output populating completed");
}

// computes the maximum frequency of a relevant value of the fft
// (it is based on the concept of z-score)
float compute_maximum_frequency_index(){
    int idx = 0;
    for(int i = 0; i < SUM_SIZE; i++)
        if(final[i] > 0.0){
            printf("ampl: %f, idx: %d\n", final[i], i);
            idx = i;
        }

    printf("Index of max freq: %d\n", idx);

    float ret = (float) ((float )idx / (float) SUM_SIZE);
    printf("intermediate ret: %f\n", ret);
    ret = (ret * (SAMPLES / duration));
    printf("intermediate ret: %f\n", ret);

    return ret;
}

// computes the fft of the signal and plots it through the shell
void fft(){
    // fft radix 2, function from library with O(N * log N) cost
    dsps_view(input, SAMPLES, 64, 10, -128, 64, '|');

    dsps_fft2r_fc32(output, SAMPLES);
    
    // reverses bits and generates a single array
    // of values from the two comples generated
    dsps_bit_rev_fc32(output, SAMPLES);
    dsps_cplx2reC_fc32(output, SAMPLES);

    float temp_1 = 0.0;
    float temp_2 = 0.0;
 
    int j = 0;
    
    for(int i = 0; i < SUM_SIZE; i++){
        j = i + SUM_SIZE;

        // power spectrum of the output coming from the fft (dB_10)
        temp_1 = 10 * log10f(((output[(i * 2) + 0] * output[(i * 2) + 0]) + (output[(i * 2) + 1] * output[(i * 2) + 1])) / SAMPLES);
        temp_2 = 10 * log10f(((output[(j * 2) + 0] * output[(j * 2) + 0]) + (output[(j * 2) + 1] * output[(j * 2) + 1])) / SAMPLES);

        // takes the greater value from the two computed
        if(temp_1 < temp_2)
            final[i] = temp_2;
        else
            final[i] = temp_1;
    }
    
    ESP_LOGI(APP_NAME_FFT, "FFT computed");
    //for(int i = 0; i < SUM_SIZE; i++)
        //printf("magnitude: %f, idx: %d\n", final[i], i);

    // arguments: buffer, buffer size, # bins, min showed, max showed, char for viewing
    ESP_LOGI(APP_NAME_FFT, "Maximum frequency: %f", compute_maximum_frequency_index());

    dsps_view(final, SUM_SIZE, 64, 10, -128, 64, '|');
    ESP_LOGI(APP_NAME_FFT, "FFT graph view computed");

    outliers_finder_test(final, SUM_SIZE);
}

void fft_init(){
    input = array;
    
    if(dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE) != ESP_OK){
        ESP_LOGE(APP_NAME_FFT, "Not possible to initialize FFT, error");
        return;
    }

    window_gen();
}

/*
    === NOTES ===

    # analysis of maximum frequency:
        in order to achieve the maximum frequency, find the outlier value with higher frequency,
        once that it is possible to compute the new sampling frequency

*/