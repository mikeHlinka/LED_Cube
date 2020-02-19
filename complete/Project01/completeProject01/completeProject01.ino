//arduino led cube code by Michael Hlinka
//created January 2019
//timmer code taken from Ananda Ghassaei and instructables.com
//https://www.instructables.com/id/Arduino-Timer-Interrupts/
//this is a blank template that does not contain any specific display code
//it is meant for being able to start on a new display without having to worry
//or deal with mixed variable names and loosing yourself within the code

/****************General definitions*******************/
#define COLUMN  49
#define ROW     7
#define TIMEDELAY 2000

int column[COLUMN] = {13,   12,  11,  10,   9,   8,  7,
                       6,    5,   4,  14,  15,  16, 17,
                      22,   24,  26,  28,  30,  32, 34,
                      38,   40,  42,  44,  46,  48, 50,
                      39,   41,  43,  45,  47,  49, 51,
                      A15, A14, A13, A12, A11, A10, A9,
                      A1,   A2,  A3,  A4,  A5,  A6, A7};
int row[ROW]       = {23,   25,  27,  29,  31,  33, 35};

volatile int gameBoard[ROW * COLUMN];
volatile int tempBoard[ROW * COLUMN];
volatile int state_var;
volatile int main_function;
volatile int past_main;

volatile long oldTime;
volatile long newTime;

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
  main_function = 0;
  past_main = 0;
  attachInterrupt(digitalPinToInterrupt(19), ISR0, FALLING);
  oldTime=0;
  newTime=3000;
  Serial.begin(9600);
  randomSeed(analogRead(0));
sei();

}

//#####################displaying###################
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

//################Switching Function##################
void ISR0(){
  sei();
      oldTime = newTime;
      main_function++;
      if(main_function >= 3)
        main_function = 0;
 cli();
}

//###################helper functions###################
//will set entire gameBoard to 1 (on)
//meant to display a fault or error has occured
void game_over(){
  cli();
    Serial.print("main_function:");
    Serial.println(main_function);
    Serial.print("state_var:");
    Serial.println(state_var);
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

//generic supporting function
//will clear whatever is stored in the tempBoard
void clear_tempBoard(){
  cli();
  for(int i = 0; i < ROW * COLUMN; i++)
    tempBoard[i] = 0;
  sei();
}

//##############main control function######################
//interrupt 1: used for updating the current display
ISR(TIMER1_COMPA_vect){
 if(main_function != past_main){
  if(main_function >= 3)
    main_function = 0;
  past_main = main_function;
  state_var = 0;
 }
 switch(main_function){
  case 0:
    burst_main();
    break;
  case 1:
    bounceBack_main();
    break;
  case 2:
    bounce_column();
    break;
  default:
    game_over();
    break;
 }
}

//##################Burst function#########################
//variables
//set up as x, y, z
volatile int center_point[3];
volatile int waitVar;

//this fucntion is called whenever a gameboard needs to be updated
void burst_main(){
  switch(state_var){
    case 0:
      setup_burst_variables();
      break;
    case 1:
      burst_wait();
      break;
    case 2:
      burst_expand1();
      break;
    case 3:
      burst_wait();
      break;
    case 4:
      burst_expand2();
      break;
    case 5:
      burst_wait();
      break;
    case 6:
      burst_collapse2();
      break;
    case 7:
      burst_wait();
      break;
    case 8:
      burst_collapse1();
      break;
    case 9:
      burst_wait();
      break;
    case 10:
      burst_collapse0();
      break;
    case 11:
      burst_wait();
      break;
    case 12:
      state_var = 0;
      break;
    default:
      game_over();
      break;
  }
}

//supporting function 
//purpose is to initlize all the variables and gameboard for
void setup_burst_variables(){
  clear_gameBoard();
  clear_tempBoard();
  burst_setup_gameBoard();
  state_var = 1;
  waitVar = 4;
}

void gen_ran_point(){
    center_point[0] = random(ROW);
    center_point[1] = random(ROW);
    center_point[2] = random(ROW);
}

//supporting function for setup function
//purpose is to initilize all the starting leds (on) for the gameBoard
void burst_setup_gameBoard(){
  cli();
    gen_ran_point();
    gameBoard[center_point[2]*COLUMN + center_point[1]*ROW + center_point[0]] = 1;
  sei();
}

void burst_wait(){
    state_var++;
}

//first function that actually effects gameBoard
void burst_expand1(){
   
  for(int z = center_point[2] - 1; z <= center_point[2] + 1; z++){
    for(int y = center_point[1] - 1; y <= center_point[1] + 1; y++){
      for(int x = center_point[0] - 1; x <= center_point[0] + 1; x++){
        if(x >= 0 && x < ROW && y >= 0 && y < ROW && z >= 0 && z < ROW)
          tempBoard[z*COLUMN + y*ROW + x] = 1;  
      }
    }
  }
  
  cli();
    for(int i = 0; i < ROW * COLUMN; i++)
      gameBoard[i] = tempBoard[i];
  sei();
  
  state_var++;
}

void burst_expand2(){
   
  for(int z = center_point[2] - 2; z <= center_point[2] + 2; z++){
    for(int y = center_point[1] - 2; y <= center_point[1] + 2; y++){
      for(int x = center_point[0] - 2; x <= center_point[0] + 2; x++){
        if(x >= 0 && x < ROW && y >= 0 && y < ROW && z >= 0 && z < ROW)
          tempBoard[z*COLUMN + y*ROW + x] = 1;  
      }
    }
  }
  cli();
    for(int i = 0; i < ROW * COLUMN; i++)
      gameBoard[i] = tempBoard[i];
  sei();
  
  state_var++;
}

void burst_collapse2(){
   
  for(int z = center_point[2] - 2; z <= center_point[2] + 2; z++){
    for(int y = center_point[1] - 2; y <= center_point[1] + 2; y++){
      for(int x = center_point[0] - 2; x <= center_point[0] + 2; x++){
        if(x >= 0 && x < ROW && y >= 0 && y < ROW && z >= 0 && z < ROW)
          tempBoard[z*COLUMN + y*ROW + x] = 0;  
      }
    }
  }

  for(int z = center_point[2] - 1; z <= center_point[2] + 1; z++){
    for(int y = center_point[1] - 1; y <= center_point[1] + 1; y++){
      for(int x = center_point[0] - 1; x <= center_point[0] + 1; x++){
        if(x >= 0 && x < ROW && y >= 0 && y < ROW && z >= 0 && z < ROW)
          tempBoard[z*COLUMN + y*ROW + x] = 1;  
      }
    }
  }
  
  cli();
    for(int i = 0; i < ROW * COLUMN; i++)
      gameBoard[i] = tempBoard[i];
  sei();
  
  state_var++;
}

void burst_collapse1(){
   
  for(int z = center_point[2] - 1; z <= center_point[2] + 1; z++){
    for(int y = center_point[1] - 1; y <= center_point[1] + 1; y++){
      for(int x = center_point[0] - 1; x <= center_point[0] + 1; x++){
        if(x >= 0 && x < ROW && y >= 0 && y < ROW && z >= 0 && z < ROW)
          tempBoard[z*COLUMN + y*ROW + x] = 0;  
      }
    }
  }

  tempBoard[center_point[2]*COLUMN + center_point[1]*ROW + center_point[0]] = 1;
  
  cli();
    for(int i = 0; i < ROW * COLUMN; i++)
      gameBoard[i] = tempBoard[i];
  sei();
  
  state_var++;
}

void burst_collapse0(){
  tempBoard[center_point[2]*COLUMN + center_point[1]*ROW + center_point[0]] = 0;
  cli();
    for(int i = 0; i < ROW * COLUMN; i++)
      gameBoard[i] = tempBoard[i];
  sei();
  
  state_var++;
}


//#################Column Bounce Function########################
//variables
volatile int bounce_column_state;
volatile int bounce_column_old_c;
volatile int bounce_column_new_c;
volatile int bounce_column_old_r;
volatile int bounce_column_new_r;

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
  clear_tempBoard();
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


//################Bounce Back Function#########################
//variables
volatile int bounceBack_ticks;
volatile int bounceBack_point;
volatile int bounceBack_dir;


//main control function for the bounce control display
//this fucntion is called whenever a gameboard needs to be updated
void bounceBack_main(){
  switch(state_var){
    case 0:
      bounceBack_setup_function();
      break;
    case 1:
      bounceBack_pick_point();
      break;
    case 2:
      bounceBack_run();
      break;
    default:
      game_over();
      break;
  }
}

//supporting function 
//purpose is to initlize all the variables and gameboard for
void bounceBack_setup_function(){
  clear_gameBoard();
  clear_tempBoard();
  bounceBack_ticks=0;
  bounceBack_setup_gameBoard();
  state_var = 1;
}

//supporting function for setup function
//purpose is to initilize all the starting leds (on) for the gameBoard
void bounceBack_setup_gameBoard(){
  cli();
    for(int i=0; i<COLUMN;i++){
      if(random(2))
        gameBoard[i]=1;
      else
        gameBoard[(ROW-1)*COLUMN+i]=1;
    }
  sei();
}

//first function that actually effects gameBoard
void bounceBack_pick_point(){
  bounceBack_point=random(COLUMN);
  if(gameBoard[bounceBack_point])
    bounceBack_dir=1;
  else{
    bounceBack_dir=-1;
    bounceBack_point+=COLUMN*(ROW-1);
  }
    
  bounceBack_ticks=0;
  state_var=2;
}

void bounceBack_run(){
  if(bounceBack_ticks < ROW-1){
    cli();
      gameBoard[bounceBack_point + bounceBack_ticks*COLUMN*bounceBack_dir]=0;
      bounceBack_ticks++;
      gameBoard[bounceBack_point + bounceBack_ticks*COLUMN*bounceBack_dir]=1;
    sei();
  } else {
    state_var=1;
  }
}













void loop(){
  newTime = millis();
}
