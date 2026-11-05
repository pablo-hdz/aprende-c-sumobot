// IMU LSM6DS3TR-C (acelerómetro + giroscopio) por Qwiic, dirección 0x6B (0x6A en IdeaSense).
// Opcional: si no está conectado, el robot funciona igual con giros por tiempo.
// Al calibrar (robot quieto) mide el sesgo del giroscopio y detecta cuál eje es vertical por la
// gravedad, así no importa cómo esté montado el módulo.
#include <math.h>
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "robot.h"

static const char *TAG = "imu";
static i2c_master_bus_handle_t bus;
static i2c_master_dev_handle_t dev;
static bool present;

static float gyro_bias[3], grav[3];
static double sum_g[3], sum_a[3];
static int calib_n;
static int vert = 2;                 // índice del eje vertical
static float yaw_deg, horiz_acc;
static bool contact;

static esp_err_t read_reg(uint8_t reg, uint8_t *buf, size_t n)
{
    return i2c_master_transmit_receive(dev, &reg, 1, buf, n, 20);
}

static esp_err_t write_reg(uint8_t reg, uint8_t v)
{
    uint8_t b[2] = { reg, v };
    return i2c_master_transmit(dev, b, 2, 20);
}

bool imu_init(void)
{
    i2c_master_bus_config_t bc = { .i2c_port = I2C_NUM_0, .sda_io_num = PIN_SDA, .scl_io_num = PIN_SCL,
                                   .clk_source = I2C_CLK_SRC_DEFAULT, .glitch_ignore_cnt = 7,
                                   .flags.enable_internal_pullup = true };
    if (i2c_new_master_bus(&bc, &bus) != ESP_OK) return false;
    const uint8_t addrs[2] = { 0x6B, 0x6A };
    for (int i = 0; i < 2; i++) {
        if (i2c_master_probe(bus, addrs[i], 30) != ESP_OK) continue;
        i2c_device_config_t dc = { .dev_addr_length = I2C_ADDR_BIT_LEN_7, .device_address = addrs[i],
                                   .scl_speed_hz = IMU_I2C_HZ };
        if (i2c_master_bus_add_device(bus, &dc, &dev) != ESP_OK) continue;
        uint8_t who = 0;
        if (read_reg(0x0F, &who, 1) != ESP_OK || who != 0x6A) {   // WHO_AM_I del LSM6DS3TR-C
            i2c_master_bus_rm_device(dev);
            continue;
        }
        write_reg(0x12, 0x44);    // CTRL3_C: BDU + auto-incremento
        write_reg(0x10, 0x6C);    // CTRL1_XL: 416 Hz, +/-8 g (los choques de sumo pasan de 4 g)
        write_reg(0x11, 0x68);    // CTRL2_G: 416 Hz, 1000 dps (los giros rápidos pasan de 250 dps)
        present = true;
        ESP_LOGI(TAG, "LSM6DS3TR-C en 0x%02X", addrs[i]);
        return true;
    }
    ESP_LOGW(TAG, "IMU no encontrada (¿cable Qwiic?). Se usan giros por tiempo.");
    return false;
}

bool imu_present(void) { return present; }

static bool imu_raw(float g[3], float a[3])     // g en grados/s, a en m/s^2
{
    uint8_t d[12];
    if (read_reg(0x22, d, 12) != ESP_OK) return false;
    for (int i = 0; i < 3; i++) {
        int16_t gr = (int16_t)((d[2 * i + 1] << 8) | d[2 * i]);
        int16_t ar = (int16_t)((d[2 * i + 7] << 8) | d[2 * i + 6]);
        g[i] = gr * 0.035f;                       // 35 mdps/LSB a 1000 dps
        a[i] = ar * 0.000244f * 9.80665f;         // 0.244 mg/LSB a 8 g
    }
    return true;
}

void imu_calib_reset(void)
{
    for (int i = 0; i < 3; i++) sum_g[i] = sum_a[i] = 0;
    calib_n = 0;
}

void imu_calib_add(void)
{
    float g[3], a[3];
    if (!present || !imu_raw(g, a)) return;
    for (int i = 0; i < 3; i++) { sum_g[i] += g[i]; sum_a[i] += a[i]; }
    calib_n++;
}

void imu_calib_finish(void)
{
    if (!present || calib_n == 0) return;
    float best = 0;
    for (int i = 0; i < 3; i++) {
        gyro_bias[i] = sum_g[i] / calib_n;
        grav[i] = sum_a[i] / calib_n;
        if (fabsf(grav[i]) > best) { best = fabsf(grav[i]); vert = i; }
    }
    yaw_deg = 0;
    ESP_LOGI(TAG, "eje vertical=%c  gravedad=%.2f m/s2  sesgo giro=%.2f %.2f %.2f dps",
             "XYZ"[vert], grav[vert], gyro_bias[0], gyro_bias[1], gyro_bias[2]);
}

void imu_update(float dt_s)
{
    contact = false;
    float g[3], a[3];
    if (!present || !imu_raw(g, a)) return;
    yaw_deg += (g[vert] - gyro_bias[vert]) * dt_s;
    float hx = 0;
    for (int i = 0; i < 3; i++)
        if (i != vert) hx += (a[i] - grav[i]) * (a[i] - grav[i]);
    horiz_acc = sqrtf(hx);
    contact = horiz_acc > IMU_CONTACT_MS2;
}

float imu_yaw_deg(void) { return yaw_deg; }
float imu_horiz_accel(void) { return horiz_acc; }
bool  imu_contact(void) { return contact; }
