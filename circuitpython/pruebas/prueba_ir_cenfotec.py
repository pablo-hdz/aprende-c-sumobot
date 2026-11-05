# Prueba de los 4 sensores IR del Sumobot 2025 (según el repo Universidad-Cenfotec/Sumobot)
# Los sensores están integrados en la placa base (PCB) y son ANALÓGICOS:
#   BLANCO -> valor BAJO (< ~4000)    NEGRO -> valor ALTO
# Correr en Thonny. Robot apoyado sobre las ruedas, sobre la superficie NEGRA.
# Luego poné un papel BLANCO debajo de cada esquina, una por una.

import board
from time import sleep, monotonic
from ideaboard import IdeaBoard

ib = IdeaBoard()
ib.motor_1.throttle = 0
ib.motor_2.throttle = 0

UMBRAL = 4000   # mismo valor que usa code_diagnostic.py del repo

sensores = (
    ("S1 adel-izq IO36", ib.AnalogIn(board.IO36)),
    ("S2 adel-der IO39", ib.AnalogIn(board.IO39)),
    ("S3 atras-izq IO34", ib.AnalogIn(board.IO34)),
    ("S4 atras-der IO35", ib.AnalogIn(board.IO35)),
)


def leer(sen, n=10):
    return sum(sen.value for _ in range(n)) // n


print("=== PRUEBA IR CENFOTEC (30 s) ===")
print("Robot sobre NEGRO. Luego pone papel BLANCO bajo cada esquina, una por una.\n")
sleep(2)
t0 = monotonic()
while monotonic() - t0 < 30:
    vals = [leer(s) for _, s in sensores]
    blancos = [v < UMBRAL for v in vals]
    linea = "  ".join("%s=%5d %s" % (n.split()[0], v, "BLANCO" if b else "negro ")
                      for (n, _), v, b in zip(sensores, vals, blancos))
    print("%4.1f s  %s" % (monotonic() - t0, linea))
    ib.pixel = (255, 255, 255) if any(blancos) else (0, 0, 40)
    sleep(0.5)
ib.pixel = (0, 0, 0)
print("\nS1=adelante-izq  S2=adelante-der  S3=atras-izq  S4=atras-der")
print("=== FIN ===")
