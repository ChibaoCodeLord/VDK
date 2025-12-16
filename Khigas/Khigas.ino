int gasPin = A0;       // MQ-2 nối vào chân analog A0
int buzzerPin = 9;     // Buzzer dùng chân số 9

void setup() {
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int gasValue = analogRead(gasPin); // đọc giá trị MQ-2
  Serial.println(gasValue);

  if (gasValue > 500) {
    // phát âm cảnh báo dạng "rung"
    for (int x = 0; x < 180; x++) {
      float s = sin(x * PI / 180);         // sin trong khoảng -1..1
      int freq = 1000 + (int)(500 * s);    // tần số dao động từ 500Hz -> 1500Hz
      tone(buzzerPin, freq);
      delay(3);                            // tốc độ dao động
    }
  } else {
    noTone(buzzerPin);  // tắt buzzer khi an toàn
  }
}
