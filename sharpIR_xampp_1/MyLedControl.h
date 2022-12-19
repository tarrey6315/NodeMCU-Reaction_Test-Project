#include "LedControl.h"

LedControl LEDarray=LedControl(12,13,15,1); 
void display_num(int n){
	byte num1[8] = {                                    
		0b11111110,0b11111100,0b11111000,0b11111100,  // no.1 arrow
		0b11111110,0b11011111,0b10001110,0b00000100};
	byte num2[8] = {                                    
		0b00010000,0b00110000,0b01111111,0b11111111,  // 2
		0b01111111,0b00110000,0b00010000,0b00000000};
	byte num3[8] = {                                   
		0b00000100,0b10001110,0b11011111,0b11111110,  //  3
		0b11111100,0b11111000,0b11111100,0b11111110};
	byte num4[8] = {                                   
		0b00100000,0b01110001,0b11111011,0b01111111,  //  4
		0b00111111,0b00011111,0b00111111,0b01111111};
	byte num5[8] = {                                  
		 0b00001000,0b00001100,0b11111110,0b11111111,  //  5
		 0b11111110,0b00001100,0b00001000,0b00000000};
	byte num6[8] = {                                  
		0b01111111,0b00111111,0b00011111,0b00111111,  //  6
		0b01111111,0b11111011,0b01110001,0b00100000};
	byte num_little[8] = {
		0b00000000,0b00000000,0b00000000,0b00011000,
		0b00011000,0b00000000,0b00000000,0b00000000};
	byte num_some[8] = {
	  0b00000000,0b00000000,0b00111100,0b00111100,
	   0b00111100,0b00111100,0b00000000,0b00000000};
	byte num_all[8] = {
		0b11111111,0b11111111,0b11111111,0b11111111,
		0b11111111,0b11111111,0b11111111,0b11111111};

  switch(n){
	case 1:
	 for(int j=0;j<8;j++) LEDarray.setRow(0,j,num1[j]); break;
	case 2:                     
	 for(int j=0;j<8;j++) LEDarray.setRow(0,j,num2[j]); break;
	case 3:                     
	 for(int j=0;j<8;j++) LEDarray.setRow(0,j,num3[j]); break;
	case 4:                     
	 for(int j=0;j<8;j++) LEDarray.setRow(0,j,num4[j]); break;
	case 5:                     
	 for(int j=0;j<8;j++) LEDarray.setRow(0,j,num5[j]); break;
	case 6:                     
	 for(int j=0;j<8;j++) LEDarray.setRow(0,j,num6[j]); break;
	case 9:                     
	 for(int j=0;j<8;j++) LEDarray.setRow(0,j,num_little[j]); break;
	case 99:                    
	 for(int j=0;j<8;j++) LEDarray.setRow(0,j,num_some[j]); break;
	case 999:                   
	 for(int j=0;j<8;j++) LEDarray.setRow(0,j,num_all[j]); break;
  }
}
