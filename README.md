# Aprende C con un Sumobot

**Autor:** Pablo Hernández

Repositorio educativo para **aprender el lenguaje C y software embebido** programando un robot
real: el **Sumobot CENFOTEC 2025** (placa **IdeaBoard de CRCibernética**, ESP32). Cada concepto de
C se explica con el código que hace que el robot vea la línea, busque al rival y lo empuje.

> **Estado:** la versión CircuitPython está probada en robots reales. El firmware en C **compila
> sin errores pero todavía no se ha probado en un robot**. Ver [qué está verificado y qué no](docs/06-estado-y-verificacion.md).

## Descripción

Un sumobot es un robot que pelea en un círculo negro con borde blanco (el *dojo*): gana el que
saca al otro. Es un buen proyecto para aprender C porque todo se ve: un error en un `if` hace que
el robot se salga del dojo.

El repositorio tiene tres partes:

1. **Ruta de aprendizaje de C** ([docs/08-aprende-c.md](docs/08-aprende-c.md)): 11 lecciones, de
   `#define` a interrupciones, cada una señalando el archivo y la función del robot donde se usa, con
   un ejercicio práctico.
2. **Firmware en C** ([`firmware-c/`](firmware-c/)) escrito para enseñar: un archivo por
   componente, comentarios en español y todos los ajustes en un solo lugar. El robot:
   - **no se sale del dojo** (4 sensores infrarrojos de piso),
   - **encuentra y empuja al rival** (sensor ultrasónico por interrupción),
   - **se adapta a cada robot** (autocalibración de sensores al arrancar),
   - **gira por grados y detecta choques** (acelerómetro/giroscopio opcional).
3. **Punto de partida en CircuitPython** ([`circuitpython/`](circuitpython/)): la misma estrategia
   en Python, para comparar ambos lenguajes, y scripts para diagnosticar el robot.

## Problema o necesidad

Aprender C suele empezar con ejercicios abstractos ("imprime los números del 1 al 10") que no
muestran para qué sirven los punteros, los `struct` o las interrupciones. A la vez, el kit del
Sumobot trae ejemplos sueltos en Python que no explican **por qué** pasan las cosas, y al probarlos
aparecen problemas reales:

- la librería del ultrasónico **falla una lectura de cada dos** con tiempos de espera cortos;
- el ejemplo del sensor IR usa un pin (IO33) que **no es el del kit 2025** (IO36/39/34/35);
- **cada robot lee distinto** los sensores, así que un umbral fijo no sirve para todos.

Este repositorio une las dos cosas: aprender C resolviendo esos problemas en un robot de verdad.

## Público objetivo

- **Estudiantes que quieren aprender C** (o pasar de Python a C) con un proyecto concreto.
- **Estudiantes del curso Sumobot CENFOTEC** que quieren entender y mejorar su robot.
- **Docentes y mentores** que buscan material para enseñar software embebido (ESP32, FreeRTOS, PWM,
  ADC, I2C, interrupciones) o para diagnosticar robots del kit.

## Origen

Nació probando un sumobot paso a paso: primero el ultrasónico, después cada componente, luego la
estrategia de pelea. Cada error encontrado (sensor en el pin equivocado, librería lenta, umbrales
distintos por robot, un robot con los sensores sin corriente) se convirtió en una prueba o una
mejora. Con la versión CircuitPython funcionando en el dojo surgió la pregunta: *¿qué tan lejos se
puede llevar esta placa programándola en C?* La historia completa está en
[docs/07-historia.md](docs/07-historia.md).

## Estado actual

| Parte | Estado |
|---|---|
| [`circuitpython/sumobot_umbrales_fijos.py`](circuitpython/sumobot_umbrales_fijos.py) | ✅ probado en el robot del autor (umbrales fijos de ese robot) |
| [`circuitpython/sumobot_base.py`](circuitpython/sumobot_base.py) | ⚠️ cargado y probado sin motores en un segundo robot; falta pelea real |
| [`circuitpython/pruebas/`](circuitpython/pruebas/) | ✅ scripts de diagnóstico usados en el robot del autor |
| [`firmware-c/`](firmware-c/) | ⚠️ compila sin errores (ESP-IDF v6.1); **no flasheado ni probado en robot** |
| [`docs/`](docs/) | ✅ teoría de C/ESP-IDF, arquitectura, módulos, instalación, tácticas, historia |
| [`docs/08-aprende-c.md`](docs/08-aprende-c.md) | ✅ 11 lecciones con ejercicios · ⏳ los ejercicios aún no tienen soluciones publicadas |

## Estructura del repositorio

```
sumobot-ideaboard/
├── README.md              este archivo
├── CONTRIBUTING.md        cómo colaborar
├── LICENSE                dominio público (The Unlicense)
├── circuitpython/         código para Thonny (lo que trae el kit)
│   ├── sumobot_umbrales_fijos.py  versión con umbrales fijos
│   ├── sumobot_base.py    versión con autocalibración para cualquier sumo
│   └── pruebas/           diagnóstico de componentes, IR y ultrasónico
├── practicas-aplicables/  prácticas con la IdeaBoard: conexión, programa, fotos y video
├── firmware-c/            firmware nativo en C (ESP-IDF)
│   └── main/              un archivo .c por componente + config.h con todos los ajustes
└── docs/                  documentación teórica y práctica
    ├── 08-aprende-c.md    ruta de lecciones de C con ejercicios
    ├── mapa-de-comunidad.md, plan-de-sostenibilidad.md, retroalimentacion.md
    └── 01..07             teoría de C, arquitectura, módulos, instalación, tácticas, estado, historia
```

## Cómo comenzar

Elige tu camino según lo que quieras hacer:

| Quiero... | Empieza aquí |
|---|---|
| **Nunca he usado C: instalar todo y prender el LED de la IdeaBoard** | Sigue la [guía paso a paso](docs/guia-correr-c.md) y el ejemplo [`practicas-aplicables/01-led-rgb`](practicas-aplicables/01-led-rgb/) |
| **Practicar C con la IdeaBoard y módulos de sensores** | Sigue las [prácticas aplicables](practicas-aplicables/) en orden: LED RGB, pulsador, buzzer... (cada una con conexión, programa, fotos y video) |
| **Aprender C desde cero con el robot** | Lee [docs/01](docs/01-que-es-c-y-esp-idf.md) y sigue la [ruta de lecciones](docs/08-aprende-c.md) en orden |
| **Que mi sumo pelee hoy** (sin instalar nada nuevo) | Abre [`circuitpython/sumobot_base.py`](circuitpython/sumobot_base.py) en Thonny y guárdalo en el robot como `code.py`. Instrucciones al inicio del archivo |
| **Saber si mi robot tiene algo dañado** | Corre [`circuitpython/pruebas/prueba_componentes.py`](circuitpython/pruebas/prueba_componentes.py) y [`calibrar_ir.py`](circuitpython/pruebas/calibrar_ir.py) en Thonny |
| **Entender cómo funciona en C** | Lee [docs/01](docs/01-que-es-c-y-esp-idf.md) → [docs/02](docs/02-arquitectura.md) → [docs/03](docs/03-modulos.md) |
| **Compilar y cargar el firmware en C** | Sigue [docs/04-instalar-y-flashear.md](docs/04-instalar-y-flashear.md) (⚠️ borra CircuitPython de la placa) |
| **Mejorar la estrategia** | Lee [docs/05-tacticas.md](docs/05-tacticas.md) y cambia valores en [`firmware-c/main/config.h`](firmware-c/main/config.h) |
| **Colaborar** | Lee [CONTRIBUTING.md](CONTRIBUTING.md) y revisa los Issues abiertos |

Necesitas: un robot del kit Sumobot CENFOTEC 2025, cable USB, 4 baterías AA y
[Thonny](https://thonny.org) (CircuitPython) o [ESP-IDF v6.1](docs/04-instalar-y-flashear.md) (C).

## Comunidad

Este proyecto está pensado para que otras personas lo **usen, lo mejoren y lo continúen sin depender del autor**:

- [**Mapa de comunidad**](docs/mapa-de-comunidad.md): dominio y trayectorias de participación (explorar → probar → aprender → contribuir → mentorear → mantener).
- [**Plan de sostenibilidad**](docs/plan-de-sostenibilidad.md): roles, cómo se toman decisiones y cómo puede crecer.
- [**Registro de retroalimentación**](docs/retroalimentacion.md): qué comentarios se recibieron y qué se cambió.
- [**Historia del proyecto**](docs/07-historia.md): cada prueba, error y corrección, convertidos en recursos reutilizables.

**¿Qué necesita este proyecto para que otra persona lo comprenda, lo use y lo mejore sin depender de mí?**
Documentación que explique el *por qué* y no solo el *cómo*, una lista honesta de lo que falta probar,
código dividido en piezas pequeñas que se entienden por separado, una puerta de entrada para cada nivel
y una licencia libre que permita continuarlo. Eso es lo que este repositorio intenta dar.

## Próximos pasos

- [ ] Probar el firmware en C en un robot real (autodiagnóstico, sensores, motores, pelea).
- [ ] Publicar soluciones de los ejercicios de la ruta de C (carpeta `soluciones/`).
- [ ] Medir el umbral de choque de la IMU y probar los giros por grados.
- [ ] Agregar fotos del robot y del cableado.
- [ ] Graficar la telemetría de una pelea para afinar la estrategia.
- [ ] Probar tácticas nuevas: esquivar al inicio, búsqueda en espiral, detector de empuje.
- [ ] Agregar un divisor de voltaje al ECHO del ultrasónico.

## Créditos y referencias

- Drivers de motores, LED, ADC, I2C e IMU adaptados de los ejemplos de **CRCibernética**,
  [espressif-ideaboard](https://github.com/CRCibernetica/espressif-ideaboard) (publicado como MIT por CRCibernética).
- Conexiones del kit tomadas como referencia de
  [Universidad-Cenfotec/Sumobot](https://github.com/Universidad-Cenfotec/Sumobot). No se copiaron
  archivos ni imágenes de ese repositorio.
- Placa IdeaBoard: [CRCibernética](https://www.crcibernetica.com/crcibernetica-ideaboard/).

## Uso de inteligencia artificial

Este proyecto se desarrolló con apoyo de **Claude** (Anthropic), usado en Claude Code:

- **Qué hizo la IA:** propuso y escribió los scripts de prueba, el código CircuitPython y el
  firmware en C; corrió las pruebas por el puerto serie en las placas conectadas; investigó la
  documentación de la IdeaBoard y del ESP32; y redactó esta documentación.
- **Qué falta verificar:** todo lo marcado como pendiente en
  [docs/06-estado-y-verificacion.md](docs/06-estado-y-verificacion.md), en especial el firmware en C
  en un robot real.

## Licencia

Proyecto **libre, de dominio público** ([The Unlicense](LICENSE)): puedes usarlo, copiarlo, modificarlo y
compartirlo sin pedir permiso ni dar crédito. Las partes adaptadas de CRCibernética conservan su
licencia MIT original (ver el final de [LICENSE](LICENSE)).
