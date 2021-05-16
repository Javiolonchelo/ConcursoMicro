#include "mbed.h"
#include "hardware.h"
#include "pulsador.h"
#include "selector.h"
#include "display.h"
#include "fft.h"
#include "control.h"

int main(void)
{
  hardware_init();
  fft_init(&jack);
  pulsador_init(&pulsador);
  selector_init(&selector);
  disp_init(&disp_rs, &disp_rw, &disp_tr, &disp_en, &disp_data);
  control_init();

  for (;;)
  {
    fsm_selector();
    fsm_pulsador();
    fsm_control();

    // Reposo
    __disable_irq();
    if (can_sleep_selector && can_sleep_pulsador && can_sleep_control && !msg_option_1 && !msg_option_2 && !msg_option_3 && !msg_pulsador_short && !msg_pulsador_long)
    {
      __WFI();
    }
    __enable_irq();
  }
}
