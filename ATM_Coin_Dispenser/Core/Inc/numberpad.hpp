/*
 * numberpad.hpp
 *
 *  Created on: Mar 20, 2024
 *      Author: khale
 */

#ifndef INC_NUMBERPAD_HPP_
#define INC_NUMBERPAD_HPP_

#include "stm32f4xx_hal.h"
#include <main.hpp>

class numberpad {
public:
	numberpad();
	//virtual ~numberpad();

	// FUNCTIONS TO GET KEYPAD VALUES
	void keypad_init(void);
	char keypad_read(void);
	static char decode_keypad(uint8_t col, uint8_t row);

	// USER FUNCTIONS




};

#endif /* INC_NUMBERPAD_HPP_ */
