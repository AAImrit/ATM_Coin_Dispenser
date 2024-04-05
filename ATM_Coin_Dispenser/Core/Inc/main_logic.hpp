/*
 * mainLogic.hpp
 *
 *  Created on: Mar 6, 2024
 *      Author: amimr
 */


#ifndef INC_MAIN_LOGIC_HPP_
#define INC_MAIN_LOGIC_HPP_

#include "stm32f4xx_hal.h"
#include "main.h"
#include "coindispenser.hpp"
#include "numberpad.hpp"
#include "LCD.hpp"
#include <string>
#include <iostream>

//extern const int DISPENSER_SIZE; //constant value cuz we have 5 coin dispensers

//Declare functions:
int stateMachine(int state, CoinDispenser *dispensers, numberpad numPad, LCD lcd);
int inventoryMoney (CoinDispenser *dispensers);
bool coinToDispense (int userValue, CoinDispenser *dispensers, int *coinDispense);
void servoSweep (CoinDispenser* cd);
int validateUserInput (int userVal, CoinDispenser *dispensers);
int roundUserInput (int userInput);

#endif /* INC_MAIN_LOGIC_HPP_ */
