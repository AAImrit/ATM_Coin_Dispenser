/*
 * numberpad.cpp
 *
 *  Created on: Mar 20, 2024
 *      Author: khale
 */

#include "numberpad.hpp"
#include "stm32f4xx_hal.h"
#include "LCD.hpp"
#include <string.h>


//#define ROW_COUNT 4
//#define COL_COUNT 3



numberpad::numberpad() {
	// TODO Auto-generated constructor stub

}

/*
numberpad::~numberpad() {
	// TODO Auto-generated destructor stub
}
*/


static char decode_keypad(uint8_t col, uint8_t row);


const unsigned char keymap[4][3] =
{
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};



const uint32_t clo_state[4]={	(GPIO_BSRR_BR4|GPIO_BSRR_BS5|GPIO_BSRR_BS6),
						(GPIO_BSRR_BS4|GPIO_BSRR_BR5|GPIO_BSRR_BS6),
						(GPIO_BSRR_BS4|GPIO_BSRR_BS5|GPIO_BSRR_BR6),
						(GPIO_BSRR_BS4|GPIO_BSRR_BS5|GPIO_BSRR_BS6)
};


void numberpad::keypad_init(void)
{

	/*Enable clock access to GPIOC*/
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;

	/*Set PC0 to PC3 as input*/
	GPIOC->MODER &= ~(GPIO_MODER_MODE0|GPIO_MODER_MODE1|GPIO_MODER_MODE2|GPIO_MODER_MODE3);

	/* Activate internal pullup resistor for PC0 to PC3*/
	GPIOC->PUPDR|=GPIO_PUPDR_PUPD0_0|GPIO_PUPDR_PUPD1_0|GPIO_PUPDR_PUPD2_0|GPIO_PUPDR_PUPD3_0;

	/*Set PC4 to PC7 as output*/
	GPIOC->MODER |= GPIO_MODER_MODE4_0|GPIO_MODER_MODE5_0|GPIO_MODER_MODE6_0|GPIO_MODER_MODE7_0;

	GPIOC->MODER &=~( GPIO_MODER_MODE4_1|GPIO_MODER_MODE5_1|GPIO_MODER_MODE6_1|GPIO_MODER_MODE7_1);

	/*Set PC4 to PC7 as high*/

	GPIOC->BSRR = GPIO_BSRR_BS4|GPIO_BSRR_BS5|GPIO_BSRR_BS6|GPIO_BSRR_BS7;
}


char numberpad::keypad_read(void)
{
	//ORIGINAL CODE
	/*
	unsigned char key=0,data=0;


	for (int i=0;i<4;i++)
	{

		GPIOC->BSRR=clo_state[i];

		data=(GPIOC->IDR)&0xF; //Get rid of data from bit 5 to bit31

		if(data != 0xF)
		{
			// check that key is released

			key=decode_keypad(i,data);
		}

	}
	*/

	// MODIFIED VERSION TO ONLY GET KEY AFTER RELEASED
	unsigned char key=0,data=0,temp=0;

	for (int i=0;i<4;i++)
	{

		GPIOC->BSRR=clo_state[i];

		data=(GPIOC->IDR)&0xF; //Get rid of data from bit 5 to bit31

		// key press detected
		if(data != 0xF)
		{
			// check that key is released
			do{
				GPIOC->BSRR=clo_state[i];

				temp=(GPIOC->IDR)&0xF; //Get rid of data from bit 5 to bit31
			}
			while(data == temp);

			key=decode_keypad(i,data);
		}
	}

	return key;
}


char numberpad::decode_keypad(uint8_t col, uint8_t row)
{

	if (row == 0xE) return keymap[0][col];
	if (row == 0xD) return keymap[1][col];
	if (row == 0xB) return keymap[2][col];
	if (row == 0x7) return keymap[3][col];
	return 0;
}


// collect user input and continuously display on LCD
const char* numberpad::numberToDisplay(LCD lcd)
{
	// LOGIC
	// define a default string "00.00"

	// initiate keyPressed variable

	// while (timeout not met)
		// get key pressed
		// update string if number
		// update LCD display

		// if confirmed pressed
			// do we need to check for a nonzero value and redirect user to while loop if zero
			// return string to be passed onto LCD display

	//else if timeout occurred
		// MAYBE display a warning
		// cancel transaction, and go back to main menu
		// return string which indicates to go back to main menu OR change return type

	char value[] = "0000";	// add decimal before sending to display
	//int count = 0;			// keep track of how many valid numbers pressed, to ensure don't exceed (hit max before stop updating)
	char lcd_data[30];

	char keyPressed;
	char displayValue[5];

	while (true) {	// change to timeout for conditional
		// get initial value
		keyPressed = keypad_read();
		HAL_Delay(100);

		// wait until get a nonzero keyPressed value
		while (keyPressed == 0){
			keyPressed = keypad_read();
			HAL_Delay(100);
		}

		//
		if ((keyPressed != '*') and (keyPressed != '#') ){

			// increase count for valid input
			// count ++;

			// update display
			//if (count < 5){

				// move characters to left
				memmove(value, value+1, strlen(value));

				// add new character to the end
				value[3] = keyPressed;

				// create copy of string and add decimal in between
				strcpy(displayValue, value);
				memmove(displayValue+2+1, displayValue+2, strlen(displayValue)-2+1);
				displayValue[2] = '.';


				// send string to LCD to update display
				lcd.setCursor(0, 0);
				sprintf(lcd_data, displayValue);
				lcd.lcd_send_string(lcd_data);

//			}


			// else do nothing, unless we want to put a warning message :( but im assuming that the user will get it
		}

		else if (keyPressed == '#'){
			// check to make sure non zero
			return value;
		}

		else{
			//maybe backspace with *
		}
	}

}

// checks if confirm button was pressed
bool numberpad::isConfirm()
{

}

// checks if cancel button was pressed
bool numberpad::isCancel()
{

}
