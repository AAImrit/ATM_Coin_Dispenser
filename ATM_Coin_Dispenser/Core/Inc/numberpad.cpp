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
#include <stdlib.h>



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
int numberpad::numberToDisplay(LCD lcd)
{
	// Initiate variables
	char value[] = "0000";	// add decimal before sending to display
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
			// move characters to left
			memmove(value, value+1, strlen(value));

			// add new character to the end
			value[3] = keyPressed;

			// create copy of string and add decimal in between
			strcpy(displayValue, value);
			memmove(displayValue+2+1, displayValue+2, strlen(displayValue)-2+1);
			displayValue[2] = '.';

			// LCD
			char line[20];
			strcpy(line, "$");
			strcat(line, displayValue);

			char* errorMessage = "";
			char* LCDAmount[4] = {"Enter Amount", line, "Press # to Continue", "Press * to Cancel"};
			lcd.lcd_write_message(false, errorMessage, LCDAmount);
		}

		else if (keyPressed == '#'){
			// convert string into a float (decimal)
			float floatVal = atof(displayValue);

			// multiply by 100 and then convert to int
			int userInput = static_cast<int>(floatVal*100);
			return userInput;
		}

		else if (keyPressed == '*'){
			return 100000;
		}
	}
}

// checks if confirm button was pressed
bool numberpad::isConfirm()
{
	// get key value
	char keyPressed = keypad_read();

	// if confirm button pressed, return true
	if (keyPressed == '#'){
		return true;
	}

	// else, return false
	return false;
}

int numberpad::getPin(LCD lcd, char* errorMessage)
{
	// want to display * in place of the pin but return the actual pin value as const char*

	// Initiate variables
	char pin[] = "    ";
	char hiddenPin[] = "    ";
	char keyPressed;
	int count = 0;

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
		if ((count < 4) and (keyPressed != '*') and (keyPressed != '#') ){
			// increase count
			count += 1;

			// move characters to left
			memmove(pin, pin+1, strlen(pin));
			memmove(hiddenPin, hiddenPin+1, strlen(hiddenPin));

			// add new character to the end
			pin[3] = keyPressed;
			hiddenPin[3] = '*';


			// update LCD
			char* LCDPin[4] = {"Enter Pin", hiddenPin, "Press # to Continue", "Press * to Cancel"};
			lcd.lcd_write_message(false, errorMessage, LCDPin);
		}

		else if (keyPressed == '#'){
			// convert string into a float (decimal)
			float pinFloat = atof(pin);

			// convert to int
			int pinInteger = static_cast<int>(pinFloat);
			return pinInteger;
		}

		else if (keyPressed == '*'){
			return 0;
		}
	}

}

