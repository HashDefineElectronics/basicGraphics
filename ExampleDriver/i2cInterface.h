/*
 * i2cInterface.h
 *
 *  Created on: Dec 14, 2018
 *      Author: Ronald Sousa http://hashdefineelectronics.com
 */

#ifndef __I2C_INTERFACE_H__
#define __I2C_INTERFACE_H__

	#include <stdint.h>

	/**
	 * Creates a interface layer I2C peripherals. This should match
	 */
	typedef struct I2CInterface {
		void (*Open) (uint_fast8_t i2cPortNumber, uint8_t slaveAddress);
		void (*Close) (void);
		void (*Write) (uint8_t * source, uint32_t length);
		void (*Read) (uint8_t * destination, uint32_t length);
	} I2CInterface;

#endif /* __I2C_INTERFACE_H__ */
