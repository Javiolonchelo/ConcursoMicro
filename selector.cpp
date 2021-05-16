#include "selector.h"

#define REFRESH_RATE_us 200000

// Asignación de pines
static AnalogIn* selector;

// Estados del autómata
typedef enum
{
  IDLE,
  MEASURE,
} estado_t;
static estado_t estado;

// Reposo
bool volatile can_sleep_selector;

// Mensajes leídos
bool msg_start_select;
bool msg_stop_select;

// Parámetro leído
int msg_number_options;

// Mensajes generados
bool msg_option_1;
bool msg_option_2;
bool msg_option_3;

// Ticker
static Ticker ticker;
static bool volatile ticker_event;

// Inicialización una sola vez
static bool init_done = false;

// Para no enviar mensajes repetidos y evitar los parpadeos en menú
static uint8_t prev_option;

// ISRs -----------------------------
static void ticker_isr(void)
{
  ticker_event = true;
  can_sleep_selector = false;
}
// ----------------------------------

// Inicialización del selector
void selector_init(AnalogIn* pin_selector)
{
  if (!init_done)
  {
    init_done = true;
    selector = pin_selector;

    msg_option_1 = false;
    msg_option_2 = false;
    msg_option_3 = false;

    prev_option = 0;
    can_sleep_selector = true;

    estado = IDLE;
  }
}

// FSM -------------------------------------------------------------------------
void fsm_selector(void)
{
  if (init_done)
  {
    switch (estado)
    {
    case IDLE:
      // Irrelevantes ---------------
      msg_stop_select = false;
      ticker_event = false;
      // ----------------------------
      if (msg_start_select)
      {
        msg_start_select = false;
        ticker.attach_us(ticker_isr, REFRESH_RATE_us);
        estado = MEASURE;
      }
      break;

    default: // case MEASURE
      // Irrelevantes ---------------
      msg_start_select = false;
      // ----------------------------
      if (ticker_event)
      {
        ticker_event = false;

        switch (selector_read(msg_number_options))
        {
        case 1:
          if (1 != prev_option)
          {
            msg_option_1 = true;
            prev_option = 1;
          }
          break;

        case 2:
          if (2 != prev_option)
          {
            msg_option_2 = true;
            prev_option = 2;
          }
          break;

        default: // case 3
          if (3 != prev_option)
          {
            msg_option_3 = true;
            prev_option = 3;
          }
          break;
        }
      }
      else if (msg_stop_select)
      {
        msg_stop_select = false;
        ticker.detach();
        estado = IDLE;
      }
      else { }
      break;
    }
  }

  // Reposo
  __disable_irq();
  if (!msg_stop_select && !msg_start_select && !ticker_event)
  {
    can_sleep_selector = true;
  }
  __enable_irq();
}
// end of FSM ------------------------------------------------------------------

int selector_read(int num_options)
{
  uint16_t value = (selector)->read_u16();
  uint8_t i = 0;

  while (i < num_options)
  {
    if ((i * 65535 / num_options) > value)
    {
      break;
    }
    i++;
  }

  return i;
}
