#include "display.h"
#include "fft.h" // Para leer el resultado de la DFT

/* Valor hexadecimal que debe leer el display lcd (modelo 2004A) en sus
pines 7-14 para realizar dichas funciones. Los valores del resto de
pines se asignarán de forma independiente, ya que facilita su uso. */
#define CLEAR_DISPLAY 0x01
#define RETURN_HOME 0x02
#define CURSOR_SET 0x0C
#define FUNCTION_SET 0x3C // 0x3C
#define POINT_TO_MEM 0X40

// Letras según la ROM del display
#define LETTER_a 0b01100010
#define LETTER_c 0b01100011
#define LETTER_d 0b01100100
#define LETTER_e 0b01100110
#define LETTER_g 0b01100111
#define LETTER_H 0b01001000
#define LETTER_h 0b01101000
#define LETTER_i 0b01101010
#define LETTER_L 0b01001100
#define LETTER_l 0b01101100
#define LETTER_M 0b01001110
#define LETTER_m 0b01101110
#define LETTER_n 0b01101101
#define LETTER_o 0b01101111
#define LETTER_r 0b01110001
#define LETTER_S 0b01010011
#define LETTER_s 0b01110011
#define LETTER_t 0b01110100
#define LETTER_u 0b01110110
#define LETTER_W 0b01010111
#define LETTER_w 0b01110111
#define LETTER_space 0b10100000
#define LETTER_exclam 0b00100010
#define LETTER_arrow_r 0b01111101

// Punteros a los pines
static DigitalOut* tr;
static DigitalOut* rs;
static DigitalOut* rw;
static DigitalOut* en;
static BusOut* data;

// Inicialización una sola vez
static bool init_done = false;

/* Estos valores representan los caracteres que usaremos para mostrar
las barras con valores del 0 al 8. Vienen definidos por la función
'create_characters()', por el método seguido para crearlos. */
const uint8_t levels[] = { 16, 7, 6, 5, 4, 3, 2, 1, 0 };

// Inicialización del display
void disp_init(DigitalOut* pin_rs, DigitalOut* pin_rw, DigitalOut* pin_tr, DigitalOut* pin_en, BusOut* pin_data)
{
  if (!init_done) // Solo se inicializa una vez
  {
    init_done = true;

    // Asignación de pines
    tr = pin_tr;
    rs = pin_rs;
    rw = pin_rw;
    en = pin_en;
    data = pin_data;

    (*tr) = 0;
    disp_off();
  }
}

void disp_on(void)
{
  (*rw) = 0; // Para lo que haremos en este proyecto, siempre valdrá 0
             // Habría sido más fácil conectarlo a GND, fallo mío :P
  (*tr) = 1;
  wait_ms(10);

  // Configuración del display mediante comandos
  send_command(FUNCTION_SET);
  wait_ms(10);
  send_command(CURSOR_SET);
  wait_ms(10);
  send_command(RETURN_HOME);
  wait_ms(10);
  send_command(CLEAR_DISPLAY);
  wait_ms(10);

  // Para crear los caracteres que usaremos en la representación
  create_characters();
}

void disp_off(void)
{
  (*rs) = 0;
  (*en) = 0;
  (*tr) = 0;
  (*data) = 0;
}

/* Envía un pulso de la señal de habilitación. Es necesario para enviar
datos y comandos al display (como si fuera un botón de confirmación). */
void disp_set(void)
{
  (*en) = 1;
  wait_us(50);
  (*en) = 0;
}

// Para enviar comandos al display
void send_command(int command)
{
  (*rs) = 0;
  (*data) = command;
  disp_set();
  wait_ms(2); // Número alto para asegurar funcionamiento correcto
}

// Para enviar datos que representen caracteres
void send_data(int command)
{
  (*rs) = 1;
  (*data) = command;
  disp_set();
}

// Envía al display un número de espacios especificado
void send_spaces(uint8_t num_spaces)
{
  for (uint8_t n = 0; n < num_spaces; n++)
  {
    send_data(LETTER_space); // Espacios
  }
}

// Crea los caracteres de niveles, de 0 a 8, inclusive
void create_characters(void)
{
  send_command(POINT_TO_MEM); // Para escribir en la CGRAM

  for (uint8_t n = 0; n < 8; n++)
  {
    for (uint8_t i = 0; i < 8; i++)
    {
      if (i < n)
      {
        send_data(0); // Fila de píxeles apagada
      }
      else
      {
        send_data(0x1F); // Fila de píxeles encendida
      }
    }
  }

  send_command(0x80); // Para escribir en la memoria de representación
}

/* Muestra por el display una gráfica de barras, con alturas de 0 a 32
píxeles cada una. Servirá para representar la DFT con 20 muestras. */
void samples_to_bars(uint8_t samples[])
{
  send_command(RETURN_HOME);
  int aux;

  /* Cuatro bucles, cuatro líneas de caracteres. Creo que esta solución
  es la más adecuada por ser la más sencilla de implementar, ya que el
  cursor salta de la línea 1 a la 3, luego a la 2 y finalmente a la 4 */
  for (uint8_t i = 0; i < 20; i++)
  {
    aux = ((samples[i] > 24) ? (samples[i] - 24) : 0); // Ajusta el dato
    send_data(((aux > 8) ? levels[8] : levels[aux]));  //  Escribe
  }

  // Las siguientes iteraciones son análogas a la anterior
  for (uint8_t i = 0; i < 20; i++)
  {
    aux = ((samples[i] > 8) ? ((samples[i] - 8)) : 0);
    send_data(((aux > 8) ? levels[8] : levels[aux]));
  }

  for (uint8_t i = 0; i < 20; i++)
  {
    aux = ((samples[i] > 16) ? ((samples[i] - 16)) : 0);
    send_data(((aux > 8) ? levels[8] : levels[aux]));
  }

  for (uint8_t i = 0; i < 20; i++)
  {
    aux = ((samples[i] > 0) ? ((samples[i])) : 0);
    send_data(((aux > 8) ? levels[8] : levels[aux]));
  }
}

// Welcome! ----------------------------------------
void disp_welcome(void)
{
  disp_on();

  send_spaces(46);

  send_data(LETTER_W);
  wait_ms(100);
  send_data(LETTER_e);
  wait_ms(100);
  send_data(LETTER_l);
  wait_ms(100);
  send_data(LETTER_c);
  wait_ms(100);
  send_data(LETTER_o);
  wait_ms(100);
  send_data(LETTER_m);
  wait_ms(100);
  send_data(LETTER_e);
  wait_ms(100);
  send_data(LETTER_exclam);
  wait_ms(2000);
}
// -------------------------------------------------

// MENU --------------------------------------------
void disp_menu(uint8_t option)
{
  send_command(CLEAR_DISPLAY);
  send_command(RETURN_HOME);

  send_spaces(41);

  switch (option)
  {
  case 1:
    send_data(LETTER_arrow_r);
    break;

  default: // case 2
    send_data(LETTER_space);
    break;
  }

  send_data(LETTER_S);
  send_data(LETTER_t);
  send_data(LETTER_a);
  send_data(LETTER_r);
  send_data(LETTER_t);

  send_spaces(2);

  switch (option)
  {
  case 1:
    send_data(LETTER_space);
    break;

  default: // case 2
    send_data(LETTER_arrow_r);
    break;
  }

  send_data(LETTER_S);
  send_data(LETTER_e);
  send_data(LETTER_t);
  send_data(LETTER_t);
  send_data(LETTER_i);
  send_data(LETTER_n);
  send_data(LETTER_g);
  send_data(LETTER_s);

  send_data(LETTER_space);
}
// -------------------------------------------------


void disp_settings(uint8_t option)
{
  send_command(CLEAR_DISPLAY);
  send_command(RETURN_HOME);

  send_data(LETTER_S);
  send_data(LETTER_m);
  send_data(LETTER_o);
  send_data(LETTER_o);
  send_data(LETTER_t);
  send_data(LETTER_h);
  send_data(LETTER_n);
  send_data(LETTER_e);
  send_data(LETTER_s);
  send_data(LETTER_s);

  send_spaces(22);

  switch (option)
  {
  case 2:
    send_data(LETTER_arrow_r);
    break;

  default: // cases 1, 3
    send_data(LETTER_space);
    break;
  }

  send_data(LETTER_M);
  send_data(LETTER_e);
  send_data(LETTER_d);
  send_data(LETTER_i);
  send_data(LETTER_u);
  send_data(LETTER_m);

  send_spaces(13);

  switch (option)
  {
  case 1:
    send_data(LETTER_arrow_r);
    break;

  default: // cases 2, 3
    send_data(LETTER_space);
    break;
  }

  send_data(LETTER_H);
  send_data(LETTER_i);
  send_data(LETTER_g);
  send_data(LETTER_h);

  send_spaces(15);

  switch (option)
  {
  case 3:
    send_data(LETTER_arrow_r);
    break;

  default: // cases 1, 2
    send_data(LETTER_space);
    break;
  }

  send_data(LETTER_L);
  send_data(LETTER_o);
  send_data(LETTER_w);
}
