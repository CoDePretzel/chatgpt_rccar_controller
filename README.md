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

Este es el proyecto correspondiente al controlador del carro RC (la parte que contempla la API con chatGPT está en el repo [ChatGPT RC Car API](https://github.com/Karosuo/chatgpt_rccar_api))

## Descripción del proyecto

Una vez logrando que chatGPT se comunique con una API que se ejcuta localmente en una PC y que esa API tenga la habilidad de enviar información serial por un puerto, entonces empieza este proyecto, el control del carro.

La secuencia necesaria sería la siguiente:
1. La PC (que ejecuta la API de chatGPT plugin) envia un mensaje al controlador por algun medio
1. El controlador identifica si el mensaje es "avanzar", "retroceder", "vuelta derecha" o "vuelta izquierda"
1. El controlador enciende o activa el boton correspondiente por una duración de tiempo mínima que permita al carro moverse en la dirección requerida por una distancia arbitraria

Hay que notar que los plugins de chatGPT no parecen contemplar mucha interacción del servicio hacia chatGPT o retroalimentación del estado de las acciones.

Es decir, chatGPT plugins solo recibe las instrucciones de chatGPT y le notifican si se completaron o NO se completaron.

Por lo tanto, no hay necesidad de darle retroalimentación del estado del carro RC a la API, de su ubicación, de su velocidad o incluso de su conectividad.

La API de chatGPT asume que el carro pudo ejecutar la instrucción en todos los casos.
** NOTA: ** Esto se puede ir mejorando en versiones proximas, pero primero que funcione, luego que se vea bonito

## Idea de solución
Vamos a describir las opciones que elegimos para solucionar cada uno de los pasos, primero

*1. La PC envia mensaje al controlador* + *2. El controlador identifica el mensaje*

La forma más fácil de lograr esto es usando UART, esto nos permite usar el programador de la tarjeta o incluso un cable FTDI directo al controlador si quisieramos (permitiendo al programa funcionar en una tarjeta sin programador)

*Nota:* Por programador se implica el programador-depurador integrado en la placa (Curiosity Nano On-board debugger)

El mensaje recibido por la PC puede ser la cadena que queramos, pero lo más fácil es que sea un solo caracter, lo que abre una posibilidad, de enviar un caracter numérico (0,1,2,3) en vez de una letra, digamos (A, R, D, I, que representa la primera letra de las direccinoes)

Recibir números nos permite usarlos como offset de direcciones, accediendo directamente al botón que corresponde el comando y así evitar tener que iterar sobre la lista de botones para encontrar el que buscamos.

Aún así tendremos que validar que los numeros recibidos están dentro del rang (0 >= COMANDO <= 3)

La alternativa de solución elegida es:
- Basada en interrupciones   
- Usa representaciones de los botones en software

*3. El controlador activa el boton por el minimo tiempo requerido*

Para medir el tiempo es indispensable usar un Timer, que en este caso, no necesita ser extremadamente preciso, aproximadamente cada segundo, itera por la lista de botones y si la bandera está activa revisa cuanto tiempo ha estado activo, para saber si lo desactiva o incrementa su contador de tiempo activo.

Veamos ahora, las herramientas usadas y el diseño de esta solución.

### Herramientas usadas
- AVR128DA48 Curiosity board
- RC Car generico con control remoto de 4 botones
- Jumper wires
- Cable USB A - uB

### Diseño del sistema

** Configuración del sistema ** 
* El sistema se ejecuta a 4 Mhz
* Tiene UART configurado a una velocidad estándar (9600)
* Tiene un Timer - con una interrupción a 1s    
* Tiene 4 salidas de GPIO que activan los 4 botones del control remoto del carro RC de manera independiente
* La aplicación tiene representacion de los botones en software (en un arreglo consecutivo) que continene la siguiente informacion
    * Activo: bool -> Determina si el GPIO que va a activar su boton correspondiente está activo o no
    * ContadorActivo: int16 -> Determina cuánto tiempo lleva activo el boton (en segundos)
    * TiempoActivoEsperado: int16 -> Dictamina cuánto tiempo (en segundos) necesita estar activo el boton para que el carro avance en la dirección indicada.

** Secuencia de interrupciones **
1. UART recibe un caracter y activa una interrupción de UARTRX.
1. La ISR de UARTRX, valida que el caracter es valido (números 0 al 3, uno por cada movimiento) y lo usa como indice para activar la bandera del boton correspondiente, en el arreglo de botones y esta ISR tiene prioridad 2, es decir, es menor al Timer 1 y 2
1. La ISR del Timer itera por la lista de botones, si tiene la bandera activa, revisa si el tiempo que lleva activo es igual al esperado, en caso de que si, desactiva el GPIO asociado, en caso de que NO, aumenta el contador de su tiempo activo.

Esta secuencia puede ser capturada con un diagrama UML de secuencia como sigue:
![alt text](MCU_Peripherals_sequence.png)

## Restricciones

Lo que hace que el sistema en realidad no sea tan complicado, siempre y cuando logremos quitar del camino varias variables.

1. El control no sería para cualquier carro, de hecho, seria exclusivo para uno que utilice un control remoto de 4 botones (avanzar, retroceder, girar llantas hacia la derecha y girar llantas hacia la izquierda), nótese que los botones de izquierda y derecha solo rotan las llantas, no las hacen girar.

1. El control permite combinación de botones, donde avanzar-retroceder se junta con girar derecha-izquierda para poder mover el carro en esa dirección.

1. La distancia que se mueve al presionar un botón es totalmente arbitraria y en este caso es aproximadamente 30 cm (~3 segundos de presionar el botón), que permiten a los motores obtener suficiente torque para mover el carro RC que es relativamente pesado.

1. Se utlizará este tiempo base y distancia minima de recorrido del carro RC para proporcionar un contexto informativo a chatGPT, que de alguna manera le permita saber que el carro avanzará o rotará cierta cantidad de unidades, donde 1 unidad = 1 avance de 30cm-3s de presionar el botón.

1. chatGPT no necesita saber la distancia real, solo que avanzó una unidad, siempre y cuando, se le pida que se muva a una ubicación y se le de contexto de cuántas unidades de separación tiene el objetivo de auto.

1. Dado que no hay retroalimentación de ningún tipo, si el texto enviado a chatGPT no contempla la ubicación real del carro RC con respecto a su objetivo, el resultado de movimiento no se espera que sea correcto

1. La velocidad de conmutación de los transistores que activarán los botones del control remoto no se analizó ni probó y no se espera que sea muy alta, sin embargo debería ser más que suficiente y no afectar la funcionalidad esperada

1. No está considerado el desface de conteo de tiempo por el Timer

1. Se considera que la velocidad de los comandos por UART nunca llegará a ser suficientemente alta como que importe si se pierden comando y aunque lo fuera, el tiempo de reacción del carro RC no es suficiente para acomodar dado caso por lo que la prioridad de interrupción es más alta en el Timer que en UART, permitiendo siempre contar el tiempo de los botones, activarlos y desactivarlos sin importar si esto hace perder algunos comandos no recibidos.