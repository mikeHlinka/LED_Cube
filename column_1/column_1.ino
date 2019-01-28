#define COLUMN  36
#define ROW     6

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
  int c;
  int r;
  int counter;
  for(c = 0; c < COLUMN; c++){
    digitalWrite(column[c], 1);
    for(counter = 0; counter < 50; counter++){
      for(r = 0; r < ROW; r++){
        digitalWrite(row[r], 0);
        delay(1);
        digitalWrite(row[r], 1);
      }
    }
  digitalWrite(column[c], 0);
  }
}
