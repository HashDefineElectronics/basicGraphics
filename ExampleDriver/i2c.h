/*
 * i2c.h
 *
 *  Created on: Dec 13, 2018
 *      Author: Ronald Sousa https://hashdefineelectronics.com
 */

#ifndef __I2C_H__
#define __I2C_H__
	#include <sys/fcntl.h>
	#include <sys/stat.h>
	#include <linux/i2c-dev.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include "i2cInterface.h"

	extern I2CInterface I2C0;

#endif /* __I2C_H__ */
