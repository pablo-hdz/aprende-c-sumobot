# Calibración de los 4 sensores IR del Sumobot - correr en Thonny.
# El programa te dice qué hacer en cada fase y mide 4 s con el robot QUIETO.
# Al final calcula un umbral recomendado para cada sensor.

import board
from time import sleep, monotonic
from ideaboard import IdeaBoard

ib = IdeaBoard()
ib.motor_1.throttle = 0
ib.motor_2.throttle = 0

nombres = ("S1 adel-izq", "S2 adel-der", "S3 atras-izq", "S4 atras-der")
pins = (board.IO36, board.IO39, board.IO34, board.IO35)
sens = [ib.AnalogIn(p) for p in pins]


def medir(segundos=4):
    lo = [70000] * 4
    hi = [0] * 4
    t0 = monotonic()
    while monotonic() - t0 < segundos:
        for i, s in enumerate(sens):
            v = s.value
            lo[i] = min(lo[i], v)
            hi[i] = max(hi[i], v)
        sleep(0.01)
    return lo, hi


def aviso(texto):
    print("\n>>> " + texto)
    for c in (3, 2, 1):
        print("    %d..." % c)
        ib.pixel = (255, 120, 0)
        sleep(0.5)
        ib.pixel = (0, 0, 0)
        sleep(0.5)
    print("    midiendo, NO MUEVAS NADA")
    ib.pixel = (0, 0, 255)


print("=== CALIBRACION IR ===")
aviso("Robot sobre NEGRO, sin papel")
negro_lo, negro_hi = medir()
for i in range(4):
    print("    %s negro: %5d - %5d" % (nombres[i], negro_lo[i], negro_hi[i]))

blanco_hi = [0] * 4
blanco_lo = [0] * 4
for i in range(4):
    aviso("Papel BLANCO plano bajo %s (solo esa esquina)" % nombres[i])
    lo, hi = medir()
    blanco_lo[i], blanco_hi[i] = lo[i], hi[i]
    print("    %s blanco: %5d - %5d" % (nombres[i], lo[i], hi[i]))

ib.pixel = (0, 0, 0)
print("\n=== RESUMEN ===")
umbrales = []
for i in range(4):
    margen = negro_lo[i] - blanco_hi[i]
    u = (negro_lo[i] + blanco_hi[i]) // 2
    umbrales.append(u)
    estado = "OK" if margen > 3000 else "POCO MARGEN, revisar"
    print("%s  blanco max %5d | negro min %5d | umbral %5d  %s"
          % (nombres[i], blanco_hi[i], negro_lo[i], u, estado))
print("\nUMBRALES =", umbrales)
print("=== FIN ===")
