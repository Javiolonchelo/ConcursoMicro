#ifndef PULSADOR_H
#define PULSADOR_H
#include "mbed.h"

// Mensajes generados por este autómata
extern bool msg_pulsador_long;
extern bool msg_pulsador_short;

// Reposo
extern bool volatile can_sleep_pulsador;

// FSM
void pulsador_init(InterruptIn* pin_pulsador);
void fsm_pulsador(void);

#endif
