/*
 * coindispenser.cpp
 *
 *  Created on: Feb 18, 2024
 *      Author: amimr
 */

#include "coindispenser.hpp"

CoinDispenser::CoinDispenser(int cap, const servo& servoObj) : capacity(cap), cdServo(servoObj) {
    // No need for additional initialization here
	servo_write(180);

	//use this style when defining a coin dispenser in the code
	//coin_dispenser dispenser(5, {0, 180, 50, 250, &htim2, TIM_CHANNEL_1, TIM_CHANNEL_1});
}

void CoinDispenser::start_PMW(){
	HAL_TIM_PWM_Start(this->cdServo.htim, this->cdServo.timChannel);
}

int CoinDispenser::map (servo *servoObj, int angle) {
	return ( (1.0*(angle - servoObj->minAngle)) / (1.0*(servoObj->maxAngle - servoObj->minAngle)) * (servoObj->maxTick - servoObj->minTick) + servoObj->minTick );
}

void CoinDispenser::servo_write(int angle) {
    // Calculate the address of the desired CCR register
    volatile uint32_t* ccr = &(this->cdServo.htim->Instance->CCR1) + (this->cdServo.ccrRegister - 1);

    // Update the value in the desired CCR register and turning servo
    *ccr = map(&(this->cdServo), angle);
}


void CoinDispenser::push_coin() {

	//push then come back to neutral position
	servo_write(100);
	HAL_Delay(200);
	servo_write(180);
}
