#include <IRremote.hpp>

const int IR_RECEIVE_PIN = 2;   
const int LED_Pin = 9;          
volatile bool ledShouldBeOn = false; 

void setup() {
  Serial.begin(9600);                     
  pinMode(LED_Pin, OUTPUT);                
  IrReceiver.begin(IR_RECEIVE_PIN); 
}
void loop() {
  Serial.println("waiting");
  if (ledShouldBeOn) {
    digitalWrite(LED_Pin, HIGH); 
    Serial.println("LED ON");
  } else {
    digitalWrite(LED_Pin, LOW);  
    Serial.println("LED OFF");
  }

  delay(10);

  if (IrReceiver.decode()) {
    Serial.println("Receive Signal!");
    
    // Hàm xử lý dữ liệu khi nhận được tín hiệu IR
    handleReceivedIRData();  
    delay(5);
    IrReceiver.resume();    
  }else{
    Serial.println("Not found Signal Receive");
    ledShouldBeOn = false;
  }

  delay(100);
}

// Hàm xử lý dữ liệu khi nhận được tín hiệu IR
void handleReceivedIRData() {
  Serial.print("Mã IR nhận được: 0x");
  Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
  Serial.print("Giao thức: ");
  Serial.println(IrReceiver.decodedIRData.protocol);

  if (IrReceiver.decodedIRData.decodedRawData== 0x101) {
    ledShouldBeOn = true; 
    Serial.println("LED ON");
  } else{
    ledShouldBeOn = false; 
    Serial.println("LED OFF");
  }
}