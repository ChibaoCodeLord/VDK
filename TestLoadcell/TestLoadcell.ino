#include <Arduino.h>
#include "HX711.h"

// Chân kết nối HX711 với ESP8266
const int LOADCELL_DOUT_PIN = D5; // DOUT = GPIO14
const int LOADCELL_SCK_PIN  = D6; // SCK  = GPIO12

HX711 scale;

// Scale factor đã hiệu chuẩn
const float SCALE_FACTOR = 425.24;

void setup() {
  Serial.begin(115200);
  delay(500);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Đặt hệ số scale đã đo được
  scale.set_scale(SCALE_FACTOR);

  // Tare cân rỗng trước khi đo
  Serial.println("Taring scale... remove all weights!");
  delay(2000);
  scale.tare();
  Serial.println("Tare done. Ready to measure weight.");
}

void loop() {
  if (scale.is_ready()) {
    // Lấy trung bình 10 lần đọc
    float weight = scale.get_units(10);
    Serial.print("Weight: ");
    Serial.print(abs(weight), 2);
    Serial.println(" g");
  } else {
    Serial.println("HX711 not found. Check wiring!");
  }

  delay(1000);  // Cập nhật 1 lần/giây
  yield();      // Cho watchdog ESP8266 biết chương trình vẫn chạy
}
