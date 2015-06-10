#include <SPI.h>// SPI Library used to clock data out to the shift registers
#define latch_pin 2// can use any pin you want to latch the shift registers
#define blank_pin 4// same, can use any pin you want for this, just make sure you pull up via a 1k to 5V
#define data_pin 11// used by SPI, must be pin 11
#define clock_pin 13// used by SPI, must be 13

int level = 0;

// LED's by color. 5-bit BAM => 8 Arrays per LED
byte red0[8], red1[8], red2[8], red3[8], red4[8];
byte green0[8], green1[8], green2[8], green3[8], green4[8];
byte blue0[8], blue1[8], blue2[8], blue3[8], blue4[8];
int color[3]; //color[0] - red, color[1] - green, color[2] - blue

int BAMBit, BAMCounter = 0;

void setup(){
  SPI.setBitOrder(MSBFIRST);//Most Significant Bit First
  SPI.setDataMode(SPI_MODE0);// Mode 0 Rising edge of data, keep clock low
  SPI.setClockDivider(SPI_CLOCK_DIV2);//Run the data in at 16MHz/2 - 8MHz
  //We use Timer 1 to refresh the cube
  TCCR1A = B00000000;//Register A all 0's since we're not toggling any pins
  TCCR1B = B00001011;//bit 3 set to place in CTC mode, will call an interrupt on a counter match
  //bits 0 and 1 are set to divide the clock by 64, so 16MHz/64=250kHz
  TIMSK1 = B00000010;//bit 1 set to call the interrupt on an OCR1A match
  OCR1A=15; // you can play with this, but I set it to 30, which means:
  //our clock runs at 250kHz, which is 1/250kHz = 4us
  //with OCR1A set to 30, this means the interrupt will be called every (30+1)x4us=124us, 
  // which gives a multiplex frequency of about 8kHz
  // OCR1A = 10 means 10+1*4us = 44us
  
  //finally set up the Outputs
  pinMode(latch_pin, OUTPUT);//Latch
  pinMode(data_pin, OUTPUT);//MOSI DATA
  pinMode(clock_pin, OUTPUT);//SPI Clock
  pinMode(blank_pin, OUTPUT);//Output Enable  important to do this last, so LEDs do not flash on boot up
  SPI.begin();//start up the SPI library

  interrupts();//let the show begin, this lets the multiplexing start
  randomSeed(analogRead(0));
}
ISR(TIMER1_COMPA_vect){
  // Seems to have no impact on flickering, will make the cube brighter if commented out
  PORTD |= 1<<blank_pin;//The first thing we do is turn all of the LEDs OFF, by writing a 1 to the blank pin

  // Set BAMBit depending on BAMCounter
  switch(BAMCounter) {
    case 0:
      BAMBit = 0;
      break;
    case 1:
      BAMBit = 1;
      break;
    case 3:
      BAMBit = 2;
      break;
    case 7:
      BAMBit = 3;
      break;
    case 15:
      BAMBit = 4;
      break;
  }

  // Select level
  // Turn LEDs on/off depending von BAM-Bit and current level
  switch (BAMBit) {
      case 0:
          SPI.transfer(red0[1 + 2*level]);
          SPI.transfer(red0[0 + 2*level]);
          SPI.transfer(green0[1 + 2*level]);
          SPI.transfer(green0[0 + 2*level]);
          SPI.transfer(blue0[1 + 2*level]);
          SPI.transfer(blue0[0 + 2*level]);
        break;
      case 1:
          SPI.transfer(red1[1 + 2*level]);
          SPI.transfer(red1[0 + 2*level]);
          SPI.transfer(green1[1 + 2*level]);
          SPI.transfer(green1[0 + 2*level]);
          SPI.transfer(blue1[1 + 2*level]);
          SPI.transfer(blue1[0 + 2*level]);
        break;
      case 2:
          SPI.transfer(red2[1 + 2*level]);
          SPI.transfer(red2[0 + 2*level]);
          SPI.transfer(green2[1 + 2*level]);
          SPI.transfer(green2[0 + 2*level]);
          SPI.transfer(blue2[1 + 2*level]);
          SPI.transfer(blue2[0 + 2*level]);
        break;
      case 3:
          SPI.transfer(red3[1 + 2*level]);
          SPI.transfer(red3[0 + 2*level]);
          SPI.transfer(green3[1 + 2*level]);
          SPI.transfer(green3[0 + 2*level]);
          SPI.transfer(blue3[1 + 2*level]);
          SPI.transfer(blue3[0 + 2*level]);
        break;
      case 4:
          SPI.transfer(red4[1 + 2*level]);
          SPI.transfer(red4[0 + 2*level]);
          SPI.transfer(green4[1 + 2*level]);
          SPI.transfer(green4[0 + 2*level]);
          SPI.transfer(blue4[1 + 2*level]);
          SPI.transfer(blue4[0 + 2*level]);
        break;
  }
  SPI.transfer(0x01 << level);
  // Activate new values
  PORTD |= 1<<latch_pin;//Latch pin HIGH
  PORTD &= 0<<latch_pin;//Latch pin LOW
  PORTD &= 0<<blank_pin;//Blank pin LOW to turn on the LEDs with the new data 
  
  level++;
  // After 4 levels start with bottom level again
  if(level == 4) {
    level = 0;
    // Cube referesh complete, increase BAMCounter
    BAMCounter++;
    // BAM-Cycle is 32 complete cube refreshs long, start again afterwards
    if(BAMCounter == 32) {
      BAMCounter = 0;
    }
  }
}
/* ##########################################################
 * ######################### START ##########################
 * ########################################################## */

int firstRun = 1;

void loop() {  
  int time = 0;
  if(firstRun == 1) {
    cube(0,0,0);
    firstRun = 0;
  }
  
  breathe(0);
  delay(500);
  breathe(1);
  delay(500);
  breathe(2);
  delay(500);
  
  for(int i = 0; i < 2; i++){
   rainbow(550-250*i); 
  }
  
  cube(0,0,0);
  shift(0,0,0,100);
  cube(0,0,0);
  shift(3,0,0,100);
  cube(0,0,0);
  shift(4,0,0,100);
  
  shift(0,0,0,100);
  shift(3,1,0,100);
  shift(4,0,0,100);
  shift(0,1,0,100);
  shift(3,0,0,100);
  shift(4,1,0,100);
  
  for(int i = 0; i < 6; i++){ 
    if(i % 2 == 0 || i == 0){
      shift(i,0,0,100);
    }
    else{
      shift(i,1,0,100);
    }
  }
  
  shift(0,2,0,100);
  shift(3,3,0,100);
  shift(4,2,0,100);
  shift(0,3,0,100);
  shift(3,2,0,100);
  shift(4,3,0,100);
  
  for(int i = 0; i < 6; i++){ 
    if(i % 2 == 0 || i == 0){
      shift(i,2,0,100);
    }
    else{
      shift(i,3,0,100);
    }
  }
  
  cube(0,0,0);
  
  for(int i = 0; i < 2; i++){
    shift(4,0,1,100);
    shift(4,1,1,100);
  }

  for(int i = 0; i < 2; i++){
    shift(4,2,1,100);
    shift(4,3,1,100);
  }
  
   for(int i = 0; i < 6; i++){
     if(i % 2 == 0 || i == 0){
       shift(i,2,1,100);
     }
     else{
       shift(i,3,1,100);
     }
   } 

   for(int i = 0; i < 6; i++){
     if(i % 2 == 0 || i == 0){
       shift(i,0,1,100);
     }
     else{
       shift(i,1,1,100);
     }
   } 

  
  cube(0,0,0);
    
  wave(0,0);
  wave(0,1);
  wave(3,0);
  wave(3,1);
  wave(4,0);
  wave(4,1);
    
  for(int i = 0; i < 6; i++){
    if(i % 2 == 0 || i == 0){
      wave(i, 0);
    }
    else{
      wave(i, 1);
    }
  }
  
  diag(75,15,0,0);
  revdiag(75,0,0,0);
  delay(500);
  diag(75,0, 15,0);
  revdiag(75,0,0,0);
  delay(500);
  diag(75,0,0,15);
  revdiag(75,0,0,0);
  delay(500);
  diag(75,15,0,0);
  revdiag(75,0,15,0);
  diag(75,0,0,15);
  revdiag(75,0,0,0);

  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 6; j++){
      randomness(random(8,64), 0, j);
      delay(250);
      cube(0,0,0);
    }
  }
  
  for(int j = 0; j < 3; j++){
    randomness(random(8,64), 50, 6);
    delay(50);
    cube(0,0,0);
  }

  for(int j = 0; j < 25; j++){
    randomness(random(8,56), 0, 6);
    delay(0);
    cube(0,0,0);
  }
  
  for(int j = 0; j < 6; j++){
    randomness(random(0,24), 50, j);
    delay(50);
    cube(0,0,0);
  }
  
  streams(4,750);
  cube(0,0,0);
  paths(4,100,0,0);
  cube(0,0,0);
  paths(4,100,1,1);
  cube(0,0,0);
  overPaths(100);
  cube(0,0,0);
  
  for(int i = 0; i < 2; i++){
     diagonals(0,0,4,300);
     diagonals(0,1,4,300);
  }
  
  for(int i = 0; i < 2; i++){
    diagonals(1,0,4,300);
    diagonals(1,1,4,300);
  }
  
  for(int i = 0; i < 2; i++){
    diagonals(2,0,4,300);
    diagonals(2,1,4,300);
  }
  
  for(int i = 0; i < 6; i++){
    explode(75,i);
    delay(150);
  }
  
  breathe(0);
  delay(500);
  breathe(1);
  delay(500);
  breathe(2);
  delay(500);
  
  shift(0,0,0,50);
  shift(6,1,0,100);
  shift(3,0,0,150);
  shift(6,1,0,200);
  shift(4,0,0,250);
  shift(6,1,0,300);
  
  explode(100,4);
  
  delay(5000);
}

void diagonals(int axis, int dir, int c, int time){ //axis: 0 - along x, 1 - along y, 2 - along z
  if(axis == 0){
    if(dir == 0){
        column(0,1,1,c);
        delay(time);
        cube(0,0,0);
        column(0,2,1,c);
        column(0,1,2,c);
        delay(time);
        cube(0,0,0);
        column(0,3,1,c);
        column(0,2,2,c);
        column(0,1,3,c);
        delay(time);
        cube(0,0,0);
        column(0,4,1,c);
        column(0,3,2,c);
        column(0,2,3,c);
        column(0,1,4,c);
        delay(time);
        cube(0,0,0);
        column(0,4,2,c);
        column(0,3,3,c);
        column(0,2,4,c);
        delay(time);
        cube(0,0,0);
        column(0,4,3,c);
        column(0,3,4,c);
        delay(time);
        cube(0,0,0);
        column(0,4,4,c);
        delay(time);
        cube(0,0,0);
    }
    else if(dir == 1){
        column(0,4,4,c);
        delay(time);
        cube(0,0,0);
        column(0,3,4,c);
        column(0,4,3,c);
        delay(time);
        cube(0,0,0);
        column(0,3,3,c);
        column(0,2,4,c);
        column(0,4,2,c);
        delay(time);
        cube(0,0,0);
        column(0,4,1,c);
        column(0,3,2,c);
        column(0,2,3,c);
        column(0,1,4,c);
        delay(time);
        cube(0,0,0);
        column(0,1,3,c);
        column(0,3,1,c);
        column(0,2,2,c);
        delay(time);
        cube(0,0,0);
        column(0,1,2,c);
        column(0,2,1,c);
        delay(time);
        cube(0,0,0);
        column(0,1,1,c);
        delay(time);
        cube(0,0,0);
    }
  }
  else if(axis == 1){
    if(dir == 0){
        column(1,0,1,c);
        delay(time);
        cube(0,0,0);
        column(2,0,1,c);
        column(1,0,2,c);
        delay(time);
        cube(0,0,0);
        column(3,0,1,c);
        column(2,0,2,c);
        column(1,0,3,c);
        delay(time);
        cube(0,0,0);
        column(4,0,1,c);
        column(3,0,2,c);
        column(2,0,3,c);
        column(1,0,4,c);
        delay(time);
        cube(0,0,0);
        column(4,0,2,c);
        column(3,0,3,c);
        column(2,0,4,c);
        delay(time);
        cube(0,0,0);
        column(4,0,3,c);
        column(3,0,4,c);
        delay(time);
        cube(0,0,0);
        column(4,0,4,c);
        delay(time);
        cube(0,0,0);
    }
    else if(dir == 1){
        column(4,0,4,c);
        delay(time);
        cube(0,0,0);
        column(4,0,3,c);
        column(3,0,4,c);
        delay(time);
        cube(0,0,0);
        column(3,0,3,c);
        column(2,0,4,c);
        column(4,0,2,c);
        delay(time);
        cube(0,0,0);
        column(4,0,1,c);
        column(3,0,2,c);
        column(2,0,3,c);
        column(1,0,4,c);
        delay(time);
        cube(0,0,0);
        column(1,0,3,c);
        column(2,0,2,c);
        column(3,0,1,c);
        delay(time);
        cube(0,0,0);
        column(1,0,2,c);
        column(2,0,1,c);
        delay(time);
        cube(0,0,0);
        column(1,0,1,c);
        delay(time);
        cube(0,0,0);
    }
  }
  else if(axis == 2){
    if(dir == 0){
        column(1,1,0,c);
        delay(time);
        cube(0,0,0);
        column(2,1,0,c);
        column(1,2,0,c);
        delay(time);
        cube(0,0,0);
        column(3,1,0,c);
        column(2,2,0,c);
        column(1,3,0,c);
        delay(time);
        cube(0,0,0);
        column(4,1,0,c);
        column(3,2,0,c);
        column(2,3,0,c);
        column(1,4,0,c);
        delay(time);
        cube(0,0,0);
        column(4,2,0,c);
        column(3,3,0,c);
        column(2,4,0,c);
        delay(time);
        cube(0,0,0);
        column(4,3,0,c);
        column(3,4,0,c);
        delay(time);
        cube(0,0,0);
        column(4,4,0,c);
        delay(time);
        cube(0,0,0);
    }
    else if(dir == 1){
        column(4,4,0,c);
        delay(time);
        cube(0,0,0);
        column(4,3,0,c);
        column(3,4,0,c);
        delay(time);
        cube(0,0,0);
        column(3,3,0,c);
        column(2,4,0,c);
        column(4,2,0,c);
        delay(time);
        cube(0,0,0);
        column(4,1,0,c);
        column(3,2,0,c);
        column(2,3,0,c);
        column(1,4,0,c);
        delay(time);
        cube(0,0,0);
        column(2,2,0,c);
        column(3,1,0,c);
        column(1,3,0,c);
        delay(time);
        cube(0,0,0);
        column(1,2,0,c);
        column(2,1,0,c);
        delay(time);
        cube(0,0,0);
        column(1,1,0,c);
        delay(time);
        cube(0,0,0);
    }
  }
 }

void column(int x, int y, int z, int c){ //x,y,z - whichever is 0 is cycled
  colorChoice(c);
  
  if(x == 0){
    for(int i = 1; i <= 4; i++){
     led(i,y,z,color[0],color[1],color[2]); 
    }
  }
  else if(y == 0){
    for(int i = 1; i <= 4; i++){
     led(x,i,z,color[0],color[1],color[2]); 
    }
  }
  else if (z == 0){
    for(int i = 1; i <= 4; i++){
     led(x,y,i,color[0],color[1],color[2]); 
    }
  }
}

void overPaths(int time){
  int c = 0;
  colorChoice(c);
  
  for(int x = 0; x < 5; x++){
    led(1,1,1,color[0],color[1],color[2]);
    delay(time);
    
    for(int i = 2; i <= 4; i++){
      led(i,1,1,color[0],color[1],color[2]);
      led(1,i,1,color[0],color[1],color[2]);
      led(1,1,i,color[0],color[1],color[2]);
      delay(time);
    }
    
    if(x > 0){
      for(int i = 2; i <= 4; i++){
        led(i,4,1,color[0],color[1],color[2]);
        led(1,i,4,color[0],color[1],color[2]);
        led(i,1,4,color[0],color[1],color[2]);
        led(4,i,1,color[0],color[1],color[2]);
        delay(time);
      }
      
      if(x > 1){ 
        for(int i = 2; i <= 4; i++){
          led(1,4,(5-i),color[0],color[1],color[2]); 
          led(4,4,i,color[0],color[1],color[2]);
          led(4,1,(5-i),color[0],color[1],color[2]);
          delay(time);
        }
        
        if(x > 2){
          for(int i = 2; i <= 4; i++){
            led(1,(5-i),1,color[0],color[1],color[2]);
            led((5-i),4,4,color[0],color[1],color[2]);
            led(4,(5-i),4,color[0],color[1],color[2]);
            led((5-i),1,1,color[0],color[1],color[2]);
            delay(time);
          }
          
          if(x > 3){
            for(int i = 2; i <= 3; i++){
              led(1,(5-i),4,color[0],color[1],color[2]);
              led(1,1,1,color[0],color[1],color[2]);
              led((5-i),1,4,color[0],color[1],color[2]);
              delay(time);
            }
          }
        }
      }
    }

    c = addColor(1,c);
    colorChoice(c);
  }
}

void streams(int c, int time){
 colorChoice(c);
 for(int a = 0; a < 6; a++){
   for(int i = 1; i <= 4; i++){
    led(1,i,4,color[0],color[1],color[2]);
    led(i,1,2,color[0],color[1],color[2]);
    led(4,(5-i),3,color[0],color[1],color[2]); 
    led(4,4,(5-i),color[0],color[1],color[2]);
    led(2,3,i,color[0],color[1],color[2]);
    led((5-i),3,2,color[0],color[1],color[2]);
    led(2,(5-i),1,color[0],color[1],color[2]);
    led(i,4,3,color[0],color[1],color[2]);
    led(3,2,(5-i),color[0],color[1],color[2]); 
    led(2,1,i,color[0],color[1],color[2]);
    led(3,i,2,color[0],color[1],color[2]);
    led((5-i),2,3,color[0],color[1],color[2]);
    delay(time);
    cube(0,0,0);
   }
 }
}

void paths(int c, int time, int type, int s){ //if type == 1 then paths change color, if s == 1, keep all on, if s == 0 turn off
  colorChoice(c);
  led(1,1,1,color[0],color[1],color[2]);
  delay(time);
  
  for(int i = 2; i <= 4; i++){
    led(i,1,1,color[0],color[1],color[2]);
    led(1,i,1,color[0],color[1],color[2]);
    led(1,1,i,color[0],color[1],color[2]);
    delay(time);
  }
  
  if(s == 0){
    colorChoice(6); 
    led(1,1,1,color[0],color[1],color[2]);
    delay(time);
    for(int i = 2; i <= 4; i++){
      led(i,1,1,color[0],color[1],color[2]);
      led(1,i,1,color[0],color[1],color[2]);
      led(1,1,i,color[0],color[1],color[2]);
      delay(time);
    }
  }
  
  c = addColor(type,c);
  colorChoice(c);
  
  led(1,4,1,color[0],color[1],color[2]);
  led(1,1,4,color[0],color[1],color[2]);
  led(4,1,1,color[0],color[1],color[2]);
  delay(time);
  
  for(int i = 2; i <= 4; i++){
    led(i,4,1,color[0],color[1],color[2]);
    led(1,i,4,color[0],color[1],color[2]);
    led(i,1,4,color[0],color[1],color[2]);
    led(4,i,1,color[0],color[1],color[2]);
    delay(time);
  }
  
  if(s == 0){
    colorChoice(6); 
    led(1,4,1,color[0],color[1],color[2]);
    led(1,1,4,color[0],color[1],color[2]);
    led(4,1,1,color[0],color[1],color[2]);
    delay(time);
    for(int i = 2; i <= 4; i++){
      led(i,4,1,color[0],color[1],color[2]);
      led(1,i,4,color[0],color[1],color[2]);
      led(i,1,4,color[0],color[1],color[2]);
      led(4,i,1,color[0],color[1],color[2]);
      delay(time);
    }
  }
  
  c = addColor(type,c);
  colorChoice(c);
  
  led(1,4,4,color[0],color[1],color[2]); 
  led(4,4,1,color[0],color[1],color[2]);
  led(4,1,4,color[0],color[1],color[2]);
  delay(time);
  
  for(int i = 2; i <= 4; i++){
    led(1,4,(5-i),color[0],color[1],color[2]); 
    led(4,4,i,color[0],color[1],color[2]);
    led(4,1,(5-i),color[0],color[1],color[2]);
    delay(time);
  }
  
  if(s == 0){
    colorChoice(6); 
    led(1,4,4,color[0],color[1],color[2]); 
    led(4,4,1,color[0],color[1],color[2]);
    led(4,1,4,color[0],color[1],color[2]);
    delay(time);
    for(int i = 2; i <= 4; i++){
      led(1,4,(5-i),color[0],color[1],color[2]); 
      led(4,4,i,color[0],color[1],color[2]);
      led(4,1,(5-i),color[0],color[1],color[2]);
      delay(time);
    }
  }
  
  c = addColor(type,c);
  colorChoice(c);
  
  led(1,4,1,color[0],color[1],color[2]);
  led(4,4,4,color[0],color[1],color[2]);
  led(4,1,1,color[0],color[1],color[2]);
  delay(time);
  
  for(int i = 2; i <= 4; i++){
    led(1,(5-i),1,color[0],color[1],color[2]);
    led((5-i),4,4,color[0],color[1],color[2]);
    led(4,(5-i),4,color[0],color[1],color[2]);
    led((5-i),1,1,color[0],color[1],color[2]);
    delay(time);
  }
  
  if(s == 0){
    colorChoice(6); 
    led(1,4,1,color[0],color[1],color[2]); 
    led(4,4,4,color[0],color[1],color[2]);
    led(4,1,1,color[0],color[1],color[2]);
    delay(time);
    for(int i = 2; i <= 4; i++){
      led(1,(5-i),1,color[0],color[1],color[2]);
      led((5-i),4,4,color[0],color[1],color[2]);
      led(4,(5-i),4,color[0],color[1],color[2]);
      led((5-i),1,1,color[0],color[1],color[2]);
      delay(time);
    }
  }
  
  c = addColor(type,c);
  colorChoice(c);
  
  led(1,4,4,color[0],color[1],color[2]); 
  led(1,1,1,color[0],color[1],color[2]);
  led(4,1,4,color[0],color[1],color[2]);
  delay(time);
  
  for(int i = 2; i <= 4; i++){
    led(1,(5-i),4,color[0],color[1],color[2]);
    led(1,1,i,color[0],color[1],color[2]);
    led((5-i),1,4,color[0],color[1],color[2]);
    delay(time);
  }
  
  if(s == 0){
    colorChoice(6); 
    led(1,4,4,color[0],color[1],color[2]); 
    led(1,1,1,color[0],color[1],color[2]);
    led(4,1,4,color[0],color[1],color[2]);
    delay(time);
    for(int i = 2; i <= 4; i++){
      led(1,(5-i),4,color[0],color[1],color[2]);
      led(1,1,i,color[0],color[1],color[2]);
      led((5-i),1,4,color[0],color[1],color[2]);
      delay(time);
    }
  }
}

int addColor(int yn, int orig){
  if(yn == 0){
   return orig; 
  }
  int temp;
  temp = ++orig;
  if(temp >= 6){
   temp = 0; 
  }
  return temp;
}

void rainbow(int time){  
  for(int i = 0; i < 6; i++){
    colorChoice(i);
    cube(color[0],color[1],color[2]);
    delay(time);
  }
}

// way: 0 - down/up, 1 - up/down, 2 - left/right, 3 right/left
// type: 0 - all on, 1 - on/off
void shift(int c, int way, int type, int time){
  
  if(way == 0){
    for(int i = 1; i <= 4; i++){
      z_layer(i, c);
      delay(time);
      if(type == 1){
        cube(0,0,0);
      }
    }
  }
  else if(way == 1){
     for(int i = 4; i >= 1; i--){
       z_layer(i, c);
       delay(time);
       if(type == 1){
         cube(0,0,0);
       }
     }
  }
  else if(way == 2){
     for(int i = 1; i <= 4; i++){
       x_layer(i, c);
       delay(time);
       if(type == 1){
         cube(0,0,0);
       }
     }
  }
  else if(way == 3){
     for(int i = 4; i >= 1; i--){
       x_layer(i, c);
       delay(time);
       if(type == 1){
         cube(0,0,0);
       }
     } 
  }
  
  if(type == 1){
   cube(0,0,0); 
  }
}

void wave(int c, int way){
  int time = 100;
  int x, z;
  
  colorChoice(c);

  for(int a = 0; a < 2; a++){
    if(a == 1){
     if(way == 0){
      way = 1;
     } 
     else if(way == 1){
      way = 0; 
     }
    }
    
    x = waveControl(way);
    for(z = 1; z <= 4; z++){
        if(z == 1 && a == 1){
         continue; 
        }
        for(int y = 1; y <= 4; y++){
          led(x, y, z, color[0], color[1], color[2]);
        }
        if(way == 0){
          x++;
        }
        else if(way == 1){
         x--; 
        }
    }
    delay(time);
    cube(0,0,0);
    
    if(a == 1){
     break; 
    }
    
    for(int i = 0; i < 2; i++){
      x = 1;
      for(int j = 1; j <= 4; j++){
         if(x == 1 || x == 2){
          if((way == 0 && i == 0) || (way == 1 && i == 1)){
            z = 2; 
          }
          else if((way == 1 && i == 0) || (way == 0 && i == 1)){
            z = 3; 
          }
         }
         else if(x == 3 || x == 4){
           if((way == 0 && i == 0) || (way == 1 && i == 1)){
             z = 3; 
           }
           else if((way == 1 && i == 0) || (way == 0 && i == 1)){
             z = 2; 
           }
         }
         for(int y = 1; y <= 4; y++){
           led(x, y, z, color[0], color[1], color[2]);
         }
         x++;
      }
      delay(time);
      cube(0,0,0);
    }
  }
}

//0 - down/up, 1 - up/down
int waveControl(int d){
 if(d == 0){
  return 1;
 }
 else if(d == 1){
  return 4;
 }
}

void randomness(int lights, int time, int c){
  int x, y, z;
  if(c >= 0 && c <= 5){
   colorChoice(c); 
  }
  
  for(int a = 0; a < lights; a++){
   x = random(1,5);
   y = random(1,5);
   z = random(1,5);
   
   if(c >= 0 && c <= 5){
     led(x,y,z,color[0],color[1],color[2]);
   }
   else if(c == 6){
     led(x,y,z,random(0,20),random(0,20),random(0,20));
   }
   delay(time);
  }
}

void explode(int time, int c)
{
     ledcol(3,3,1,c);
     ledcol(2,3,1,c);
     ledcol(3,2,1,c);
     ledcol(2,2,1,c);
     
     delay(time*2.5);
     ledcol(2,2,2,c);
     ledcol(3,3,2,c);   
     ledcol(2,3,2,c);
     ledcol(3,2,2,c);
     delay(time*2.25);
     ledcol(3,3,1,6);
     ledcol(2,3,1,6);
     ledcol(3,2,1,6);
     ledcol(2,2,1,6);
     ledcol(2,2,3,c);
     ledcol(3,3,3,c);   
     ledcol(2,3,3,c);
     ledcol(3,2,3,c);
     delay(time*1.7);
      ledcol(2,2,4,c);
     ledcol(3,3,4,c);   
     ledcol(2,3,4,c);
     ledcol(3,2,4,c);
     ledcol(3,3,2,6);
     ledcol(2,3,2,6);
     ledcol(3,2,2,6);
     ledcol(2,2,2,6);
     delay(time*1);
     z_layer(4,c);
     delay(time);
     ledcol(1,1,3,c);
     ledcol(1,2,3,c);
     ledcol(1,3,3,c);
     ledcol(1,4,3,c);
     ledcol(2,1,3,c);
     ledcol(3,1,3,c);
     ledcol(4,1,3,c);
     ledcol(4,2,3,c);
     ledcol(4,3,3,c);
     ledcol(4,4,3,c);
     ledcol(2,4,3,c);
     ledcol(3,4,3,c);
     ledcol(3,3,3,6);
     ledcol(2,3,3,6);
     ledcol(3,2,3,6);
     ledcol(2,2,3,6);
     delay(time*1);
     ledcol(1,1,2,c);
     ledcol(1,2,2,c);
     ledcol(1,3,2,c);
     ledcol(1,4,2,c);
     ledcol(2,1,2,c);
     ledcol(3,1,2,c);
     ledcol(4,1,2,c);
     ledcol(4,2,2,c);
     ledcol(4,3,2,c);
     ledcol(4,4,2,c);
     ledcol(2,4,2,c);
     ledcol(3,4,2,c);
     z_layer(4,6);
     delay(time);
     ledcol(1,1,1,c);
     ledcol(1,2,1,c);
     ledcol(1,3,1,c);
     ledcol(1,4,1,c);
     ledcol(2,1,1,c);
     ledcol(3,1,1,c);
     ledcol(4,1,1,c);
     ledcol(4,2,1,c);
     ledcol(4,3,1,c);
     ledcol(4,4,1,c);
     ledcol(2,4,1,c);
     ledcol(3,4,1,c);
     z_layer(3,6);
     delay(time);
     z_layer(2,6);
     delay(time);
     z_layer(1,6);
     delay(time);
}
  


void diag(int time,int r, int g, int b)
{
   
  led(1,1,1,r,g,b);
  delay(time);
  led(2,1,1,r,g,b);
  led(1,2,1,r,g,b);
  led(1,1,2,r,g,b);
  delay(time);
  led(2,2,1,r,g,b);
  led(2,1,2,r,g,b);
  led(1,2,2,r,g,b);
  led(3,1,1,r,g,b);
  led(1,3,1,r,g,b);
  led(1,1,3,r,g,b);
  delay(time);
  led(2,2,2,r,g,b);
  led(3,1,2,r,g,b);
  led(1,3,2,r,g,b);
  led(2,3,1,r,g,b);
  led(3,2,1,r,g,b);
  led(2,1,3,r,g,b);
  led(1,2,3,r,g,b);
  led(1,4,1,r,g,b);
  led(1,1,4,r,g,b);
  led(4,1,1,r,g,b);
  delay(time);
  led(4,2,1,r,g,b);
  led(4,1,2,r,g,b);
  led(1,2,4,r,g,b);
  led(1,4,2,r,g,b);
  led(2,4,1,r,g,b);
  led(2,1,4,r,g,b);
  led(1,3,3,r,g,b);
  led(3,1,3,r,g,b);
  led(3,3,1,r,g,b);
  led(2,2,3,r,g,b);
  led(2,3,2,r,g,b);
  led(2,3,2,r,g,b);
  led(3,2,2,r,g,b);
  
  delay(time);
  led(1,3,4,r,g,b);
  led(3,1,4,r,g,b);
  led(2,2,4,r,g,b);
  led(1,4,3,r,g,b);
  led(4,1,3,r,g,b);
  led(2,3,3,r,g,b);
  led(3,2,3,r,g,b);
  led(4,2,2,r,g,b);
  led(2,4,2,r,g,b);
  led(3,3,2,r,g,b);
  led(3,4,1,r,g,b);
  led(4,3,1,r,g,b);
  delay(time);
  led(4,4,1,r,g,b);
  led(3,4,2,r,g,b);
  led(4,3,2,r,g,b);
  led(2,4,3,r,g,b);
  led(4,2,3,r,g,b);
  led(3,3,3,r,g,b);
  led(1,4,4,r,g,b);
  led(4,1,4,r,g,b);
  led(2,3,4,r,g,b);
  led(3,2,4,r,g,b);
  delay(time);
  led(4,4,2,r,g,b);
  led(3,4,3,r,g,b);
  led(4,3,3,r,g,b);
  led(4,4,2,r,g,b);
  led(2,4,4,r,g,b);
  led(4,2,4,r,g,b);
  led(3,3,4,r,g,b);
  delay(time);
  led(4,4,3,r,g,b);
  led(3,4,4,r,g,b);
  led(4,4,2,r,g,b);
  led(4,3,4,r,g,b);
  delay(time);
  led(4,4,4,r,g,b);
  delay(time*2);  
}
void revdiag(int time, int r, int g, int b)
{
  led(4,4,4,r,g,b);
  delay(time);
  led(4,4,3,r,g,b);
  led(3,4,4,r,g,b);
  led(4,4,2,r,g,b);
  led(4,3,4,r,g,b);
  delay(time);
  led(4,4,2,r,g,b);
  led(3,4,3,r,g,b);
  led(4,3,3,r,g,b);
  led(4,4,2,r,g,b);
  led(2,4,4,r,g,b);
  led(4,2,4,r,g,b);
  led(3,3,4,r,g,b);
  delay(time);
  led(4,4,1,r,g,b);
  led(3,4,2,r,g,b);
  led(4,3,2,r,g,b);
  led(2,4,3,r,g,b);
  led(4,2,3,r,g,b);
  led(3,3,3,r,g,b);
  led(1,4,4,r,g,b);
  led(4,1,4,r,g,b);
  led(2,3,4,r,g,b);
  led(3,2,4,r,g,b);
  delay(time);
  led(1,3,4,r,g,b);
  led(3,1,4,r,g,b);
  led(2,2,4,r,g,b);
  led(1,4,3,r,g,b);
  led(4,1,3,r,g,b);
  led(2,3,3,r,g,b);
  led(3,2,3,r,g,b);
  led(4,2,2,r,g,b);
  led(2,4,2,r,g,b);
  led(3,3,2,r,g,b);
  led(3,4,1,r,g,b);
  led(4,3,1,r,g,b);
  delay(time);
  led(4,2,1,r,g,b);
  led(4,1,2,r,g,b);
  led(1,2,4,r,g,b);
  led(1,4,2,r,g,b);
  led(2,4,1,r,g,b);
  led(2,1,4,r,g,b);
  led(1,3,3,r,g,b);
  led(3,1,3,r,g,b);
  led(3,3,1,r,g,b);
  led(2,2,3,r,g,b);
  led(2,3,2,r,g,b);
  led(2,3,2,r,g,b);
  led(3,2,2,r,g,b);
  delay(time);
   led(2,2,2,r,g,b);
  led(3,1,2,r,g,b);
  led(1,3,2,r,g,b);
  led(2,3,1,r,g,b);
  led(3,2,1,r,g,b);
  led(2,1,3,r,g,b);
  led(1,2,3,r,g,b);
  led(1,4,1,r,g,b);
  led(1,1,4,r,g,b);
  led(4,1,1,r,g,b);
  delay(time);
  led(2,2,1,r,g,b);
  led(2,1,2,r,g,b);
  led(1,2,2,r,g,b);
  led(3,1,1,r,g,b);
  led(1,3,1,r,g,b);
  led(1,1,3,r,g,b);
  delay(time);
  led(2,1,1,r,g,b);
  led(1,2,1,r,g,b);
  led(1,1,2,r,g,b);
  delay(time);
  led(1,1,1,r,g,b);
  delay(time*2);
  
  
  
 
  
 
  
}
void breathe(int color)
{
  switch(color)
  {
  case 0:
    for(int i = 0; i<20; i++)
 {
  cube(i,0,0);
 delay(20); 
 }
 delay(400);
  for(int i = 20; i>=0; i--)
 {
  cube(i,0,0);
 delay(20); 
 }
 delay(400);
    break;
    case 1:
    for(int i = 0; i<20; i++)
 {
  cube(0,i,0);
 delay(20); 
 }
 delay(400);
  for(int i = 20; i>=0; i--)
 {
  cube(0,i,0);
 delay(50); 
 }
 delay(400);
    break;
    case 2:
    for(int i = 0; i<20; i++)
 {
  cube(0,0,i);
 delay(20); 
 }
 delay(400);
  for(int i = 20; i>=0; i--)
 {
  cube(0,0,i);
 delay(20); 
 }
 delay(400);
    break;
    
  }
  

}

//non pattern code
//checkConstrains(), cube(), ledrun() and led() were copied, layer (x,y,z) functions were modified, everything else we wrote
//
//
//
//
//
//
//
///

//red - 0, orange - 1, yellow - 2, green - 3, blue - 4, purple - 5, all off - 6
void colorChoice(int c){
  resetColor();
  switch(c){
   case 0:
    color[0] = 15;
    break;
   case 1:
    color[0] = 18;
    color[1] = 4;
    break;
   case 2:
    color[0] = 15;
    color[1] = 9;
    break;
   case 3:
    color[1] = 15;
    break;
   case 4:
    color[2] = 15;
    break;
   case 5:
    color[0] = 9;
    color[2] = 15;
    break;
   case 6:
    resetColor();
  }
}

void resetColor(){
 for(int i = 0; i < 3; i++){
  color[i] = 0;
 } 
}

void x_layer(int layer, int c) {
  // Cycle through layer
  colorChoice(c);
  
  for(int z = 1; z <= 4; z++) {
    // Cycle through LEDs
    for(int y = 1; y <= 4; y++) {
      led(layer, y, z, color[0], color[1], color[2]);
    } // LEDs
  }
}

void y_layer(int layer, int c) {
  // Cycle throug layer
  colorChoice(c);
  
  for(int z = 1; z <= 4; z++) {
    // Cycle through LEDs
    for(int x = 1; x <= 4; x++) {
      led(x, layer, z, color[0], color[1], color[2]);
    } // LEDs
  }
}

void z_layer(int layer, int c) {
  // Cycle through LEDs
  colorChoice(c);
  
  for(int y = 1; y <= 4; y++) {
    for(int x = 1; x <= 4; x++) {
      led(x, y, layer, color[0], color[1], color[2]);
    } // LEDs
  }
}

void cube(byte red, byte green, byte blue) {
  // Cycle throug layer
  for(int z = 1; z <= 4; z++) {
    // Cycle through LEDs
    for(int y = 1; y <= 4; y++) {
      for(int x = 1; x <= 4; x++) {
        led(x, y, z, red, green, blue);
      }
    } // LEDS
  }
}

void led(int x, int y, int z, byte red, byte green, byte blue) {
  int byteNum;
  int bitPos;
  // Check parameters
  x = checkConstrains(x, 1, 4);
  y = checkConstrains(y, 1, 4);
  z = checkConstrains(z, 1, 4);
  red = checkConstrains(red, 0, 31);
  green = checkConstrains(green, 0, 31);
  blue = checkConstrains(blue, 0, 31);
  
  // There are two bytes per color => 16 Bit <=> 16 LED for each Layer
  // First or second byte 
  // z - 1 = Layer 0-3
  if(x > 2) {
    byteNum = 1 + 2*(z-1);
  } else {
    byteNum = 0 + 2*(z-1);
  }
  // Which bit of the byte do we want to write
  if(x == 2 || x == 4) {
    bitPos = 4 + y - 1;
  } else {
    bitPos = y - 1;
  }

  // RED (5-bit BAM => each array equals one bit)
  bitWrite(red0[byteNum], bitPos, bitRead(red, 0));
  bitWrite(red1[byteNum], bitPos, bitRead(red, 1));
  bitWrite(red2[byteNum], bitPos, bitRead(red, 2));
  bitWrite(red3[byteNum], bitPos, bitRead(red, 3));
  bitWrite(red4[byteNum], bitPos, bitRead(red, 4));

  // GREEN
  bitWrite(green0[byteNum], bitPos, bitRead(green, 0));
  bitWrite(green1[byteNum], bitPos, bitRead(green, 1));
  bitWrite(green2[byteNum], bitPos, bitRead(green, 2));
  bitWrite(green3[byteNum], bitPos, bitRead(green, 3));
  bitWrite(green4[byteNum], bitPos, bitRead(green, 4));

  // BLUE
  bitWrite(blue0[byteNum], bitPos, bitRead(blue, 0));
  bitWrite(blue1[byteNum], bitPos, bitRead(blue, 1));
  bitWrite(blue2[byteNum], bitPos, bitRead(blue, 2));
  bitWrite(blue3[byteNum], bitPos, bitRead(blue, 3));
  bitWrite(blue4[byteNum], bitPos, bitRead(blue, 4));
}

int checkConstrains(int value, int min, int max) {
  if(value < min) {
    return min;
  } else if (value > max) {
    return max;
  } else {
    return value;
  }
}

void ledrun(int time) {
  // Cycle throug color

    for(int i = 1; i<5; ++i)
    {
      led(i,1,1,15,0,0);
      delay(time);
      led(i,1,1,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,2,1,15,0,0);
      delay(time);
      led(i,2,1,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,3,1,15,0,0);
      delay(time);
      led(i,3,1,0,0,0);
      delay(time/2);
    }
     for(int i = 4; i>0; --i)
    {
      led(i,4,1,15,0,0);
      delay(time);
      led(i,4,1,0,0,0);
      delay(time/2);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,4,2,15,0,0);
      delay(time);
      led(i,4,2,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,3,2,15,0,0);
      delay(time);
      led(i,3,2,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,2,2,15,0,0);
      delay(time);
      led(i,2,2,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,1,2,15,0,0);
      delay(time);
      led(i,1,2,0,0,0);
      delay(time/2);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,1,3,15,0,0);
      delay(time);
      led(i,1,3,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,2,3,15,0,0);
      delay(time);
      led(i,2,3,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,3,3,15,0,0);
      delay(time);
      led(i,3,3,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,4,3,15,0,0);
      delay(time);
      led(i,4,3,0,0,0);
      delay(time);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,4,4,15,0,0);
      delay(time);
      led(i,4,4,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,3,4,15,0,0);
      delay(time);
      led(i,3,4,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,2,4,15,0,0);
      delay(time);
      led(i,2,4,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,1,4,15,0,0);
      delay(time);
      led(i,1,4,0,0,0);
      delay(time/2);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,1,4,0,15,0);
      delay(time);
      led(i,1,4,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,2,4,0,15,0);
      delay(time);
      led(i,2,4,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,3,4,0,15,0);
      delay(time);
      led(i,3,4,0,0,0);
      delay(time/2);
    }
     for(int i = 4; i>0; --i)
    {
      led(i,4,4,0,15,0);
      delay(time);
      led(i,4,4,0,0,0);
      delay(time/2);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,4,3,0,15,0);
      delay(time);
      led(i,4,3,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,3,3,0,15,0);
      delay(time);
      led(i,3,3,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,2,3,0,15,0);
      delay(time);
      led(i,2,3,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,1,3,0,15,0);
      delay(time);
      led(i,1,3,0,0,0);
      delay(time/2);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,1,2,0,15,0);
      delay(time);
      led(i,1,2,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,2,2,0,15,0);
      delay(time);
      led(i,2,2,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,3,2,0,15,0);
      delay(time);
      led(i,3,2,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,4,2,0,15,0);
      delay(time);
      led(i,4,2,0,0,0);
      delay(time/2);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,4,1,0,15,0);
      delay(time);
      led(i,4,1,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,3,1,0,15,0);
      delay(time);
      led(i,3,1,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,2,1,0,15,0);
      delay(time);
      led(i,2,1,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,1,1,0,15,0);
      delay(time);
      led(i,1,1,0,0,0);
      delay(time/2);
    }
    
    for(int i = 1; i<5; ++i)
    {
      led(i,1,1,0,0,15);
      delay(time);
      led(i,1,1,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,2,1,0,0,15);
      delay(time);
      led(i,2,1,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,3,1,0,0,15);
      delay(time);
      led(i,3,1,0,0,0);
      delay(time/2);
    }
     for(int i = 4; i>0; --i)
    {
      led(i,4,1,0,0,15);
      delay(time);
      led(i,4,1,0,0,0);
      delay(time/2);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,4,2,0,0,15);
      delay(time);
      led(i,4,2,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,3,2,0,0,15);
      delay(time);
      led(i,3,2,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,2,2,0,0,15);
      delay(time);
      led(i,2,2,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,1,2,0,0,15);
      delay(time);
      led(i,1,2,0,0,0);
      delay(time/2);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,1,3,0,0,15);
      delay(time);
      led(i,1,3,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,2,3,0,0,15);
      delay(time);
      led(i,2,3,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,3,3,0,0,15);
      delay(time);
      led(i,3,3,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,4,3,0,0,15);
      delay(time);
      led(i,4,3,0,0,0);
      delay(time);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,4,4,0,0,15);
      delay(time);
      led(i,4,4,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,3,4,0,0,15);
      delay(time);
      led(i,3,4,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,2,4,0,0,15);
      delay(time);
      led(i,2,4,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,1,4,0,0,15);
      delay(time*2);
      led(i,1,4,0,0,0);
      delay(time/2);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,1,4,15,0,0);
      delay(time);
      led(i,1,4,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,1,3,15,0,0);
      delay(time);
      led(i,1,3,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,1,2,15,0,0);
      delay(time);
      led(i,1,2,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,1,1,15,0,0);
      delay(time);
      led(i,1,1,0,0,0);
      delay(time/2);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,1,1,0,15,0);
      delay(time);
      led(i,1,1,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,1,2,0,15,0);
      delay(time);
      led(i,1,2,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,1,3,0,15,0);
      delay(time);
      led(i,1,3,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,1,4,0,15,0);
      delay(time);
      led(i,1,4,0,0,0);
      delay(time/2);
    }
    for(int i = 1; i<5; ++i)
    {
      led(i,1,4,0,0,15);
      delay(time);
      led(i,1,4,0,0,0);
      delay(time/2);
      
    }
     for(int i = 4; i>0; --i)
    {
      led(i,1,3,0,0,15);
      delay(time);
      led(i,1,3,0,0,0);
      delay(time/2);
    }
     for(int i = 1; i<5; ++i)
    {
      led(i,1,2,0,0,15);
      delay(time);
      led(i,1,2,0,0,0);
      delay(time/2);
    } for(int i = 4; i>0; --i)
    {
      led(i,1,1,0,0,15);
      delay(time);
      led(i,1,1,0,0,0);
      delay(time/2);
    }    
}

void ledcol(int x, int y, int z, int c)
{
  
  colorChoice(c);
  led(x,y,z,color[0], color[1], color[2]);
  
}
