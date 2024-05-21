# Individual project #
Individual project for "IoT - Alogorithms and Services 2023-2024" course.

Lorenzo Pecorari - 1885161 - pecorari.1885161@studenti.uniroma1.it

The main goal of the project is to provide a system that should be able to collect information, compute an aggregate function and communicate the results to a server. Through sensors and the Fast Fourirer Transofrm, the microcontroller have to compute the maximum frequency of an unknown signal given as input in order to find the new sampling frequency and compute a new collection of data: the adaptation to this new frequency should allow to increase the energy saving. After computing a new collection of data basing on this new frequency, the ESP32-S3 have to send the new computation to a server using the WiFi connection.

## Hardware and Software
The required material for this project is the following:
<li>Espressif ESP32-S3 v3.1</li>
<li>Arduino Uno</li>
<li>Ina219</li>
<li>"Analog audio input" circuit</li>
<li>Jumper cables</li>
<li>2x USB Type-C cables</li>
<br>
Software, frameworks, libraries and services used for the handling the hardware are the following:
<li>FreeRTOS</li>
<li>ESP-IDF</li>
<li>ESP-DSC</li>
<li>HiveMQ</li>
<li>MQTT Explorer</li>
<li>Arduino IDE</li>

## Setting up everything
In order to make working the code contained into this repository, is needed to open an ESP-IDF terminal into the folder <code>codes/delivery</code> and executing the command <code>idf.py build</code>. After that, connect the ESP32 to the audio circuit in order to give power supply and same ground to this last one but the output pin conencted to the GPIO_NUM_1. Once that, execute <code>idf.py flash</code> and connect the audio connector to the dedicated output port of the device that will reproduce the signal to be given as input to the microcontroller. After starting to play the signal, it is possible to start to monitor what the ESP32 does through <code>idf.py monitor</code>. A visible output is returned to the user thanks to some prints on the terminal window.

For the energy consumption monitoring, is needed to connect the Ina219 between the power supply and the ESP32 itself while the pins to the equivalent ones on the Arduino Uno. The output of this microcontroller can be visualized using a serial monitor with baud rate 115200.

## Maximum sampling frequency
Since the device offers the possibility to capture analog signal from the external world, using the ADC (analog to digital converter) is possible to convert that signal into a flow of digital values. 

All the following functions are implemented into the <code>adc_fft_utils.h</code> file.

### Capturing the analog signal
The initialization of that tool is executed through the <code>init_adc()</code> function: it sets the unit #1 with an attenuation of 12 dB and the default bit value for the width. Those values are chosen in order to achieve a good accuracy for the input signal. 

Values are being detected using the <code>get_adc_values()</code> function. This one get the voltage from the configured pin (<code>GPIO_NUM_1</code>) and stores it into the variable <code>input</code>, an array of <code>SAMPLES</code> float values. This constant number of values corresponds to 1024.

Between each sample, the code throws a delay of a value of <code>duration</code> ms. The value of this variable depends on what is the sampling frequency adopted by the system; for the initial phase (oversampling) it is applied a frequency of 1000 Hz

### FFT - Fast Fourier Transform
After sampling the external input, the system computes the FFT for achieving the maximum frequency of the signal. 

In order to avoid borders discontinuity the signal may suffer, it is needed to generate and multiply the input signal for the Hann window before filling the variable that will be used for computing the FFT. The variable <code>output</code> is an array of <code> 2 * SAMPLES</code> float values: each of them will have the value of the previous multiplication if the index is even or zero otherwise. This choice was done in order to allow the library function to compute operation only on one real signal, given that the imaginary one is null.

The final result obtained from the library function <code>dsps_fft2r_fc32()</code> will be bit-reversed and transformed into two real signal (all into the same array).

For computing the power spectrum of the signal, the variable <code>final</code>, an array of <code>FINAL_SIZE</code> = <code>SAMPLES / 2</code> floats, will have, in dB<sub>10</sub>, the maximum of one of each value of the outputs array previously computed (into <code>output</code>).

### Computing the maximum frequency
In order to find relevant values, it is useful to determine the z-score value on each element of <code>final</code>. 

This mathematical-statistical tools allows to compute the "weight" of a specific value onto a distrubution of values. It is obtainable from the quotient between the difference of the value and the mean value and the standard deviation of that set of samples.The result of that computation returs if that value is an outlier or not.

After executing the function <code>max_freq_calc()</code>, the system finds the outlier value with higher frequency: in other words, the value with a relevant weight that has the higher value. That index, return value of <code>max_f_idx()</code>, will be divided by the value of <code>duration</code>.
 The follwing result is obtained by the formula that allows to find the sampling frequqncy:<br>

 $ \text{maximum frequency} = \left( \frac{\text{maximum index}}{\text{SAMPLES}} \right) \times \text{(sampling frequency)} = \left( \frac{\text{maximum index}}{\text{SAMPLES}} \right) \times \left( \frac{\text{SAMPLES}}{\text{duration}} \right) = \left( \frac{\text{maximum index}}{\text{duration}} \right)$ .

 That new frequency represents the maximum frequqncy of the input signal and, thanks to the Sampling theorem, it is possible to set the new sampling frequqncy to: <br>

$ \text {samping frequency} \geq 2 \times \text {maximum frequqncy}  = 2 \times\left( \frac{\text{maximum index}}{\text{duration}} \right)$ .

In order to avoid possible leaks during the sampling, the new sampling frequqncy adopted by the system is equal to 2,01 times that new maximum frequqncy found. Consequently, the new value of duration will be: <br>

$ \text {duration} = 2.01 \times \text {maximum frequqncy} $ .

### Aggregate function over a window
Once the optimal frequqncy is known, it can be sampled a new set of values that allows to save a certain amount of energy with respect to the oversampling situation. The aggregate function over a temporal window can now be computed using that and, given a time interval, the system will perform the collection of values. Using the function <code>aggregate_over_window()</code> it is possible to do that.

Starting from the time given as input to the function, the number of samples the system have to take, is obtained from the ceil of the quotient betweer the time expressed in seconds above the variable <code>duration</code>. Analogously, the ceil of those samples and <code>SAMPLES</code> returns the number of cycles needed to satisfy that window. With a double cycle, the system takes the input from ADC and uses an array to track the intermediate values computed over each cycle. In the end, the sum of those average values will be divided by the number of cycles.

## Communication with the nearby server

The locally computed values have to be send to an edge server using MQTT in a secure way.

The following cited functions are implemented into <code>mqtt_utils.h</code> and <code>wifi_utils.h</code>.

### Connecting to a WiFi router
The essential step for connecting to the server is inside the connection to a WiFi router that allows to connect over the Internet.

Most of events of that are related to it being caputered by the ad hoc function <code>event_handler()</code> that have the role to manage events and possible reconnection to the router. Inside the system, this function is used by two different variable that have the aim to trigger it once an event of WiFi or IP address occurs.

Through some library functions, the microcontroller tries to connect to the router in "station" mode, a way to behave as a client into the network, having its own MAC and IP address. It is also required to being authenticated using credentials if the router requires them.

### MQTT communication

As soon as the system is connected to the router, it will try to connect to the server that behaves as a broker. 

Using HiveMQ with an authenticated communication using credentials and a TLS certificate, the ESP32 is capable to communicate with the broker, connecting, subscribing and subscribing to a certain topic other than publishing and receiving messages. The certificate is manually given by using the command <code>openssl s_client -connect HOSTNAME.s1.eu.hivemq.cloud:PORT -showcerts > CERTIFICATE_NAME.txt</code>. The result stores information into a txt file and, from it, is needed to take the take the second certificate in it, representing the CA root certificate, and saving it into a file with one of the compatible extensions for certificates (ex. <code>pem</code>). After converting the certificate into a stream of bytes through the command <code>xxd -i CERTIFICATE_NAME.pem > CERTIFICATE_NAME.h</code>, the system retrieves it by the include into the file that needed it. In this project, the population of the fields <code>broker</code> and <code>credentials</code> of the structure <code>conf</codez>, which type is <code>esp_mqtt_client_config_t</code>, guarantees correct connection to the broker.

Through <code>start_mqtt()</code> the system can initializate the connection to the server and start the client, managing events with the function <code>esp_mqtt_client_init()</code>.

After the computation of the aggregate function into the previously called functions, it is possible to send those vales thanks to thr function <code>send_value_to_broker()</code>: it takes two variables as input and generate a string representing the message to send to the broker. More in detail, the message is published using the library function <code>esp_mqtt_client_publish()</code>.
