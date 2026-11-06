# 8. Aprende C con el sumobot: ruta de lecciones

Cada lección explica **un concepto de C** y te lleva al **lugar exacto del firmware** donde el robot
lo usa. Al final hay un ejercicio para practicar con tu propio robot.

**Antes de empezar:** lee [1. Qué es C y ESP-IDF](01-que-es-c-y-esp-idf.md) e instala ESP-IDF con
[4. Instalar y cargar](04-instalar-y-flashear.md). Para cada ejercicio: edita, `idf.py build`,
`idf.py -p COMx flash monitor`.

> Los ejercicios son propuestas de práctica: todavía no tienen solución publicada. Si resuelves uno,
> ¡compártelo con un Pull Request! (ver [CONTRIBUTING.md](../CONTRIBUTING.md)).

| # | Concepto de C | Dónde verlo | Nivel |
|---|---|---|---|
| 1 | Constantes con `#define` | [`config.h`](../firmware-c/main/config.h) | 🟢 |
| 2 | Funciones, parámetros y tipos | [`motors.c`](../firmware-c/main/motors.c): `drive()`, `throttle()` | 🟢 |
| 3 | Condicionales `if / else` | [`motors.c`](../firmware-c/main/motors.c): `throttle()` | 🟢 |
| 4 | Arreglos y ciclos `for` | [`ir.c`](../firmware-c/main/ir.c): `ir_read_mv()`, `ir_white()` | 🟢 |
| 5 | `struct` e inicializadores | [`main.c`](../firmware-c/main/main.c) y [`sonar.c`](../firmware-c/main/sonar.c): `gpio_config_t` | 🟡 |
| 6 | `enum` y `switch`: máquinas de estados | [`main.c`](../firmware-c/main/main.c): `state_t`, `fight()` | 🟡 |
| 7 | Módulos: `.h` y `.c`, `static` | [`robot.h`](../firmware-c/main/robot.h) | 🟡 |
| 8 | Punteros | [`imu.c`](../firmware-c/main/imu.c): `read_reg()` | 🔴 |
| 9 | Operaciones de bits | [`imu.c`](../firmware-c/main/imu.c): `imu_raw()`; `1ULL << PIN` en `gpio_config_t` | 🔴 |
| 10 | Tiempo sin bloquear | [`main.c`](../firmware-c/main/main.c): `esp_timer_get_time()`, `vTaskDelayUntil()` | 🔴 |
| 11 | Interrupciones y `volatile` | [`sonar.c`](../firmware-c/main/sonar.c): `echo_isr()` | 🔴 |

---

## Lección 1 · Constantes con `#define`

```c
#define DIST_ATTACK_CM      60      // atacar si hay algo más cerca que esto
#define SPEED_SEARCH        0.35f
```

`#define NOMBRE valor` hace que, **antes de compilar**, cada `NOMBRE` del código se reemplace por
`valor`. No ocupa memoria y permite cambiar un ajuste en un solo lugar. La `f` de `0.35f` indica
que es un `float`.

**Ejercicio:** cambia `DIST_ATTACK_CM` a 40 y observa en la telemetría (`dist`) desde qué
distancia el estado pasa a `ATACA`.

## Lección 2 · Funciones, parámetros y tipos

```c
void drive(float left, float right)
{
    throttle(MOTOR_LEFT, left * MOTOR_FWD);
    throttle(MOTOR_RIGHT, right * MOTOR_FWD);
}
```

- `void` = la función no devuelve nada. `float left` = recibe un número decimal.
- Una función esconde los detalles: `main.c` dice `drive(1, 1)` sin saber nada del puente H.

**Ejercicio:** escribe en `motors.c` una función `void spin(float speed)` que gire en el lugar
(`drive(speed, -speed)`), declárala en `robot.h` y úsala en el estado BUSCA.

## Lección 3 · Condicionales

```c
if (t > 0)      { set_duty(chan_a[m], d);    set_duty(chan_b[m], 0); }
else if (t < 0) { set_duty(chan_a[m], 0);    set_duty(chan_b[m], d); }
else            { set_duty(chan_a[m], MAXD); set_duty(chan_b[m], MAXD); }   // freno
```

Un `if / else if / else` elige **una** de tres ramas: adelante, atrás o freno.

**Ejercicio:** agrega una "zona muerta": si `fabsf(t) < 0.1f`, tratarlo como 0 (freno).

## Lección 4 · Arreglos y ciclos `for`

```c
void ir_white(const int mv[4], bool white[4])
{
    for (int i = 0; i < 4; i++) white[i] = alive[i] && mv[i] < thr[i];
}
```

- `int mv[4]` = 4 enteros seguidos en memoria: `mv[0]` a `mv[3]`, uno por sensor.
- El `for` repite el bloque con `i = 0, 1, 2, 3`.
- `&&` = "y": blanco solo si el sensor está vivo **y** su lectura está bajo el umbral.

**Ejercicio:** escribe `int ir_count_white(const bool white[4])` que devuelva cuántos sensores ven
blanco, e imprímelo en la telemetría.

## Lección 5 · `struct` e inicializadores

```c
gpio_config_t boot = { .pin_bit_mask = 1ULL << PIN_BOOT, .mode = GPIO_MODE_INPUT,
                       .pull_up_en = GPIO_PULLUP_ENABLE };
```

Un `struct` agrupa varios datos bajo un nombre. Con `.campo = valor` se llenan solo los campos que
importan; el resto queda en 0. ESP-IDF configura casi todo así.

**Ejercicio:** busca en `motors.c` el `struct` que configura el temporizador PWM y explica en un
comentario qué hace cada campo.

## Lección 6 · `enum` y `switch`: máquinas de estados

```c
typedef enum { ST_WAIT, ST_COUNTDOWN, ST_SEARCH, ST_ATTACK, ST_PUSH, ST_BACKUP, ST_TURN, ST_FWD_ESCAPE } state_t;
...
switch (st) {
case ST_BACKUP: ...
case ST_TURN:   ...
default:        ...   // BUSCA / ATACA / EMPUJA
}
```

- Un `enum` da nombres a números (`ST_WAIT` = 0, `ST_COUNTDOWN` = 1...). El código se lee como el
  diagrama de [Arquitectura](02-arquitectura.md#máquina-de-estados-de-la-pelea).
- `switch` salta directo al `case` del estado actual. `break` termina el caso.

**Ejercicio:** agrega un estado `ST_DODGE` que al empezar la pelea gire 90° antes de buscar
(una de las [tácticas propuestas](05-tacticas.md#ideas-tácticas-para-probar-no-implementadas)).

## Lección 7 · Módulos: `.h`, `.c` y `static`

`robot.h` **declara** qué funciones existen (`void drive(float left, float right);`). Cada `.c`
las **define**. Lo que lleva `static` (como `throttle()` en `motors.c`) es privado de su archivo:
nadie más puede llamarlo ni romperlo.

**Ejercicio:** crea un módulo nuevo `buzzer.c` con `buzzer_init()` y `beep(ms)` (un zumbador en
IO27, ver [peripherals de CRCibernética](https://github.com/CRCibernetica/espressif-ideaboard/blob/master/skills/ideaboard-esp-idf/references/peripherals.md)),
declara sus funciones en `robot.h`, agrégalo a `main/CMakeLists.txt` y haz que suene en la cuenta
regresiva.

## Lección 8 · Punteros

```c
static esp_err_t read_reg(uint8_t reg, uint8_t *buf, size_t n)
{
    return i2c_master_transmit_receive(dev, &reg, 1, buf, n, 20);
}
```

- `uint8_t *buf` = **dirección de memoria** donde la función debe escribir los bytes que lea.
- `&reg` = "la dirección de `reg`".
- Así una función puede llenar un arreglo del que la llama. `ir_read_mv(int mv[4])` hace lo mismo:
  en C, un arreglo como parámetro es en realidad un puntero a su primer elemento.

**Ejercicio:** explica en un comentario por qué `ir_read_mv()` puede "devolver" 4 valores aunque
su tipo sea `void`.

## Lección 9 · Operaciones de bits

```c
int16_t gr = (int16_t)((d[2 * i + 1] << 8) | d[2 * i]);
```

La IMU manda cada número en **2 bytes** (bajo y alto). `<< 8` corre el byte alto 8 posiciones a la
izquierda y `|` lo junta con el bajo: dos bytes → un entero de 16 bits con signo. En
`1ULL << PIN_BOOT` se usa lo mismo para formar una **máscara** con un 1 en la posición del pin.

**Ejercicio:** si `d[0] = 0x34` y `d[1] = 0x12`, ¿cuánto vale `gr`? (Respuesta: `0x1234` = 4660.)

## Lección 10 · Tiempo sin bloquear

```c
vTaskDelayUntil(&wake, pdMS_TO_TICKS(LOOP_PERIOD_MS));   // ciclo exacto de 5 ms
...
until = now + MS(T_BACKUP_MS);                           // "retroceder hasta este instante"
...
if (now >= until) { ... }                                // ¿ya pasó?
```

En vez de "esperar 350 ms" (y quedar ciego a la línea), se guarda **hasta cuándo** dura la
maniobra y se pregunta en cada ciclo si ya terminó. El robot sigue leyendo sensores mientras tanto.

**Ejercicio:** haz que el LED parpadee en el estado BUSCA usando `now` (por ejemplo, encendido si
`(now / 250000) % 2 == 0`), sin usar `vTaskDelay`.

## Lección 11 · Interrupciones y `volatile`

```c
static volatile int32_t echo_us = -1;

static void echo_isr(void *arg)        // se ejecuta solo cuando el pin ECHO cambia
{
    int64_t now = esp_timer_get_time();
    if (gpio_get_level(PIN_ECHO)) t_rise = now;
    else if (t_rise) { echo_us = (int32_t)(now - t_rise); ... }
}
```

- Una **interrupción** detiene lo que esté haciendo el procesador, ejecuta `echo_isr()` y regresa.
  Por eso mide el eco con precisión de microsegundos sin que el programa lo espere.
- `volatile` le dice al compilador: "esta variable puede cambiar en cualquier momento (la cambia la
  interrupción); léela siempre de memoria".

**Ejercicio:** agrega un contador `volatile uint32_t echo_count` que suba en cada eco recibido e
imprímelo cada segundo. ¿Cuántas mediciones por segundo hace el ultrasónico?

---

## Siguiente paso

Cuando termines la ruta, elige una mejora de [Tácticas](05-tacticas.md) o un pendiente de
[Estado y verificación](06-estado-y-verificacion.md) y propónla como Issue.
