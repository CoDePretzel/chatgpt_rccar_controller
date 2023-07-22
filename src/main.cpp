/**
 * Author: Rafael Karosuo
 * 
 * Title: chatGPT RC CAR Control Interface
 * 
 * Programa que recibe una letra por UART y si es parte de la lista esperada
 * Envia un trigger a EVSYS para empezar un timer y prender un GPIO
 * Al término del timer, el gpio se apaga 
 * 
 * Esta funcionalidad logra hacer que el MCU reciba comandos por UART
 * Basados en este caso en un solo caractér por comando
 * Y dependiendo de ese caracter se prenda un cierto GPIO por cierta cantidad de tiempo
 * Y todo esto suceda por hardware (excepto la comparacion del caracter recibido en UART)
 * 
 * La aplicación es el control de un carro RC, que recibe instrucciones de una API local
 * controlada por chatGPT
 * La API local dependiendo del comando indicado, envia un caracter específico al MCU
 * El MCU con el control mencionado arriba, activa un botón del control del carro
 * Y este se mueve hacia en frente, atrás, vuelta derecha o vuelta izquierda
 * por el tiempo dado por el timer
*/
extern "C"
{
#include <avr/interrupt.h>
}


// #include <avr/io.h>

// Configure UART to do a loopback whenever receives one character
// only if that char is part of the expected list

extern "C"
{    
    ISR(USART0_RXC_vect)
    {

    }

    #include <UART.h>
}

// Basado en TB3216-Getting-Started-with-USART-DS90003216
void USART1_sendChar(char c)
{
    while (!(USART1.STATUS & USART_DREIF_bm))
    {
        ;
    } 
    USART1.TXDATAL = c;
}

void systemInit()
{
    // Configuracion basica UART1 a 4MHZ Async/normal mode    
    #define CPU_FREQ 4000000UL
    #define NORMAL_MODE 16U
    #define ASYNC_MODE 64U
    #define BAUD_RATE 115200UL
    USART1.BAUD = (ASYNC_MODE*CPU_FREQ)/(NORMAL_MODE*BAUD_RATE);
    USART1.CTRLB |= USART_TXEN_bm;

    // Configuracion PINs ubicacion por defecto para TX/RX UART1
    PORTC.DIR |= PIN0_bm; // 1 output / 0 input

}

void dummy_delay_500ms()
{
    for(int i; i<(CPU_FREQ/2); i++){}
}


int main()
{   
    systemInit();
    while(1)
    {
        USART1_sendChar('A');
        dummy_delay_500ms();
    }
}