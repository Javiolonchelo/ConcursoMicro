#ifndef HARDWARE_H
#define HARDWARE_H

#include "mbed.h"

// Entrada de audio
extern AnalogIn jack;

// Pulsador
extern InterruptIn pulsador;

// Selector
extern AnalogIn selector;

// Display
extern DigitalOut disp_tr;
extern DigitalOut disp_rs;
extern DigitalOut disp_rw;
extern DigitalOut disp_en;
extern BusOut disp_data;

// Inicialización del hardware
void hardware_init(void);

#endif
