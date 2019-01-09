/*
 * i2c.c
 *
 *  Created on: Dec 14, 2018
 *      Author: Ronald Sousa https://hashdefineelectronics.com
 *
 *  This is an embedded linux I2C wrapper
 */
#include "i2c.h"

static int file;

static uint8_t SlaveAddress;


static void Open(uint_fast8_t i2cPortNumber, uint8_t slaveAddress) {
	  char filename[20];

	  SlaveAddress = slaveAddress;

	  snprintf(filename, 19, "/dev/i2c-%d", i2cPortNumber);

	  file = open(filename, O_RDWR);
	  if (file < 0) {
	    /* ERROR HANDLING; you can check error to see what went wrong */
	    exit(1);
	  }

	  if (ioctl(file, I2C_SLAVE, SlaveAddress) < 0) {
	    /* ERROR HANDLING; you can check error to see what went wrong */
	    exit(1);
	  }
}

static void Close(void) {
	if(file) {
		close(file);
	}
}


static void Write(uint8_t * source, uint32_t length) {
	if(!file || !source) {
		// invalid file
		return;
	}

	if (write(file, source, length) != length) {
	/* ERROR HANDLING: i2c transaction failed */
	}
}

static void Read(uint8_t * destination, uint32_t length) {
	if(!file) {
		// invalid file
		return;
	}

	/* Using I2C Read, equivalent of i2c_smbus_read_byte(file) */
	if (read(file, destination, length) != length) {
	/* ERROR HANDLING: i2c transaction failed */
	return;
	}
}


/**
 * this is the I2C interface instance
 */
I2CInterface I2C0 = {Open, Close, Write, Read };
