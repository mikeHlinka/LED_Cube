//arduino led cube code by Michael Hlinka
//created January 2019
//timmer code taken from Ananda Ghassaei and instructables.com
//https://www.instructables.com/id/Arduino-Timer-Interrupts/
//this display is meant to pick a random point on the cube
//and light up touching leds going outward

/*
#define COLUMN  36
#define ROW     6

int row[6] = {47, 45, 43, 41, 39, 37};
int column[36] = {13, 12, 11, 10,  9, 8,
                   7,  6,  5,  4,  3, 2,
                  14, 15, 16, 17, 18, 19,
                  22, 24, 26, 28, 30, 32,
                  23, 25, 27, 29, 31, 33,
                  36, 38, 40, 42, 44, 46}; 
*/

#define COLUMN  49
#define ROW     7


int column[COLUMN] = {13,   12,  11,  10,   9,   8,  7,
                       6,    5,   4,  14,  15,  16, 17,
                      22,   24,  26,  28,  30,  32, 34,
                      38,   40,  42,  44,  46,  48, 50,
                      39,   41,  43,  45,  47,  49, 51,
                      A15, A14, A13, A12, A11, A10, A9,
                      A1,   A2,  A3,  A4,  A5,  A6, A7};

int row[ROW] = {23,25,27,29,31,33,35};
volatile int gameBoard[ROW * COLUMN];
volatile int tempBoard[ROW * COLUMN];
volatile int state_var;

//set up as x, y, z
int center_point[3];
int waitVar;

void setup(){
//code from Anada Ghassaei
cli();
//set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 130;// = (16*10^6) / (2000*64) - 1 (must be <256)
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
  for(int i = 0; i < ROW * COLUMN; i++)
    tempBoard[i] = 0;
  sei();
}

//generic debugging function
//will dsiplay the current gameboard to the serial monitor
void print_gameBoard(){
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
 main_burst();
}

//main control function for the bounce control display
//this fucntion is called whenever a gameboard needs to be updated
void main_burst(){
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












void loop(){
  
}
