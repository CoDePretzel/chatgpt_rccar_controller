# Documento de Diseño Control RC Car para chatGPT

## Tabla de Contenidos

1. [Introducción](#Introducción)
1. [Descripción del proyecto](#Descripción-del-proyecto)
1. [Idea de solución](#Idea-de-solución)
1. [Requerimientos y restricciones](#Requerimientos-y-restricciones)

## Introducción

[CoDe Pretzel](codepretzel.org) es una comunidad de sistemas embebidos de habla hispana que busca promulgar la práctica de desarrollo de sistemas embebidos.

Por lo tanto, lo primero que pensamos cuando vimos chatGPT plugins, fué:

* *Qué pasaría si le conectamos un carro RC y le decimos que lo maneje a un lugar?*

Y este es el proyecto correspondiente al controlador del carro RC (la parte que contempla la API con chatGPT está en el repo [ChatGPT RC Car API](https://github.com/Karosuo/chatgpt_rccar_api)) y es a la que le daremos mayor atención, debido a que es la sección que nos gusta.

## Descripción del proyecto

Una vez logrando que chatGPT se comunique con una API que se ejcuta localmente en una PC y que esa API tenga la habilidad de enviar información serial por un puerto, entonces empieza este proyecto, el control del carro.

La secuencia sería la siguiente:
1. La PC (que ejecuta la API de chatGPT plugin) envia un mensaje al controlador por algun medio
1. El controlador identifica si el mensaje es "avanzar", "retroceder", "vuelta derecha" o "vuelta izquierda"
1. El controlador enciende o activa el boton correspondiente por una duración de tiempo mínima que permita al carro moverse en la dirección requerida por una distancia arbitraria

Hay que notar que los plugins de chatGPT no parecen contemplar mucha interacción o retroalimentación del estado de las acciones.

Es decir, chatGPT plugins solo reciben las instrucciones de chatGPT y le notifican si se completaron o NO se completaron.

Por lo tanto, no hay necesidad de darle retro alimentación del estado del carro RC a la API, de su ubicación, de su velocidad o incluso de su conectividad.

La API de chatGPT asume que el carro pudo ejecutar la instrucción en todos los casos.
** NOTA: ** Esto se puede ir mejorando en versiones proximas, pero primero que funcione, luego que se vea bonito

## Idea de solución
Vamos a describir las opciones que elegimos para solucionar cada uno de los pasos, primero

*1. La PC envia mensaje al controlador*

La forma más fácil de lograr esto es usando UART, en velocidades estándar, esto nos permite usar el programador de la tarjeta o incluso un cable FTDI a otro puerto UART del controlador si quisieramos (permitiendo al programa funcionar en una tarjeta sin programador)

El mensaje recibido por la PC puede ser lo que queramos, pero lo más fácil es que sea un caracter.

Lo que abre una posibilidad, de enviar un caracter numérico (0,1,2,3) en vez de una letra, digamos (A, R, D, I, que representa la primera letra de las direccinoes)

Recibir números nos permite usarlos como índice de direcciones y así poder acceder al 

Por lo tanto

La alternativa de solución elegida es:
- Basada en interrupciones   
- Usa representaciones de los botones en software

Veamos ahora, las herramientas usadas y el diseño de esta solución.

### Herramientas usadas
- AVR128DA48 Curiosity board
- RC Car generico con control remoto de 4 botones
- Jumper wires
- Cable USB A - uB

### Diseño del sistema

** Configuración del sistema ** 
* El sistema se ejecuta a 4 Mhz
* Tiene UART configurado a una velocidad estándar (9600 o 115200)
* Tiene un Timer - con una interrupción a 1s    
* Tiene 4 salidas de GPIO que activan los 4 botones del control remoto del carro RC de manera independiente
* La aplicación tiene representacion de los botones en software (en un arreglo consecutivo) que continene la siguiente informacion
    * Activo: bool -> Determina si el GPIO que va a activar su boton correspondiente está activo o no
    * ContadorActivo: int16 -> Determina cuánto tiempo lleva activo el boton (en segundos)
    * TiempoActivoEsperado: int16 -> Dictamina cuánto tiempo (en segundos) necesita estar activo el boton para que el carro avance en la dirección indicada (valor totalmente arbitrario)

** Secuencia de interrupciones **
1. UART recibe un caracter y activa una interrupción de UARTRX.
1. La ISR de UARTRX, valida que el caracter es valido (números 0 al 3, uno por cada movimiento) y lo usa como indice para activar la bandera del boton correspondiente, en el arreglo de botones y esta ISR tiene prioridad 2, es decir, es menor al Timer 1 y 2
1. La ISR del Timer itera por la lista de botones, si tiene la bandera activa, revisa si el tiempo que lleva activo es igual al esperado, en caso de que si, desactiva el GPIO asociado, en caso de que NO, aumenta el contador de su tiempo activo.

Esta secuencia puede ser capturada con un diagrama UML de secuencia como sigue:

## Restricciones

**Nota:** Trabjo en progreso

Lo que hace que el sistema en realidad no sea tan complicado, siempre y cuando logremos quitar del camino varias variables y elementos que veremos en [Idea de solución](#Idea-de-solución).


Primero, el control no sería para cualquier carro, de hecho, seria exclusivo para uno que utilice un control remoto de 4 botones (avanzar, retroceder, girar llantas hacia la derecha y girar llantas hacia la izquierda)

