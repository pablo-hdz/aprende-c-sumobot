// ============================================================================
//  Mi primer programa en C para la IdeaBoard: el LED RGB
//  El LED WS2812B de la placa está conectado al pin IO2.
//  Muestra rojo, verde, azul y blanco, y después un arcoíris.
//  Si presionas el botón BOOT, el LED se apaga mientras lo mantienes.
// ============================================================================
#include <stdio.h>
#include "freertos/FreeRTOS.h"     // sistema operativo del ESP32: tareas y esperas
#include "freertos/task.h"         // vTaskDelay()
#include "driver/gpio.h"           // pines digitales (botón BOOT)
#include "led_strip.h"             // driver del LED RGB WS2812B

#define PIN_LED     2              // el LED RGB de la IdeaBoard está en IO2
#define PIN_BOOT    0              // el botón BOOT está en IO0 (0 = presionado)
#define BRILLO      0.2f           // 0.0 a 1.0 (al 100 % encandila)

static led_strip_handle_t led;     // "manija" para controlar el LED

// Prepara el LED: le dice al driver en qué pin está y qué tipo de LED es
static void led_iniciar(void)
{
    led_strip_config_t config = {
        .strip_gpio_num = PIN_LED,
        .max_leds = 1,                                        // la placa tiene un solo LED
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
    };
    led_strip_rmt_config_t rmt = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,                    // 10 MHz
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&config, &rmt, &led));
    led_strip_clear(led);
}

// Pone un color: rojo, verde y azul van de 0 a 255
static void led_color(uint8_t r, uint8_t g, uint8_t b)
{
    led_strip_set_pixel(led, 0, r * BRILLO, g * BRILLO, b * BRILLO);
    led_strip_refresh(led);
}

// Rueda de colores: pos de 0 a 255 recorre todo el arcoíris
static void led_arcoiris(uint8_t pos)
{
    if (pos < 85)       led_color(255 - pos * 3, pos * 3, 0);
    else if (pos < 170) { pos -= 85;  led_color(0, 255 - pos * 3, pos * 3); }
    else                { pos -= 170; led_color(pos * 3, 0, 255 - pos * 3); }
}

static void esperar_ms(int ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }

// En ESP-IDF el programa empieza en app_main() (no en main())
void app_main(void)
{
    led_iniciar();

    gpio_config_t boton = { .pin_bit_mask = 1ULL << PIN_BOOT, .mode = GPIO_MODE_INPUT,
                            .pull_up_en = GPIO_PULLUP_ENABLE };
    gpio_config(&boton);

    printf("Hola desde la IdeaBoard! Mi primer programa en C\n");

    while (1) {                                    // repetir para siempre
        printf("Rojo\n");    led_color(255, 0, 0);     esperar_ms(700);
        printf("Verde\n");   led_color(0, 255, 0);     esperar_ms(700);
        printf("Azul\n");    led_color(0, 0, 255);     esperar_ms(700);
        printf("Blanco\n");  led_color(255, 255, 255); esperar_ms(700);

        printf("Arcoiris\n");
        for (int i = 0; i < 256; i++) {
            if (gpio_get_level(PIN_BOOT) == 0) led_color(0, 0, 0);   // BOOT presionado: apagar
            else                               led_arcoiris(i);
            esperar_ms(10);                        // 256 x 10 ms = ~2.5 s de arcoíris
        }
    }
}
