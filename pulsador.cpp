#include "pulsador.h"

// Estados del autómata
typedef enum { OFF, BOUNCE, ON } estado_t;
static estado_t estado;

// Asignación de pines
static InterruptIn* sw;

// Mensajes generados
bool msg_pulsador_long;
bool msg_pulsador_short;

// Reposo
bool volatile can_sleep_pulsador;

// Eventos del pulsador
static bool fall_event = false;
static bool rise_event = false;

// Temporizadores
static Timeout timeout_bounce;
static Timeout timeout_1S;
static bool volatile timeout_bounce_event;
static bool volatile timeout_1S_event;

// ISRs -----------------------------
static void fall_isr(void)
{
  fall_event = true;
  can_sleep_pulsador = false;
}
static void rise_isr(void)
{
  rise_event = true;
  can_sleep_pulsador = false;
}
static void timeout_bounce_isr(void)
{
  timeout_bounce_event = true;
  can_sleep_pulsador = false;
}
static void timeout_1S_isr(void)
{
  timeout_1S_event = true;
  can_sleep_pulsador = false;
}
// ----------------------------------

// Inicialización una sola vez
static bool init_done = false;

// Inicialización del pulsador
void pulsador_init(InterruptIn* pin_pulsador)
{
  if (!init_done)
  {
    init_done = true;
    sw = pin_pulsador;
    (sw)->fall(fall_isr);
    (sw)->rise(rise_isr);

    can_sleep_pulsador = true;
    estado = OFF;
  }
}

// FSM -------------------------------------------------------------------------
void fsm_pulsador(void)
{
  if (init_done)
  {
    switch (estado)
    {
    case OFF:
      // Irrelevantes ---------------
      rise_event = false;
      timeout_1S_event = false;
      timeout_bounce_event = false;
      // ----------------------------

      if (fall_event)
      {
        fall_event = false;
        timeout_bounce.attach_us(timeout_bounce_isr, 30000);
        timeout_1S.attach_us(timeout_1S_isr, 1000000);
        estado = BOUNCE;
      }
      break;

    case BOUNCE: // Gestión de rebotes
      // Irrelevantes ---------------
      fall_event = false;
      rise_event = false;
      // ----------------------------

      if (timeout_bounce_event)
      {
        timeout_bounce_event = false;

        if ((*sw) == 0)
        {
          estado = ON;
        }
        else
        {
          estado = OFF;
        }
      }
      break;

    default: // case ON
      // Irrelevantes ---------------
      fall_event = false;
      timeout_bounce_event = false;
      // ----------------------------
      if (rise_event)
      {
        rise_event = false;

        if (timeout_1S_event)
        {
          timeout_1S_event = false;
          msg_pulsador_long = true;
        }
        else
        {
          msg_pulsador_short = true;
        }

        timeout_1S.detach();
        timeout_bounce.attach_us(timeout_bounce_isr, 30000);
        estado = BOUNCE;
      }
      break;
    }
  }

  // Reposo
  __disable_irq();
  if (!rise_event && !fall_event && !timeout_1S_event && !timeout_bounce_event)
  {
    can_sleep_pulsador = true;
  }
  __enable_irq();
}
// end of FSM ------------------------------------------------------------------
