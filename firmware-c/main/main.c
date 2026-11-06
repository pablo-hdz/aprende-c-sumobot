// ============================================================================
//  Sumobot CENFOTEC 2025 - firmware nativo ESP-IDF
//
//  Misma estrategia que la versión en CircuitPython, pero:
//   - ciclo de control fijo de 200 Hz que nunca se bloquea (el ultrasónico mide en segundo plano)
//   - autocalibración de los 4 IR sobre negro y detección de sensores muertos
//   - giros exactos por grados y "sigue empujando tras un choque" si la IMU está conectada
//   - watchdog: si el programa se cuelga, la placa se reinicia con los motores frenados
//   - autodiagnóstico al encender y telemetría CSV por USB
//
//  Uso: robot dentro del dojo sobre NEGRO -> encender -> LED naranja -> presionar y soltar BOOT
//       -> 5 s sin tocarlo (calibra) -> pelea. BOOT durante la pelea = detener.
//
//  LED: naranja espera | rojo parpadeando cuenta/calibra | azul busca | verde ataca
//       blanco empuja tras choque | rojo escapa de la línea adelante | amarillo escapa de la línea atrás
//  Diagnóstico al encender: verde = todo bien | amarillo = sin IMU | cian = ultrasónico no responde
//  Al terminar de calibrar: 3 parpadeos morados = algún sensor IR dañado o sin corriente
// ============================================================================
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
#include "esp_random.h"
#include "esp_log.h"
#include "robot.h"

static const char *TAG = "sumo";

typedef enum { ST_WAIT, ST_COUNTDOWN, ST_SEARCH, ST_ATTACK, ST_PUSH, ST_BACKUP, ST_TURN, ST_FWD_ESCAPE } state_t;
static const char *STATE_NAME[] = { "ESPERA", "CUENTA", "BUSCA", "ATACA", "EMPUJA", "RETROCEDE", "GIRA", "AVANZA" };

#define MS(x) ((int64_t)(x) * 1000)

static bool boot_pressed(void) { return gpio_get_level(PIN_BOOT) == 0; }

static void wdt_sleep_ms(int ms)          // espera alimentando el watchdog
{
    while (ms > 0) {
        int step = ms > 100 ? 100 : ms;
        vTaskDelay(pdMS_TO_TICKS(step));
        esp_task_wdt_reset();
        ms -= step;
    }
}

static void blink(uint8_t r, uint8_t g, uint8_t b, int times, int ms)
{
    for (int i = 0; i < times; i++) {
        pixel_set(r, g, b);  wdt_sleep_ms(ms);
        pixel_set(0, 0, 0);  wdt_sleep_ms(ms);
    }
}

// ---------------------------------------------------------------- autodiagnóstico al encender
static void self_test(void)
{
    wdt_sleep_ms(400);                       // dar tiempo al ultrasónico de disparar varias veces
    bool imu_ok = imu_present();
    bool sonar_ok = sonar_ever_responded();
    int mv[4];
    ir_read_mv(mv);
    ESP_LOGI(TAG, "Diagnóstico: IMU %s | ultrasónico %s | IR mV %d %d %d %d",
             imu_ok ? "OK" : "NO", sonar_ok ? "OK" : "SIN RESPUESTA", mv[0], mv[1], mv[2], mv[3]);
    if (imu_ok && sonar_ok) blink(0, 255, 0, 1, 500);
    if (!imu_ok)   blink(255, 200, 0, 2, 200);
    if (!sonar_ok) blink(0, 255, 255, 2, 200);
}

// ---------------------------------------------------------------- espera de BOOT (imprime sensores)
static void wait_for_start(void)
{
    ESP_LOGI(TAG, "Listo. Robot sobre NEGRO. Presiona y suelta BOOT para empezar.");
    pixel_set(255, 100, 0);
    int64_t last_print = 0;
    while (!boot_pressed()) {
        int64_t now = esp_timer_get_time();
        if (now - last_print > MS(500)) {
            last_print = now;
            int mv[4];
            ir_read_mv(mv);
            printf("ESPERA  IR mV %4d %4d %4d %4d  dist %.1f cm\n", mv[0], mv[1], mv[2], mv[3], sonar_cm());
        }
        vTaskDelay(pdMS_TO_TICKS(20));
        esp_task_wdt_reset();
    }
    while (boot_pressed()) {                  // arranca al SOLTAR, como el código de Cenfotec
        vTaskDelay(pdMS_TO_TICKS(20));
        esp_task_wdt_reset();
    }
}

// ---------------------------------------------------------------- cuenta regresiva + calibración
static void countdown_and_calibrate(void)
{
    ESP_LOGI(TAG, "Calibrando sobre negro (no mover)...");
    int64_t t0 = esp_timer_get_time();
    ir_calib_reset();
    imu_calib_reset();
    wdt_sleep_ms(500);                        // que la mano se aleje
    while (esp_timer_get_time() - t0 < MS(START_DELAY_MS - 600)) {
        int mv[4];
        ir_read_mv(mv);
        ir_calib_add(mv);
        imu_calib_add();
        int64_t left_ms = (MS(START_DELAY_MS) - (esp_timer_get_time() - t0)) / 1000;
        if ((left_ms / 500) % 2) pixel_set(255, 0, 0); else pixel_set(40, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
        esp_task_wdt_reset();
    }
    int dead = ir_calib_finish();
    imu_calib_finish();
    int thr[4]; bool alive[4];
    ir_get_thresholds(thr, alive);
    printf("UMBRALES mV %d %d %d %d  ACTIVOS %d %d %d %d\n",
           thr[0], thr[1], thr[2], thr[3], alive[0], alive[1], alive[2], alive[3]);
    if (dead) blink(180, 0, 255, 3, 80);
    while (esp_timer_get_time() - t0 < MS(START_DELAY_MS)) {
        pixel_set(255, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
        esp_task_wdt_reset();
    }
    ESP_LOGI(TAG, "¡A pelear!");
}

// ---------------------------------------------------------------- pelea
static void fight(void)
{
    state_t st = ST_SEARCH;
    int64_t until = 0, last_seen = -MS(10000), last_contact = -MS(10000), last_tel = 0;
    int turn_dir = 1;
    float turn_target = TURN_DEG, yaw0 = 0;
    TickType_t wake = xTaskGetTickCount();
    int64_t prev = esp_timer_get_time();

    while (true) {
        vTaskDelayUntil(&wake, pdMS_TO_TICKS(LOOP_PERIOD_MS));
        esp_task_wdt_reset();
        int64_t now = esp_timer_get_time();
        float dt = (now - prev) / 1e6f;
        prev = now;

        if (boot_pressed()) {                 // BOOT durante la pelea: detener
            motors_brake();
            ESP_LOGW(TAG, "Detenido con BOOT");
            while (boot_pressed()) { vTaskDelay(pdMS_TO_TICKS(20)); esp_task_wdt_reset(); }
            return;
        }

        // --- sensores
        int mv[4];
        bool w[4];
        ir_read_mv(mv);
        ir_white(mv, w);
        imu_update(dt);
        if (imu_contact()) last_contact = now;
        float d = sonar_cm();
        if (d > 0 && d < DIST_ATTACK_CM) last_seen = now;
        bool front = w[0] || w[1];
        bool back = w[2] || w[3];

        // --- borde primero: quedarse en el dojo le gana a todo
        if (front && st != ST_BACKUP) {
            st = ST_BACKUP;
            until = now + MS(T_BACKUP_MS);
            if (w[0] && !w[1])      turn_dir = 1;          // línea a la izquierda -> girar a la derecha
            else if (w[1] && !w[0]) turn_dir = -1;         // línea a la derecha -> girar a la izquierda
            else                    turn_dir = (esp_random() & 1) ? 1 : -1;
            turn_target = (w[0] && w[1]) ? 180 : TURN_DEG;
            drive(-SPEED_ESCAPE, -SPEED_ESCAPE);
            pixel_set(255, 0, 0);
            printf("LINEA ADELANTE  IR mV %d %d %d %d\n", mv[0], mv[1], mv[2], mv[3]);
        } else if (back && !front && st != ST_BACKUP && st != ST_FWD_ESCAPE) {
            st = ST_FWD_ESCAPE;
            until = now + MS(T_FWD_ESCAPE_MS);
            drive(1.0f, 1.0f);
            pixel_set(255, 150, 0);
            printf("LINEA ATRAS  IR mV %d %d %d %d\n", mv[0], mv[1], mv[2], mv[3]);
        }

        // --- máquina de estados
        switch (st) {
        case ST_BACKUP:
            if (now >= until) {
                st = ST_TURN;
                yaw0 = imu_yaw_deg();
                int t_ms = imu_present() ? T_TURN_MAX_MS : T_TURN_MS * (turn_target >= 180 ? 2 : 1);
                until = now + MS(t_ms);
                drive(turn_dir * SPEED_ESCAPE, -turn_dir * SPEED_ESCAPE);
            }
            break;
        case ST_TURN: {
            bool done = now >= until;
            if (imu_present() && fabsf(imu_yaw_deg() - yaw0) >= turn_target) done = true;
            if (done) st = ST_SEARCH;
            break;
        }
        case ST_FWD_ESCAPE:
            if (now >= until) st = ST_SEARCH;
            break;
        default: {                           // BUSCA / ATACA / EMPUJA
            bool sees = (now - last_seen) <= MS(ATTACK_HOLD_MS) || last_seen == now;
            bool pushing = imu_present() && (st == ST_ATTACK || st == ST_PUSH)
                           && (now - last_contact) <= MS(CONTACT_HOLD_MS);
            if (sees) {
                st = ST_ATTACK;
                drive(SPEED_ATTACK, SPEED_ATTACK);
                pixel_set(0, 255, 0);
            } else if (pushing) {
                st = ST_PUSH;
                drive(SPEED_ATTACK, SPEED_ATTACK);
                pixel_set(255, 255, 255);
            } else {
                st = ST_SEARCH;
                drive(SPEED_SEARCH, -SPEED_SEARCH);         // girar en el lugar buscando
                pixel_set(0, 0, 255);
            }
            break;
        }
        }

        // --- telemetría CSV: T,ms,estado,S1,S2,S3,S4(mV),dist,yaw,acc_horiz,choque
        if (TELEMETRY_MS && now - last_tel >= MS(TELEMETRY_MS)) {
            last_tel = now;
            printf("T,%lld,%s,%d,%d,%d,%d,%.1f,%.1f,%.1f,%d\n", (long long)(now / 1000), STATE_NAME[st],
                   mv[0], mv[1], mv[2], mv[3], d, imu_yaw_deg(), imu_horiz_accel(), imu_contact());
        }
    }
}

void app_main(void)
{
    motors_init();                            // primero: motores frenados desde el arranque
    pixel_init();
    pixel_set(255, 100, 0);

    gpio_config_t boot = { .pin_bit_mask = 1ULL << PIN_BOOT, .mode = GPIO_MODE_INPUT,
                           .pull_up_en = GPIO_PULLUP_ENABLE };
    ESP_ERROR_CHECK(gpio_config(&boot));

    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));  // vigilar este ciclo
    ir_init();
    sonar_init();
    imu_init();
    printf("\n=== Sumobot CENFOTEC 2025 - firmware ESP-IDF ===\n");
    printf("Telemetria: T,ms,estado,S1,S2,S3,S4(mV),dist_cm,yaw_deg,acc_horiz,choque\n");
    self_test();

    while (true) {
        motors_brake();
        wait_for_start();
        countdown_and_calibrate();
        fight();                              // vuelve si se presiona BOOT
    }
}
