int leds[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};  // 5 LED trên các chân PWM/digital
int potPin = A0;                 // biến trở nối vào A0
int delayTime = 0;               // thời gian delay giữa các LED

void setup() {
  for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i++) {
    pinMode(leds[i], OUTPUT);
  }
}

void loop() {
  // đọc biến trở để lấy tốc độ (0 - 1023) -> chuyển sang delay (0 - 500 ms)
  delayTime = analogRead(potPin) / 4;

  // chạy LED từ trái sang phải
  for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i++) {
    digitalWrite(leds[i], HIGH);   // bật LED i
    delay(delayTime);              // dừng theo tốc độ
    digitalWrite(leds[i], LOW);    // tắt LED i
  }
  for (int i = sizeof(leds) / sizeof(leds[0]) - 2; i > 0; i--) {
    digitalWrite(leds[i], HIGH);
    delay(delayTime);
    digitalWrite(leds[i], LOW);
  }
}
