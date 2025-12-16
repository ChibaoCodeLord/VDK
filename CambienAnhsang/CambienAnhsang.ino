int LED = 13;
int LIGHT = 0;
int val = 0;

void setup() {
    pinMode(LED, OUTPUT);   
    Serial.begin(9600);
}

void loop() {

      val = analogRead(LIGHT); // Read voltage value (0 -1023)
      Serial.println(val);

      if (val < 600) {
       digitalWrite(LED, LOW);}
      else {
       digitalWrite(LED, HIGH);
      }
      
      delay(10);
}