/*
 * coindispenser.cpp
 * purpose: an object for each coin dispensing tube. This contains any repeated function/code we might
 * need to use for the coin dispensing. This file is the main body of the methods defined in the .hpp file
 *  Created on: Feb 18, 2024
 *      Author: Amreen Imrit
 */

#include "coindispenser.hpp"

CoinDispenser::CoinDispenser(int cap, const servo& servoObj) : capacity(cap), cdServo(servoObj) {
	/*
	 * the constructor of the object. This is called the first time to initialize the object
	 * Note: use this style when defining a coin dispenser in the code
	 * coin_dispenser dispenser2 (5, {0, 180, 50, 250, &htim2, TIM_CHANNEL_1, TIM_CHANNEL_1});
	 *
	 * Args:
	 * Cap ->  the amount of coins the tube can hold
	 * servoObj -> the servo parameters to initialize the servo the coin dispenser tube uses
	 *
	 * Returns:
	 * none
	 */

	servo_write(180);

}

void CoinDispenser::start_PMW(){
	//starts the PWM for the servo's pin/channel
	HAL_TIM_PWM_Start(this->cdServo.htim, this->cdServo.timChannel);
}

int CoinDispenser::map (servo *servoObj, int angle) {
	/*
	 * converts an angle to ticks the servo need to turn
	 *
	 * Args:
	 * servoObj -> the servo we are working with
	 * angle -> the angle desired
	 *
	 * Returns:
	 * the amount of tick the servo has to do to reach the desired angle
	 */

	return ( (1.0*(angle - servoObj->minAngle)) / (1.0*(servoObj->maxAngle - servoObj->minAngle)) * (servoObj->maxTick - servoObj->minTick) + servoObj->minTick );
}

void CoinDispenser::servo_write(int angle) {
	/*
	 * write the desired angle to the servo causing it to turn
	 *
	 * Args:
	 * angle -> the desired angle
	 *
	 * Return:
	 * None
	 */

    // Calculate the address of the desired CCR register (i.e the channel we want to send the pulse to)
    volatile uint32_t* ccr = &(this->cdServo.htim->Instance->CCR1) + (this->cdServo.ccrRegister - 1);

    // Update the value in the desired CCR register and turning servo
    *ccr = map(&(this->cdServo), angle);
}


void CoinDispenser::push_coin() {
	//push coin then come back to neutral position
	servo_write(100);
	HAL_Delay(200);
	servo_write(180);
}
