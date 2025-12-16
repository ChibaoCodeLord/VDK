const int trigPin = 9;     
const int echoPin = 10;    
const int buzzerPin = 4;   

long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // --- Đo khoảng cách ---
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // --- Kiểm tra khoảng cách ---
  if (distance >= 10 && distance <= 20) {
    for (int x = 0; x < 180; x++) {
      float sinVal = sin(x * (3.1416 / 180.0));     
      int toneVal = 2000 + int(sinVal * 1000);      
      tone(buzzerPin, toneVal);                    
      delay(2);                                    
    }
  } else {
    noTone(buzzerPin); 
  }

  delay(50); 
}



