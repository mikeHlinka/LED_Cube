//arduino led cube code by Michael Hlinka
//created January 2019
//timmer code taken from Ananda Ghassaei and instructables.com
//https://www.instructables.com/id/Arduino-Timer-Interrupts/

#define COLUMN  36
#define ROW     6

int row[6] = {47, 45, 43, 41, 39, 37};
int column[36] = {13, 12, 11, 10,  9, 8,
                   7,  6,  5,  4,  3, 2,
                  14, 15, 16, 17, 18, 19,
                  22, 24, 26, 28, 30, 32,
                  23, 25, 27, 29, 31, 33,
                  36, 38, 40, 42, 44, 46}; 

volatile int gameBoard[ROW * COLUMN];
volatile int tempBoard[ROW * COLUMN];
int bounce_column_old_c;
int bounce_column_new_c;
int bounce_column_old_r;
int bounce_column_new_r;
volatile int bounce_column_state;

void setup(){
//code from Anada Ghassaei
cli();
//set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 150;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);

//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 1562;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);


//my code
  int i;
  for(i = 0; i < ROW; i++){
    pinMode(row[i], OUTPUT);
    digitalWrite(row[i], HIGH);
  }
  for(i = 0; i < COLUMN; i++){
    pinMode(column[i], OUTPUT);
    digitalWrite(column[i], LOW);
  }
  for(i = 0; i < ROW * COLUMN; i++)
    gameBoard[i] = 0;
    
  Serial.begin(9600);
  bounce_column_state = 0;
sei();
}

int inter0_r;
int inter0_c;
int inter0_hold;
//interrupt 0: used for diplsying the board
ISR(TIMER0_COMPA_vect){
  for(inter0_r = 0; inter0_r < ROW; inter0_r++){
    digitalWrite(row[inter0_r], 0);
    inter0_hold = inter0_r * COLUMN;
    for(inter0_c = 0; inter0_c < COLUMN; inter0_c++)
      digitalWrite(column[inter0_c], gameBoard[inter0_hold + inter0_c]);
    delay(1);
    digitalWrite(row[inter0_r], 1);
  }
}

//interrupt 1: used for updating the whatever display is active
ISR(TIMER1_COMPA_vect){
 bounce_column();
}

//generic supporting function
//will set entire gameBoard to 1 (on)
//meant to display a fault or error has occured
void game_over(){
  cli();
    for(int i = 0; i < ROW * COLUMN; i++)
      gameBoard[i] = 1;
  sei();
}

//generic supporting function
//will clear whatever is stored in the gameBoard
void clear_gameBoard(){
  cli();
  for(int i = 0; i < ROW * COLUMN; i++)
    gameBoard[i] = 0;
  sei();
}

//main control function for the bounce control display
//this fucntion is called whenever a gameboard needs to be updated
void bounce_column(){
  switch(bounce_column_state){
    case 0:
      setup_column();
      break;
    case 1:
      increase_column();
      break;
    case 2:
      decrease_column();
      break;
    default:
      game_over();
      break;
  }
}

//supporting function for the bounce column display
//purpose is to initlize all the variables and gameboard for bounce column
void setup_column(){
  clear_gameBoard();
  bounce_column_setup_gameBoard();
  bounce_column_old_c = 0;
  bounce_column_new_c = 0;
  bounce_column_old_r = 0;
  bounce_column_new_r = 0;
  bounce_column_state = 1;
}

//supporting function for the bounce column display
//purpose is to initilize all the starting leds that are on
void bounce_column_setup_gameBoard(){
  cli();
    gameBoard[0] = 1;
  sei();
}

//supporting function for the bounce column display
//starts with only displaying the first row and increases in size each time
void increase_column(){
  for(int i = 0; i < ROW * COLUMN; i++)
    tempBoard[i] = gameBoard[i];
  
  bounce_column_new_c = bounce_column_old_c + 1;
  if(bounce_column_new_c >= COLUMN){
    bounce_column_new_c = 0;
    bounce_column_new_r = bounce_column_old_r + 1;
    if(bounce_column_new_r >= ROW)
      bounce_column_new_r = 0;
  }

  for(int i = 0; i <= bounce_column_new_r; i++){
    tempBoard[i*COLUMN + bounce_column_old_c] = 0;
    tempBoard[i*COLUMN + bounce_column_new_c] = 1;
  }

  if(bounce_column_old_c == COLUMN - 2 && bounce_column_old_r == ROW - 1){
    bounce_column_state = 2;
    bounce_column_old_c = COLUMN - 1;
    bounce_column_old_r = ROW - 1;
    bounce_column_new_r = ROW - 1;
    tempBoard[0] = 0;
  } else {
    bounce_column_old_c = bounce_column_new_c;
    bounce_column_old_r = bounce_column_new_r;
  }
  
  cli();
    for(int i = 0; i < ROW * COLUMN; i++)
      gameBoard[i] = tempBoard[i];
  sei();
  
}

////supporting function for the bounce column display
//starts with displaying row and decreases in size each time
void decrease_column(){
  for(int i = 0; i < ROW * COLUMN; i++)
    tempBoard[i] = gameBoard[i];
  
  bounce_column_new_c = bounce_column_old_c - 1;
  if(bounce_column_new_c < 0){
    bounce_column_new_c =  COLUMN - 1;
    tempBoard[bounce_column_old_r * COLUMN] = 0;
    bounce_column_new_r = bounce_column_old_r - 1;
  }

  for(int i = 0; i <= bounce_column_new_r; i++){
    tempBoard[i*COLUMN + bounce_column_old_c] = 0;
    tempBoard[i*COLUMN + bounce_column_new_c] = 1;
  }

  if(bounce_column_new_r == 0 && bounce_column_new_c == 0){
    bounce_column_state = 1;
    bounce_column_new_c = 0;
    bounce_column_new_r = 0;
    bounce_column_old_c = 0;
    bounce_column_old_r = 0;
    tempBoard[0] = 1;
  } else {
    bounce_column_old_c = bounce_column_new_c;
    bounce_column_old_r = bounce_column_new_r; 
  }
  
  cli();
    for(int i = 0; i < ROW * COLUMN; i++)
      gameBoard[i] = tempBoard[i];
  sei();
}

void loop(){
  
}
