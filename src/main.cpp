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
}


int main()
{    
    while(1)
    {

    }
}