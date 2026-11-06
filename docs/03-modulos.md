# 3. Módulos, archivo por archivo

```
firmware-c/
├── CMakeLists.txt          nombre del proyecto (3 líneas)
├── sdkconfig.defaults      configuración del chip: 8 MB, watchdog 2 s, FreeRTOS a 1 kHz
├── main/
│   ├── CMakeLists.txt      lista de archivos .c y drivers que se usan
│   ├── idf_component.yml   descarga el driver del LED (led_strip)
│   ├── config.h            TODOS los ajustes del robot
│   ├── robot.h             las funciones públicas de cada módulo
│   ├── main.c              diagnóstico, calibración, máquina de estados, telemetría
│   ├── motors.c            motores (PWM)
│   ├── pixel.c             LED RGB
│   ├── ir.c                sensores IR de piso (ADC)
│   ├── sonar.c             ultrasónico (interrupción + tarea)
│   └── imu.c               acelerómetro y giroscopio (I2C)
(la documentación está en la carpeta docs/ del repositorio)
```

## `config.h` — ajustes

El único archivo que hace falta tocar para afinar el robot. Pines, velocidades, tiempos, umbrales,
distancia de ataque. Cada línea está comentada. Ver [Tácticas y ajustes](05-tacticas.md).

## `motors.c` — motores

El puente H **L9110S** tiene dos entradas por motor (A y B):

| A | B | Resultado |
|---|---|---|
| PWM | 0 | gira hacia adelante, más rápido cuanto más ancho el pulso |
| 0 | PWM | gira hacia atrás |
| 1 | 1 | **freno** |

- Usa el periférico **LEDC** a 50 Hz con 14 bits, igual que la librería `ideaboard.py` de CircuitPython
  (con esa frecuencia se ajustaron las velocidades del código CircuitPython).
- `drive(izq, der)`: velocidades de −1.0 a 1.0. Aplica `MOTOR_FWD` y `MOTOR_LEFT/RIGHT` de `config.h`.
- `motors_brake()`: freno. Es lo primero que se ejecuta al encender.

## `pixel.c` — LED RGB

- El WS2812B recibe colores por un protocolo de pulsos de ~1 µs; el periférico **RMT** los genera.
- `pixel_set(r, g, b)`: aplica el brillo de `config.h` y solo refresca si el color cambió.

## `ir.c` — sensores IR de piso

- Configura el **ADC1** con atenuación de 12 dB (rango hasta ~3.1 V) en IO36, IO39, IO34, IO35.
- Convierte cada lectura a **milivoltios** con la calibración de fábrica del chip
  (`adc_cali_line_fitting`).
- Descarta las primeras 8 lecturas (salen bajas justo después de configurar).
- `ir_read_mv()`: promedia 4 lecturas por sensor.
- `ir_calib_add()` / `ir_calib_finish()`: autocalibración sobre negro (ver [Arquitectura](02-arquitectura.md#autocalibración-de-los-sensores-ir)).
- `ir_white()`: dice qué sensores ven blanco. Un sensor dañado nunca cuenta como blanco.

## `sonar.c` — ultrasónico

- **Tarea propia** en el núcleo 1: dispara TRIG (pulso de 10 µs), espera el eco hasta 40 ms, pausa
  8 ms y repite.
- **Interrupción** en el pin ECHO (ambos flancos): guarda la hora de subida, y al bajar calcula el
  ancho del pulso y despierta a la tarea.
- `sonar_cm()`: última distancia (`ancho_µs / 58`), o −1 si no hubo eco o está a más de 150 cm.
- `sonar_ever_responded()`: para el autodiagnóstico (¿el pin ECHO subió alguna vez?).

## `imu.c` — acelerómetro y giroscopio (opcional)

- Busca el **LSM6DS3TR-C** en I2C (0x6B, o 0x6A en la IdeaSense) y confirma su `WHO_AM_I` (0x6A).
- Lo configura a 416 Hz, ±8 g y ±1000 °/s (los choques y giros de sumo son bruscos).
- **Calibración** (robot quieto): mide el sesgo del giroscopio y detecta **qué eje es vertical**
  por la gravedad. Así funciona sin importar cómo esté montado el módulo.
- `imu_update(dt)`: integra la velocidad de giro → ángulo acumulado (`imu_yaw_deg`), y calcula la
  aceleración horizontal sin gravedad → **choque** si supera 6 m/s².
- Si no está conectada, todo devuelve 0 y el robot usa giros por tiempo.

## `main.c` — el cerebro

| Función | Qué hace |
|---|---|
| `app_main()` | arranque: frena motores, inicia todo, activa el watchdog, diagnóstico, y repite ESPERA → CUENTA → PELEA |
| `self_test()` | LED verde si IMU y ultrasónico responden; amarillo sin IMU; cian sin ultrasónico |
| `wait_for_start()` | LED naranja, imprime sensores cada 0.5 s, arranca al **soltar** BOOT |
| `countdown_and_calibrate()` | 5 s: calibra IR e IMU, avisa sensores dañados |
| `fight()` | ciclo de 200 Hz con la máquina de estados y la telemetría |

### Telemetría CSV

Durante la pelea, cada 50 ms, sale por USB una línea así:

```
T,12345,ATACA,3120,3105,1850,2010,23.4,-87.5,2.1,0
  ms   estado S1   S2   S3   S4  dist  yaw  acc choque
```

Se puede ver con `idf.py monitor` o cualquier terminal serie a 115200 baudios, y guardar para
graficar la pelea.
