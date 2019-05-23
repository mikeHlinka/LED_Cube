#define COLUMN  49
#define ROW     7
#define STORAGESIZE 30

int row[ROW] = {36, 38, 40, 42, 44, 46, 48};

int column[COLUMN] = {A11, A10,  A9,  A8, A14, A13, A12,
                       51,  49,  47,  45,  43,  41,  39,
                       37,  35,  33,  31,  29,  27,  25,
                       34,  32,  30,  28,  26,  24,  22,
                       A7,  A6,  A5,  17,  16,  15,  14,
                       52,  50,   A4,   A3,   4,   5,   6,
                        7,   8,   9,  10,  11,  12,  13}; 

void setup() {
  Serial.begin(9600);
  int i;
  for(i = 0; i <  ROW; i++){
    pinMode(row[i], OUTPUT);
    digitalWrite(row[i], HIGH);
  }
  for(i = 0; i < COLUMN; i++){
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
