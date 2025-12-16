int leds[10] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

void setup() {
  for (int i = 0; i < 10; i++) {
    pinMode(leds[i], OUTPUT);
  }
}

void loop() {
  // quét 0 → 359 độ để mô phỏng sóng sin
  for (int deg = 0; deg < 360; deg += 5) {
    float rad = deg * 3.14159 / 180.0;   // đổi độ sang radian
    float sinVal = (sin(rad) + 1.0) / 2; // chuẩn hóa 0 → 1

    // tính số LED cần sáng theo giá trị sin
    int numOn = (int)(sinVal * 10); // từ 0 → 10

    // bật LED tương ứng
    for (int i = 0; i < 10; i++) {
      if (i < numOn) {
        digitalWrite(leds[i], HIGH);
      } else {
        digitalWrite(leds[i], LOW);
      }
    }
    delay(50); // tốc độ sóng
  }
}
