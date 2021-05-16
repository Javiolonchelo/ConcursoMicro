#ifndef FFT_H
#define FFT_H

#include "mbed.h"
#include "arm_const_structs.h" // Para usar el algoritmo FFT

#define BUFFER_SIZE 256
#define FFT_LENGTH 256
#define BARS 20 // Número de columnas disponibles en el display

// Parámetro leído
extern uint8_t msg_smooth_value;

// Parámetro generado
extern uint8_t msg_result[BARS];

// Inicialización del cálculo
void fft_init(AnalogIn* pin_input);

// Función principal
void fft(void);

// Otras funciones
void write_buffer(void);
void calc(void);
void mean_calc(uint8_t samples[]);

#endif
