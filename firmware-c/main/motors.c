// Motores: puente H L9110S en IO12/IO14 (motor 1) e IO13/IO15 (motor 2).
// PWM LEDC a MOTOR_PWM_HZ con 14 bits. Igual que ideaboard.py: throttle > 0 -> A con PWM,
// throttle < 0 -> B con PWM, throttle = 0 -> ambos en alto = freno.
#include <math.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "robot.h"

#define MODE   LEDC_LOW_SPEED_MODE
#define TIMER  LEDC_TIMER_0
#define RES    LEDC_TIMER_14_BIT
#define MAXD   ((1u << 14) - 1)

static const ledc_channel_t chan_a[2] = { LEDC_CHANNEL_0, LEDC_CHANNEL_2 };   // IO12, IO13
static const ledc_channel_t chan_b[2] = { LEDC_CHANNEL_1, LEDC_CHANNEL_3 };   // IO14, IO15

void motors_init(void)
{
    ledc_timer_config_t t = { .speed_mode = MODE, .timer_num = TIMER, .duty_resolution = RES,
                              .freq_hz = MOTOR_PWM_HZ, .clk_cfg = LEDC_AUTO_CLK };
    ESP_ERROR_CHECK(ledc_timer_config(&t));
    const int gpios[4] = { 12, 14, 13, 15 };
    const ledc_channel_t chans[4] = { chan_a[0], chan_b[0], chan_a[1], chan_b[1] };
    for (int i = 0; i < 4; i++) {
        ledc_channel_config_t c = { .gpio_num = gpios[i], .speed_mode = MODE, .channel = chans[i],
                                    .timer_sel = TIMER, .duty = 0, .hpoint = 0 };
        ESP_ERROR_CHECK(ledc_channel_config(&c));
    }
    motors_brake();
}

static void set_duty(ledc_channel_t ch, uint32_t d)
{
    ledc_set_duty(MODE, ch, d);
    ledc_update_duty(MODE, ch);
}

static void throttle(int motor, float t)
{
    int m = motor - 1;
    if (t > 1.0f) t = 1.0f;
    if (t < -1.0f) t = -1.0f;
    uint32_t d = (uint32_t)(fabsf(t) * MAXD);
    if (t > 0)      { set_duty(chan_a[m], d);    set_duty(chan_b[m], 0); }
    else if (t < 0) { set_duty(chan_a[m], 0);    set_duty(chan_b[m], d); }
    else            { set_duty(chan_a[m], MAXD); set_duty(chan_b[m], MAXD); }   // freno
}

void drive(float left, float right)
{
    throttle(MOTOR_LEFT, left * MOTOR_FWD);
    throttle(MOTOR_RIGHT, right * MOTOR_FWD);
}

void motors_brake(void) { drive(0, 0); }
