/*
 * LCD.hpp
 *
 *  Created on: Mar 20, 2024
 *      Author: khale
 *
 *      SOURCE: https://blog.embeddedexpert.io/?p=1764
 */

#ifndef INC_LCD_HPP_
#define INC_LCD_HPP_


#include <main.h>
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <i2c.hpp>
#include <string.h>

class LCD {
public:
	LCD();
	//virtual ~LCD();

public:
	void lcd_write_i2c(char saddr, uint8_t *buffer, uint8_t length);
	void lcd_send_cmd(char cmd);
	void lcd_send_data(char data);
	void setCursor(int a, int b);
	void lcd_send_string(char *str);
	void lcd_init(void);
	void lcd_clear(void);
	void lcd_write_message(bool clear, char* errorMessage, char* message[4]);
};

#endif /* INC_LCD_HPP_ */
