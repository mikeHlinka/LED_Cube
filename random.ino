int row[6] = {47, 45, 43, 41, 39, 37};
int column[36] = {13, 12, 11, 10,  9, 8,
                   7,  6,  5,  4,  3, 2,
                  14, 15, 16, 17, 18, 19,
                  22, 24, 26, 28, 30, 32,
                  23, 25, 27, 29, 31, 33,
                  36, 38, 40, 42, 44, 46}; 

void setup() {
  Serial.begin(9600);
  int i;
  for(i = 0; i < 6; i++){
    pinMode(row[i], OUTPUT);
    digitalWrite(row[i], HIGH);
  }
  for(i = 0; i < 36; i++){
    pinMode(column[i], OUTPUT);
    digitalWrite(column[i], LOW);
  }
  randomSeed(analogRead(0));
}

void loop() {
  int randomNumber;
  
  for(int r = 0; r < 6; r++){
    digitalWrite(row[r], 0);
    for(int c = 0; c < 36; c++){
      randomNumber = random(10);
      if(randomNumber > 6)
        digitalWrite(column[c], 1);
      else
        digitalWrite(column[c], 0);  
    }
    delay(1);
    digitalWrite(row[r], 1);
  }
}
