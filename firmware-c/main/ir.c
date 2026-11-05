// Sensores IR de piso: 4 fototransistores analógicos de la placa base Sumobot 2025 en ADC1.
// Medido en el kit: BLANCO = voltaje BAJO (~140-200 mV), NEGRO / aire = ALTO (hasta ~3130 mV).
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "robot.h"

static const char *TAG = "ir";
static const int pins[4] = IR_PINS;
static const char *names[4] = { "S1 adel-izq", "S2 adel-der", "S3 atras-izq", "S4 atras-der" };

static adc_oneshot_unit_handle_t adc;
static adc_cali_handle_t cali;
static adc_channel_t chan[4];

static int  thr[4] = { 2518, 2518, 503, 503 };   // por defecto: los del código ganador
static bool alive[4] = { true, true, true, true };
static int64_t calib_sum[4];
static int calib_n;

void ir_init(void)
{
    adc_oneshot_unit_init_cfg_t unit_cfg = { .unit_id = ADC_UNIT_1 };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc));
    adc_oneshot_chan_cfg_t chan_cfg = { .atten = ADC_ATTEN_DB_12, .bitwidth = ADC_BITWIDTH_DEFAULT };
    for (int i = 0; i < 4; i++) {
        adc_unit_t unit;
        ESP_ERROR_CHECK(adc_oneshot_io_to_channel(pins[i], &unit, &chan[i]));
        ESP_ERROR_CHECK(adc_oneshot_config_channel(adc, chan[i], &chan_cfg));
    }
    adc_cali_line_fitting_config_t cali_cfg = {
        .unit_id = ADC_UNIT_1, .atten = ADC_ATTEN_DB_12, .bitwidth = ADC_BITWIDTH_DEFAULT,
#if CONFIG_IDF_TARGET_ESP32
        .default_vref = 1100,        // solo se usa si el chip no trae calibración de fábrica
#endif
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_cfg, &cali));

    int dummy[4];                    // las primeras lecturas tras configurar salen bajas: descartarlas
    for (int k = 0; k < 8; k++) ir_read_mv(dummy);
}

void ir_read_mv(int mv[4])
{
    for (int i = 0; i < 4; i++) {
        int sum = 0, raw = 0;
        for (int k = 0; k < IR_SAMPLES; k++) {
            adc_oneshot_read(adc, chan[i], &raw);
            sum += raw;
        }
        adc_cali_raw_to_voltage(cali, sum / IR_SAMPLES, &mv[i]);
    }
}

void ir_calib_reset(void)
{
    for (int i = 0; i < 4; i++) calib_sum[i] = 0;
    calib_n = 0;
}

void ir_calib_add(const int mv[4])
{
    for (int i = 0; i < 4; i++) calib_sum[i] += mv[i];
    calib_n++;
}

int ir_calib_finish(void)
{
    int dead_mask = 0;
    if (calib_n == 0) return 0;
    for (int i = 0; i < 4; i++) {
        int black = (int)(calib_sum[i] / calib_n);
        if (black < IR_DEAD_MV) {
            alive[i] = false;
            dead_mask |= 1 << i;
            ESP_LOGW(TAG, "%s: negro=%4d mV -> DAÑADO o sin 3V3/GND, se ignora", names[i], black);
            continue;
        }
        alive[i] = true;
        if (black > IR_SATURATED_MV) {
            thr[i] = IR_SAT_THRESHOLD_MV;
        } else {
            float f = (i < 2) ? IR_FACTOR_FRONT : IR_FACTOR_BACK;
            thr[i] = (int)(black * f);
            if (thr[i] < IR_MIN_THRESHOLD_MV) thr[i] = IR_MIN_THRESHOLD_MV;
        }
        ESP_LOGI(TAG, "%s: negro=%4d mV -> umbral %4d mV", names[i], black, thr[i]);
    }
    return dead_mask;
}

void ir_white(const int mv[4], bool white[4])
{
    for (int i = 0; i < 4; i++) white[i] = alive[i] && mv[i] < thr[i];
}

void ir_get_thresholds(int t[4], bool a[4])
{
    for (int i = 0; i < 4; i++) { t[i] = thr[i]; a[i] = alive[i]; }
}
