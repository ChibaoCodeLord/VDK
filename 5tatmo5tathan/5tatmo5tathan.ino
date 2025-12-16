// 8 LED lần lượt từ chân 4 đến 11
int leds[8] = {11, 5, 6, 10, 8, 9, 7, 4};     // nút bấm
bool cut = false;      // cờ báo nhấn nút

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(leds[i], OUTPUT);
  }

  attachInterrupt(0, setCut, RISING);
}

void loop() {
  if (cut) {
    fadeFirstFour();    // 4 LED đầu mờ dần 2 s
    offLastFour();      // 4 LED sau tắt ngay
    while (true);       // dừng hẳn chương trình
  }

  // Hiệu ứng qua lại
  for (int i = 0; i < 8; i++) {
    digitalWrite(leds[i], HIGH);
    delay(50);
    digitalWrite(leds[i], LOW);
  }
  for (int i = 6; i > 0; i--) {
    digitalWrite(leds[i], HIGH);
    delay(50);
    digitalWrite(leds[i], LOW);
  }
}

// ======= HÀM NGẮT =======
void setCut() {
  cut = true;  // chỉ đặt cờ, không làm gì nặng
}

// Làm 4 LED đầu (leds[0..3]) mờ dần trong 2 giây
void fadeFirstFour() {
  const int fadeTime = 2000; // 2 s
  const int steps = 50;      // số bước giảm
  for (int level = 255; level >= 0; level -= 255/steps) {
    for (int i = 0; i < 4; i++) {
      analogWrite(leds[i], level); // PWM để giảm sáng
    }
    delay(fadeTime / steps);
  }
  
}

// Tắt 4 LED cuối (leds[4..7]) ngay lập tức
void offLastFour() {
  for (int i = 4; i < 8; i++) {
    digitalWrite(leds[i], LOW);
  }
}
