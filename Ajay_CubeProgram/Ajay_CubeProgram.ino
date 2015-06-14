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
cube(0,0,0);
delay(500);
ledswirl(50,0,0);
ledswirl(50,4,1);
delay(500);



 

}
void ledswirl(int time, int c, int dir)
{
  if(dir == 1)
  {
  for(int y = 1; y<5; y++)
  {
    for(int x = 1; x<5; x++)
    {
      ledcol(x,y,1,c);
      delay(time);
      ledcol(x,y,1,6);    
    }
    for(int z = 1; z<5; z++)
    {
      ledcol(4,y,z,c);
      delay(time);
      ledcol(4,y,z,6);    
    }
    for(int x = 4; x>0; x--)
    {
      ledcol(x,y,4,c);
      delay(time);
      ledcol(x,y,4,6);   
    }
    for(int z = 4; z>0; z--)
    {
      ledcol(1,y,z,c);
      delay(time);
      ledcol(1,y,z,6); 
    }  
   }  
  }
  if(dir == 0)
  {
  for(int y = 4; y>0; y--)
  {
    for(int x = 1; x<5; x++)
    {
      ledcol(x,y,1,c);
      delay(time);
      ledcol(x,y,1,6);    
    }
    for(int z = 1; z<5; z++)
    {
      ledcol(4,y,z,c);
      delay(time);
      ledcol(4,y,z,6);    
    }
    for(int x = 4; x>0; x--)
    {
      ledcol(x,y,4,c);
      delay(time);
      ledcol(x,y,4,6);   
    }
    for(int z = 4; z>0; z--)
    {
      ledcol(1,y,z,c);
      delay(time);
      ledcol(1,y,z,6); 
    }  
   }  
  }
}
void swirl(int time, int c)
{
  column(3,0,2,c);
  column(3,0,1,c); 
  delay(time);
  column(4,0,1,c);
  delay(time);
  column(4,0,2,c); 
  delay(time);
  column(3,0,3,c);
  column(4,0,3,c);
  delay(time); 
  column(4,0,4,c);
  delay(time);
  column(3,0,4,c); 
  delay(time);
  column(2,0,3,c);
  column(2,0,4,c);
  delay(time); 
  column(1,0,4,c);
  delay(time);
  column(1,0,3,c);
  delay(time);
  column(2,0,2,c); 
  column(1,0,2,c);
  delay(time);
  column(1,0,1,c); 
  delay(time);
  column(2,0,1,c);
  delay(time);
}
void ending(int time)
{
   for(int z =4;z>0;--z)
   {
      for(int y =4;y>0;--y)
      {
         for(int x =4;x>0;--x)
         {
           if(z==1)
             time+=20;
           if(x==1&&z==1&&y==1)
           delay(500);
           ledcol(x,y,z,6);
           delay(time);
           
         } 
       } 
    }  
}
void xyz(int time, int c)
{
 
column(4,4,0,c);

for(int i = 1; i<5; i++)
{
  column(0,4,i,c);
column(4,0,i,c);
delay(time);
column(0,4,i,6);
column(4,0,i,6); 
column(4,4,0,c);
}
for(int i = 3; i>0; i--)
{
column(0,4,i,c);
column(4,0,i,c);
delay(time);
column(0,4,i,6);
column(4,0,i,6);
column(4,4,0,c);
}
column(4,4,0,6);
column(0,4,1,c);
for(int i = 3; i>0; i--)
{
column(i,4,0,c);
column(i,0,1,c);
delay(time);
column(i,4,0,6);
column(i,0,1,6);
column(0,4,1,c);
}
for(int i = 2; i<5; i++)
{
  column(i,4,0,c);
column(i,0,1,c);
delay(time);
column(i,4,0,6);
column(i,0,1,6); 
column(0,4,1,c);
}
column(0,4,1,6);
column(4,0,1,c);
for(int i = 3; i>0; i--)
{
column(0,i,1,c);
column(4,i,0,c);
delay(time);
column(4,i,0,6);
column(0,i,1,6);
column(4,0,1,c);
}
for(int i = 2; i<5; i++)
{
  column(4,i,0,c);
column(0,i,1,c);
delay(time);
column(4,i,0,6);
column(0,i,1,6); 
column(4,0,1,c);
}


  
}
void newroll(int time, int c)
{
  z_layer(1,c);
  delay(time); 
  column(0,4,2,c);
  column(0,1,1,6);
  delay(time);
  column(0,4,3,c);
  column(0,2,1,6);
  delay(time);
  column(0,4,4,c);
  column(0,3,1,6);
  delay(time);
  column(0,3,4,c);
  column(0,4,1,6);
  delay(time);
  column(0,2,4,c);
  column(0,4,2,6);
  delay(time);
  column(0,1,4,c);
  column(0,4,3,6);
  delay(time);
  column(0,1,3,c);
  column(0,4,4,6);
  delay(time);
  column(0,1,2,c);
  column(0,3,4,6);
  delay(time);
  column(0,1,1,c);
  column(0,2,4,6);
  delay(time);
  column(0,2,1,c);
  column(0,1,4,6);
  delay(time);
  column(0,3,1,c);
  column(0,1,3,6);
  delay(time);
  
}
void outrand(int time)
{
  for(int i = 0; i<2; i++)
  {
   led(1, random(1,5),random(1,5),random(5,16),random(5,16),random(5,16));
   led(random(1,5),1 ,random(1,5),random(5,16),random(5,16),random(5,16));
   delay(time);
   cube(0,0,0);
   led(4, random(1,5),random(1,5),random(5,16),random(5,16),random(5,16));
   led(random(1,5), 4,random(1,5),random(5,16),random(5,16),random(5,16) );
   delay(time);
  }
  
  
}
void roll(int time, int c)
{
  int x = 1;
  int y = 1;

    for(y;y<5;y++)
    {
      ledcol(x,1,y,c); 
      ledcol(x,2,y,c);
      ledcol(x,3,y,c);
      ledcol(x,4,y,c);
      delay(time);
      ledcol(x,1,y,6); 
      ledcol(x,2,y,6);
      ledcol(x,3,y,6);
      ledcol(x,4,y,6);
    }
    
    for(x=2;x<5;x++)
    {
      ledcol(x,1,y,c); 
      ledcol(x,2,y,c);
      ledcol(x,3,y,c);
      ledcol(x,4,y,c);
      delay(time);
      ledcol(x,1,y,6); 
      ledcol(x,2,y,6);
      ledcol(x,3,y,6);
      ledcol(x,4,y,6);
    }
    

    for(y=3;y>0;y--)
    {
      ledcol(x,1,y,c); 
      ledcol(x,2,y,c);
      ledcol(x,3,y,c);
      ledcol(x,4,y,c);
      delay(time);
      ledcol(x,1,y,6); 
      ledcol(x,2,y,6);
      ledcol(x,3,y,6);
      ledcol(x,4,y,6);
    }
    

    for(x=3;x>0;x--)
    {
      ledcol(x,1,y,c); 
      ledcol(x,2,y,c);
      ledcol(x,3,y,c);
      ledcol(x,4,y,c);
      delay(time);
      ledcol(x,1,y,6); 
      ledcol(x,2,y,6);
      ledcol(x,3,y,6);
      ledcol(x,4,y,6);
    } 
}
void colmove(int time, int c)
{
  int x = 1;
  int y = 1;

    for(y;y<5;y++)
    {
      ledcol(x,y,1,c); 
      ledcol(x,y,2,c);
      ledcol(x,y,3,c);
      ledcol(x,y,4,c);
      delay(time);
      ledcol(x,y,1,6); 
      ledcol(x,y,2,6);
      ledcol(x,y,3,6);
      ledcol(x,y,4,6);
    }
    
    for(x=2;x<5;x++)
    {
      ledcol(x,y,1,c); 
      ledcol(x,y,2,c);
      ledcol(x,y,3,c);
      ledcol(x,y,4,c);
      delay(time);
      ledcol(x,y,1,6); 
      ledcol(x,y,2,6);
      ledcol(x,y,3,6);
      ledcol(x,y,4,6);
    }
    

    for(y=3;y>0;y--)
    {
      ledcol(x,y,1,c); 
      ledcol(x,y,2,c);
      ledcol(x,y,3,c);
      ledcol(x,y,4,c);
      delay(time);
      ledcol(x,y,1,6); 
      ledcol(x,y,2,6);
      ledcol(x,y,3,6);
      ledcol(x,y,4,6);
    }
    

    for(x=3;x>0;x--)
    {
      ledcol(x,y,1,c); 
      ledcol(x,y,2,c);
      ledcol(x,y,3,c);
      ledcol(x,y,4,c);
      delay(time);
      ledcol(x,y,1,6); 
      ledcol(x,y,2,6);
      ledcol(x,y,3,6);
      ledcol(x,y,4,6);
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
void colorMorph(int time) {
  int red, green, blue;
  int keepColorTime = time * 150;
  
  delay(keepColorTime);
  // RED + GREEN
  for(int green = 0; green <= 31; green++) {
    cube(31, green, 0);
    delay(time);
  }
  delay(keepColorTime);
  // GREEN - RED
  for(int red = 31; red >= 0; red --) {
    cube(red, 31, 0);
    delay(time);
  }
  delay(keepColorTime);
  // GREEN + BLUE
  for(int blue = 0; blue <= 31; blue++) {
    cube(0, 31, blue);
    delay(time);
  }
  delay(keepColorTime);
  // BLUE - GREEN
  for(int green = 31; green >= 0; green --) {
    cube(0, green, 31);
    delay(time);
  }
  delay(keepColorTime);
  // BLUE + RED
  for(int red = 0; red <= 31; red++) {
    cube(red, 0,31);
    delay(time);
  }
  delay(keepColorTime);
  // RED - BLUE + GREEN
  green = 0;
  for(int blue = 31; blue >= 0; blue --) {
    cube(31, green, blue);
    delay(time);
    green++;
  }
  delay(keepColorTime);
  // GREEN - RED + BLUE
  blue = 0;
  for(int red = 31; red >= 0; red --) {
    cube(red, 31, blue);
    delay(time);
    blue++;
  }
  delay(keepColorTime);
  // GREEN + RED + BLUE
  for(int red = 0; red <= 31; red++) {
    cube(red, 31, 31);
    delay(time);
  }
  delay(keepColorTime);
  // RED - GREEN - BLUE
  blue = 31;
  for(int green = 31; green >= 0; green --) {
    cube(31, green, blue);
    delay(time);
    blue--;
  }
}


/**
 * colorSetting:
 *   - 1 randomColor
 *   - 2 blueColor
 *   - 3 randomColor2 (better colors)
 */
void smoothRain(int time, int dropsTotal, int maxDropsPerCycle, int colorSetting) {
  // Check params
  maxDropsPerCycle = checkConstrains(maxDropsPerCycle, 1, 16);
  // Current amount of raindrops falling
  int numRainDrops = 0;
  // Amount of raindrops launched
  int numLaunchedRaindrops = 0;
  // Rain drops to launch this cycle
  int numDropsToLaunch = 0;
  // Each tower can have a drop at different pos 0 = none, 1 = top, 2 = uppermiddle, 3 = lowermiddle, 4 = bottom, 5 = bottom2
  int rainArray[16];
  // Seem to be some problems so init this array with 0
  for(int l = 0; l < 16; l++) {
    rainArray[l] = 0;
  }
  // Color of each towers raindrop
  byte colorArrayRed[16];
  byte colorArrayGreen[16];
  byte colorArrayBlue[16];
  // Color array to call functions on
  byte colorArray[3];
  // Current pos that is edited/update, ...
  int pos = 0;
  // Rain until launched rain drops == total raindrops and all rain drops
  // have fallen to ground => numRainDrops == 0
  while(numLaunchedRaindrops < dropsTotal || numRainDrops > 0) {

    // How many raindrops should be launched
    if(numLaunchedRaindrops < dropsTotal) {
      // How many?
      if((dropsTotal - numLaunchedRaindrops) > maxDropsPerCycle) {
        
        numDropsToLaunch = random(1, maxDropsPerCycle);
      } else {
        
        numDropsToLaunch = random(1, dropsTotal - numLaunchedRaindrops);
      }
    } else {
      
      numDropsToLaunch = 0;
    }
    // Make sure we don't try to launch more raindrops than we have empty pos for
    if((numDropsToLaunch + numRainDrops) > 16) {
      numDropsToLaunch = 16 - numRainDrops;
    }

    // Update existing LEDs
    for(int updPos = 0; updPos < 16; updPos++) {
      // Let raindrop sink if one exists at that pos
      if(rainArray[updPos] > 0) {
        rainArray[updPos]++;
        // Let raindrop vanish after hitting bottom
        if(rainArray[updPos] > 5) {
          rainArray[updPos] = 0;
          numRainDrops--;
        }
      }
    }

    // Launch new raindrops
    for(int i = 0; i < numDropsToLaunch; i++) {
      bool posFound = false;
      // Find an empty position
      int availPos = 16 - numRainDrops;
      int addAtXthFreePos = random(1, availPos);
      int counter = 0;
      for(int k = 0; k < 16; k++) {
        if(rainArray[k] == 0) {
          counter++;
        }
        if(counter == addAtXthFreePos) {
          pos = k;
          break;
        }
        
      }

      // Translate pos to x,y
      int x = pos % 4; // [0,3]
      int y = ((pos - x) / 4) + 1; // [1,4]
      x++; // Normalize x from [0,3] to [1,4]
      // Generate color
      if(colorSetting == 1) {
        randomColor(colorArray);
      } else if(colorSetting == 2) {
        blueColor(colorArray);  
      } else {
        randomColor2(colorArray);
      }

      // Update colorArrays
      colorArrayRed[pos] = colorArray[0];
      colorArrayGreen[pos] = colorArray[1];
      colorArrayBlue[pos] = colorArray[2];

      // Update rainArray
      rainArray[pos] = 1;
      numRainDrops++;
      numLaunchedRaindrops++;
    }

    // Update cube in steps to fade color transition
    int totalSteps = 16;
    for(int step = 1; step <= totalSteps; step++) {
      // Update raindrops
      for(int updPos = 0; updPos < 16; updPos++) {
        // Translate pos to x,y
        int x = updPos % 4; // [0,3]
        int y = ((updPos - x) / 4) + 1; // [1,4]
        x++; // Normalize x from [0,3] to [1,4]

        // Which layer
        int z = 5 - rainArray[updPos]; // [0,4] 
        z = checkConstrains(z, 1, 4); // normalized to [1,4] 
        if(rainArray[updPos] == 0) {
          z = 1;
        } 

        int remainingSteps = totalSteps / step;

        // UPDATE CURRENT LED
        if(rainArray[updPos] != 0) {
          // Get current color
          getColor(x, y, z, colorArray);
          // Calc next color
          calcUpdatedColor(remainingSteps, colorArray, colorArrayRed[updPos], colorArrayGreen[updPos], colorArrayBlue[updPos]);
          // Update LED       
          led(x, y, z, colorArray[0], colorArray[1], colorArray[2]);
        }
        // UPDATE PREVIOUS LED
          // Only update if pos = 0 => maybe an led needs to fade out
          // or pos < 1 => Top layer has no previous led
          if(rainArray[updPos] != 1) {
            if(rainArray[updPos] != 0) {
              z++;
            }
            getColor(x, y, z, colorArray);
            // Calc next color, target if OFF
            calcUpdatedColor(remainingSteps, colorArray, 0, 0, 0);
            // Update LED       
            led(x, y, z, colorArray[0], colorArray[1], colorArray[2]);
          }
        // UPDATE DONE
      }
      // Wait
      delay(time);

    }

  }
}

void calcUpdatedColor(int remainingSteps, byte colorArray[], byte targetRed, byte targetGreen, byte targetBlue) {
  int delta, stepSize, curColor, tarColor;
  curColor = colorArray[0]; 
  tarColor = targetRed; 
  delta = tarColor - curColor;
  stepSize = delta / remainingSteps;
  // New color
  colorArray[0] = curColor + stepSize;

  curColor = colorArray[1];
  tarColor = targetGreen; 
  delta = tarColor - curColor;
  stepSize = delta / remainingSteps;
  // New color
  colorArray[1] = curColor + stepSize;

  curColor = colorArray[2];
  tarColor = targetBlue;
  delta = tarColor - curColor;
  stepSize = delta / remainingSteps;
  // New color
  colorArray[2] = curColor + stepSize;
}


/* ##########################################################
 * ######################### STOP ###########################
 * ########################################################## */

void randomColor(byte colorArray[]) {
  int minDelta = 30;
  int deltaRG, deltaGB;
  colorArray[0] = random(8, 31);
  colorArray[1] = random(8, 31);
  colorArray[2] = random(8, 31);
  // To get better colors and not white make sure the 
  // delta between colors is at least minDelta
  if(colorArray[0] > colorArray[1]) {
    deltaRG = colorArray[0] - colorArray[1];
  } else {
    deltaRG = colorArray[1] - colorArray[0];
  }
  if(colorArray[1] > colorArray[2]) {
    deltaGB = colorArray[1] - colorArray[2];
  } else {
    deltaGB = colorArray[2] - colorArray[1];
  }

  int delta = deltaRG + deltaGB;

  int diff = delta - minDelta;

  // if diff is negative the delta is not big enough
  if(diff < 0) {
    int removeColor = random(0, 2);
    colorArray[removeColor] = colorArray[removeColor] + diff;
    colorArray[removeColor] = checkConstrains(colorArray[removeColor], 0, 31);
    // Add some color to another
    int addColor = random(0, 2);
    if(addColor != removeColor) {
      colorArray[addColor] = colorArray[addColor] + random(1, 20);
      colorArray[addColor] = checkConstrains(colorArray[addColor], 0, 31);
    }
  }

}

void randomColor2(byte colorArray[]) {
  // Set one color to full
  int bright = random(0, 2);
  colorArray[bright] = 31;
  int first = 1;
  for(int i = 0; i < 3; i++) {
    if(i != bright) {
      if(first = 1) {
        int bright2 = random(0, 5);
        if(bright2 > 3) {
          colorArray[i] = 31;
        } else if(bright2 > 1) {
          colorArray[i] = random(10, 31);  
        } else {
          colorArray[i] = 0;
        }
        first = 0;
      } else {
        int show = random(0, 1);
        if(show == 1) {
          colorArray[i] = random(0, 15);  
        } else {
          colorArray[i] = 0;
        }        
      }
    }
  }
}

void blueColor(byte colorArray[]) {
  colorArray[1] = random(5, 31);
  colorArray[2] = random(colorArray[1], 31);
  // colorArray[0] = random(0, colorArray[1]);
  // if(colorArray[0] > 2) {
  //   colorArray[0] = colorArray[0] - 2;
  // }
  colorArray[0] = 0;
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

void ledTest(int time) {
  // Cycle throug color
  for(int c = 1; c <= 3; c++) {
    // Cycle throug layer
    for(int l = 1; l <= 4; l++) {
      // Cycle through LEDs
      for(int y = 1; y <= 4; y++) {
        for(int x = 1; x <= 4; x++) {
          // Cycle through brightness
          for(int b = 0; b <= 15; b++) {
            switch (c) {
                case 1:
                  led(x, y, l, b, 0, 0);
                  break;
                case 2:
                  led(x, y, l, 0, b, 0);
                  break;
                default:
                  led(x, y, l, 0, 0, b);
            }
            delay(time);  
          } // brightness
        }
      } // LEDs
    }// Layse
  }// Color
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
    color[0] = 0;
    color[1] = 0;
    color[2] = 0;
    break;
  }
}

void resetColor(){
 for(int i = 0; i < 3; i++){
  color[i] = 0;
 } 
}
void layers( int time)
{
 z_layer(1,4); 
 delay(time);
 z_layer(2,4);
 delay(time);
 z_layer(3,4);
 delay(time);
 z_layer(4,4);
 delay(time);
 z_layer(4,3); 
 delay(time);
 z_layer(3,3);
 delay(time);
 z_layer(2,3);
 delay(time);
 z_layer(1,3);
 delay(time);
  
}


void ledcol(int x, int y, int z, int c)
{
  
  colorChoice(c);
  led(x,y,z,color[0], color[1], color[2]);
  
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

void getColor(int x, int y, int z, byte colorArray[]) {
  int byteNum;
  int bitPos;
  // Check parameters
  x = checkConstrains(x, 1, 4);
  y = checkConstrains(y, 1, 4);
  z = checkConstrains(z, 1, 4);

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

  // RED
  colorArray[0] = 0;
  bitWrite(colorArray[0], 0, bitRead(red0[byteNum], bitPos));
  bitWrite(colorArray[0], 1, bitRead(red1[byteNum], bitPos));
  bitWrite(colorArray[0], 2, bitRead(red2[byteNum], bitPos));
  bitWrite(colorArray[0], 3, bitRead(red3[byteNum], bitPos));
  bitWrite(colorArray[0], 4, bitRead(red4[byteNum], bitPos));

  // GREEN
  colorArray[1] = 0;
  bitWrite(colorArray[1], 0, bitRead(green0[byteNum], bitPos));
  bitWrite(colorArray[1], 1, bitRead(green1[byteNum], bitPos));
  bitWrite(colorArray[1], 2, bitRead(green2[byteNum], bitPos));
  bitWrite(colorArray[1], 3, bitRead(green3[byteNum], bitPos));
  bitWrite(colorArray[1], 4, bitRead(green4[byteNum], bitPos));

  // BLUE
  colorArray[2] = 0;
  bitWrite(colorArray[2], 0, bitRead(blue0[byteNum], bitPos));
  bitWrite(colorArray[2], 1, bitRead(blue1[byteNum], bitPos));
  bitWrite(colorArray[2], 2, bitRead(blue2[byteNum], bitPos));
  bitWrite(colorArray[2], 3, bitRead(blue3[byteNum], bitPos));
  bitWrite(colorArray[2], 4, bitRead(blue4[byteNum], bitPos));
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
