int JoyStick_x = A1;  
int JoyStick_y = A0;  
int button = 2;      

// Các chân LED
int ledUp = 7;
int ledDown = 6;
int ledLeft = 4;
int ledRight = 5;

bool stateUp = false;
bool stateDown = false;
bool stateLeft = false;
bool stateRight = false;

void setup() {
  pinMode(JoyStick_x, INPUT);
  pinMode(JoyStick_y, INPUT);
  pinMode(button, INPUT_PULLUP);

  pinMode(ledUp, OUTPUT);
  pinMode(ledDown, OUTPUT);
  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  int x = analogRead(JoyStick_x);
  int y = analogRead(JoyStick_y);
  int z = digitalRead(button);

  Serial.print("X=");
  Serial.print(x);
  Serial.print(", Y=");
  Serial.print(y);
  Serial.print(", Btn=");
  Serial.println(z);

  if (y > 1000) {
    stateUp = true;
  } else if (y < 100) {
    stateDown = true;
  }

  if (x > 1000) {
    stateRight = true;
  } else if (x < 100) {
    stateLeft = true;
  }

  if (z == LOW) {
    stateUp = stateDown = stateLeft = stateRight = false;
  }

  digitalWrite(ledUp, stateUp ? HIGH : LOW);
  digitalWrite(ledDown, stateDown ? HIGH : LOW);
  digitalWrite(ledLeft, stateLeft ? HIGH : LOW);
  digitalWrite(ledRight, stateRight ? HIGH : LOW);

  delay(100);
}
