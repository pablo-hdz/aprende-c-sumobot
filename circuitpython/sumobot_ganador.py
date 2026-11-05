# Sumobot CENFOTEC 2025 - VERSION GANADORA (sin memoria de ataque)
# Basado en "codigo inicio/code.py" de github.com/Universidad-Cenfotec/Sumobot
# Cambios:
#   - Ultrasónico leído directo (~6 ms) en vez de hcsr04.mpy, que con timeout
#     corto fallaba 1 de cada 2 lecturas y con timeout largo tardaba ~70 ms.
#   - Umbral IR propio para cada sensor (los delanteros dan valores más altos).
#   - Revisa la línea blanca en todo momento, también mientras ataca o busca.
#   - Cuenta regresiva de 5 s después de soltar el botón BOOT.
#   - Si algo falla o se presiona Stop, los motores se detienen siempre.
#
# Para guardarlo en el robot: Thonny -> Guardar como -> Dispositivo CircuitPython -> code.py

import board
import digitalio
import keypad
import random
from time import sleep, monotonic, monotonic_ns
from ideaboard import IdeaBoard

# ------------------------------------------------------------------ AJUSTES
MOTORES = True            # False = prueba en seco: imprime decisiones sin mover motores
DIST_ATAQUE = 60          # cm: si hay algo más cerca que esto, atacar
VEL_ATAQUE = 1.0
VEL_BUSQUEDA = 0.35
VEL_ESCAPE = 0.8
T_RETROCESO = 0.35        # s retrocediendo al ver la línea adelante
T_GIRO = 0.30             # s girando después de retroceder

# Umbrales IR medidos en este robot (BLANCO = valor menor al umbral)
#           S1 adel-izq  S2 adel-der  S3 atras-izq  S4 atras-der
UMBRALES = (50000,       50000,       10000,        10000)

# ------------------------------------------------------------------ HARDWARE
ib = IdeaBoard()
ib.motor_1.throttle = 0
ib.motor_2.throttle = 0

keys = keypad.Keys((board.IO0,), value_when_pressed=False, pull=True)

infrarrojos = (
    ib.AnalogIn(board.IO36),  # S1 adelante izquierdo
    ib.AnalogIn(board.IO39),  # S2 adelante derecho
    ib.AnalogIn(board.IO34),  # S3 atrás izquierdo
    ib.AnalogIn(board.IO35),  # S4 atrás derecho
)

trig = digitalio.DigitalInOut(board.IO25)
trig.direction = digitalio.Direction.OUTPUT
trig.value = False
echo = digitalio.DigitalInOut(board.IO26)
echo.direction = digitalio.Direction.INPUT


# ------------------------------------------------------------------ SENSORES
def distancia_cm(max_cm=150):
    """Distancia en cm, o -1 si no hay eco. Tarda ~3-10 ms."""
    lim = monotonic_ns() + 30_000_000
    while echo.value:
        if monotonic_ns() > lim:
            return -1
    trig.value = True
    trig.value = False
    while not echo.value:
        if monotonic_ns() > lim:
            return -1
    t1 = monotonic_ns()
    lim_pulso = t1 + max_cm * 58_000
    while echo.value:
        if monotonic_ns() > lim_pulso:
            return -1
    return (monotonic_ns() - t1) / 58_000


def leer_linea():
    """Lista [S1, S2, S3, S4] con 1 = blanco (borde), 0 = negro."""
    return [int(s.value < u) for s, u in zip(infrarrojos, UMBRALES)]


# ------------------------------------------------------------------ MOTORES
def motores(izq, der):
    if MOTORES:
        ib.motor_1.throttle = izq
        ib.motor_2.throttle = der


def stop():
    ib.motor_1.throttle = 0
    ib.motor_2.throttle = 0


def mover_vigilando(izq, der, t, color):
    """Mueve por t segundos, pero se detiene antes si los sensores de adelante ven blanco
    mientras va hacia adelante. Devuelve False si tuvo que cortar."""
    ib.pixel = color
    motores(izq, der)
    t0 = monotonic()
    while monotonic() - t0 < t:
        s = leer_linea()
        if (izq > 0 or der > 0) and (s[0] or s[1]):
            return False
    return True


# ------------------------------------------------------------------ CONDUCTA
def escapar_linea(s):
    """s = [S1, S2, S3, S4]. Aleja el robot del borde."""
    fi, fd, ai, ad = s
    if fi or fd:
        print("LINEA ADELANTE", s, "-> retrocede y gira")
        ib.pixel = (255, 0, 0)
        motores(-VEL_ESCAPE, -VEL_ESCAPE)
        sleep(T_RETROCESO)
        if fi and not fd:
            giro = 1           # línea a la izquierda -> gira a la derecha
        elif fd and not fi:
            giro = -1          # línea a la derecha -> gira a la izquierda
        else:
            giro = random.choice((-1, 1))
        motores(giro * VEL_ESCAPE, -giro * VEL_ESCAPE)
        sleep(T_GIRO * (2 if fi and fd else 1))
    elif ai or ad:
        print("LINEA ATRAS", s, "-> adelante a fondo")
        mover_vigilando(1.0, 1.0, 0.25, (255, 150, 0))


def esperar_inicio():
    print("Listo. Presiona y suelta BOOT para empezar.")
    ib.pixel = (255, 100, 0)
    while True:
        ev = keys.events.get()
        if ev and ev.released:
            break
    for i in range(5, 0, -1):   # 5 s reglamentarios
        print("Inicia en", i)
        ib.pixel = (255, 0, 0) if i % 2 else (0, 0, 0)
        sleep(1)
    print("¡A pelear!")


# ------------------------------------------------------------------ PRINCIPAL
def main():
    esperar_inicio()
    ultimo_print = 0
    while True:
        s = leer_linea()
        if any(s):
            escapar_linea(s)
            continue

        d = distancia_cm()
        if 0 < d < DIST_ATAQUE:
            ib.pixel = (0, 255, 0)
            motores(VEL_ATAQUE, VEL_ATAQUE)
            estado = "ATACA"
        else:
            ib.pixel = (0, 0, 255)
            motores(VEL_BUSQUEDA, -VEL_BUSQUEDA)   # gira en su lugar buscando
            estado = "BUSCA"

        if monotonic() - ultimo_print > 0.3:
            ultimo_print = monotonic()
            print("%s  dist=%5.1f  IR=%s" % (estado, d, s))


try:
    main()
finally:
    stop()
    ib.pixel = (0, 0, 0)
    print("Motores detenidos")
