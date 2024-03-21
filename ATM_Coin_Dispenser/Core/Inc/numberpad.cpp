/*
 * numberpad.cpp
 *
 *  Created on: Mar 20, 2024
 *      Author: khale
 */

#include "numberpad.hpp"
#include "stm32f4xx_hal.h"


//#define ROW_COUNT 4
//#define COL_COUNT 3



numberpad::numberpad() {
	// TODO Auto-generated constructor stub

}

/*
numberpad::~numberpad() {
	// TODO Auto-generated destructor stub
}
*/


static char decode_keypad(uint8_t col, uint8_t row);


const unsigned char keymap[4][3] =
{
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};



const uint32_t clo_state[4]={	(GPIO_BSRR_BR4|GPIO_BSRR_BS5|GPIO_BSRR_BS6),
						(GPIO_BSRR_BS4|GPIO_BSRR_BR5|GPIO_BSRR_BS6),
						(GPIO_BSRR_BS4|GPIO_BSRR_BS5|GPIO_BSRR_BR6),
						(GPIO_BSRR_BS4|GPIO_BSRR_BS5|GPIO_BSRR_BS6)
};


void numberpad::keypad_init(void)
{

	/*Enable clock access to GPIOC*/
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;

	/*Set PC0 to PC3 as input*/
	GPIOC->MODER &= ~(GPIO_MODER_MODE0|GPIO_MODER_MODE1|GPIO_MODER_MODE2|GPIO_MODER_MODE3);

	/* Activate internal pullup resistor for PC0 to PC3*/
	GPIOC->PUPDR|=GPIO_PUPDR_PUPD0_0|GPIO_PUPDR_PUPD1_0|GPIO_PUPDR_PUPD2_0|GPIO_PUPDR_PUPD3_0;

	/*Set PC4 to PC7 as output*/
	GPIOC->MODER |= GPIO_MODER_MODE4_0|GPIO_MODER_MODE5_0|GPIO_MODER_MODE6_0|GPIO_MODER_MODE7_0;

	GPIOC->MODER &=~( GPIO_MODER_MODE4_1|GPIO_MODER_MODE5_1|GPIO_MODER_MODE6_1|GPIO_MODER_MODE7_1);

	/*Set PC4 to PC7 as high*/

	GPIOC->BSRR = GPIO_BSRR_BS4|GPIO_BSRR_BS5|GPIO_BSRR_BS6|GPIO_BSRR_BS7;
}


char numberpad::keypad_read(void)
{

	unsigned char key=0,data=0;

	for (int i=0;i<4;i++)
	{

		GPIOC->BSRR=clo_state[i];

		data=(GPIOC->IDR)&0xF; /*Get rid of data from bit 5 to bit31*/

		if(data != 0xF)
		{
			key=decode_keypad(i,data);
		}

	}

	return key;
}


char numberpad::decode_keypad(uint8_t col, uint8_t row)
{

	if (row == 0xE) return keymap[0][col];
	if (row == 0xD) return keymap[1][col];
	if (row == 0xB) return keymap[2][col];
	if (row == 0x7) return keymap[3][col];
	return 0;
}
