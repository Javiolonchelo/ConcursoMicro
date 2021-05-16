# Introducción

La Delegación de Alumnos de la ETSIST organizó un concurso de microcontroladores. Todos los proyectos fueron realmente fascinantes, para mí fue todo un placer poder participar :D

Aquí os dejo mi proyecto: un **analizador de espectro de audio** que puedes usar con tu propio móvil u ordenador.

# Hardware

## Listado de componentes

|             Componente | Cantidad | Información                                                                                          | Precio                                                                                                                                                                                                                                                                                                                                                                                                                                                   |
| ---------------------: | :------: | ---------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|             μC LPC1768 |    1     | [Manual de usuario](https://www.nxp.com/docs/en/user-guide/UM10360.pdf)                              | 68.53 € en [RS](<https://es.rs-online.com/web/p/kits-de-desarrollo-de-microcontroladores/7039238/?cm_mmc=ES-PLA-DS3A-_-google-_-PLA_ES_ES_Raspberry_Pi_%26_Arduino_y_M%C3%B3dulos_de_Desarrollo_Whoop-_-(ES:Whoop!)+Kits+de+Desarrollo+de+Microcontroladores+(2)-_-7039238&matchtype=&pla-339042025339&gclid=Cj0KCQjw4v2EBhCtARIsACan3nyGwa5u02b9Y0W_TOfWcrtByl7H_uylNY6VHLho1Rw2BUhF2Gse9r0aAqfQEALw_wcB&gclsrc=aw.ds>) (era prestado para el concurso) |
|      Display LCD 2004A |    1     | [Datasheet](https://www.beta-estore.com/download/rk/RK-10290_410.pdf)                                | 10.99 € en [Amazon](https://www.amazon.es/2004-M%C3%B3dulo-pantalla-2004A-Arduino/dp/B00SHU2R9M)                                                                                                                                                                                                                                                                                                                                                         |
|       Transistor BC547 |    1     | [Datasheet](https://www.mouser.es/datasheet/2/308/1/BC550_D-2310266.pdf)                             | 0.04 € en [Amazon](https://www.amazon.es/TOOGOO-100pzs-BC547-92-transistor/dp/B00UFJKBEG/ref=sr_1_9?dchild=1&keywords=bc547&qid=1621122991&sr=8-9)                                                                                                                                                                                                                                                                                                       |
|         Op. Amp. ua741 |    1     | [Datasheet](https://www.ti.com/lit/ds/symlink/ua741.pdf)                                             | 0.36 € en [Mouser Electronics](https://www.mouser.es/ProductDetail/Texas-Instruments/UA741CPSR?qs=0O%2FZFlpUpJWvjNym25kwRg%3D%3D)                                                                                                                                                                                                                                                                                                                        |
|          Condensadores |    1     | 270 nF                                                                                               | alrededor de 0.20 €                                                                                                                                                                                                                                                                                                                                                                                                                                      |
|           Resistencias |    3     | 220 Ω (x1), 1 MΩ (x2)                                                                                | Poquito, pero varía mucho según la tienda                                                                                                                                                                                                                                                                                                                                                                                                                |
|         Potenciómetros |    3     | 5 kΩ, 10 kΩ, 50 kΩ                                                                                   | menos de 2.50 € en [Mouser electronics](https://www.mouser.es/ProductDetail/BI-Technologies-TT-Electronics/P160KN2-4QC20B10K?qs=%252BUYXD5bnyXp0dKMvuiQ1XQ%3D%3D&mgh=1&vip=1&gclid=Cj0KCQjw4v2EBhCtARIsACan3nye1P69UVHaKwZ8lj5um4bELRXDn4sVjYRROK_ulTCR6Nwah088bMQaAmjWEALw_wcB)                                                                                                                                                                         |
| Conectores Jack hembra |    2     | [Esquema](https://www.electronicaembajadores.com/datos/fotos/articulos/grandes/ct/ctc1/ctc1500a.jpg) | 2.50 € en [Electrónica Embajadores](https://www.electronicaembajadores.com/es/Productos/Detalle/CTC1500/conectores/conectores-jack/conector-jack-6-3-mm-base-hembra-panel-2-polos-abierta)                                                                                                                                                                                                                                                               |
|                        |          |                                                                                                      | `TOTAL (sin μC): ~ 19 €, TOTAL (con μC): ~ 87 €`                                                                                                                                                                                                                                                                                                                                                                                                         |

## Parte frontal

Existen tres potenciómetros para el control del dispositivo:

- El primer potenciómetro permite regular el contraste del display.
- El segundo potenciómetro permite aumentar o disminuir la ganancia de la entrada.
- El último es el de Selección, para poder escoger la opción deseada.

También tenemos un pulsador que permite confirmar dicha selección. Este también sirve para conmutar el estado del sistema de encendido a apagado y viceversa, mediante una pulsación larga. Algunas características que aquí no comento se describen en la caja del proyecto.

![Parte frontal](Figures\Frontal.JPG)

## Interior de la caja

![Interior de la caja](Figures\CircuitoZoomExplicado.jpeg)

A través de dos entradas de audio mediante conectores jack (en mono, ya que ambos canales se encuentran en corto), se recoge una señal de audio analógica. Se hace pasar por una etapa de amplificación de tensión, de ganancia regulable, y se añade un _offset_ de corriente continua para realizar correctamente la medida.

![Etapa de amplificación de tensión](Figures/EtapaTension.svg)

Cabe mencionar que, en este caso, la tensión manejada es `Vcc = 3.3 V`, puesto que es la tensión que ofrece el microcontrolador a la salida de sus pines.

También podemos encontrar un transistor que regula la alimentación del display, para conmutar el encendido y apagado del mismo.

# Software

Se ha empleado Keil μVision 5 como IDE para el desarrollo de código C++, usando las librerías MBED y CMSIS DSP.

El sistema calcula la DFT de dicha señal mediante el algoritmo FFT aportado por la librería CMSIS – DSP, usando 256 muestras. El resultado es representado en el display LCD, gestionado sin ninguna librería específica.

## Sobre la FFT

Las capacidades de este microcontrolador me han permitido obtener una frecuencia de muestreo máxima de 26 kHz. Esto no lo tuve en cuenta a la hora de comprar mis componentes para el filtrado. Como tenemos un condensador con frecuencia de corte de 20 kHz (para eliminar ruido inaudible), las frecuencias a partir de 13 kHz producen aliasing. Usando un barrido en frecuencia se puede comprobar perfectamente, además de que existe cierta saturación y se pueden apreciar los armónicos. Recomiendo ver el vídeo de demostración.

## Estructura del software

El funcionamiento de este sistema es gobernado por tres autómatas, implementando una interfaz que permite encender el sistema, apagarlo, cambiar la configuración del suavizado de representación y comenzar el análisis espectral.

Adjunto un diagrama de estados del sistema, además de fotos y vídeos mostrando la funcionalidad.

# Vídeo de demostración

![demo](https://youtu.be/l5mfLCQSGKE)

<a href="http://www.youtube.com/watch?feature=player_embedded&v=l5mfLCQSGKE" target="_blank"><img src="http://img.youtube.com/vi/l5mfLCQSGKE/0.jpg" alt="IMAGE ALT TEXT HERE" width="600" height="430" border="2" /></a>

# Conclusión

Este proyecto ha sido toda una aventura para mí. He aprendido muchísimo y, gracias a los demás participantes, he descubierto que esto apenas es la punta del iceberg.

Lo mejor de todo es que no ha sido un proyecto perfecto. Todos los fallos que he ido acumulando me han servido para comprobar de primera mano las consecuencias de un diseño imprudente, como son el aliasing, la saturación, el ruido eléctrico, el sobrecalentamiento del procesador...

Por ello, creo que este es el mejor resultado posible, aquel que me ha brindado más placer por el camino del aprendizaje.

Me ha gustado tanto que, tal vez, ¡me acabe cambiando de carrera! Os iré contando, queridos internautas :)

# Curiosidades y anécdotas

- Tras la cinta decorativa que rodea al pulsador, asoma un pequeño circulito de color negro. Es un altavoz piezoeléctrico, que no pudo ser implementado por superar el tamaño de imagen límite (por usar la versión de prueba de Keil μVision).

- El apartado de configuración estaba pensado para tener hasta dos opciones más y otros cuantos submenús, pero la limitación del tamaño de imagen me impidió seguir por esta línea de desarrollo :(

- Mi amigo Jorge ([@ironjorgarcia](https://github.com/ironjorgarcia)), estudiante de Física en la UAM, se quedó a dormir en mi casa el día anterior a la presentación de resultados. Antes de irnos a dormir, **SE SENTÓ** en mi proyecto por accidente, rompiendo así el potenciómetro del contraste. Sin embargo, tras arreglarlo, descubrimos que su salvaje caída sobre mi querido proyecto mejoró notablemente el funcionamiento del resto de los controles. El trasero de mi amigo es responsable de esta gran mejora experimentada por la interfaz de usuario ヾ(-\_- )ゞ
