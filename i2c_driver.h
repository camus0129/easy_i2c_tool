#ifndef __I2C_DRIVER_H__
#define __I2C_DRIVER_H__
/**********************************************************
File  Name: 		i2c_driver.h

Description:        i2c driver interface

Auther: 			Sven

Create Date: 		2024-12-19 14:18:04
***********************************************************/

/**********************************************************
Header Files Include
***********************************************************/
#include <stdio.h>
#include <linux/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

/**********************************************************
MACRO defines
***********************************************************/

/**********************************************************
Data Type defines
***********************************************************/

/**********************************************************
Function Declaration
***********************************************************/
int i2c_init(int bus_num);
int i2c_write_buf(int device_fd, unsigned char chip_addr, unsigned char reg_addr, unsigned char *buf, unsigned char len);
int i2c_read_buf(int device_fd, unsigned char chip_addr, unsigned char reg_addr, unsigned char *buf, unsigned char len);
int i2c_write_byte(int device_fd, unsigned char chip_addr, unsigned char reg_addr, unsigned char value);
#endif