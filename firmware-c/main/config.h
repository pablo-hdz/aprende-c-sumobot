// ============================================================================
//  Sumobot CENFOTEC 2025 - firmware nativo ESP-IDF para la IdeaBoard
//  Todos los ajustes del robot están aquí. Cambiar, compilar y flashear.
// ============================================================================
#pragma once

// ---------------------------------------------------------------- PINES (kit Cenfotec 2025, medidos)
#define PIN_BOOT        0       // botón BOOT, 0 = presionado
#define PIN_PIXEL       2       // LED WS2812B
#define PIN_TRIG        25      // ultrasónico TRIG (medido en el kit: IO25)
#define PIN_ECHO        26      // ultrasónico ECHO (medido en el kit: IO26)
#define PIN_SDA         21      // Qwiic / IMU
#define PIN_SCL         22
// Sensores IR analógicos (ADC1): S1 adel-izq, S2 adel-der, S3 atrás-izq, S4 atrás-der
#define IR_PINS         { 36, 39, 34, 35 }

// ---------------------------------------------------------------- MOTORES
#define MOTOR_LEFT      1       // motor_1 es la rueda izquierda (como en el código de Cenfotec)
#define MOTOR_RIGHT     2
#define MOTOR_FWD       1       // poner -1 si "adelante" va hacia atrás
#define MOTOR_PWM_HZ    50      // igual que la librería ideaboard.py (con esto se ajustó la versión CircuitPython)

// ---------------------------------------------------------------- CONDUCTA (valores de la versión CircuitPython)
#define LOOP_PERIOD_MS      5       // ciclo de control: 200 Hz
#define START_DELAY_MS      5000    // regla de sumo: 5 s tras soltar BOOT (se calibra en este tiempo)
#define DIST_ATTACK_CM      60      // atacar si hay algo más cerca que esto
#define SPEED_ATTACK        1.0f
#define SPEED_SEARCH        0.35f   // giro de búsqueda (subir a 0.45 si no tiene fuerza para girar)
#define SPEED_ESCAPE        0.8f
#define T_BACKUP_MS         350     // retroceso al ver la línea adelante
#define T_TURN_MS           300     // giro tras retroceder si NO hay IMU (el doble si ambos sensores)
#define TURN_DEG            120     // giro tras retroceder si HAY IMU (180 si ambos sensores)
#define T_TURN_MAX_MS       900     // tope de seguridad del giro con IMU
#define T_FWD_ESCAPE_MS     250     // avance a fondo al ver la línea atrás
#define ATTACK_HOLD_MS      0       // memoria de ataque sin IMU (0 = como la versión CircuitPython)
#define CONTACT_HOLD_MS     300     // con IMU: sigue empujando este tiempo tras sentir un choque

// ---------------------------------------------------------------- AUTOCALIBRACIÓN IR (en milivoltios)
// Equivalencias con CircuitPython: mV = valor * 3300 / 65535  (50000 -> 2518 mV, 10000 -> 503 mV)
#define IR_SAMPLES          4       // promedio por lectura
#define IR_SATURATED_MV     2800    // si sobre negro marca más que esto, el sensor está saturado...
#define IR_SAT_THRESHOLD_MV 2518    // ...y se usa este umbral (= 50000 en CircuitPython)
#define IR_FACTOR_FRONT     0.75f   // si no: umbral = negro * 0.75 (S1, S2)
#define IR_FACTOR_BACK      0.50f   // umbral = negro * 0.50 (S3, S4)
#define IR_MIN_THRESHOLD_MV 300     // el blanco real lee ~140-200 mV
#define IR_DEAD_MV          300     // sobre negro por debajo de esto = sensor sin corriente (falta 3V3/GND)

// ---------------------------------------------------------------- ULTRASÓNICO
#define SONAR_MAX_CM        150     // más lejos = "no hay nada"
#define SONAR_TIMEOUT_MS    40      // tiempo máximo esperando el eco
#define SONAR_GAP_MS        8       // pausa entre disparos

// ---------------------------------------------------------------- IMU (LSM6DS3TR-C por Qwiic, opcional)
#define IMU_CONTACT_MS2     6.0f    // aceleración horizontal que cuenta como choque
#define IMU_I2C_HZ          400000

// ---------------------------------------------------------------- TELEMETRÍA
#define TELEMETRY_MS        50      // línea CSV por USB cada 50 ms (0 = apagada)
#define PIXEL_BRIGHTNESS    0.25f
