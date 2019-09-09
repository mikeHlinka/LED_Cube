int button = -1;
int bitReady = 0;
long lastTime = 0;

int data1=0;
int data2=0;

void setup() {
  // put your setup code here, to run once:
  attachInterrupt(digitalPinToInterrupt(19), ISR0, FALLING);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  Serial.begin(9600);
  Serial.println("Ready to begin.");
}

void loop() {
  if(bitReady){
    bitReady=0;
    Serial.print("button: ");
    Serial.print(data1);
    Serial.println(data2);
  }
}


void ISR0(){
  bitReady = 1;
  data1=digitalRead(2);
  data2=digitalRead(3);
}
