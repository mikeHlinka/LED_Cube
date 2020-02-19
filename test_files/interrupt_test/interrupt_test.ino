int bitReady = 0;

volatile long newTime;
volatile long oldTime;

void setup() {
  // put your setup code here, to run once:
  attachInterrupt(digitalPinToInterrupt(19), ISR0, FALLING);
  Serial.begin(9600);
  Serial.println("Ready to begin.");
  oldTime=millis();
}

void loop() {
  if(bitReady && oldTime + 2000 < newTime){
    Serial.print("new:");
    Serial.println(newTime);
    Serial.print("old:");
    Serial.println(oldTime);
    oldTime = newTime;
    bitReady=0;
    Serial.println("press");
    Serial.println(" ");
  }
}


void ISR0(){
  sei();
  bitReady=1;
  newTime=millis();
  cli();
}
