/*
 * coindispenser.hpp
 * purpose: an object for each coin dispensing tube. This contains any repeated function/code we might
 * need to use for the coin dispensing. This files define all variable and methods the CoinDispenser object will contain
 *  Created on: Feb 18, 2024
 *      Author: Amreen Imrit
 */

#ifndef INC_COINDISPENSER_HPP_
#define INC_COINDISPENSER_HPP_

#include "stm32f4xx_hal.h"
#include "main.hpp"

typedef struct {
	int minAngle; //minAngle of the servo
	int maxAngle; //maxAngle of the servo
	int minTick; //the ticks maps for the minAngle
	int maxTick; //the ticks maps for the maxAngle

	TIM_HandleTypeDef* htim; //the timer the servo uses
	uint32_t timChannel; //the channel it is in
	uint32_t ccrRegister; //1-4 -> should match the channel number
} servo;

class CoinDispenser {
public:
	int coinCapacity; //total amount of coins that can be placed in tube
	int coinValue; //value of the coin this dispenser store. e.g toonies = 200
	servo cdServo; // the servo associated with this

private:
	int coinLeft; //amount of coin left in the tube //should this be an unsinged int,
	//do I have to check that coinLeft does not go below 0??

public:
	//constructor
	CoinDispenser(int cap, const servo& servoObj);

	//functions to do stuff
	void start_PMW ();
	void servo_write(int angle);
	void push_coin (int amount = 1);
	int get_coin_left();
	int get_money_left();
	//might need a function to validate amount of coins left, if 0 display error or something

private:
	int map (servo* cdServo, int angle);

};


#endif /* INC_COINDISPENSER_HPP_ */
