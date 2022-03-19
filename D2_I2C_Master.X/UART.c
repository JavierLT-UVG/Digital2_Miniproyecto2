#include "UART.h"

void config_uart(void)
{
    SPBRGH = 0;
    SPBRG = 8;                  // Selector de Baud Rate (115 200)
    TXSTAbits.SYNC = 0;         // Modo asíncrono
    TXSTAbits.BRGH = 1;         // High Baud Rate alta velocidad
    BAUDCTLbits.BRG16 = 1;      // Generador de Baud Rate de 8 bits
    RCSTAbits.SPEN = 1;         // Habilitar puertos seriales
}

void config_tx(void)
{
    PORTCbits.RC6 = 0;          // Habilitar pin TX como salida
    TXSTAbits.TX9 = 0;          // Envío de 8 bits
    TXSTAbits.TXEN = 1;         // Habilitar envío de datos
    return;
}

void config_rx(void)
{ 
    PORTCbits.RC7 = 1;          // Habilitar pin RX como entrada
    RCSTAbits.RX9 = 0;          // Recepción de 8 bits
    RCSTAbits.CREN = 1;         // Habilitar recepción de datos
    PIE1bits.RCIE = 1;          // Habilitar interrupciones de recepción
    PIR1bits.RCIF = 0;          // Apagar bandera de interrupciones de recepción
    return;
}

void tx_char(char dato)
{
    TXREG = dato;       // Enviar dato
    __delay_ms(1);      // Esperar a que se termine de enviar
    return;
}

void tx_int(uint8_t dato)
{
    TXREG = dato;       // Enviar dato
    __delay_ms(1);      // Esperar a que se termine de enviar
    return;
}

void tx_string(char dato[])
{
    for(int i = 0; i<strlen(dato); i++) // Ir de 0 hasta la longitud del string
    {
        TXREG = dato[i];    // Mostrar cada char del string de 1 en 1
        __delay_ms(1);     // Delay entre cada char mostrado
    }
    return;
}