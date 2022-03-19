/*
 * File:   I2C.c
 * Author: fjltu
 *
 * Created on 26 de febrero de 2022, 07:20 PM
 */


#include "I2C.h"

void i2c_config_master(void)
{
    SSPCONbits.WCOL = 0;        // No collision
    SSPCONbits.SSPOV = 0;       // No overflow
    SSPCONbits.SSPEN = 1;       // Habilitar puertos seriales SDA y SCL
    SSPCONbits.CKP = 0;         // No usado en Master mode
    SSPCONbits.SSPM = 0b1000;   // Fosc / (4 * (SSPADD+1))
    
    SSPADD = 9;                 // Fosc / (4*Clock) - 1 = 4M /(4*100k) - 1 = 9
    
    SSPCON2 = 0;                // Condiciones iniciales del registro en 0
    SSPSTAT = 0;
    SSPSTATbits.SMP = 1;        // Slew rate control enabled (100kHz)
    
    TRISCbits.TRISC3 = 1;       // Habilitar SCK y SDA
    TRISCbits.TRISC4 = 1;       //
    return;    
}

void i2c_config_slave(uint8_t address)
{
    SSPCONbits.WCOL = 0;        // No collision
    SSPCONbits.SSPOV = 0;       // No overflow
    SSPCONbits.SSPEN = 1;       // Habilitar puertos seriales SDA y SCL
    SSPCONbits.CKP = 1;         // Release clock
    SSPCONbits.SSPM = 0b0110;   // Address de 7 bits
    
    SSPADD = address;           // Declarar dirección del Slave
    
    SSPCON2bits.SEN = 1;        // Clock stretching is enabled 
    SSPSTATbits.SMP = 1;        // Slew rate control enabled (100kHz)
    
    TRISCbits.TRISC3 = 1;       // Habilitar SCK y SDA
    TRISCbits.TRISC4 = 1;       //
    
    PIE1bits.SSPIE = 1;         // Habilitar interrupción de SPI
    PIR1bits.SSPIF = 0;         // Limpiar bandera de overflow
    return;
}

void i2c_iddle_mode(void)
{
    // OR-ing R/W bit with SEN, RSEN, PEN, RCEN, or ACKEN will indicate if the MSSP is in Idle mode
    while ((SSPSTAT & 0b100) || (SSPCON2 & 0b11111));
    return;
}

void i2c_start(void)
{
    i2c_iddle_mode();
    SSPCON2bits.SEN = 1;        // Initiate Start condition on SDA and SCL pins
    return;
}

void i2c_stop(void)
{
    i2c_iddle_mode();
    SSPCON2bits.PEN = 1;        // Initiate Stop condition on SDA and SCL pins
    return;
}

void i2c_write(unsigned data)
{
    i2c_iddle_mode();
    SSPBUF = data;
    return;
}

uint8_t i2c_read(void)
{
    i2c_iddle_mode();               // Esperar a que el MSSP se desocupe
    SSPCON2bits.RCEN = 1;           // Enables Receive mode 
    
    i2c_iddle_mode();               // Esperar a que el MSSP se desocupe
    uint8_t data = SSPBUF;          // Leer el buffer y almacenar en variable
    
    i2c_iddle_mode();               // Esperar a que el MSSP se desocupe
    SSPCON2bits.ACKDT = 1;          // Enviar bit de acknowledge
    
    SSPCON2bits.ACKEN = 1;          // Iniciar secuencia de acknowledge
    return data;                    // Regresar valor del dato leído
}