int ALARM = 4;        // Chân digital số 4 điều khiển buzzer
int LM35 = 0;         // Chân analog A0 đọc tín hiệu LM35
float tmpVal = 0;     // Biến lưu nhiệt độ
float sinVal;         // Biến trung gian tính sin
int toneVal;          // Biến lưu tần số âm thanh buzzer
unsigned long tepTimer; // Biến đếm thời gian

void setup(void) {
  Serial.begin(9600);      // Giao tiếp Serial tốc độ 9600 bps
  pinMode(ALARM, OUTPUT);  // Chân buzzer là ngõ ra
}

void loop(void) {
  int val = analogRead(LM35);              // Đọc giá trị từ LM35 (0 - 1023)
  float voltage = val * (5.0 / 1024.0);    // Đổi về V
  tmpVal = (float) voltage / 0.01;

  if (tmpVal > 27) {
    // Nếu nhiệt độ lớn hơn 27 độ thì phát âm thanh
    for (int x = 0; x < 180; x++) {
      sinVal = (sin(x * (3.1412 / 180)));
      toneVal = 2000 + (int(sinVal * 1000));
      tone(ALARM, toneVal);   // Phát âm tần số toneVal tại chân 4
      delay(2);               // Trì hoãn một chút để tạo sóng
    }
  } else {
    noTone(ALARM);  // Nếu nhiệt độ <= 27, tắt buzzer
  }

  if (millis() - tepTimer > 500) {   // Mỗi 500 ms
    tepTimer = millis();             // Cập nhật lại thời gian
    Serial.print("temperature: ");
    Serial.print(tmpVal);
    Serial.println("C");
  }
}
