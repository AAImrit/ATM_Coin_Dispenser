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
	char* errMessage = "";

	switch (state) {
		// START PAGE
		case 0: {
			// LCD
			char* LCDStartPage[4] = {"", "Welcome", "Press # To Start", ""};
			lcd.lcd_write_message(true, errMessage, LCDStartPage);

			bool buttonPress = numPad.isConfirm(); // wait for the confirm button to be pressed

			// ADD - Timeout
			while (buttonPress != true){
				buttonPress = numPad.isConfirm();
			}

			globalPreviousState = 0; // set the previous state to the current one
			return 1; // return state 1 (move on to next case)
		}


		// ENTER PIN
		case 1: {
			// LCD
			// display error message if user inputed wrong pin (been through case 1 before)
			if (globalPreviousState == 1){
				errMessage = "Invalid: ";
			}

			char* LCDPin[4] = {"Enter Pin", "", "Press # to Continue", "Press * to Cancel"};
			lcd.lcd_write_message(true, errMessage, LCDPin);


			int pin = numPad.getPin(lcd, errMessage); // get the user pin

			// checking if cancel or pin is valid (matches internal password)
			if (pin != 1234){

				if (pin == 100000){ // check for cancel
					return 0;// return to start page
				}

				globalPreviousState = 1; // set the previous state to the current one

				return 1; // return to current state
			}

			globalPreviousState = 1; // set the previous state to the current one
			return 2; // return state 2 (move on to next case)
		}


		// ENTER AMOUNT & DISPENSING
		case 2: {

			// GETTING USER INPUT
			// LCD: Enter Amount
			char* LCDAmount[4] = {"Enter Amount", "$00.00", "Press # to Continue", "Press * to Cancel"};
			lcd.lcd_write_message(true, errMessage, LCDAmount);

			int userInput = numPad.numberToDisplay(lcd);// get withdrawal amount and convert to int

			if (userInput == 100000){ // check for cancel
				return 0;
			}

			//round and check for valid input
			int userVal = roundUserInput(userInput);
			int isInputValid = validateUserInput(userVal, dispensers);

			if (isInputValid != 1){ // Check for errors
				if (isInputValid == 100000){ // Error #1 - User Enters Blank Value
					char* LCDError1[4] = {"Error: Zero Amount", "Enter Value > $00.00", "Press # to Retry", ""};
					lcd.lcd_write_message(true, errMessage, LCDError1);

				} else { // Error #2 - User Enters Amount Exceeding Inventory, Display the Amount
					char* inventoryLine = convertIntToFloatString(isInputValid, "Enter Value < $"); // convert int into a string for the LCD
					char* LCDError2[4] = {"Error: Low Inventory", inventoryLine, "Press # to Retry", ""};
					lcd.lcd_write_message(true, errMessage, LCDError2);
				}

				bool buttonPress = numPad.isConfirm(); // wait for the confirm button to be pressed

				// ADD - Timeout
				while (buttonPress != true){
					buttonPress = numPad.isConfirm();
				}

				return 2; // return to current state
			}

			// DISPENSING
			// get number of coins required for each type
			int coinDispense[DISPENSER_SIZE] = {0,0,0,0,0};
			bool isDispense = coinToDispense(userVal, dispensers, coinDispense);

			if (!isDispense) { //cannot dispense coins, not enough coins of right type
				char line[20];
				sprintf(line, "%dT %dL %dQ %dD %dN", dispensers[0].get_coin_left(), dispensers[1].get_coin_left(), dispensers[2].get_coin_left(), dispensers[3].get_coin_left(), dispensers[4].get_coin_left());
				char* LCDError3[4] = {"Not Enough Coins!", "Current Inventory:", line, "Press # to Retry"};
				lcd.lcd_write_message(true, errMessage, LCDError3);

				bool buttonPress = numPad.isConfirm(); // wait for the confirm button to be pressed

				// ADD - Timeout
				while (buttonPress != true){
					buttonPress = numPad.isConfirm();
				}

				return 2; // return to current state
			}


			// if the ATM can return the amount
			char* dispenseLine = convertIntToFloatString(userVal, "Dispensing: "); // convert userVal into a string for the LCD

			// LCD: Dispensing
			char line[20];
			sprintf(line, "%dT %dL %dQ %dD %dN", coinDispense[0], coinDispense[1], coinDispense[2], coinDispense[3], coinDispense[4]);
			char* LCDDispense[4] = {"", dispenseLine, line, ""};
			lcd.lcd_write_message(true, errMessage, LCDDispense);
			HAL_Delay(2000); // add time delay

			// Dispensing Coins
			for (int i=0; i<DISPENSER_SIZE; i++) {
				if (coinDispense[i] > 0) {
					dispensers[i].push_coin(coinDispense[i]);
				}
			}

			// LCD: Complete Transaction
			char* LCDComplete[4] = {"Transaction Complete", "Please Pick Up", "Your Coins", ""};
			lcd.lcd_write_message(true, errMessage, LCDComplete);
			HAL_Delay(5000); // time delay

			globalPreviousState = 2;
			return 0; // go back to home page
		}
	}
}


int roundUserInput (int userInput) {
	// for rounding user input upwards to 5 cents multiple

	// get the last digit from the value, from the remainder of modulus operation
	int lastVal = userInput%10;

	// setting it up so that we always round up to the nearest 5 cents no matter what
	// round up because want to give the user more than they need, not less
	if (lastVal == 0){
		// no rounding required
		return userInput;
	}

	if (lastVal <=5) {
		// round to 5
		userInput = userInput + (5-lastVal);
	} else {
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

char* convertIntToFloatString(int integer, char* LCDMessage){
	// convert int into a string
	char floatString[100];
	sprintf(floatString, "%d", integer);

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
	static char data[20];
	strcpy(data, LCDMessage);
	strcat(data, floatString);

	return data;
}
