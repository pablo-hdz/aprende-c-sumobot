// ============================================================================
//  Ejemplo 02: botón o sensor táctil -> el LED RGB cambia de color
//
//  Sirve con cualquier módulo DIGITAL de 3 pines de los kits (botón, táctil,
//  inclinación, reed, Hall, obstáculo IR...). Conexión con 3 cables hembra-hembra:
//
//      Módulo        IdeaBoard
//      S / DO   -->  IO27
//      + / VCC  -->  3.3V  (el pin de 3.3V de la misma fila que IO27)
//      - / GND  -->  GND
//
//  Al encender, el programa mide cómo está el sensor en REPOSO (sin tocarlo).
//  Cualquier cambio respecto a ese reposo cuenta como "activado". Así funciona
//  tanto con módulos que dan 1 al tocarlos como con los que dan 0.
// ============================================================================
#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "led_strip.h"

#define PIN_LED       2         // LED RGB de la placa
#define PIN_SENSOR    27        // señal del módulo
#define BRILLO        0.2f
#define CICLO_MS      10        // leer el sensor cada 10 ms
#define ESTABLE       3         // lecturas iguales seguidas para aceptar un cambio (anti-rebote)

static led_strip_handle_t led;

// Colores que va recorriendo con cada toque
static const uint8_t COLORES[][3] = {
    {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}, {255, 0, 255}, {0, 255, 255}, {255, 255, 255},
};
static const char *NOMBRES[] = { "rojo", "verde", "azul", "amarillo", "morado", "cian", "blanco" };
#define N_COLORES (sizeof(COLORES) / sizeof(COLORES[0]))

static void led_iniciar(void)
{
    led_strip_config_t config = {
        .strip_gpio_num = PIN_LED, .max_leds = 1, .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
    };
    led_strip_rmt_config_t rmt = { .clk_src = RMT_CLK_SRC_DEFAULT, .resolution_hz = 10 * 1000 * 1000 };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&config, &rmt, &led));
    led_strip_clear(led);
}

static void led_color(uint8_t r, uint8_t g, uint8_t b, float brillo)
{
    led_strip_set_pixel(led, 0, r * brillo, g * brillo, b * brillo);
    led_strip_refresh(led);
}

void app_main(void)
{
    led_iniciar();

    // Configurar IO27 como ENTRADA. La resistencia interna "pull-down" evita que el pin
    // quede "flotando" (leyendo valores al azar) si el módulo está desconectado.
    gpio_config_t entrada = {
        .pin_bit_mask = 1ULL << PIN_SENSOR,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&entrada));

    // 1) Aprender el estado de reposo: 20 lecturas en 200 ms, sin tocar el sensor
    printf("No toques el sensor: midiendo el reposo...\n");
    int unos = 0;
    for (int i = 0; i < 20; i++) {
        unos += gpio_get_level(PIN_SENSOR);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    int reposo = (unos > 10) ? 1 : 0;
    printf("Reposo = %d. Listo: toca o presiona el sensor.\n", reposo);

    // 2) Ciclo principal
    int color = 0, toques = 0;
    bool activado = false;        // estado aceptado (ya filtrado)
    int iguales = 0;              // cuántas lecturas seguidas distintas al estado aceptado
    led_color(COLORES[color][0], COLORES[color][1], COLORES[color][2], 0.05f);   // tenue = esperando

    while (1) {
        bool lectura = gpio_get_level(PIN_SENSOR) != reposo;   // true = distinto al reposo

        // Anti-rebote: solo aceptar el cambio si se repite ESTABLE veces seguidas
        if (lectura != activado) {
            if (++iguales >= ESTABLE) {
                activado = lectura;
                iguales = 0;
                if (activado) {                                 // flanco: recién tocado
                    toques++;
                    color = (color + 1) % N_COLORES;
                    printf("Toque #%d -> %s\n", toques, NOMBRES[color]);
                } else {
                    printf("Soltado\n");
                }
            }
        } else {
            iguales = 0;
        }

        // Mientras está activado el LED brilla fuerte; en reposo queda tenue
        led_color(COLORES[color][0], COLORES[color][1], COLORES[color][2], activado ? BRILLO : 0.05f);
        vTaskDelay(pdMS_TO_TICKS(CICLO_MS));
    }
}
