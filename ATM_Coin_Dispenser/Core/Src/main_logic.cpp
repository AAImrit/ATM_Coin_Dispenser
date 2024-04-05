/*
 * main_logic.cpp
 * purpose: for the main logic of the program so it doesn't get overwritten?
 */

// HEADER FILES
#include "stm32f4xx_hal.h"
#include "coindispenser.hpp"
#include "numberpad.hpp"
#include "LCD.hpp"
#include "main_logic.hpp"

// ADDED LIBRARIES
#include <stdlib.h>
#include <iostream>
#include <string.h>

// Initialize variables
const int DISPENSER_SIZE = 5;
char lcd_data[30];
int key;
int globalPreviousState = 0;
std::string error_msg = "";


int stateMachine(int state, CoinDispenser *dispensers, numberpad numPad, LCD lcd) {
	switch (state) {

		// START PAGE
		case 0:
		{
			// LCD
			char* errMessageStart = "";
			char* LCDStartPage[4] = {"", "Welcome", "Press # To Start", ""};
			lcd.lcd_write_message(true, errMessageStart, LCDStartPage);

			// wait for the confirm button to be pressed
			bool buttonPress = numPad.isConfirm();

			// ADD - Timeout
			while (buttonPress != true){
				buttonPress = numPad.isConfirm();
			}

			// set the previous state to the current one
			globalPreviousState = 0;

			// return state 1 (move on to next case)
			return 1;
		}


		// ENTER PIN
		case 1:
		{
			// LCD
			char* errMessagePin = "";

			// display error message if user inputed wrong pin (been through case 1 before)
			if (globalPreviousState == 1){
				errMessagePin = "Invalid: ";
			}

			char* LCDPin[4] = {"Enter Pin", "", "Press # to Continue", "Press * to Cancel"};
			lcd.lcd_write_message(true, errMessagePin, LCDPin);


			// get the user pin
			int pin = numPad.getPin(lcd, errMessagePin);

			// checking if cancel or pin is valid (matches internal password)
			if (pin != 1234){

				// check for cancel
				if (pin == 0){
					// return to start page
					return 0;
				}

				// set the previous state to the current one
				globalPreviousState = 1;

				// return to current state
				return 1;
			}


			// set the previous state to the current one
			globalPreviousState = 1;

			// return state 2 (move on to next case)
			return 2;
		}


		// ENTER AMOUNT & DISPENSING
		case 2:
		{

			// GETTING USER INPUT
			// LCD - Enter Amount
			char* errMessageDispense = "";
			char* LCDAmount[4] = {"Enter Amount", "$00.00", "Press # to Continue", "Press * to Cancel"};
			lcd.lcd_write_message(true, errMessageDispense, LCDAmount);

			// get withdrawal amount and convert to int
			int userInput = numPad.numberToDisplay(lcd);

			// check for cancel
			if (userInput == 100000){
				// return to start page
				return 0;
			}

			// round input
			int userVal = roundUserInput(userInput);

			// check if input is valid
			int isInputValid = validateUserInput(userVal, dispensers);

			if (isInputValid != 1){
				// Error #1 - User Enters Blank Value
				if (isInputValid == 100000){
					char* LCDError1[4] = {"Error: Zero Amount", "", "Enter Value > $00.00", "Press # to Retry"};
					lcd.lcd_write_message(true, errMessageDispense, LCDError1);
				}

				// Error #2 - User Enters Amount Exceeding Inventory, Display the Amount
				else{
					// convert int into a string
					char floatString[100];
					sprintf(floatString, "%d", isInputValid);

					int length = strlen(floatString);

					// adding zeroes to beginning of string to ensure correct decimal placement
					if (length < 4){
						int diff = 4-length;
						for (int i = 0; i < diff; i++){
							memmove(floatString+1, floatString, ++length);
							*floatString = '0';
						}
					}

					// add the decimal
					int position = strlen(floatString) - 2;
					memmove(floatString+position+1, floatString+position, strlen(floatString)-position+1);
					floatString[position] = '.';

					// concatenate the inventory amount with message
					char line[20];
					strcpy(line, "Enter Value < $");
					strcat(line, floatString);

					// send to LCD
					char* LCDError2[4] = {"Error: Low Inventory", "", line, "Press # to Retry"};
					lcd.lcd_write_message(true, errMessageDispense, LCDError2);
				}

				// wait for the confirm button to be pressed
				bool buttonPress = numPad.isConfirm();

				// ADD - Timeout
				while (buttonPress != true){
					buttonPress = numPad.isConfirm();
				}

				// return to current state
				return 2;
			}

			// DISPENSING

			// initiate variable to hold number of coins required for each type
			int coinDispense[DISPENSER_SIZE] = {0,0,0,0,0};

			// get number of coins required for each type
			bool isDispense = coinToDispense(userVal, dispensers, coinDispense);

			char line[20];
			sprintf(line, "%dT %dL %dQ %dD %d5", coinDispense[0], coinDispense[1], coinDispense[2], coinDispense[3], coinDispense[4]);

			if (!isDispense) {
				char* LCDError3[4] = {"Not Enough Coins!", "Current Inventory:", line, "Press # to Retry"};
				lcd.lcd_write_message(true, errMessageDispense, LCDError3);

				// wait for the confirm button to be pressed
				bool buttonPress = numPad.isConfirm();

				// ADD - Timeout
				while (buttonPress != true){
					buttonPress = numPad.isConfirm();
				}

				// return to current state
				return 2;
			}


			// if the ATM can return the amount

			// SHOULD TURN INTO FUNCTION
			// convert int into a string
			char floatString[100];
			sprintf(floatString, "%d", userVal);

			int length = strlen(floatString);

			// adding zeroes to beginning of string to ensure correct decimal placement
			if (length < 4){
				int diff = 4-length;
				for (int i = 0; i < diff; i++){
					memmove(floatString+1, floatString, ++length);
					*floatString = '0';
				}
			}

			// add the decimal
			int position = strlen(floatString) - 2;
			memmove(floatString+position+1, floatString+position, strlen(floatString)-position+1);
			floatString[position] = '.';

			// concatenate the inventory amount with message
			char dispenseLine[20];
			strcpy(dispenseLine, "Dispensing: $");
			strcat(dispenseLine, floatString);

			// LCD - Dispensing
			char* LCDDispense[4] = {"", dispenseLine, line, ""};
			lcd.lcd_write_message(true, errMessageDispense, LCDDispense);

			// Dispensing Coins
			for (int i=0; i<DISPENSER_SIZE; i++) {
				if (coinDispense[i] > 0) {
					dispensers[i].push_coin(coinDispense[i]);
				}
			}

			// LCD - Complete Transaction
			char* LCDComplete[4] = {"Transaction Complete", "Please Pick Up", "Your Coins", ""};
			lcd.lcd_write_message(true, errMessageDispense, LCDComplete);

			// Added a time delay, can change this
			HAL_Delay(5000);

			return 0;
		}
	}
}


int roundUserInput (int userInput) {
	// for rounding user input upwards to 5 cents multiple

	// get the last digit from the value, from the remainder of modulus operation
	int lastVal = userInput%10;

	// setting it up so that we always round up to the nearest 5 cents no matter what
	// round up because want to give the user more than they need, not less
	if (lastVal == 0)
	{
		// no rounding required
		return userInput;
	}

	if (lastVal <=5)
	{
		// round to 5
		userInput = userInput + (5-lastVal);
	}

	else
	{
		// round to 10
		userInput = userInput + (10-lastVal);
	}

	return userInput;
}

int validateUserInput (int userVal, CoinDispenser *dispensers) {
	// check if user input is greater than ATM limit - $10.00 = 1000
	if (userVal == 0) {
		return 100000;
	}

	// checks if user input is greater than the ATM inventory
	int inventorySum = inventoryMoney(dispensers);
	if (userVal > inventorySum) {
		return inventorySum;
	}


	return 1;
}

//user_function ----------------------------------------------------------
int inventoryMoney (CoinDispenser *dispensers) {
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

bool coinToDispense (int userValue, CoinDispenser *dispensers, int *coinDispense) {
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
void servoSweep (CoinDispenser* cd) {
	int angles[5] = {25, 70, 90, 120, 180};

	for (int i = 0; i < 5; i++) {
		cd->servo_write(angles[i]);
		HAL_Delay(200);
		//servo_write(0);
		HAL_Delay(200);
	}
	HAL_Delay(600);

}
