/*
 * NumberPad.cpp
 * Purpose: an object for the number pad This contains any repeated function/code we might need to use.
 * This file is the main body of the methods defined in the .hpp file
 *  Created on: Mar 14, 2024
 *      Author: Khalesah Alli
 */

//#include <main.hpp>
#include <numberpad.hpp>
#include <string.h>

NumberPad::NumberPad() {
	// TODO Auto-generated constructor stub

}


// NUMBER PAD FUNCTIONS

// get the key that is currently being pressed, will be called within numberToDisplay()
char NumberPad::getKey(){

	// Column 1
	HAL_GPIO_WritePin(Col1_GPIO_Port, Col1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Col2_GPIO_Port, Col2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Col3_GPIO_Port, Col3_Pin, GPIO_PIN_RESET);

	if (HAL_GPIO_ReadPin(Row1_GPIO_Port, Row1_Pin) == 1) {
		return '1';
	}

	if (HAL_GPIO_ReadPin(Row2_GPIO_Port, Row2_Pin) == 1) {
		return '4';
	}

	if (HAL_GPIO_ReadPin(Row3_GPIO_Port, Row3_Pin) == 1) {
		return '7';
	}

	if (HAL_GPIO_ReadPin(Row4_GPIO_Port, Row4_Pin) == 1) {
		return '*';
		//return 42;	// represents * key
	}

	// Column 2
	HAL_GPIO_WritePin(Col1_GPIO_Port, Col1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Col2_GPIO_Port, Col2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(Col3_GPIO_Port, Col3_Pin, GPIO_PIN_RESET);

	if (HAL_GPIO_ReadPin(Row1_GPIO_Port, Row1_Pin) == 1) {
		return '2';
	}

	if (HAL_GPIO_ReadPin(Row2_GPIO_Port, Row2_Pin) == 1) {
		return '5';
	}

	if (HAL_GPIO_ReadPin(Row3_GPIO_Port, Row3_Pin) == 1) {
		return '8';
	}

	if (HAL_GPIO_ReadPin(Row4_GPIO_Port, Row4_Pin) == 1) {
		return '0';
	}

	// Column 3
	HAL_GPIO_WritePin(Col1_GPIO_Port, Col1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Col2_GPIO_Port, Col2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Col3_GPIO_Port, Col3_Pin, GPIO_PIN_SET);

	if (HAL_GPIO_ReadPin(Row1_GPIO_Port, Row1_Pin) == 1) {
		return '3';
	}

	if (HAL_GPIO_ReadPin(Row2_GPIO_Port, Row2_Pin) == 1) {
		return '6';
	}

	if (HAL_GPIO_ReadPin(Row3_GPIO_Port, Row3_Pin) == 1) {
		return '9';
	}

	if (HAL_GPIO_ReadPin(Row4_GPIO_Port, Row4_Pin) == 1) {
		return '#';
		//return 35;	// represents # key
	}

	return 'N';		// corresponds to NO key being pressed
}

// collect user input and continuously display on LCD
// need to return number value for code lol or can convert later on
// HAVING PROBLEM WITH STRING TYPES!!
std::string NumberPad::numberToDisplay(){

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
	int count = 0;			// keep track of how many valid numbers pressed, to ensure don't exceed (hit max before stop updating)

	char keyPressed;
	char displayValue[5];

	while (true) {	// add timeout to conditional
		keyPressed = getKey();

		if ((keyPressed != '*') and (keyPressed != '#')){
			// increase count for valid input
			count ++;

			// update display
			if (count < 4){
				// move characters to left
				memmove(value, value+1, strlen(value));

				// add new character to the end
				value[3] = keyPressed;

				// create copy of string and add decimal in between
				strcpy(displayValue, value);
				memmove(displayValue+2+1, displayValue+2, strlen(displayValue)-2+1);
				displayValue[2] = '.';

				// send string to LCD to update display
				//MICHELLE
			}

			// else do nothing, unless we want to put a warning message :( but im assuming that the user will get it
		}

		else if (keyPressed == '#'){
			// check to make sure non zero
			std::string str(value);
			return str;
		}

		else{
			//maybe backspace with *
		}
	}
}


// TIMING MIGHT BE A PROBLEM WITH THESE FUNCTIONS, MAYBE JUST USE THE getKey() FUNCTION AND DO IF STATEMENT COMPARISON

// checks if confirm button was pressed, assuming that # key used as confirm
bool NumberPad::isConfirm(){
	char keyPressed = getKey();

	if (keyPressed == '#') {
			return true;
			//return 35;	// represents # key
	}

	else{
		return false;
	}
}

// might want to write an ISR for this so user can cancel at any time
// checks if cancel button was pressed, assuming that * key used as cancel
bool NumberPad::isCancel(){
	char keyPressed = getKey();

	if (keyPressed == '*') {
		return true;
		//return 42;	// represents * key
	}

	else{
		return false;
	}
}

