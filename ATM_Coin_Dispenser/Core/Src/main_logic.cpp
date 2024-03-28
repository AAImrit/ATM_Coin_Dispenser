/*
 * main_logic.cpp
 * purpose: for the main logic of the program so it doesn't get overwritten?
 */


#include "stm32f4xx_hal.h"
//#include "main.hpp"
#include "coindispenser.hpp"
#include "main_logic.hpp"
#include <string>
#include <iostream>

const int DISPENSER_SIZE = 5;

void main_logic(CoinDispenser *dispensers) {
//main logic and stuff to do in while loop goes here

	//get RFID
	//getUserInput
	std::string userInput = "02.50";
	int userVal = convert_user_input(userInput);

	bool isInputValid = validateUserInput(userVal, dispensers);
	if (isInputValid) {
		int coinDispense[DISPENSER_SIZE] = {0,0,0,0,0};
		bool isDispense = coin_to_dispense(userVal, dispensers, coinDispense);


		if (isDispense) {
			//print something on screen saying it will dispense
			for (int i=0; i<DISPENSER_SIZE; i++) {
				if (coinDispense[i] > 0) {
					dispensers[i].push_coin(coinDispense[i]);
				}
			}
			//print something about finished dispensing
		} else {
			//print something about can't dispense and go back to main menu
		}

	} else {
		//print something about input not valid
	}

}

int convert_user_input (std::string userInput) {
	//for converting user input from a string to a rounded int that can be used for everything else
	float floatUserInput = std::stof(userInput);
	int conUserInput = static_cast<int>(floatUserInput*100);

	int lastVal = conUserInput%10;
	//setting it up so that we always round up no matter what
	if (lastVal == 0) {
		return conUserInput;
	}
	if (lastVal <=5) {
		conUserInput = conUserInput + (5-lastVal);
	} else {
		conUserInput = conUserInput + (10-lastVal);
	}

	return conUserInput;

}

bool validateUserInput (int userVal, CoinDispenser *dispensers) {
	if (userVal > 1000) {
		return false;
	}

	int inventorySum = inventory_money(dispensers);
	if (userVal > inventorySum) {
		return false;
	}

	return true;
}

//user_function ----------------------------------------------------------
int inventory_money (CoinDispenser *dispensers) {
	/* calculates total amount of money in the coinDispenser
	 *
	 * Args:
	 * dispensers -> the array that hold all coin dispenser objects
	 *
	 * Return:
	 * sum -> total amount of money in all coinDispensers
	 */
	int sum = 0;
	for (int i = 0; i < DISPENSER_SIZE; i++) {
		//Note, do I need to validate that the sum is < bit width?
		sum += dispensers[i].get_money_left();
	}

	return sum;
}

bool coin_to_dispense (int userValue, CoinDispenser *dispensers, int *coinDispense) {
	/* calculates how much of each thing to dispense
	 *
	 * Args:
	 * userValue -> user value that has been rounded and converted to int, multiplied by 100
	 * dispensers -> the array that hold all coin dispenser objects
	 * coinDispene -> array that contains how many of each coin we will dispense
	 *
	 * Return:
	 * True/False -> success, whether our dispenser can break down
	 */

	for (int i = 0; i < DISPENSER_SIZE && userValue > 0; i++) {
		int coin = userValue/(dispensers[i].coinValue);
		int coinLeft = dispensers[i].get_coin_left();
		if (coinLeft < coin){
			coin = coinLeft;
		}

		coinDispense[i] = coin;
		userValue = userValue - coin*dispensers[i].coinValue;
	}

	if (userValue > 0) {
		return false; //the coins we have cannot dispense what they want
	}
	return true; //we can dispense the amount requested
}

void servo_sweep (CoinDispenser* cd) {
	int angles[5] = {25, 70, 90, 120, 180};

	for (int i = 0; i < 5; i++) {
		cd->servo_write(angles[i]);
		HAL_Delay(200);
		//servo_write(0);
		HAL_Delay(200);
	}
	HAL_Delay(600);

}
