# Glosario

Términos que aparecen en la documentación y en el código, explicados en una línea.

| Término | Qué significa | Dónde aparece |
|---|---|---|
| **ADC** | Convertidor analógico-digital: transforma un voltaje en un número | [`ir.c`](../firmware-c/main/ir.c) (sensores IR) |
| **Atenuación (12 dB)** | Ajuste del ADC para poder medir voltajes de hasta ~3.1 V | `ir_init()` |
| **Bootloader** | Pequeño programa que arranca primero y carga el firmware | al flashear |
| **CH340** | Chip que convierte USB a serie en la IdeaBoard; crea el puerto `COMx` | [04-instalar](04-instalar-y-flashear.md) |
| **CircuitPython** | Versión de Python para microcontroladores; lo que trae el kit de fábrica | [`circuitpython/`](../circuitpython/) |
| **Compilar** | Traducir el código C a instrucciones de máquina (`idf.py build`) | [01-que-es-c](01-que-es-c-y-esp-idf.md) |
| **Dojo** | El círculo negro con borde blanco donde pelean los sumobots | todo el proyecto |
| **ESP32** | El microcontrolador de la IdeaBoard: 2 núcleos, WiFi y Bluetooth | [01-que-es-c](01-que-es-c-y-esp-idf.md) |
| **ESP-IDF** | Kit oficial de Espressif para programar el ESP32 en C | [01-que-es-c](01-que-es-c-y-esp-idf.md) |
| **Firmware** | El programa que vive en la memoria de la placa y corre al encender | [`firmware-c/`](../firmware-c/) |
| **Flashear** | Escribir el firmware en la memoria flash de la placa (`idf.py flash`) | [04-instalar](04-instalar-y-flashear.md) |
| **FreeRTOS** | Sistema operativo de tiempo real que permite tener varias tareas | [02-arquitectura](02-arquitectura.md) |
| **GPIO** | Pin de entrada/salida digital (0 o 1) | botón BOOT, ultrasónico |
| **I2C** | Bus de 2 cables (SDA, SCL) para hablar con sensores como la IMU | [`imu.c`](../firmware-c/main/imu.c) |
| **IMU** | Acelerómetro + giroscopio: mide aceleración y velocidad de giro | [`imu.c`](../firmware-c/main/imu.c) |
| **Interrupción (ISR)** | Código que se ejecuta al instante cuando cambia un pin | [`sonar.c`](../firmware-c/main/sonar.c): `echo_isr()` |
| **IR (sensor infrarrojo)** | Emite luz infrarroja y mide cuánta rebota: blanco refleja mucho, negro poco | [`ir.c`](../firmware-c/main/ir.c) |
| **L9110S** | Puente H: el chip que deja que los motores giren en ambos sentidos | [`motors.c`](../firmware-c/main/motors.c) |
| **LEDC** | Periférico del ESP32 que genera señales PWM | [`motors.c`](../firmware-c/main/motors.c) |
| **Máquina de estados** | El robot está siempre en un estado (BUSCA, ATACA...) y cambia según reglas | [`main.c`](../firmware-c/main/main.c) |
| **Puntero** | Variable que guarda una dirección de memoria | [lección 8](08-aprende-c.md#lección-8--punteros) |
| **PWM** | Pulsos de ancho variable; más ancho = el motor recibe más potencia | [`motors.c`](../firmware-c/main/motors.c) |
| **Qwiic / STEMMA QT** | Conector de 4 cables para sensores I2C | IMU |
| **RMT** | Periférico que genera pulsos con tiempos exactos; maneja el LED | [`pixel.c`](../firmware-c/main/pixel.c) |
| **Telemetría** | Datos que el robot envía mientras funciona (aquí, líneas CSV por USB) | [03-modulos](03-modulos.md#telemetría-csv) |
| **Umbral** | Valor que separa "blanco" de "negro" en un sensor IR | autocalibración |
| **Watchdog** | Vigilante que reinicia la placa si el programa se cuelga | `sdkconfig.defaults`, `main.c` |
| **WS2812B** | LED RGB "inteligente" que recibe el color por un solo cable | [`pixel.c`](../firmware-c/main/pixel.c) |
