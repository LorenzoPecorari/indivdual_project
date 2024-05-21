#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }

  uint32_t currentFrequency;
  ina219.setCalibration_16V_400mA();

  if (!ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  Serial.println("Measuring voltage and current with INA219 ...");
}

void loop(void) {

  float power_mW = 0;

  power_mW = ina219.getPower_mW();
  
  Serial.print("Power:         ");
  Serial.print(power_mW);
  Serial.println(" mW");
  Serial.println("");

  delay(500);
}
