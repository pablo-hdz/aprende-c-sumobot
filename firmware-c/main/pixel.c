// LED RGB WS2812B en IO2 con el componente led_strip (RMT).
#include "led_strip.h"
#include "esp_err.h"
#include "robot.h"

static led_strip_handle_t strip;
static uint8_t last[3] = { 1, 1, 1 };

void pixel_init(void)
{
    led_strip_config_t sc = { .strip_gpio_num = PIN_PIXEL, .max_leds = 1, .led_model = LED_MODEL_WS2812,
                              .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
                              .flags.invert_out = false };
    led_strip_rmt_config_t rc = { .clk_src = RMT_CLK_SRC_DEFAULT, .resolution_hz = 10 * 1000 * 1000,
                                  .flags.with_dma = false };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&sc, &rc, &strip));
    led_strip_clear(strip);
}

void pixel_set(uint8_t r, uint8_t g, uint8_t b)
{
    r = (uint8_t)(r * PIXEL_BRIGHTNESS);
    g = (uint8_t)(g * PIXEL_BRIGHTNESS);
    b = (uint8_t)(b * PIXEL_BRIGHTNESS);
    if (r == last[0] && g == last[1] && b == last[2]) return;   // no refrescar si no cambió
    last[0] = r; last[1] = g; last[2] = b;
    led_strip_set_pixel(strip, 0, r, g, b);
    led_strip_refresh(strip);
}
