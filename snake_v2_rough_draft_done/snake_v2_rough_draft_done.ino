//arduino led cube code by Michael Hlinka
//created January 2019
//timmer code taken from Ananda Ghassaei and instructables.com
//https://www.instructables.com/id/Arduino-Timer-Interrupts/
//this is a blank template that does not contain any specific display code
//it is meant for being able to start on a new display without having to worry
//or deal with mixed variable names and loosing yourself within the code

//it is more or less working now!!!!!

#define COLUMN  49
#define ROW     7
#define STORAGESIZE 30
/*
int row[ROW] = {36, 38, 40, 42, 44, 46, 48};

int column[COLUMN] = {A11, A10,  A9,  A8, A14, A13, A12,
                       51,  49,  47,  45,  43,  41,  39,
                       37,  35,  33,  31,  29,  27,  25,
                       34,  32,  30,  28,  26,  24,  22,
                       A7,  A6,  A5,  17,  16,  15,  14,
                       52,  50,   A4,   A3,   4,   5,   6,
                        7,   8,   9,  10,  11,  12,  13}; 
*/
int column[COLUMN] = {13,   12,  11,  10,   9,   8,  7,
                       6,    5,   4,  14,  15,  16, 17,
                      22,   24,  26,  28,  30,  32, 34,
                      38,   40,  42,  44,  46,  48, 50,
                      39,   41,  43,  45,  47,  49, 51,
                      A15, A14, A13, A12, A11, A10, A9,
                      A1,   A2,  A3,  A4,  A5,  A6, A7};

int row[ROW] = {23,25,27,29,31,33,35};

/*
 * the input buttons are: left  ==> 18   working
 *                        up    ==> 19   working
 *                        down  ==> 3   not working
 *                        right ==> 21   not working
 */

volatile int gameBoard[ROW * COLUMN];
volatile int tempBoard[ROW * COLUMN];
volatile int state_var;

//start of the snake specific variables

//all sotrage variables are in x, y, z format
//looking at the front: x === left/right
//                      y === up/down
//                      z === in-to/out-of page
int storage[STORAGESIZE];
//index into storage where the "head" of the snake can be found
int head;
//index into storage where the "tail" of the snake can be found
int tail;
//sotrage variable for describing where the snake will go next 
//value 0 === +x going right
//      1 === -x going left
//      2 === +y going up
//      3 === -y going down
//      4 === +z going into
//      5 === -z going outof
volatile int curDir;

//storage variable for the actual button number that was queued last
//value: 0 === left
//       1 === up
//       2 === down
//       3 === right
volatile int buttonNum;

//holds the position for the next fruit
int fruit[3];

int curDirLookUpTable[24] = {4, 2, 3, 5,
                             5, 2, 3, 4,
                             1, 4, 5, 0,
                             1, 4, 5, 0,
                             0, 2, 3, 1,
                             1, 2, 3, 0};





void setup(){
//code from Anada Ghassaei
cli();
//set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 160;// = (16*10^6) / (2000*64) - 1 (must be <256)
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
  /*
   * ****************************************************************** need to change back
   */
  OCR1A = 20562;// = (16*10^6) / (1*1024) - 1 (must be <65536)
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
  pinMode(19, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(18), ISR0, RISING);
  attachInterrupt(digitalPinToInterrupt(19), ISR1, RISING);
  attachInterrupt(digitalPinToInterrupt(2), ISR2, RISING);
  attachInterrupt(digitalPinToInterrupt(21), ISR3, RISING);

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
      //first_function();
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
  //function_setup_gameBoard();
  state_var = 1;
}


//supporting function for setup function
//purpose is to initilize all the starting leds (on) for the gameBoard
void function_setup_gameBoard(){
  //Serial.println("start setupGameboard");
  head = 3;
  tail = 0;
  storage[0] = 0;
  storage[1] = 0;
  storage[2] = 0;
  
  storage[3] = 1;
  storage[4] = 0;
  storage[5] = 0;

  //initlize snake to be moving in positive x direction (right)
  curDir = 0;
  
  //set the buttonNum to "false"
  buttonNum = -1;
  fruit[0] = 4;
  fruit[1] = 0;
  fruit[2] = 0;
  gameBoard[fruit[1]*COLUMN + fruit[2]*ROW + fruit[0]] = 1;
  //genFruit();
  
  cli();
    gameBoard[storage[head+1]*COLUMN + storage[head+2]*ROW + storage[head]] = 1;
    gameBoard[storage[tail+1]*COLUMN + storage[tail+2]*ROW + storage[tail]] = 1;
  sei();

  //Serial.println("done setupGameboard");
}

//first function that actually effects gameBoard
void first_function(){
  //Serial.print("start main function. round:");
  
  //changed head direction
  if(buttonNum > -1)
    updateCurDir();
  //moving the head 
  //Serial.println("start main switch");
  switch(curDir){
    case 0:
      //going right, snake head[0]++
      if(head + 5 < STORAGESIZE){
        storage[head+3] = storage[head] + 1;
        storage[head+4] = storage[head+1];
        storage[head+5] = storage[head+2]; 
        head = head + 3;
      } else {
        storage[0] = storage[head] + 1;
        storage[1] = storage[head+1];
        storage[2] = storage[head+2];
        head = 0;
      }
      break;
    case 1:
      //going left, snake head[0]--
      if(head + 5 < STORAGESIZE){
        storage[head+3] = storage[head] - 1;
        storage[head+4] = storage[head+1];
        storage[head+5] = storage[head+2]; 
        head = head + 3;
      } else {
        storage[0] = storage[head] - 1;
        storage[1] = storage[head+1];
        storage[2] = storage[head+2];
        head = 0;
      }
      break;
    case 2:
      //going up, snake head[1]++
      if(head + 5 < STORAGESIZE){
        storage[head+3] = storage[head];
        storage[head+4] = storage[head+1] + 1;
        storage[head+5] = storage[head+2]; 
        head = head + 3;
      } else {
        storage[0] = storage[head];
        storage[1] = storage[head+1] + 1;
        storage[2] = storage[head+2];
        head = 0;
      }
      break;
    case 3:
      //going down, snake head[1]--
      if(head + 5 < STORAGESIZE){
        storage[head+3] = storage[head];
        storage[head+4] = storage[head+1] - 1;
        storage[head+5] = storage[head+2]; 
        head = head + 3;
      } else {
        storage[0] = storage[head];
        storage[1] = storage[head+1] - 1;
        storage[2] = storage[head+2];
        head = 0;
      }
      break;
    case 4:
      //going into, snake head[2]++
      if(head + 5 < STORAGESIZE){
        storage[head+3] = storage[head];
        storage[head+4] = storage[head+1];
        storage[head+5] = storage[head+2] + 1; 
        head = head + 3;
      } else {
        storage[0] = storage[head];
        storage[1] = storage[head+1];
        storage[2] = storage[head+2] + 1;
        head = 0;
      }
      break;
    case 5:
      //going outof, snake head[2]--
      if(head + 5 < STORAGESIZE){
        storage[head+3] = storage[head];
        storage[head+4] = storage[head+1];
        storage[head+5] = storage[head+2] - 1; 
        head = head + 3;
      } else {
        storage[0] = storage[head];
        storage[1] = storage[head+1];
        storage[2] = storage[head+2] - 1;
        head = 0;
      }
      break;
  }
  //Serial.println("end main switcher");
  //making sure the user hasn't lost yet
  checkHeadBounds();
  //checking if the user has gotten a fruit yet
  checkFruit();
  //Serial.println("end main function");
  //print_gameboard();
  //print_variables();
}

void checkHeadBounds(){
  if(storage[head  ] >= ROW  ||  storage[head  ] < 0 ||
     storage[head+1] >= ROW  ||  storage[head+1] < 0 ||
     storage[head+2] >= ROW  ||  storage[head+2] < 0){
      state_var = -1;
      game_over();
     }
}

void genFruit(){
  //Serial.println("start genFruit");
  int fruitFlag = 1;
  int tempHead;
  while(fruitFlag){
    fruitFlag = 0;
    fruit[0] = random(ROW);
    fruit[1] = random(ROW);
    fruit[2] = random(ROW);
    tempHead = tail;
    while(tempHead != head){
      if(fruit[0] == storage[tempHead  ]  && 
         fruit[1] == storage[tempHead+1]  &&
         fruit[2] == storage[tempHead+2]){
          fruitFlag = 1;
          break;
         }
         if(tempHead + 3 < STORAGESIZE)
            tempHead = tempHead + 3;
         else
            tempHead = 0;
    }
  }
  cli();
    gameBoard[fruit[1]*COLUMN + fruit[2]*ROW + fruit[0]] = 1;
  sei(); 
  //Serial.println("end genFruit");
}


void checkFruit(){
  //if the snakes head is on the fruit
  if(storage[head  ] == fruit[0]  &&
     storage[head+1] == fruit[1]  &&
     storage[head+2] == fruit[2]){
      //snake needs to eat
      genFruit();
     } else {
      if(tail + 3 < STORAGESIZE){
        cli();
          //Serial.println("inside + checkfruit");
          gameBoard[storage[head+1]*COLUMN + storage[head+2]*ROW + storage[head]] = 1;
          gameBoard[storage[tail+1]*COLUMN + storage[tail+2]*ROW + storage[tail]] = 0;
        sei();
        tail = tail + 3;  
      } else {
        cli();
          //Serial.println("inside - checkfruit");
          gameBoard[storage[head+1]*COLUMN + storage[head+2]*ROW + storage[head]] = 1;
          gameBoard[storage[1]*COLUMN + storage[2]*ROW + storage[0]] = 0;
        sei();
        tail = 0;
      }
   }
}

void print_variables(){
  cli();
    Serial.println("storage:");
    for(int i = 0; i < STORAGESIZE/3; i++){
      Serial.print(storage[i*3+0]);
      Serial.print(storage[i*3+1]);
      Serial.println(storage[i*3+2]);
    }
    Serial.print("head:");
    Serial.println(head);
    Serial.print("tail:");
    Serial.println(tail);
    Serial.print("fruit:");
    Serial.print(fruit[0]);
    Serial.print(fruit[1]);
    Serial.println(fruit[2]);

  sei();
}

//this function updates the values stored in curDir[]
void updateCurDir(){
    //button has been pushed, time to update curDir
    curDir = curDirLookUpTable[curDir*4 + buttonNum];
    buttonNum = -1;
}


//solid blue
void ISR0(){
  //buttonNum = 0;
  cli();
    gameBoard[0] = 1;
    gameBoard[1] = 0;
    gameBoard[2] = 0;
    gameBoard[3] = 0;
  sei();
}

//striped blue
void ISR1(){
  //buttonNum = 1;
  cli();
    
    gameBoard[0] = 0;
    gameBoard[1] = 1;
    gameBoard[2] = 0;
    gameBoard[3] = 0;
  sei();
}

//striped orange
void ISR2(){
  //buttonNum = 2;
  cli();
    gameBoard[0] = 0;
    gameBoard[1] = 0;
    gameBoard[2] = 1;
    gameBoard[3] = 0;
  sei();
}

//solid orange
void ISR3(){
  //buttonNum = 3;
  cli();
    gameBoard[0] = 0;
    gameBoard[1] = 0;
    gameBoard[2] = 0;
    gameBoard[3] = 1;
  sei();
}













void loop(){
  
}
