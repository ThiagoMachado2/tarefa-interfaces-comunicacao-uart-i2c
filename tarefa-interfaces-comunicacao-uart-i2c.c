#include "hardware/clocks.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "pio_matrix.pio.h"
#include "hardware/clocks.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define BUTTON_A 5
#define BUTTON_B 6
#define LED_GREEN 11
#define LED_BLUE 12

#define MATRIX 7
#define NUM_PIXELS 25

// Matriz de números para exibição
const double numbers[10][25] = {
    // Numero 0
    {0.0, 1.0, 1.0, 1.0, 0.0,
     1.0, 1.0, 0.0, 0.0, 1.0,
     1.0, 0.0, 1.0, 0.0, 1.0,
     1.0, 0.0, 0.0, 1.0, 1.0,
     0.0, 1.0, 1.0, 1.0, 0.0},

    // Numero 1
    {0.0, 0.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 1.0, 0.0,
     0.0, 0.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 0.0, 0.0,
     0.0, 1.0, 1.0, 1.0, 0.0},

    // Numero 2
    {0.0, 0.0, 1.0, 1.0, 0.0,
     1.0, 0.0, 0.0, 1.0, 0.0,
     0.0, 0.0, 0.0, 1.0, 0.0,
     0.0, 0.0, 1.0, 0.0, 0.0,
     0.0, 1.0, 1.0, 1.0, 1.0},

    // Numero 3
    {0.0, 1.0, 1.0, 1.0, 0.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 0.0, 1.0, 1.0, 0.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 1.0, 1.0, 1.0, 0.0},

    // Numero 4
    {0.0, 0.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 1.0, 0.0,
     1.0, 1.0, 1.0, 1.0, 0.0,
     0.0, 0.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 0.0, 0.0},

    // Numero 5
    {1.0, 1.0, 1.0, 1.0, 1.0,
     0.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 0.0,
     1.0, 1.0, 1.0, 1.0, 1.0},

    // Numero 6
    {1.0, 1.0, 1.0, 1.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0},

    // Numero 7
    {1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 1.0, 0.0,
     0.0, 0.0, 1.0, 0.0, 0.0,
     0.0, 1.0, 0.0, 0.0, 0.0},

    // Numero 8
    {0.0, 1.0, 1.0, 1.0, 0.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 1.0, 1.0, 1.0, 0.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     0.0, 1.0, 1.0, 1.0, 0.0},

    // Numero 9
    {1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 1.0,
     1.0, 1.0, 1.0, 1.0, 1.0,
     1.0, 0.0, 0.0, 0.0, 0.0,
     1.0, 1.0, 1.0, 1.0, 1.0}};

void update_matrix(const double *pattern, PIO pio, uint sm)
{
    uint32_t value;
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        int inverted_index = NUM_PIXELS - 1 - i;
        value = ((uint32_t)(pattern[inverted_index] * 30) << 8);
        pio_sm_put_blocking(pio, sm, value);
    }
}

// Tempo mínimo entre dois acionamentos do botão (ms)
const uint32_t DEBOUNCE = 200;
uint32_t ultimo_tempo_botao_a = 0;
uint32_t ultimo_tempo_botao_b = 0;

// Estados dos LEDs e flags para controle de interrupção
volatile bool led_green_estado = false;
volatile bool led_blue_estado = false;
volatile bool button_a_pressionado = false;
volatile bool button_b_pressionado = false;

// Objeto do display SSD1306 (global)
ssd1306_t ssd;

// Interrupção dos botões (com debounce)
void gpio_callback(uint gpio, uint32_t events)
{
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (gpio == BUTTON_A && (tempo_atual - ultimo_tempo_botao_a > DEBOUNCE))
    {
        ultimo_tempo_botao_a = tempo_atual;
        button_a_pressionado = true;
    }

    if (gpio == BUTTON_B && (tempo_atual - ultimo_tempo_botao_b > DEBOUNCE))
    {
        ultimo_tempo_botao_b = tempo_atual;
        button_b_pressionado = true;
    }
}
int main()
{
    stdio_init_all();
    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }

    // Configuração da matriz de LEDs
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, MATRIX);

    // Inicializa o display SSD1306
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configuração dos LEDs
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, 0);

    // Configuração dos botões com interrupções
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    char ultimo_caractere = '\0';
    bool mensagem_exibida = false;

    while (true)
    {   
        // Verifica se a conexão USB está ativa e se a mensagem ainda não foi exibida
        if (stdio_usb_connected() && !mensagem_exibida)
        {
            printf("Digite um caractere ou número:\n");

            // Exibe a mensagem inicial no display, centralizada
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, "DIGITE UM", 35, 5); 
            ssd1306_draw_string(&ssd, "CARACTERE OU", 20, 20);
            ssd1306_draw_string(&ssd, "NUMERO", 45, 35);
            ssd1306_send_data(&ssd);

            mensagem_exibida = true;  // Marca que a mensagem já foi exibida
        }

        if (button_a_pressionado)
        {
            button_a_pressionado = false;
            led_green_estado = !led_green_estado;
            gpio_put(LED_GREEN, led_green_estado);
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, led_green_estado ? "LED VERDE LIGADO" : "LED VERDE DESLIGADO", 0, 0);
            ssd1306_send_data(&ssd);
        }

        if (button_b_pressionado)
        {
            button_b_pressionado = false;
            led_blue_estado = !led_blue_estado;
            gpio_put(LED_BLUE, led_blue_estado);
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, led_blue_estado ? "LED AZUL LIGADO" : "LED AZUL DESLIGADO", 0, 0);
            ssd1306_send_data(&ssd);
        }

        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT && c != '\n' && c != '\r' && c != ultimo_caractere)
        {
            printf("Recebido: %c\n", c);
            ssd1306_fill(&ssd, false);
            ssd1306_draw_char(&ssd, c, 50, 25);
            ssd1306_send_data(&ssd);
            ultimo_caractere = c;

            if (c >= '0' && c <= '9')
            {
                int num = c - '0';
                update_matrix(numbers[num], pio, sm);
            }
        }

        sleep_ms(10);
    }
}