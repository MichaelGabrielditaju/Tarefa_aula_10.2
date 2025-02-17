#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/clocks.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define JOYSTICK_X_PIN 27  // GPIO para eixo X
#define JOYSTICK_Y_PIN 26  // GPIO para eixo Y
#define JOYSTICK_PB 22     // GPIO para botão do Joystick
#define Botao_A 5          // GPIO para botão A
#define WIDTH 128          // Largura do display
#define HEIGHT 64          // Altura do display
#define Botao_B 6          // GPIO para botão B
#define LED_VERMELHO 13    // GPIO para LED Vermelho
#define LED_VERDE 11       // GPIO para LED Verde
#define LED_AZUL 12        // GPIO para LED Azul
#define DEBOUNCE 50        // Debouncing em ms
#define PWM_MAX 4095       // Valor máximo para PWM
#define NEUTRO_MIN 1900    // Valor mínimo neutro
#define NEUTRO_MAX 2300    // Valor máximo neutro

// Variáveis globais
bool led_verde_estado = false;  // Estado do LED Verde
bool pwm_ativos = true;          // Estado do PWM
bool borda_estado = false;       // Estado da borda

uint32_t tam_quadrado_min = 8;
uint32_t tam_quadrado_atual = 8;
uint32_t tam_quadrado_max = 30;

void gpio_irq_handler(uint gpio, uint32_t events) {
    static absolute_time_t last_time = {0}; 
    absolute_time_t now = get_absolute_time();

    if (absolute_time_diff_us(last_time, now) > DEBOUNCE * 1000) { // Debouncing
        last_time = now;

        if (gpio == JOYSTICK_PB) {
            led_verde_estado = !led_verde_estado; // Alterna o estado do LED Verde
            gpio_put(LED_VERDE, led_verde_estado);
            borda_estado = !borda_estado; // Alterna o estado da borda
        } else if (gpio == Botao_A) {
            pwm_ativos = !pwm_ativos; // Alterna o estado do PWM
        }else if(gpio == Botao_B){
          if(tam_quadrado_atual<tam_quadrado_max){
            tam_quadrado_atual++;
          }
          else{
            tam_quadrado_atual=tam_quadrado_min; // Aumenta o tamanho do quadrado até um limite estabelecido
          }
        }
    }
}

void inicializa_leds() {
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
}

void inicializa_botoes() {
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);

    gpio_init(Botao_A);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_pull_up(Botao_A);

    gpio_init(Botao_B);
    gpio_set_dir(Botao_B, GPIO_IN);
    gpio_pull_up(Botao_B);
}

void inicializa_pwm() {
    gpio_set_function(LED_VERMELHO, GPIO_FUNC_PWM);
    gpio_set_function(LED_AZUL, GPIO_FUNC_PWM);

    uint slice_numero_vermelho = pwm_gpio_to_slice_num(LED_VERMELHO);
    uint slice_numero_azul = pwm_gpio_to_slice_num(LED_AZUL);

    pwm_set_wrap(slice_numero_vermelho, PWM_MAX);
    pwm_set_wrap(slice_numero_azul, PWM_MAX);

    pwm_set_enabled(slice_numero_vermelho, true);
    pwm_set_enabled(slice_numero_azul, true);
}

int main() {
    inicializa_leds();
    inicializa_botoes();
    inicializa_pwm();

    // Configuração de interrupção
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(Botao_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(Botao_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // I2C Inicialização
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_t ssd; // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);  

    uint16_t adc_value_x;
    uint16_t adc_value_y;  
    char str_x[5];  // Buffer para armazenar a string
    char str_y[5];  // Buffer para armazenar a string  
    int square_x = 60; // Posição inicial do quadrado no eixo X
    int square_y = 30; // Posição inicial do quadrado no eixo Y

    while (true) {
        // Leitura dos valores do joystick
        adc_select_input(1); // Eixo X
        adc_value_x = adc_read();
        adc_select_input(0); // Eixo Y
        adc_value_y = adc_read();    

        // Verifica se os valores estão em uma posição neutra
        if (adc_value_x >= NEUTRO_MIN && adc_value_x <= NEUTRO_MAX &&
            adc_value_y >= NEUTRO_MIN && adc_value_y <= NEUTRO_MAX) {
            // Apaga os LEDs se os valores do joystick estiverem neutros
            pwm_set_gpio_level(LED_VERMELHO, 0);
            pwm_set_gpio_level(LED_AZUL, 0);
        } else {
            // Controla a intensidade dos LEDs RGB
            if (pwm_ativos) {
                // LED Vermelho aumenta à medida que o joystick se afasta do neutro no eixo X
                uint16_t led_vermelho_brightness = (adc_value_x >= NEUTRO_MAX) ?
                    (adc_value_x * PWM_MAX) / 10000 : 
                    (NEUTRO_MIN - adc_value_x) * PWM_MAX / (NEUTRO_MIN - 0);

                // LED Azul aumenta à medida que o joystick se afasta do neutro no eixo Y
                uint16_t led_azul_brightness = (adc_value_y >= NEUTRO_MAX) ?
                    (adc_value_y * PWM_MAX) / 10000 :
                    (NEUTRO_MIN - adc_value_y) * PWM_MAX / (NEUTRO_MIN - 0);

                pwm_set_gpio_level(LED_VERMELHO, led_vermelho_brightness);
                pwm_set_gpio_level(LED_AZUL, led_azul_brightness);
            } else {
                pwm_set_gpio_level(LED_VERMELHO, 0);
                pwm_set_gpio_level(LED_AZUL, 0);
            }
        }

        // Atualiza a posição do quadrado
        square_x = (adc_value_x * (WIDTH - 8)) / 4095; // Mapeia para a largura do display
        square_y = (adc_value_y * (HEIGHT - 8)) / 4095; // Mapeia para a altura do display

        // Atualiza o display
        ssd1306_fill(&ssd, false); // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 60, borda_estado, false); // Desenha a borda
        ssd1306_rect(&ssd, square_y, square_x, tam_quadrado_atual, tam_quadrado_atual, true, false); // Desenha o quadrado
        ssd1306_send_data(&ssd); // Atualiza o display

        sleep_ms(100); // Atraso para controle da taxa de atualização
    }
}