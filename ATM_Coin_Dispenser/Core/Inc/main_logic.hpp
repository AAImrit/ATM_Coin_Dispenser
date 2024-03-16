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

//extern const int DISPENSER_SIZE; //constant value cuz we have 5 coin dispensers

//Declare functions:
void main_logic(CoinDispenser *dispensers, int DISPENSER_SIZE);
int inventory_money (CoinDispenser *dispensers);
bool coin_to_dispense (int userValue, CoinDispenser *dispensers, int *coinDispense);
void servo_sweep (CoinDispenser* cd);

#endif /* INC_MAIN_LOGIC_HPP_ */
