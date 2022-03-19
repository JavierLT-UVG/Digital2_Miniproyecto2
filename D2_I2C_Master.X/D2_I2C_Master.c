/*
 * File:   D2_I2C_Master.c
 * Author: fjltu
 *
 * Created on 23 de febrero de 2022, 09:06 PM
 */


// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
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
#include "LCD.h"
#include "UART.h"
#include "I2C.h"
#define _XTAL_FREQ 4000000

//============================================================================
//============================ VARIABLES GLOBALES ============================
//============================================================================
uint8_t color;                          // Recibe el estado del semáforo (0,1,2)
uint8_t color_rx;                          // Recibe el estado del semáforo (0,1,2)
unsigned char verde[] = "Verde   ";     // Almacenan los strings del semáforo
unsigned char amarillo[] = "Amarillo";  //
unsigned char rojo[] = "Rojo    ";      //

struct sensor
{
    uint8_t var8bits;
    uint8_t mapeado;
    uint8_t centenas;
    uint8_t decenas;
    uint8_t unidades;
}sensor;


//============================================================================
//========================= DECLARACIÓN DE FUNCIONES =========================
//============================================================================
void config_io(void);
void config_reloj(void);
void config_int(void);
void divisor(uint16_t num, uint8_t *centena, uint8_t *decena, uint8_t *unidad);


//============================================================================
//============================== INTERRUPCIONES ==============================
//============================================================================
void __interrupt() isr (void)
{
    if(PIR1bits.RCIF)               // Interrupción de RX del UART 
    {
        color_rx = RCREG;              // Almacenar el estado del semáforo
        PIR1bits.RCIF = 0;          // Limpiar bandera
    }
}

//============================================================================
//=================================== MAIN ===================================
//============================================================================
void main(void) 
{
    config_reloj();         // Configurar reloj en 4MHz
    config_io();            // Configurar entradas y salidas
    
    ini_LCD();              // Inicializar LCD
    w_Titulos();            // Escribir layout en LCD
    
    config_uart();          // Configurar comunicación UART
    config_tx();            // Habilitar envío de datos UART
    config_rx();            // Habilitar recepción de datos UART
    
    config_int();           // Habilitar interrupciones
    
    i2c_config_master();    // Configurar pic en I2C como master
    
    while(1)
    {
        color = 3;
        // Establecer comunicación con el sensor
        i2c_start();            // Empezar comunicación I2C
        i2c_write(0b10010001);  // Hablarle al sensor de temperatura (Leerlo)
        sensor.var8bits = i2c_read();   // Recibir mediciones de temperatura
        i2c_stop();             // Terminar comunicación con el sensor
        __delay_ms(500);
        
        // Establecer comunicación con el PIC slave
        i2c_start();            // Empezar comunicación I2C
        i2c_write(0x76);         // Hablarle pic slave (Escribirle)
        i2c_write(color);       // Enviarle estado actual del semáforo
        i2c_stop();             // Terminar comunicación con pic
        __delay_ms(500);
        
        // Dividir en dígitos el valor mapeado del sensor
        divisor(sensor.var8bits, &sensor.centenas, &sensor.decenas, &sensor.unidades);
        
        // Escribir el valor del sensor en LCD
        set_Cursor(1,0);
        w_Char(sensor.centenas + '0');
        w_Char(sensor.decenas + '0');
        w_Char(sensor.unidades + '0');
        
        // Enviar por UART el estado del semáforo
        sensor.mapeado = sensor.var8bits + 33;      // Sumar 33 a la variable
        tx_int(sensor.mapeado);                    // Enviar mapeada
        
        // Actualizar el estado del semáforo
        switch(color)
        {
            case 1:     // 1 = Verde
                set_Cursor(1,8);
                w_String(verde);
                break;
            case 2:     // 2 = Amarillo
                set_Cursor(1,8);
                w_String(amarillo);
                break;
            case 3:     // 3 = Rojo
                set_Cursor(1,8);
                w_String(rojo);
                break;
            default:    // Default = No sobreescribir
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
    
    TRISA = 0;                  // PORTD    (LCD)
    TRISC = 0b00;               // RS y E   (LCD)
    TRISE = 0;
    
    PORTA = 0;                  // Condiciones iniciales
    PORTC = 0;                  // 
    PORTE = 0;
    color = 1;                  //
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

void divisor(uint16_t num, uint8_t *centena, uint8_t *decena, uint8_t *unidad)  // Divisor que saca los dígitos de un número
{
    *centena = num / 100;       // Obtener centenas
    uint8_t aux = num % 100;    // Auxliar que almacena decenas y unidades
    *decena = aux / 10;         // Obtener decenas
    *unidad = aux % 10;         // Obtener unidades
    return;
}