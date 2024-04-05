/*
 * LCD.cpp
 *
 *  Created on: Mar 20, 2024
 *      Author: khale
 *
 *      SOURCE: https://blog.embeddedexpert.io/?p=1764
 */

#include "LCD.hpp"
#include "stdint.h"
#include "stm32f4xx.h"
#define SLAVE_ADDRESS_LCD (0x27) // change this according to ur setup

// Initializing variables
char lcdData[30];


LCD::LCD() {
	// TODO Auto-generated constructor stub

}

/*
LCD::~LCD() {
	// TODO Auto-generated destructor stub
}
*/

void LCD::lcd_write_message(bool clear, char* errorMessage, char* message[4])
{
	// clear LCD screen
	if (clear == true){
		lcd_clear();
	}

	// iterate through the message array
	for (int i = 0; i < 4; i++){

		// first Line
		if (i == 0){
			// concatenate the error message with the first line
			char line[30];
			strcpy(line, errorMessage);
			strcat(line, message[i]);

			// calculate the offset value for centering
			int colCursor = (20 - strlen(line))/2;

			// write to LCD
			setCursor(colCursor, i);
			sprintf(lcdData, line);
			lcd_send_string(lcdData);
		}

		// all Other Lines
		else{
			// get the line
			char* line = message[i];

			// calculate the offset value for centering
			int colCursor = (20 - strlen(line))/2;

			// write to LCD
			setCursor(colCursor, i);
			sprintf(lcdData, line);
			lcd_send_string(lcdData);
		}
	}
}

void LCD::lcd_write_i2c(char saddr,uint8_t *buffer, uint8_t length)
{
	while (I2C1->SR2 & I2C_SR2_BUSY);           //wait until bus not busy

	I2C1->CR1 |= I2C_CR1_START;                 //generate start

	while (!(I2C1->SR1 & I2C_SR1_SB)){;}		//wait until start is generated

	I2C1->DR = saddr<< 1;                 	 	// Send slave address
	while (!(I2C1->SR1 & I2C_SR1_ADDR)){;}      //wait until address flag is set

	(void)I2C1->SR2; 							//Clear SR2
	//sending the data
	for (uint8_t i=0;i<length;i++)
	 {
		I2C1->DR=buffer[i]; 					//filling buffer with command or data
		while (!(I2C1->SR1 & I2C_SR1_BTF));
	 }

	I2C1->CR1 |= I2C_CR1_STOP;					//wait until transfer finished
}

void LCD::lcd_send_cmd(char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	lcd_write_i2c(SLAVE_ADDRESS_LCD,(uint8_t *)data_t,4);
}

void LCD::lcd_send_data(char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=1
	data_t[1] = data_u|0x09;  //en=0, rs=1
	data_t[2] = data_l|0x0D;  //en=1, rs=1
	data_t[3] = data_l|0x09;  //en=0, rs=1
	lcd_write_i2c(SLAVE_ADDRESS_LCD,(uint8_t *)data_t,4);
}

void LCD::setCursor(int a, int b)
{
	// a represents distance from LHS
	// b represents the LCD row

	int i=0;
	switch(b){
	case 0:lcd_send_cmd(0x80);break;
	case 1:lcd_send_cmd(0xC0);break;
	case 2:lcd_send_cmd(0x94);break;
	case 3:lcd_send_cmd(0xd4);break;}
	for(i=0;i<a;i++)
	lcd_send_cmd(0x14);
}

void LCD::lcd_send_string(char *str)
{
	while (*str) lcd_send_data (*str++);
}

void LCD::lcd_init(void)
{
//	i2c_init();
	// 4 bit initialisation
	HAL_Delay(50);  // wait for >40ms
	lcd_send_cmd (0x3);
	HAL_Delay(5);  // wait for >4.1ms
	lcd_send_cmd (0x3);
	HAL_Delay(1);  // wait for >100us
	lcd_send_cmd (0x3);
	HAL_Delay(10);
	lcd_send_cmd (0x2);  // 4bit mode
	HAL_Delay(10);

  // display initialization
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void LCD::lcd_clear(void)
{
	#define LCD_CLEARDISPLAY 0x01
	lcd_send_cmd(LCD_CLEARDISPLAY);
	HAL_Delay(100);
}
