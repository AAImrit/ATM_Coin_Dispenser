/*
 * coindispenser.h
 *
 *  Created on: Feb 18, 2024
 *      Author: amimr
 */

#ifndef INC_COINDISPENSER_HPP_
#define INC_COINDISPENSER_HPP_

#include "stm32f4xx_hal.h"
#include "main.hpp"

typedef struct {
	int minAngle;
	int maxAngle;
	int minTick;
	int maxTick;

	TIM_HandleTypeDef* htim;
	uint32_t timChannel; //the channel it is in
	uint32_t ccrRegister; //1-4
} servo;

class CoinDispenser {
public:
	int capacity; //amount of stuff inside the coin dispenser
	servo cdServo; // the servo associated with this

	//constructor
	CoinDispenser(int cap, const servo& servoObj);

	//functions to do stuff
	//void start_PMW (TIM_HandleTypeDef* htim, uint32_t timChannel);
	void start_PMW ();
	//void servo_write(int angle, TIM_HandleTypeDef* htim, uint32_t ccrRegister);
	void servo_write(int angle);
	void push_coin ();

private:
	int map (servo* cdServo, int angle);

};


#endif /* INC_COINDISPENSER_HPP_ */
