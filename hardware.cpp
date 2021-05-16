#include "hardware.h"

// Entrada de audio
AnalogIn jack(p19);

// Pulsador
InterruptIn pulsador(p23);

// Selector
AnalogIn selector(p20);

// Display
DigitalOut disp_tr(p21);
DigitalOut disp_rs(p30);
DigitalOut disp_rw(p29);
DigitalOut disp_en(p28);
BusOut disp_data(p8, p9, p10, p11, p12, p13, p14, p15);

// Inicialización una sola vez
static bool init_done = false;

void hardware_init(void)
{
  if (!init_done)
  {
    init_done = true;
    pulsador.mode(PullUp);
  }
}
