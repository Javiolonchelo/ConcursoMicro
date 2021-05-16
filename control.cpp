#include "control.h"
#include "pulsador.h"
#include "selector.h"
#include "display.h"
#include "fft.h"

#define REFRESH_RATE_FFT_us 50000

// Estados del autómata
typedef enum
{
  OFF,
  MENU,
  SETTINGS,
  ON
} estado_t;
static estado_t estado;

// Reposo
bool volatile can_sleep_control;

// Ticker
static Ticker ticker;
static bool volatile ticker_event;
// ISRs -----------------------------
static void ticker_isr(void)
{
  ticker_event = true;
  can_sleep_control = false;
}
// ----------------------------------

// Para decidir según la opción escogida
static uint8_t chosen_option;

// Inicialización una sola vez
static bool init_done = false;

void control_init(void)
{
  if (!init_done)
  {
    init_done = true;

    // Parámetros generados
    msg_smooth_value = 3;
    msg_number_options = 0;

    // Mensajes generados
    msg_stop_select = false;
    msg_start_select = false;

    estado = OFF;
    chosen_option = 0;
    can_sleep_control = true;
    what_to_show();
  }
}

// FSM -------------------------------------------------------------------------
void fsm_control(void)
{
  if (init_done)
  {
    switch (estado)
    {
    case OFF:
      // Irrelevantes -----------------------
      msg_pulsador_short = false;
      msg_option_1 = false;
      msg_option_2 = false;
      msg_option_3 = false;
      ticker_event = false;
      // ------------------------------------
      if (msg_pulsador_long)
      {
        msg_pulsador_long = false;
        msg_start_select = true;
        msg_number_options = 2;
        disp_welcome(); // "Welcome!"
        disp_menu(chosen_option);
        estado = MENU;
      }
      break;

    case MENU:
      // Irrelevantes -----------------------
      msg_option_3 = false;
      ticker_event = false;
      // ------------------------------------
      if (msg_option_1)
      {
        msg_option_1 = false;
        chosen_option = 1;
        what_to_show(); // "-> Start   Settings"
      }
      else if (msg_option_2)
      {
        msg_option_2 = false;
        chosen_option = 2;
        what_to_show(); // "Start  -> Settings"
      }
      else if (msg_pulsador_short)
      {
        msg_pulsador_short = false;
        where_to_go();
        what_to_show();
      }
      else if (msg_pulsador_long)
      {
        set_OFF();
      }
      else { }
      break;

    case SETTINGS:
      if (msg_option_1) // High
      {
        msg_option_1 = false;
        chosen_option = 1;
        what_to_show();
      }
      else if (msg_option_2) // Medium
      {
        msg_option_2 = false;
        chosen_option = 2;
        what_to_show();
      }
      else if (msg_option_3) // Low
      {
        msg_option_3 = false;
        chosen_option = 3;
        what_to_show();
      }
      else if (msg_pulsador_short)
      {
        msg_pulsador_short = false;
        where_to_go();
        what_to_show();
      }
      else if (msg_pulsador_long)
      {
        set_OFF();
      }
      else { }
      break;

    default: // case ON
      // Irrelevantes -----------------------
      msg_option_1 = false;
      msg_option_2 = false;
      msg_option_3 = false;
      // ------------------------------------
      if (msg_pulsador_short)
      {
        msg_pulsador_short = false;
        msg_start_select = true;
        msg_number_options = 2;
        ticker.detach();
        disp_menu(chosen_option);
        estado = MENU;
      }
      else if (msg_pulsador_long) // Normal
      {
        set_OFF();
      }
      else if (ticker_event)
      {
        ticker_event = false;
        fft();
        what_to_show();
      }
      else { }
      break;
    }
  }

  // Reposo
  __disable_irq();
  if (!msg_pulsador_short && !msg_pulsador_long && !msg_option_1 && !msg_option_2 && !msg_option_3)
  {
    can_sleep_control = true;
  }
  __enable_irq();
}
// end of FSM ------------------------------------------------------------------

// "Apaga" la pantalla
void set_OFF(void)
{
  msg_pulsador_long = false;
  ticker.detach();
  msg_stop_select = true;
  disp_off();
  estado = OFF;
}

// Efectúa el cambio de estado según la opción escogida
void where_to_go(void)
{
  switch (estado)
  {
  case MENU:
    switch (chosen_option)
    {
    case 1: // ON
      msg_stop_select = true;
      ticker.attach_us(ticker_isr, REFRESH_RATE_FFT_us);
      estado = ON;
      break;

    default: // case 2 - Settings
      msg_number_options = 3;
      estado = SETTINGS;
      break;
    }
    break;

  case SETTINGS:
    msg_number_options = 2;
    estado = MENU;
    switch (chosen_option)
    {
    case 1: // High
      msg_smooth_value = 5;
      break;

    case 2: // Medium
      msg_smooth_value = 3;
      break;

    default: // case 3 - Low
      msg_smooth_value = 1;
      break;
    }
    break;

  default: // case ON and OFF - Ya indicados en la FSM
    break;
  }
}

// Ejecuta las funciones adecuadas para la pantalla
void what_to_show(void)
{
  switch (estado)
  {
  case MENU:
    disp_menu(chosen_option);
    break;

  case SETTINGS:
    disp_settings(chosen_option);
    break;

  default: // case ON
    samples_to_bars(msg_result);
    break;
  }
}
