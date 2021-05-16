#include "fft.h"

// Asignación de pines
static AnalogIn* input;

// Buffers de cálculo
static q15_t samples_1[BUFFER_SIZE];
static q15_t samples_2[2 * BUFFER_SIZE];
static q15_t samples_3[FFT_LENGTH];

// Parámetro leído
uint8_t msg_smooth_value;

// Parámetro generado
uint8_t msg_result[BARS];

// Para calcular la media de cierta cantidad de muestras
static uint8_t mean[5 * BARS];

// Variable auxiliar para realizar la media de muestras en cada barra
static uint8_t j = 0;

// Instancia de la FFT
const static arm_cfft_instance_q15* fft_instance = &arm_cfft_sR_q15_len256;

// Inicialización una sola vez
static bool init_done = false;

// Indica el número de muestras que habrá en cada barra
static uint8_t jump;

// Muestrea la señal, realiza la DFT y aplica el suavizado. 
void fft(void)
{
  if (init_done)
  {
    write_buffer();
    calc();

    if (1 != msg_smooth_value) // Evita realizar operaciones innecesarias
    {
      mean_calc(msg_result);
    }
  }
}

// Calcula una media de cierta cantidad de muestras anteriores.
void mean_calc(uint8_t samples[])
{
  int aux;

  // Guarda las muestras actuales
  for (uint8_t a = 0; a < BARS; a++)
  {
    mean[a + j] = samples[a];
  }

  for (uint8_t b = 0; b < BARS; b++)
  {
    aux = 0;

    for (uint8_t n = 0; n < msg_smooth_value; n++)
    {
      aux += mean[(n * BARS) + b];
    }
    mean[b + j] = aux / msg_smooth_value;
  }

  for (uint8_t a = 0; a < BARS; a++)
  {
    samples[a] = mean[a + j];
  }

  (j < (BARS* (msg_smooth_value - 1))) ? (j += 20) : (j = 0);
}

void calc(void)
{
  // Ordena las muestras como números complejos interpolando ceros
  for (int i = 0; i < (2 * BUFFER_SIZE); i++)
  {
    samples_2[i] = ((i % 2) ? 0 : samples_1[i]);
  }

  // Realiza la DFT con el algoritmo Fast Fourier Transform (FFT)
  arm_cfft_q15(fft_instance, samples_2, 0, 1);

  // Calcula el módulo de la respuesta en frecuencia para cada muestra
  arm_cmplx_mag_q15(samples_2, samples_3, uint32_t(BUFFER_SIZE));

  //
  for (uint8_t i = 0; i < BARS; i++)
  {
    int aux = 0;
    for (uint8_t j = 0; j < jump; j++)
    {
      aux += samples_3[jump * i + 1 + j];
    }
    msg_result[i] = uint8_t(8 * sqrt(sqrt(float(aux / jump))));
    // Usar logaritmos deriva en un tamaño de imagen excesivo para esta versión de Keil :(
    // La raíz cuarta de las muestras de la DFT msg_resulta en una representación bastante agradable a la vista, que para números grandes se aproxima bastante al logaritmo en base dos, y ese ha sido el único criterio que he seguido.
  }
}

void fft_init(AnalogIn* pin_input)
{
  if (!init_done)
  {
    init_done = true;
    input = pin_input;
    jump = FFT_LENGTH / (BARS * 2);
  }
}

void write_buffer(void)
{
  for (int n = 0; n < BUFFER_SIZE; n++)
  {
    samples_1[n] = (input)->read_u16();
  }
}
