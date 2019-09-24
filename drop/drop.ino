//arduino led cube code by Michael Hlinka
//created January 2019
//timmer code taken from Ananda Ghassaei and instructables.com
//https://www.instructables.com/id/Arduino-Timer-Interrupts/
//this is a blank template that does not contain any specific display code
//it is meant for being able to start on a new display without having to worry
//or deal with mixed variable names and loosing yourself within the code

#define COLUMN  49
#define ROW     7
#define WIDTH   2
#define WAIT    4

int column[COLUMN] = {13,   12,  11,  10,   9,   8,  7,
                       6,    5,   4,  14,  15,  16, 17,
                      22,   24,  26,  28,  30,  32, 34,
                      38,   40,  42,  44,  46,  48, 50,
                      39,   41,  43,  45,  47,  49, 51,
                      A15, A14, A13, A12, A11, A10, A9,
                      A1,   A2,  A3,  A4,  A5,  A6, A7};
int row[ROW]       = {23,   25,  27,  29,  31,  33, 35};

volatile int gameBoard[ROW * COLUMN];
int tempBoard[ROW * COLUMN];
volatile int state_var;

int org_x;
int org_y;
int org_z;
int drop_tick;
int total_ticks;
int wait_num;

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
  randomSeed(analogRead(0));
  
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
        for(inter0_var = 0; inter0_var < 8; inter0_var++)
          digitalWrite(column[inter0_c], 1);
      digitalWrite(column[inter0_c], 0);
      
    }
    digitalWrite(row[inter0_r], 1);
  }
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
  for(int i = 0; i < ROW * COLUMN; i++){
    gameBoard[i] = 0;
  }
  sei();
}

//will clear the temp board
void clear_tempBoard(){
  for(int i = 0; i < ROW * COLUMN; i++)
    tempBoard[i] = 0;
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

//interrupt 1: used for updating the current display
ISR(TIMER1_COMPA_vect){
 main_function();
}

//main control function for the bounce control display
//this fucntion is called whenever a gameboard needs to be updated
void main_function(){
  switch(state_var){
    case 0:
      setup_function();
      break;
    case 1:
      drop_check();
      break;
    case 2:
      drop_wait();
      break;
    case 3:
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
  state_var = 2;
}

void drop_wait(){
  if(wait_num < WAIT){
    wait_num++;
  } else {
    wait_num = 0;
    state_var = 1;
  }
}

//supporting function for setup function
//purpose is to initilize all the starting leds (on) for the gameBoard
void function_setup_gameBoard(){
  org_x = 1;
  org_y = 2;
  org_z = 3;
  int temp_point;
  total_ticks = 7;
  
  total_ticks = total_ticks + WIDTH - 1;
  wait_num = 20;
  drop_tick = 0;
  cli();
    gameBoard[org_z*COLUMN + org_y*ROW + org_x] = 1;
  sei();
}

//function to be called during dropping
//checks if drop needs to spread, or start anew
void drop_check(){
  if(drop_tick < total_ticks){
    drop_tick++;
    drop_do();
    state_var = 2;
  } else {
    state_var = 0;
  }
}

//first function that actually effects gameBoard
void drop_do(){
  int min_distance = drop_tick - WIDTH;
  int cur_point;
  cli();
    for(int x=org_x-WIDTH-drop_tick;x<org_x+WIDTH+drop_tick;x++){
      for(int y=org_y-WIDTH-drop_tick;y<org_y+WIDTH+drop_tick;y++){
        for(int z=org_z-WIDTH-drop_tick;z<org_z+WIDTH+drop_tick;z++){
          if(x >= 0 && x < ROW && y >= 0 && x < ROW && z >= 0 && z < ROW)
            gameBoard[z*COLUMN + y*ROW + x] = 1;
        }
      }
    }
  sei();
}

int get_rad(int x, int y, int z){
  int temp = max(abs(x-org_x),abs(y-org_y));
  return max(temp, abs(z-org_z));
}
















void loop(){
  
}
