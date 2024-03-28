/*
 * i2c.hpp
 *
 *  Created on: Mar 20, 2024
 *      Author: khale
 */

#ifndef INC_I2C_HPP_
#define INC_I2C_HPP_

#include "stm32f4xx.h"
#include <stdint.h>
#include "stdio.h"

class i2c {
public:
	i2c();
	//virtual ~i2c();

	void i2c_init();
	char i2c_readByte(char saddr,char maddr,char *data);
	void i2c_writeByte(char saddr,char maddr,char data);
	void i2c_WriteMulti(char saddr,char maddr,char *buffer, uint8_t length);
	void i2c_ReadMulti(char saddr,char maddr, int n, char* data);
	void i2c_bus_scan(void);
};

#endif /* INC_I2C_HPP_ */
