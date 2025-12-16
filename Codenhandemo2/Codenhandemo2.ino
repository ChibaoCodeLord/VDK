#include <IRremote.hpp>

const int IR_SEND_PIN = 3;               
const int DOLINE_DO_PIN = 7;            

const unsigned long ON_CODE = 0x1;      
const unsigned long OFF_CODE = 0x0;      

void setup() {
  Serial.begin(9600);
  pinMode(DOLINE_DO_PIN, INPUT);
  IrSender.begin(IR_SEND_PIN);
}

void loop() {
  int lineState = digitalRead(DOLINE_DO_PIN); 
  Serial.print("Line DO value: ");
  Serial.println(lineState);

  if (lineState == LOW) {   
    IrSender.sendRC5(ON_CODE, 8);
    Serial.println("Sent RC5 ON Code");
  } else {                   
    IrSender.sendRC5(OFF_CODE, 8);
    Serial.println("Sent RC5 OFF Code");
  }

  delay(200); 
}