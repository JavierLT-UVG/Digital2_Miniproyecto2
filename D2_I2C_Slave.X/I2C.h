/* 
 * File: I2C.h
 * Author: Francisco Javier López Turcios
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef I2C_H
#define	I2C_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdint.h>
#define _XTAL_FREQ 4000000

void i2c_config_master(void);
void i2c_config_slave(uint8_t address);
void i2c_iddle_mode(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(unsigned data);
uint8_t i2c_read(void);


#endif	