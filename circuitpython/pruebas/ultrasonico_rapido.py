# Lectura rápida del HC-SR04 sin la librería hcsr04.mpy
# (esa librería falla 1 de cada 2 lecturas con timeout_sec < 0.1).
# Cada lectura tarda ~3-8 ms en vez de ~70 ms.
# Para usarla en sumobot_corregido.py: copiá la función distancia_cm()
# y los objetos trig/echo, y reemplazá sonar.dist_cm() por distancia_cm().

import board, digitalio
from time import sleep, monotonic_ns
from ideaboard import IdeaBoard

ib = IdeaBoard()
ib.motor_1.throttle = 0
ib.motor_2.throttle = 0

trig = digitalio.DigitalInOut(board.IO25)
trig.direction = digitalio.Direction.OUTPUT
trig.value = False
echo = digitalio.DigitalInOut(board.IO26)
echo.direction = digitalio.Direction.INPUT


def distancia_cm(max_cm=200):
    """Devuelve la distancia en cm, o -1 si no hubo eco."""
    lim = monotonic_ns() + 30_000_000          # nunca más de 30 ms en total
    while echo.value:                          # esperar que termine un eco viejo
        if monotonic_ns() > lim:
            return -1
    trig.value = True
    trig.value = False
    while not echo.value:                      # esperar que empiece el eco
        if monotonic_ns() > lim:
            return -1
    t1 = monotonic_ns()
    lim_pulso = t1 + max_cm * 58_000          # 58 us por cm (ida y vuelta)
    while echo.value:
        if monotonic_ns() > lim_pulso:
            return -1                          # objeto más lejos que max_cm
    return (monotonic_ns() - t1) / 58_000


if __name__ == "__main__":
    print("=== ULTRASONICO RAPIDO (TRIG=IO25, ECHO=IO26) ===")
    while True:
        t0 = monotonic_ns()
        d = distancia_cm()
        ms = (monotonic_ns() - t0) / 1e6
        print("%6.1f cm  (%.1f ms)" % (d, ms) if d != -1 else "   -1 (sin eco)")
        ib.pixel = (0, 255, 0) if 0 < d < 75 else (0, 0, 255)
        sleep(0.06)
