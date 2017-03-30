#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

//////////////////RASPBERRY PI SETUP///////////////////////////////////
// See README.md for details on 

// Program originally from Keith Ong
// Modified by Thomas Lindner for Francium temperature readout
// Put eight thermistor measurements into a text file every 5 seconds



/////////////////LTC2495 ADDRESSES AND CHANNELS////////////////////////////////

#define  ADC_ADD1 0x44  //I2C address of ADC1, for channels labeled CH0-CH15
#define  ADC_ADD2 0x45  //I2C address of ADC2, for channels labeled CH16-CH31

//ADC Input Channels (not the same as CH labels on PCB)
#define IN0  0XB0
#define IN1  0XB8
#define IN2  0XB1
#define IN3  0xB9
#define IN4  0xB2
#define IN5  0XBA
#define IN6  0XB3
#define IN7  0XBB
#define IN8  0XB4
#define IN9  0xBC
#define IN10 0xB5
#define IN11 0xBD
#define IN12 0xB6
#define IN13 0xBE
#define IN14 0xB7
#define IN15 0xBF


//Mapping CH labels on PCB to ADC input channels
//ADC1
#define CH0 IN8
#define CH1 IN9
#define CH2 IN10
#define CH3 IN11
#define CH4 IN12
#define CH5 IN13
#define CH6 IN14
#define CH7 IN15
#define CH8 IN0
#define CH9 IN1
#define CH10 IN2
#define CH11 IN3
#define CH12 IN4
#define CH13 IN5
#define CH14 IN6
#define CH15 IN7
//ADC2
#define CH16 IN15
#define CH17 IN14
#define CH18 IN13
#define CH19 IN12
#define CH20 IN11
#define CH21 IN10
#define CH22 IN9
#define CH23 IN8
#define CH24 IN7
#define CH25 IN6
#define CH26 IN5
#define CH27 IN4
#define CH28 IN3
#define CH29 IN2
#define CH30 IN1
#define CH31 IN0


#define IN10_IN11 0xA5;  //Differential measurement IN10-IN11
#define TEMP 0xC0; //set init 2 to TEMP to measure internal temperature sensor of the LTC2495.



int main(){

////////////////////////Settings//////////////////////////////
unsigned char init1; ///Input Channel selection
unsigned char init2;  //Sets gain and other settings.  0x87 gain 256, 0x86 gain 128, 0x85 gain 64, 0x80 no gain

double gain=256;//accounts for gain in voltage conversion
double vref=2.5;//reference voltage used


/////////////////////I2C_Variables//////////////////////////
unsigned char x; //stores SIG bit followed by most significant 7 bits
unsigned char y; //stores next 8 bits;
unsigned char z; //first 2 bits of z is last 2 bits of conversion
int data; //stores conversion value, will be 65536 at Input=vref/2, 0 at Input=COM;
unsigned char buff[3]; //buffer that stores bytes to be written or read;
int fd;  //file descriptor

///////////////////NTC Measurement/////////////////////////
double NTC_voltage=0;
double NTC_resistance=0;
double rt=49900; //resistance of divider resistor for NTC measurement
 
  while(1)
  {


    // temperatures for 8 channels
    double NTC_temp[8];

    int i;
    for(i = 0; i < 8; i++){

      fd=wiringPiI2CSetup(ADC_ADD1);
      if(fd==-1)
	return 1;


      if(i==0) init1=CH0;
      if(i==1) init1=CH1;
      if(i==2) init1=CH2;
      if(i==3) init1=CH3;
      if(i==4) init1=CH4;
      if(i==5) init1=CH5;
      if(i==6) init1=CH6;
      if(i==7) init1=CH7;

      init2=0x80; // Use same gain all the time...
    
      delay(250);
      
      buff[0] = init1;
      buff[1] = init2;
      write(fd, buff, 2); //send init1 and init 2 to adc LTC2495
      
      delay(250);   //need to wait some time before first conversion
      
      read(fd, buff, 3); //read 3 bytes from LTC2495
      x=buff[0];
      y=buff[1];
      z=buff[2];
      
      close(fd);
      
      data=((x<<10)|(y<<2)|(z>>6)); //data=7 bits of X, followed by 8 bits of y and then first 2 bits of z
      data&=0xFFFF;
      
      //Conversion to negative if 1st bit of x is 0
      if(x>>7==0)
	{
	  data|=0xFFFF0000;
	}
      
      
    
      gain=1;
      NTC_voltage=data*(vref/(2*gain))/65536;
      NTC_resistance=(rt*NTC_voltage)/(vref-NTC_voltage);//Resistance calculated from voltage divider
      NTC_temp[i]=-21.67*log(NTC_resistance)+224.62; //Equation derived from PS103J2 NTC reference table
      
      printf("NTC temp (ch=%i) = %.4fC \n",i,NTC_temp[i]);

      delay(125);
    }

    // Open output file, appending current temperatures

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char filename[100];
    sprintf(filename,"/home/franciumpi/temperature_data/temp_%d_%d_%d.txt",
	    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    FILE *f = fopen(filename, "a");
    if (f == NULL)
      {
	printf("Error opening file!\n");
	exit(1);
      }


    fprintf(f, "%d-%d-%d %d:%d:%d %lu %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f\n",
	    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)t,
	    NTC_temp[0], NTC_temp[1], NTC_temp[2], NTC_temp[3], 
	    NTC_temp[4], NTC_temp[5], NTC_temp[6], NTC_temp[7]);

    fclose(f);

    printf("\n\n");



  }
}




