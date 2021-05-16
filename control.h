#ifndef CONTROL_H
#define CONTROL_H
#include "mbed.h"

// Parámetro generado
extern uint8_t picked_option;

// Reposo
extern bool volatile can_sleep_control;

// FSM
void control_init(void);
void fsm_control(void);

// Otras funciones
void where_to_go(void);
void what_to_show(void);
void set_OFF(void);
void set_ON(void);

#endif
