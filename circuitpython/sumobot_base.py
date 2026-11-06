# Sumobot CENFOTEC 2025 - CODIGO BASE (estrategia base + autocalibración)
# Sirve para cualquier sumo del kit 2025 sin tocar nada.
#
# Cómo se usa:
#   1. Poner el robot DENTRO del dojo, sobre el NEGRO.
#   2. Encender. LED naranja = esperando.
#   3. Presionar y soltar BOOT. Durante la cuenta de 5 s el robot mide el negro
#      con cada sensor IR y calcula sus propios umbrales (NO moverlo).
#   4. Si un sensor IR está dañado o sin corriente, lo ignora y avisa con LED morado.
#
# LED en pelea: azul = busca | verde = ataca | rojo = línea adelante | naranja = línea atrás
#
# Guardar en el robot: Thonny -> Guardar como -> Dispositivo CircuitPython -> code.py

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

# Autocalibración (valores probados en pelea real)
NEGRO_SATURADO = 55000    # si el sensor marca más que esto sobre negro...
UMBRAL_SATURADO = 50000   # ...se usa este umbral (sensores delanteros)
FACTOR_ADELANTE = 0.75    # si no, umbral = negro * 0.75 (S1, S2: reaccionan menos al blanco)
FACTOR_ATRAS = 0.5        # umbral = negro * 0.5 (S3, S4: blanco ~2800)
UMBRAL_MINIMO = 6000      # blanco real en estos sensores es ~2800-4000
SENSOR_MUERTO = 6000      # si sobre negro marca menos que esto, el sensor no sirve

# ------------------------------------------------------------------ HARDWARE
ib = IdeaBoard()
ib.motor_1.throttle = 0
ib.motor_2.throttle = 0

keys = keypad.Keys((board.IO0,), value_when_pressed=False, pull=True)

NOMBRES = ("S1 adel-izq", "S2 adel-der", "S3 atras-izq", "S4 atras-der")
infrarrojos = (
    ib.AnalogIn(board.IO36),  # S1 adelante izquierdo
    ib.AnalogIn(board.IO39),  # S2 adelante derecho
    ib.AnalogIn(board.IO34),  # S3 atrás izquierdo
    ib.AnalogIn(board.IO35),  # S4 atrás derecho
)
umbrales = [50000, 50000, 10000, 10000]   # se reemplazan al calibrar
activos = [True, True, True, True]

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
    """Lista [S1, S2, S3, S4] con 1 = blanco (borde), 0 = negro. Sensores dañados = 0."""
    return [int(a and s.value < u) for s, u, a in zip(infrarrojos, umbrales, activos)]


def calibrar(segundos):
    """Mide el negro con cada sensor y calcula su umbral. El robot debe estar quieto."""
    suma = [0, 0, 0, 0]
    n = 0
    t0 = monotonic()
    while monotonic() - t0 < segundos:
        for i, s in enumerate(infrarrojos):
            suma[i] += s.value
        n += 1
        restante = segundos - (monotonic() - t0)
        ib.pixel = (255, 0, 0) if int(restante) % 2 else (40, 0, 0)
        sleep(0.02)
    hay_muerto = False
    for i in range(4):
        negro = suma[i] // n
        if negro < SENSOR_MUERTO:
            activos[i] = False
            hay_muerto = True
            print("%s: negro=%5d -> DAÑADO o sin corriente, se ignora" % (NOMBRES[i], negro))
        elif negro > NEGRO_SATURADO:
            umbrales[i] = UMBRAL_SATURADO
            print("%s: negro=%5d -> umbral %d" % (NOMBRES[i], negro, umbrales[i]))
        else:
            factor = FACTOR_ADELANTE if i < 2 else FACTOR_ATRAS
            umbrales[i] = max(int(negro * factor), UMBRAL_MINIMO)
            print("%s: negro=%5d -> umbral %d" % (NOMBRES[i], negro, umbrales[i]))
    if hay_muerto:
        for _ in range(3):                 # aviso: morado parpadeando
            ib.pixel = (180, 0, 255)
            sleep(0.15)
            ib.pixel = (0, 0, 0)
            sleep(0.15)


# ------------------------------------------------------------------ MOTORES
def motores(izq, der):
    if MOTORES:
        ib.motor_1.throttle = izq
        ib.motor_2.throttle = der


def stop():
    ib.motor_1.throttle = 0
    ib.motor_2.throttle = 0


def mover_vigilando(izq, der, t, color):
    """Mueve por t segundos; corta si los sensores de adelante ven blanco yendo hacia adelante."""
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
    print("Listo. Robot sobre NEGRO. Presiona y suelta BOOT para empezar.")
    ib.pixel = (255, 100, 0)
    while True:
        ev = keys.events.get()
        if ev and ev.released:
            break
    print("Calibrando sobre negro (no mover)...")
    sleep(0.5)                 # que la mano se aleje del robot
    calibrar(4.0)              # 4.5 s + avisos = ~5 s reglamentarios
    print("UMBRALES =", umbrales, " ACTIVOS =", activos)
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
