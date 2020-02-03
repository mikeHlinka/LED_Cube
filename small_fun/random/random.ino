
#define COLUMN  49
#define ROW     7

int column[COLUMN] = {13,   12,  11,  10,   9,   8,  7,
                       6,    5,   4,  14,  15,  16, 17,
                      22,   24,  26,  28,  30,  32, 34,
                      38,   40,  42,  44,  46,  48, 50,
                      39,   41,  43,  45,  47,  49, 51,
                      A15, A14, A13, A12, A11, A10, A9,
                      A1,   A2,  A3,  A4,  A5,  A6, A7};
int row[ROW]       = {23,   25,  27,  29,  31,  33, 35};

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
  randomSeed(analogRead(0));
}

void loop() {
  int randomNumber;
  
  for(int r = 0; r < ROW; r++){
    digitalWrite(row[r], 0);
    for(int c = 0; c < COLUMN; c++){
      digitalWrite(column[c], random(2)); 
    }
    delay(1);
    digitalWrite(row[r], 1);
  }
}
