import numpy as np
import sounddevice as sd

def generate_tone(frequencies, duration, sample_rate = 44100):
    t = np.linspace(0, duration, int(sample_rate * duration), endpoint = False)
    signal = np.zeros_like(t)
    
    for frequency in frequencies:
        signal += np.sin(2 * np.pi * frequency * t)
    
    signal /= len(frequencies)
    
    return signal

def play_tone_loop(signal, sample_rate=44100):
    while True:
        sd.play(signal, samplerate=sample_rate)
        sd.wait() 

frequencies = [71, 183, 260]

duration = 2.0

signal = generate_tone(frequencies, duration)

play_tone_loop(signal)

