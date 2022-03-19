/*
 * File:   D2_I2C_Slave.c
 * Author: fjltu
 *
 * Created on 23 de febrero de 2022, 10:03 PM
 */


// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = ON       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#include <string.h>
#include "I2C.h"
#define _XTAL_FREQ 4000000

#define led_roja PORTBbits.RB7
#define led_amarilla PORTBbits.RB5
#define led_verde PORTBbits.RB3

//============================================================================
//============================ VARIABLES GLOBALES ============================
//============================================================================
uint8_t color;                          // Recibe el estado del semáforo (0,1,2)
uint8_t leer_buff;                      // Variable únicamente para leer buffer

//============================================================================
//========================= DECLARACIÓN DE FUNCIONES =========================
//============================================================================
void config_io(void);
void config_reloj(void);
void config_int(void);

//============================================================================
//============================== INTERRUPCIONES ==============================
//============================================================================
void __interrupt() isr(void){
    if(PIR1bits.SSPIF   == 1){
        
        SSPCONbits.CKP  = 0;        // Pausar reloj
        
        // Pic esclavo recibe información
        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW)
        {
            __delay_us(7);
            leer_buff = SSPBUF;     // Leer el buffer
            __delay_us(2);
            
            PIR1bits.SSPIF = 0;     // Limpiar bandera de interrupción 
            SSPCONbits.CKP = 1;     // Reanudar reloj
            
            while(!SSPSTATbits.BF); // Esperar hasta tener todos los bits
            color = SSPBUF;         // Almacenar estado del semáforo
            __delay_us(250);
        }
        
        PIR1bits.SSPIF = 0;         // Limpiar bandera de interrupción
    }
}
//============================================================================
//=================================== MAIN ===================================
//============================================================================
void main(void) 
{
    config_reloj();         // Configurar reloj en 4MHz
    config_io();            // Configurar entradas y salidas
    config_int();
    i2c_config_slave(0x76);  // Configurar como slave en I2C con address de 0x50
    
    while(1)
    {
        switch(color)   // Actualizar el estado del semáforo
        {
            PORTD = color;
            case 1:     // 1 = Verde
                led_verde = 1;
                led_amarilla = 0;
                led_roja = 0;
                break;
            case 2:     // 2 = Amarillo
                led_verde = 0;
                led_amarilla = 1;
                led_roja = 0;
                break;
            case 3:     // 3 = Rojo
                led_verde = 0;
                led_amarilla = 0;
                led_roja = 1;
                break;
            default:    // Default = nada
                break;
        }
    }
}

//============================================================================
//================================ FUNCIONES =================================
//============================================================================
void config_io(void)
{
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0;                  // Salida de LEDs
    TRISD = 0;
    TRISE = 0;
    
    PORTB = 0;                  // Definir conficiones iniciales
    PORTC = 0;                  //
    PORTE = 0;
    color = 0;                  // Color empieza en 1 porque 0 no está definido
    return;
}

void config_reloj(void)
{
    OSCCONbits.IRCF2 = 1;       // 4MHz
    OSCCONbits.IRCF1 = 1;       // 
    OSCCONbits.IRCF0 = 0;       // 
    OSCCONbits.SCS = 1;         // Reloj interno
    return;
}

void config_int(void)
{
    INTCONbits.GIE  = 1;        // Activar interrupciones
    INTCONbits.PEIE = 1;        // Activar interrupciones periféricas
    return;
}