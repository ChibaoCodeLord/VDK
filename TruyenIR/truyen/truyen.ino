
#include <IRremote.hpp>

const int IR_SEND_PIN = 3;  
bool turnOn = true;

const unsigned long LED_ON_CODE = 0x1; 
const unsigned long LED_OFF_CODE = 0x0;

void setup() {
  Serial.begin(9600);
  IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK);
}
void loop() {
  
  if (turnOn) {
    IrSender.sendRC5(LED_ON_CODE, 8);
    Serial.println("Sent RC5 ON Code");
    Serial.println(LED_ON_CODE);
  } else {
    IrSender.sendRC5(LED_OFF_CODE, 8);
    Serial.println("Sent RC5 OFF Code");
    Serial.println(LED_OFF_CODE);
  }

  turnOn = !turnOn; 
  delay(2000);      
}