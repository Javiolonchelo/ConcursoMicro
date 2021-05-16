#ifndef DISPLAY_H
#define DISPLAY_H
#include "mbed.h"
#include "arm_math.h"

// Inicialización del display
void disp_init(DigitalOut* pin_rs, DigitalOut* pin_rw, DigitalOut* pin_tr, DigitalOut* pin_en, BusOut* pin_data);

// Manejo y configuración del display
void disp_set(DigitalOut* disp_en);
void send_command(int COMMAND);
void send_data(int command);
void create_characters(void);
void send_spaces(uint8_t num_spaces);

// Órdenes dadas por el autómata de control
void disp_on(void);
void disp_off(void);
void disp_menu(uint8_t option);
void disp_welcome(void);
void disp_settings(uint8_t option);
void samples_to_bars(uint8_t samples[]);

#endif
