//arduino led cube code by Michael Hlinka
//created January 2019
//timmer code taken from Ananda Ghassaei and instructables.com
//https://www.instructables.com/id/Arduino-Timer-Interrupts/
//this is a blank template that does not contain any specific display code
//it is meant for being able to start on a new display without having to worry
//or deal with mixed variable names and loosing yourself within the code

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
volatile int state_var;

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

sei();
}

int inter0_r;
int inter0_c;
int inter0_hold;
int inter0_var;
//interrupt 0: used for diplsying the board
ISR(TIMER0_COMPA_vect){
  for(inter0_r = 0; inter0_r < ROW; inter0_r++){
    digitalWrite(row[inter0_r], 0);
    inter0_hold = inter0_r * COLUMN;
    for(inter0_c = 0; inter0_c < COLUMN; inter0_c++){
      if(gameBoard[inter0_hold + inter0_c])
        for(inter0_var = 0; inter0_var < 5; inter0_var++)
          digitalWrite(column[inter0_c], 1);
      digitalWrite(column[inter0_c], 0);
      
    }
    digitalWrite(row[inter0_r], 1);
  }
}

//interrupt 1: used for updating the current display
ISR(TIMER1_COMPA_vect){
 main_function();
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

//generic debugging function
//outputs the current gameboard to the serial monitor
void print_gameboard(){
  cli();
  for(int z = 0; z < ROW; z++){
    for(int y = 0; y < ROW; y++){
      for(int x = 0; x < ROW; x++){
        Serial.print(gameBoard[z*COLUMN + y*ROW + x]);
      }
      Serial.println("\t");
    }
    Serial.println("\t");
  }
  Serial.println("\t");
  Serial.println("\t");
  sei();
}

//main control function for the bounce control display
//this fucntion is called whenever a gameboard needs to be updated
void main_function(){
  switch(state_var){
    case 0:
      setup_function();
      break;
    case 1:
      first_function();
      break;
    default:
      game_over();
      break;
  }
}

//supporting function 
//purpose is to initlize all the variables and gameboard for
void setup_function(){
  clear_gameBoard();
  function_setup_gameBoard();
  state_var = 1;
}

//supporting function for setup function
//purpose is to initilize all the starting leds (on) for the gameBoard
void function_setup_gameBoard(){
  cli();
    
  sei();
}

//first function that actually effects gameBoard
void first_function(){
  
}


















void loop(){
  
}
