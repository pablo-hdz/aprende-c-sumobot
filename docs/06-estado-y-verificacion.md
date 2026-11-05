# 6. Estado y qué falta verificar

Este documento separa lo que **se comprobó** de lo que **todavía no**. Si pruebas algo de la lista
"pendiente", abre un Issue con tus resultados.

## Comprobado

| Qué | Cómo se comprobó |
|---|---|
| El firmware en C **compila** sin errores con ESP-IDF v6.1 | `idf.py build` → `Project build complete`, binario de 0x33390 bytes (≈205 KB, 86 % de la partición libre) |
| Pines del kit: IR en IO36/39/34/35, ultrasónico TRIG IO25 / ECHO IO26 | medido en placas reales con CircuitPython 9.2.4 |
| En los IR, **blanco = voltaje bajo** (~140–200 mV) y **negro = alto** | medido en el robot del autor (blanco ~2850, negro 12000–62238 en unidades de CircuitPython) |
| Cada robot lee distinto (adelante: 1100–3130 mV sobre negro) | medido en 2 robots; por eso existe la autocalibración |
| Un robot leyó los 4 IR en 0 V incluso en el aire | medido. Causa **probable, no confirmada**: falta 3V3/GND en el conector de sensores (según la guía de conexiones de Cenfotec) |
| La librería `hcsr04.mpy` falla 1 de cada 2 lecturas con `timeout_sec` < 0.1 s | medido; leer el pin directo da ~6 ms por lectura |
| La estrategia base **ganó una pelea** | versión CircuitPython [`sumobot_ganador.py`](../circuitpython/sumobot_ganador.py), reportado por el autor |
| Umbrales 50000/10000 (CircuitPython) = 2518/503 mV | conversión `mV = valor × 3300 / 65535` |

## Pendiente (no probado todavía)

- [ ] **Flashear el firmware en C en un robot real** y ver el autodiagnóstico.
- [ ] Que la autocalibración de los IR calcule umbrales que detecten bien el blanco en el dojo.
- [ ] Que los motores a **50 Hz con 14 bits** en C se comporten igual que en CircuitPython.
- [ ] Que el **ultrasónico por interrupción** mida igual que la versión directa en CircuitPython.
- [ ] La IMU: dirección 0x6B, giros por grados y detección de choques (umbral 6 m/s² sin medir).
- [ ] Que el watchdog reinicie la placa si el programa se cuelga.
- [ ] Una pelea completa con el firmware en C.
- [ ] Conectar un divisor de voltaje al ECHO del ultrasónico (con SELECT–Vin puede llegar a ~6 V
      a un pin de 3.3 V).

## De dónde sale cada dato

- Pines, voltajes e IC de la placa: repositorio oficial de CRCibernética
  [espressif-ideaboard](https://github.com/CRCibernetica/espressif-ideaboard) (licencia MIT).
- Conexiones del kit 2025: repositorio de la Universidad Cenfotec
  [Universidad-Cenfotec/Sumobot](https://github.com/Universidad-Cenfotec/Sumobot) (solo como
  referencia; no se copiaron archivos ni imágenes).
- Mediciones: hechas por el autor en robots del kit durante el desarrollo.
