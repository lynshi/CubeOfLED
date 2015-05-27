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
  
  for(int i = 0; i < 4; i++){
    cube(0,0,0);
    rainbow(1050-250*i);
  }
  
  for(int i = 0; i < 3; i++){ //0 - red, 1 - green, 2 - blue
    cube(0,0,0);
    vertical(i);
  }
  
  allVertical();
  
  for(int i = 0; i < 3; i++){ //0 - red, 1 - green, 2 - blue
    cube(0,0,0);
    horizontal(i);
  }
  
  allHorizontal();
  
    cube(0,0,0);
    
    for(int i = 0; i < 3; i++){
      wave(i);
    }
}

void rainbow(int time){
  cube(15,0,0);
  delay(time);
  cube(18,4,0);
  delay(time);
  cube(15,9,0);
  delay(time);
  cube(0,15,0);
  delay(time);
  cube(0,0,15);
  delay(time);
  cube(9,0,15);
  delay(time);
}

void vertical(int color){
  int r = 0, g = 0, b = 0;
  int time = 100;
  
  switch(color){
   case 0:
     r = 15;
     break;
   case 1:
     g = 15;
     break;
   case 2:
     b = 15;
     break; 
  }
  
  for(int z = 1; z <= 4; z++){
   delay(time);
    for(int x = 1; x <= 4; x++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, r, g, b);
     } 
    }
  }
  delay(time);
}

void allVertical(){
  int time = 100;
  
  for(int z = 1; z <= 4; z++){
   delay(time);
    for(int x = 1; x <= 4; x++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 15, 0, 0);
     } 
    }
  }
  for(int z = 4; z >= 1; z--){
   delay(time);
    for(int x = 1; x <= 4; x++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 0, 15, 0);
     } 
    }
  }
  for(int z = 1; z <= 4; z++){
   delay(time);
    for(int x = 1; x <= 4; x++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 0, 0, 15);
     } 
    }
  }
  for(int z = 4; z >= 1; z--){
   delay(time);
    for(int x = 1; x <= 4; x++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 15, 0, 0);
     } 
    }
  }
  for(int z = 1; z <= 4; z++){
   delay(time);
    for(int x = 1; x <= 4; x++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 0, 15, 0);
     } 
    }
  }
  for(int z = 4; z >= 1; z--){
   delay(time);
    for(int x = 1; x <= 4; x++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 0, 0, 15);
     } 
    }
  }
  delay(time);
}

void horizontal(int color){
  int r = 0, g = 0, b = 0;
  int time = 100;
  
  switch(color){
   case 0:
     r = 15;
     break;
   case 1:
     g = 15;
     break;
   case 2:
     b = 15;
     break; 
  }
  
  for(int x = 1; x <= 4; x++){
   delay(time);
    for(int z = 1; z <= 4; z++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, r, g, b);
     } 
    }
  }
  delay(time);
}

void allHorizontal(){
  int time = 100;
  
  for(int x = 1; x <= 4; x++){
   delay(time);
    for(int z = 1; z <= 4; z++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 15, 0, 0);
     } 
    }
  }
  for(int x = 4; x >= 1; x--){
   delay(time);
    for(int z = 1; z <= 4; z++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 0, 15, 0);
     } 
    }
  }
  for(int x = 1; x <= 4; x++){
   delay(time);
    for(int z = 1; z <= 4; z++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 0, 0, 15);
     } 
    }
  }
  for(int x = 4; x >= 1; x--){
   delay(time);
    for(int z = 1; z <= 4; z++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 15, 0, 0);
     } 
    }
  }
  for(int x = 1; x <= 4; x++){
   delay(time);
    for(int z = 1; z <= 4; z++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 0, 15, 0);
     } 
    }
  }
  for(int x = 4; x >= 1; x--){
   delay(time);
    for(int z = 1; z <= 4; z++){
     for(int y = 1; y <= 4; y++){
      led(x, y, z, 0, 0, 15);
     } 
    }
  }
  delay(time);
}

void wave(int color){
  int r = 0, g = 0, b = 0;
  int time = 100;
  int x = 1;
  
  switch(color){
   case 0:
     r = 15;
     break;
   case 1:
     g = 15;
     break;
   case 2:
     b = 15;
     break; 
  }
  
  int z = 0;
  for(z = 1; z <= 4; z++){
      for(int y = 1; y <= 4; y++){
        led(x,y,z,r,g,b);
      }
      x++;
  }
  delay(time);
  cube(0,0,0);
  
  x = 1;
  for(int i = 1; i <= 4; i++){
     if(x == 1 || x == 2){
      z = 2; 
     }
     else if(x == 3 || x == 4){
      z = 3; 
     }
     for(int y = 1; y <= 4; y++){
       led(x,y,z,r,g,b);
     }
     x++;
  }
  delay(time);
  cube(0,0,0);
  
  x = 1;
  for(int i = 1; i <= 4; i++){
      if(x == 1 || x == 2){
        z = 3; 
      }
      else if(x == 3 || x == 4){
        z = 2; 
      }
      for(int y = 1; y <= 4; y++){
        led(x,y,z,r,g,b);
      }
      x++;
  }
  delay(time);
  cube(0,0,0);
  
  x = 4;
  for(z = 1; z <= 4; z++){
      for(int y = 1; y <= 4; y++){
        led(x,y,z,r,g,b);
      }
      --x;
  }
  delay(time);
  cube(0,0,0);
  
  x = 1;
  for(int i = 1; i <= 4; i++){
      if(x == 1 || x == 2){
        z = 3; 
      }
      else if(x == 3 || x == 4){
        z = 2; 
      }
      for(int y = 1; y <= 4; y++){
        led(x,y,z,r,g,b);
      }
      x++;
  }
  delay(time);
  cube(0,0,0);
  
  x = 1;
  for(int i = 1; i <= 4; i++){
     if(x == 1 || x == 2){
      z = 2; 
     }
     else if(x == 3 || x == 4){
      z = 3; 
     }
     for(int y = 1; y <= 4; y++){
       led(x,y,z,r,g,b);
     }
     x++;
  }
  delay(time);
  cube(0,0,0);
  
  x = 1;
  for(z = 1; z <= 4; z++){
      for(int y = 1; y <= 4; y++){
        led(x,y,z,r,g,b);
      }
      x++;
  }
  delay(time);
  cube(0,0,0);
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
