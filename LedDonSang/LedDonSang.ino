void setup() {
  // put your setup code here, to run once:
  for(int i=2;i<=11;i++){
    pinMode(i,OUTPUT);
  }
}

void loop() {
 
  for(int i = 2 ; i <=11 ; i++){
    digitalWrite(i,HIGH);
    delay(100);
    digitalWrite(i,LOW);
  }
  for(int i = 11 ; i>=2 ;i --){
    digitalWrite(i,HIGH);
    delay(100);
    digitalWrite(i,LOW);
  }
}