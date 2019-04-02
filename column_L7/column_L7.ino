#define COLUMN  49
#define ROW     7

int row[ROW] = {36, 38, 40, 42, 44, 46, 48};

int column[COLUMN] = {A11, A10,  A9,  A8, A14, A13, A12,
                       51,  49,  47,  45,  43,  41,  39,
                       37,  35,  33,  31,  29,  27,  25,
                       34,  32,  30,  28,  26,  24,  22,
                       21,  19,  18,  17,  16,  15,  14,
                       52,  50,   2,   3,   4,   5,   6,
                        7,   8,   9,  10,  11,  12,  13}; 



void setup() {
  Serial.begin(9600);
  int i;
  for(i = 0; i < ROW; i++){
    pinMode(row[i], OUTPUT);
    digitalWrite(row[i], HIGH);
  }
  for(i = 0; i < COLUMN; i++){
    pinMode(column[i], OUTPUT);
    digitalWrite(column[i], LOW);
  }

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  
  //randomSeed(analogRead(0));
}



void loop() {

  /*
  for(int r = 4; r < ROW; r++){
    digitalWrite(row[r], 0);
    //Serial.println("new row");
    for(int c = 0; c < COLUMN; c++){
      digitalWrite(column[c], 1);
      //Serial.print(c%COLUMN);
      //Serial.print(",");
      //Serial.println(c/COLUMN);
      delay(200);
      digitalWrite(column[c], 0);
    }
    digitalWrite(row[r], 1);
  }
  */
  int i = digitalRead(A0);
  if(i == HIGH){
    Serial.println("a0 pressed");
  }
  i = digitalRead(A1);
  if(i == HIGH){
    Serial.println("a1 pressed");
  }
  i = digitalRead(A2);
  if(i == HIGH){
    Serial.println("a2 pressed");
  }
  i = digitalRead(A3);
  if(i == HIGH){
    Serial.println("a3 pressed");
  }
  
  
}
