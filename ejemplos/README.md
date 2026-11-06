# Ejemplos de C para la IdeaBoard

Primero se aprende con la **IdeaBoard sola** y módulos de 3 cables; después se pasa al
[firmware del sumobot](../firmware-c/). Si nunca has usado C, empieza por la
[guía para correr C](../docs/guia-correr-c.md).

| # | Ejemplo | Necesitas | Aprendes | Estado |
|---|---|---|---|---|
| 01 | [LED RGB](01-rgb-ideaboard/) | solo la placa | primer programa, funciones, `while` | ✅ probado |
| 02 | [Botón o sensor táctil](02-boton-tactil/) | módulo digital + 3 cables | entradas digitales, anti-rebote, flancos | ✅ probado |
| 03 | Melodías con buzzer | buzzer pasivo | PWM, arreglos de notas | 💡 próximo |
| 04 | Luz y perilla | fotoresistencia / potenciómetro | ADC (entrada analógica) | 💡 pendiente |
| 05 | Alarma de inclinación | inclinación / Hall / reed | interrupciones, `volatile` | 💡 pendiente |
| 06 | Termómetro | DHT11 | protocolo de 1 cable, tiempos en µs | 💡 pendiente |
| 07 | Control remoto | receptor IR + control | periférico RMT, `switch` | 💡 pendiente |

Todos usan la misma estructura de proyecto ESP-IDF: `CMakeLists.txt`, `sdkconfig.defaults` y
`main/main.c`. Para crear uno nuevo, copia una carpeta y cambia `main.c`.

¿Hiciste uno de los pendientes? Compártelo con un Pull Request (ver [CONTRIBUTING](../CONTRIBUTING.md)).
