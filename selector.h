#ifndef SELECTOR_H
#define SELECTOR_H
#include "mbed.h"
#endif

// Mensajes leídos
extern bool msg_start_select;
extern bool msg_stop_select;

// Parámetro leído
extern int msg_number_options;

// Mensajes generados
extern bool msg_option_1;
extern bool msg_option_2;
extern bool msg_option_3;

// Reposo
extern bool volatile can_sleep_selector;

// FSM
void selector_init(AnalogIn* pin_selector);
void fsm_selector(void);

// Otras funciones
int selector_read(int n);
