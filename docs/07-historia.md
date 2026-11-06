# 7. Historia del proyecto: qué se probó, qué falló y qué se aprendió

El firmware en C no salió de la nada: es el resultado de probar el sumobot paso a paso, encontrar
errores y corregirlos. Este registro sirve para que nadie tenga que volver a descubrir lo mismo.

| # | Iteración | Qué pasó | Qué se aprendió / cambió |
|---|---|---|---|
| 1 | Probar el ultrasónico con distintos timeouts (CircuitPython) | Con `timeout_sec` < 0.1 s, **una lectura sí y una no** daba −1. Con 0.1 s funcionaba pero tardaba ~70 ms | Se midió el pulso ECHO directo con `digitalio`: el sensor estaba bien, el problema era la librería `hcsr04.mpy`. Se escribió una lectura directa de ~6 ms ([ultrasonico_rapido.py](../circuitpython/pruebas/ultrasonico_rapido.py)) |
| 2 | Probar todos los componentes | LED y motores bien. El IR en IO33 y el acelerómetro no respondían | El código de ejemplo usaba IO33, pero el kit 2025 tiene **4 IR en IO36/39/34/35**. El acelerómetro necesita el cable Qwiic |
| 3 | Calibrar los 4 IR | Los traseros detectaban blanco perfecto (~2850); los delanteros solo bajaban a ~30000 | **Un umbral por sensor**: 50000 adelante, 10000 atrás ([calibrar_ir.py](../circuitpython/pruebas/calibrar_ir.py)) |
| 4 | Primer código de pelea | Funcionó; luego quedó quieto en rojo sobre la línea | Probablemente baterías bajas (sospecha del autor): el LED mostraba que el código corría, pero los motores no se movían |
| 5 | "Memoria de ataque" de 600 ms para no soltar al rival | Una pelea bien, otra **se cayó del dojo** | Al perder al rival seguía a fondo hacia el borde. Se volvió a la versión **sin memoria** ([sumobot_umbrales_fijos.py](../circuitpython/sumobot_umbrales_fijos.py)) |
| 6 | Cargar el código a otros sumos | Cada robot leía distinto; uno tenía los **4 IR en 0 V** incluso en el aire | Causa probable (sin confirmar): falta 3V3/GND en el conector de sensores. Nació la **autocalibración** con detección de sensores dañados ([sumobot_base.py](../circuitpython/sumobot_base.py)) |
| 7 | Investigar la placa a fondo | El rango del ADC (2819 a 62238) parecía raro | Sale exacto del código de CircuitPython: offset de 142 mV y saturación a 3134 mV |
| 8 | Pasar a C con ESP-IDF | Compila sin errores | Ultrasónico por interrupción, ciclo fijo de 200 Hz, watchdog, IMU opcional. **Falta probarlo en un robot** |

## Recursos reutilizables que dejó cada iteración

- [`circuitpython/pruebas/`](../circuitpython/pruebas/): scripts para diagnosticar un sumo en minutos.
- [`circuitpython/sumobot_base.py`](../circuitpython/sumobot_base.py): versión que sirve para cualquier sumo del kit.
- [`firmware-c/`](../firmware-c/): la misma estrategia en C, modular y documentada.
- Esta tabla: los errores típicos del kit y su causa.
