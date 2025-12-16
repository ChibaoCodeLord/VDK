#include "HX711.h"

const int DOUT = 5;     
const int HX_SCK  = 6;  

HX711 scale;
float calibration_factor = 108150.5;

void setup() {
  Serial.begin(9600); // Arduino TX ra ESP
  scale.begin(DOUT, HX_SCK);
  scale.set_scale(calibration_factor);
  scale.tare();
}

void loop() {
  if (scale.is_ready()) {
    float weight = scale.get_units(10);
    Serial.println(weight); // in ra \n cho ESP8266 đọc
  } else {
    Serial.println("HX711 not ready");
  }
  delay(200);
}
