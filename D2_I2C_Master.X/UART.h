/* 
 * File: UART.h
 * Author: Francisco Javier López Turcios
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef UART_H
#define	UART_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <string.h>
#define _XTAL_FREQ 4000000

void config_uart(void);             // Habilitar comunicación UART
void config_tx(void);               // Configurar envío del UART
void config_rx(void);               // Configurar recepción del UART
void tx_char(char dato);            // Enviar chars
void tx_int(uint8_t dato);          // Enviar uint8_t
void tx_string(char dato[]);        // Enviar strings

#endif	
