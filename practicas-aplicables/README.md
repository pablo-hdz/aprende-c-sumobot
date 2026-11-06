# Prácticas aplicables con la IdeaBoard

Prácticas cortas para aprender C con la **IdeaBoard** y los módulos de sensores de los kits
(Keyestudio, OSOYOO). Primero se domina la placa sola; después se pasa al
[firmware del sumobot](../firmware-c/). Si nunca has usado C, empieza por la
[guía para correr C](../docs/guia-correr-c.md).

## Cómo está documentada cada práctica

Todas siguen el mismo formato, para que cualquiera pueda hacerlas sin ayuda:

| Sección | Qué tiene |
|---|---|
| 1. Explicación | cómo funciona el módulo y qué concepto de C se practica |
| 2. Materiales | lista exacta, con foto del módulo |
| 3. Conexión | tabla pin a pin y foto del cableado |
| 4. El programa | las partes importantes del código, explicadas |
| 5. Cómo correr el programa | pasos en VS Code, con captura |
| 6. Resultado esperado | lo que sale en el Monitor, imágenes y video |
| 7. Retos | ejercicios para modificar el código |
| 8. Estado y créditos | si se probó en una placa real y de dónde vienen las cosas |

## Prácticas

| # | Práctica | Necesitas | Aprendes | Estado |
|---|---|---|---|---|
| 01 | [LED RGB](01-led-rgb/) | solo la placa | primer programa, funciones, `while` | ✅ probado · 📷 foto/video pendientes |
| 02 | [Pulsador digital](02-pulsador-digital/) | Digital Push Button Module + 3 cables | entradas digitales, anti-rebote, flancos | ✅ probado · 📷 fotos y 🎥 video |
| 03 | Melodías con buzzer | buzzer pasivo | PWM, arreglos de notas | 💡 próxima |
| 04 | Luz y perilla | fotoresistencia / potenciómetro | ADC (entrada analógica) | 💡 pendiente |
| 05 | Alarma de inclinación | inclinación / Hall / reed | interrupciones, `volatile` | 💡 pendiente |
| 06 | Termómetro | DHT11 | protocolo de 1 cable, tiempos en µs | 💡 pendiente |
| 07 | Control remoto | receptor IR + control | periférico RMT, `switch` | 💡 pendiente |

## Crear una práctica nueva

1. Copia la carpeta de una práctica (por ejemplo `02-pulsador-digital`) y cámbiale el nombre.
2. Cambia el nombre del proyecto en su `CMakeLists.txt` y escribe tu código en `main/main.c`.
3. Copia el `README.md` y llena las 8 secciones. Pon tus fotos y video en `img/`.
4. Pruébala en una placa real antes de marcarla como ✅.
5. Compártela con un Pull Request (ver [CONTRIBUTING](../CONTRIBUTING.md)).
