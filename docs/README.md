# Documentación: aprende C con un Sumobot

Guía teórica y práctica del firmware nativo en C para el **Sumobot CENFOTEC 2025** con la placa
**IdeaBoard de CRCibernética** (ESP32).

| # | Documento | Para qué sirve |
|---|---|---|
| 0 | [**Guía: cómo correr C**](guia-correr-c.md) | Qué es C, instalar todo, primer programa en la PC y el LED RGB en la IdeaBoard |
| 1 | [Qué es C y ESP-IDF](01-que-es-c-y-esp-idf.md) | Conceptos base: lenguaje C, microcontrolador, FreeRTOS, compilar y flashear |
| 2 | [Arquitectura del sistema](02-arquitectura.md) | Cómo está organizado el programa y cómo fluye la información |
| 3 | [Módulos, archivo por archivo](03-modulos.md) | Qué hace cada archivo `.c` y sus funciones principales |
| 4 | [Instalar y cargar en un sumobot](04-instalar-y-flashear.md) | Pasos para compilar, flashear, usar y volver a CircuitPython |
| 5 | [Tácticas y ajustes](05-tacticas.md) | Estrategia de pelea, parámetros de `config.h` y cómo afinarlos |
| 6 | [Estado y qué falta verificar](06-estado-y-verificacion.md) | Qué está probado, qué no, y de dónde salen los datos |
| 7 | [Historia del proyecto](07-historia.md) | Qué se probó, qué falló y qué se aprendió en cada iteración |
| 8 | [**Aprende C con el sumobot**](08-aprende-c.md) | 11 lecciones de C, cada una en el código real del robot, con ejercicios |
| – | [Glosario](glosario.md) | Términos técnicos explicados en una línea (ADC, PWM, I2C, watchdog...) |

## Comunidad

| Documento | Para qué sirve |
|---|---|
| [Mapa de comunidad](mapa-de-comunidad.md) | Dominio, perfiles y trayectorias de participación |
| [Plan de sostenibilidad](plan-de-sostenibilidad.md) | Roles, decisiones y cómo crecer sin el autor |
| [Registro de retroalimentación](retroalimentacion.md) | Comentarios recibidos y cambios hechos |

## En una frase

El robot repite 200 veces por segundo: **leer sensores → decidir → mover motores**. Revisa
primero la línea blanca (no salirse del dojo), después busca al rival con el ultrasónico y lo
empuja. Todo se ajusta en un solo archivo: [`main/config.h`](../firmware-c/main/config.h).

## Hardware del kit (resumen)

| Pieza | Conexión |
|---|---|
| Placa | IdeaBoard: ESP32-WROOM-32E, 8 MB flash, USB por CH340G |
| Motores | Puente H L9110S: motor 1 en IO12/IO14, motor 2 en IO13/IO15 |
| LED RGB | WS2812B en IO2 |
| 4 sensores IR de piso | IO36 (adelante-izq), IO39 (adelante-der), IO34 (atrás-izq), IO35 (atrás-der) |
| Ultrasónico HC-SR05 | TRIG en IO25, ECHO en IO26 |
| IMU LSM6DS3TR-C (opcional) | I2C por cable Qwiic, SDA IO21, SCL IO22, dirección 0x6B |
| Botón | BOOT en IO0 |
