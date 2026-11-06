# Código CircuitPython (Thonny)

Para robots con el CircuitPython de fábrica del kit. No requiere instalar nada además de
[Thonny](https://thonny.org) (intérprete: **CircuitPython (genérico)**, puerto `USB-SERIAL CH340`).

| Archivo | Para qué |
|---|---|
| [`sumobot_base.py`](sumobot_base.py) | **Recomendado.** Estrategia base + autocalibración de los IR al arrancar. Sirve para cualquier sumo del kit |
| [`sumobot_umbrales_fijos.py`](sumobot_umbrales_fijos.py) | La misma estrategia con umbrales fijos medidos en el robot del autor |
| [`pruebas/prueba_componentes.py`](pruebas/prueba_componentes.py) | Prueba LED, motores, IR, IMU y ultrasónico (descomentar la prueba deseada al final) |
| [`pruebas/calibrar_ir.py`](pruebas/calibrar_ir.py) | Mide cada sensor IR sobre negro y blanco y calcula su umbral |
| [`pruebas/prueba_ir_cenfotec.py`](pruebas/prueba_ir_cenfotec.py) | Muestra los 4 IR en vivo durante 30 s |
| [`pruebas/ultrasonico_rapido.py`](pruebas/ultrasonico_rapido.py) | Lectura del ultrasónico sin `hcsr04.mpy` (~6 ms en vez de ~70 ms) |

## Cargar en el robot

1. Abre el archivo en Thonny.
2. **Archivo → Guardar como → Dispositivo CircuitPython → `code.py`** (reemplaza el actual).
3. Con `sumobot_base.py`: robot sobre **negro** dentro del dojo → LED naranja → presionar y soltar
   **BOOT** → 5 s sin tocarlo (calibra) → pelea.

Asegúrate de que `code.py` tenga **un solo programa**: si pegas dos, solo corre el primero.
