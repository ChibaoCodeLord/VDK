int leds[5] = {3, 5, 6, 9, 10};
int pin = A0;   // nên dùng A0 thay vì 0 cho chân analog
int value = 0;

void setup() {
  // Cấu hình tất cả chân LED là OUTPUT
  for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i++) {
    pinMode(leds[i], OUTPUT);
  }
}

void loop() {
  value = analogRead(pin);   // đọc biến trở (0 - 1023)
  value /= 4;                // chuyển sang 0 - 255

  // Xuất PWM cho tất cả LED
  for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i++) {
    analogWrite(leds[i], value);
  }
}
