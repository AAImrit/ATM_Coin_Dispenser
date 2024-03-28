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
	// LCD Main Page

	// LCD Insert Card
	//get RFID

	// LCD Input Withdrawal Amount
	//getUserInput - from Number Pad, include a timeout
	std::string userInput = "10.10";

	// LCD Display - Calculating...
	// convert the input into an integer
	int userVal = convert_user_input(userInput);

	// check if the user input is valid (under ATM inventory amount)
	bool isInputValid = validateUserInput(userVal, dispensers);

	// if the user input is valid
	if (isInputValid) {

		// initiate variable to hold number of coins required for each type
		int coinDispense[DISPENSER_SIZE] = {0,0,0,0,0};

		// getting number of coins required for each type
		bool isDispense = coin_to_dispense(userVal, dispensers, coinDispense);

		// if the ATM can return the amount
		if (isDispense) {
			// LCD Display - Dispensing... specify the amount dispensed for each type
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
	// for converting user input from a string to a rounded int that can be used for everything else

	// convert string into a float (decimal)
	float floatUserInput = std::stof(userInput);

	// multiply 100 to remove decimal and convert to int
	int conUserInput = static_cast<int>(floatUserInput*100);

	// get the last digit from the value, from the remainder of modulus operation
	int lastVal = conUserInput%10;

	// setting it up so that we always round up to the nearest 5 cents no matter what
	// round up because want to give the user more than they need, not less
	if (lastVal == 0)
	{
		// no rounding required
		return conUserInput;
	}

	if (lastVal <=5)
	{
		// round to 5
		conUserInput = conUserInput + (5-lastVal);
	}

	else
	{
		// round to 10
		conUserInput = conUserInput + (10-lastVal);
	}

	return conUserInput;
}

bool validateUserInput (int userVal, CoinDispenser *dispensers) {
	// check if user input is greater than ATM limit - $10.00 = 1000
	if (userVal > 1000) {
		return false;
	}

	// checks if user input is greater than the ATM inventory
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
	 * Arguments:
	 * dispensers -> the array that holds all coin dispenser objects
	 *
	 * Return:
	 * sum -> total amount of money in all coinDispensers
	 */

	// initialize sum variable
	int sum = 0;

	// iterate through the dispenser array and calculate the total amount of inventory
	for (int i = 0; i < DISPENSER_SIZE; i++) {
		// This will always be less than the bit width
		sum += dispensers[i].get_money_left();
	}

	return sum;
}

bool coin_to_dispense (int userValue, CoinDispenser *dispensers, int *coinDispense) {
	/* calculates how much of each coin to dispense
	 *
	 * Arguments:
	 * userValue -> user value that has been rounded and converted to int, multiplied by 100
	 * dispensers -> the array that holds all coin dispenser objects
	 * coinDispense -> array that contains how many of each coin we will dispense
	 *
	 * Return:
	 * True/False -> success, whether our dispenser can break down
	 */

	// iterate through the 5 different coin dispensers while userValue > 0
	for (int i = 0; i < DISPENSER_SIZE && userValue > 0; i++) {
		// get the number of coins requires
		int coin = userValue/(dispensers[i].coinValue);

		// determine how many coins are in ATM inventory
		int coinLeft = dispensers[i].get_coin_left();

		// compare number of coins required with inventory
		if (coinLeft < coin){
			// if there are not enough coins in inventory, set "coin" to whatever is left
			coin = coinLeft;
		}

		// activate coin dispenser to dispense the number of coins
		coinDispense[i] = coin;

		// deduct value of coins dispensed from the userValue
		userValue = userValue - coin*dispensers[i].coinValue;
	}

	// if by the end, ATM did not have the coins to dispense the user amount
	if (userValue > 0) {
		return false; //the coins we have cannot dispense what they want
	}

	// else - returned what they wanted
	return true; //we can dispense the amount requested
}

// FOR TESTING MOTORS
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
