// Ultrasónico HC-SR05: medición en segundo plano por interrupción.
// Una tarea dispara TRIG; la interrupción del pin ECHO toma el tiempo exacto de subida y bajada
// con esp_timer y despierta a la tarea. El ciclo de control solo lee la última distancia:
// nunca se bloquea esperando el eco (en CircuitPython esto costaba hasta 30 ms por ciclo).
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "esp_err.h"
#include "robot.h"

static TaskHandle_t task;
static volatile int64_t t_rise;
static volatile int32_t echo_us = -1;     // ancho del último eco, -1 = sin eco
static volatile bool got_echo;
static volatile bool ever_high;

static void echo_isr(void *arg)
{
    int64_t now = esp_timer_get_time();
    if (gpio_get_level(PIN_ECHO)) {
        t_rise = now;
        ever_high = true;
    } else if (t_rise) {
        echo_us = (int32_t)(now - t_rise);
        t_rise = 0;
        got_echo = true;
        BaseType_t hpw = pdFALSE;
        vTaskNotifyGiveFromISR(task, &hpw);
        portYIELD_FROM_ISR(hpw);
    }
}

static void sonar_task(void *arg)
{
    while (true) {
        if (gpio_get_level(PIN_ECHO) == 0) {           // solo disparar si el eco anterior terminó
            got_echo = false;
            t_rise = 0;
            gpio_set_level(PIN_TRIG, 1);
            esp_rom_delay_us(10);
            gpio_set_level(PIN_TRIG, 0);
            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(SONAR_TIMEOUT_MS));
            if (!got_echo) echo_us = -1;               // nada respondió a tiempo
        }
        vTaskDelay(pdMS_TO_TICKS(SONAR_GAP_MS));
    }
}

void sonar_init(void)
{
    gpio_config_t trig = { .pin_bit_mask = 1ULL << PIN_TRIG, .mode = GPIO_MODE_OUTPUT };
    gpio_config_t echo = { .pin_bit_mask = 1ULL << PIN_ECHO, .mode = GPIO_MODE_INPUT,
                           .intr_type = GPIO_INTR_ANYEDGE };
    ESP_ERROR_CHECK(gpio_config(&trig));
    ESP_ERROR_CHECK(gpio_config(&echo));
    gpio_set_level(PIN_TRIG, 0);
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    xTaskCreatePinnedToCore(sonar_task, "sonar", 3072, NULL, 10, &task, 1);
    ESP_ERROR_CHECK(gpio_isr_handler_add(PIN_ECHO, echo_isr, NULL));
}

float sonar_cm(void)
{
    int32_t us = echo_us;
    if (us <= 0) return -1;
    float cm = us / 58.0f;
    return cm > SONAR_MAX_CM ? -1 : cm;
}

bool sonar_ever_responded(void) { return ever_high; }
