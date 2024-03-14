/*
 * NumberPad.hpp
 * Purpose: an object for the number pad This contains any repeated function/code we might need to use.
 * This file defines all variable and methods the NumberPad object will contain
 *  Created on: Mar 14, 2024
 *      Author: Khalesah Alli
 */

#ifndef INC_NUMBERPAD_HPP_
#define INC_NUMBERPAD_HPP_

#include <main.hpp>
#include "stm32f4xx_hal.h"
#include <string>

class NumberPad {
public:
	// constructor, don't think I need any object properties
	NumberPad();

	// unsure if this is required, automatically generated
	//virtual ~NumberPad();

	// NUMBER PAD FUNCTIONS

	// get the key that is currently being pressed, will be called within numberToDisplay()
	char getKey();

	// collect user input and continuously display on LCD
	// const char* numberToDisplay();
	std::string numberToDisplay();

	// checks if confirm button was pressed
	bool isConfirm();

	// checks if cancel button was pressed
	bool isCancel();
};

#endif /* INC_NUMBERPAD_HPP_ */
