// Interfaces de los módulos del firmware
#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "config.h"

// motors.c ------------------------------------------------------------------
void motors_init(void);
void drive(float left, float right);      // -1.0 .. 1.0 cada rueda
void motors_brake(void);

// pixel.c -------------------------------------------------------------------
void pixel_init(void);
void pixel_set(uint8_t r, uint8_t g, uint8_t b);

// ir.c ----------------------------------------------------------------------
void ir_init(void);
void ir_read_mv(int mv[4]);                // milivoltios calibrados
void ir_calib_reset(void);
void ir_calib_add(const int mv[4]);        // acumular lecturas sobre negro
int  ir_calib_finish(void);                // calcula umbrales; devuelve máscara de sensores muertos
void ir_white(const int mv[4], bool white[4]);
void ir_get_thresholds(int thr[4], bool alive[4]);

// sonar.c -------------------------------------------------------------------
void  sonar_init(void);                    // arranca la tarea de medición en segundo plano
float sonar_cm(void);                      // última distancia, -1 = nada
bool  sonar_ever_responded(void);          // ¿se vio alguna vez el pin ECHO subir?

// imu.c ---------------------------------------------------------------------
bool  imu_init(void);                      // true si encontró el LSM6DS3TR-C
bool  imu_present(void);
void  imu_calib_reset(void);
void  imu_calib_add(void);                 // acumular con el robot quieto
void  imu_calib_finish(void);              // sesgo del giroscopio y eje vertical
void  imu_update(float dt_s);              // integra el giro y detecta choques
float imu_yaw_deg(void);                   // ángulo acumulado (con signo)
float imu_horiz_accel(void);               // m/s^2 horizontal sin gravedad
bool  imu_contact(void);                   // choque en esta lectura
