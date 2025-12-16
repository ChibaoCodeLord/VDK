int trigPin = 5;
int echoPin = 6;

int buzzer = 4;
long duration;
int distance;

void setup(){
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);
}
void loop(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pusleIn(echoPin, HIGH);
  distance = duration*0.034/2;
  if(distance<=50){
    for(int i = 0; i<180; i++){
      float sinVal = sin(i*3.14/180);
      int toneVal = 2000+ int(1000*sinVal);
      tone(buzzer, toneVal);
      delay(2);
    }
   
  }else{
    noTone(buzzer);
  }
}