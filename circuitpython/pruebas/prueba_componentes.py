# Prueba de todos los componentes del Sumobot (IdeaBoard ESP32, CircuitPython)
# Al final del archivo, descomentá la prueba que querés correr.
# Pines: ultrasónico TRIG=IO25 ECHO=IO26, IR de línea IO33, LSM6DS3TRC por I2C (0x6b)

import board, digitalio
from time import sleep, monotonic, monotonic_ns
from ideaboard import IdeaBoard

ib = IdeaBoard()
ib.motor_1.throttle = 0
ib.motor_2.throttle = 0


def prueba_led():
    for nombre, c in (("BLANCO", (255, 255, 255)), ("ROJO", (255, 0, 0)),
                      ("VERDE", (0, 255, 0)), ("AZUL", (0, 0, 255))):
        print("LED", nombre)
        ib.pixel = c
        sleep(1.5)
    ib.pixel = (0, 0, 0)
    print("LED apagado")


def prueba_motores(v=0.5):
    try:
        for nombre, m in (("motor_1", ib.motor_1), ("motor_2", ib.motor_2)):
            for sentido, t in (("ADELANTE", v), ("ATRAS", -v)):
                print(nombre, sentido, t)
                ib.pixel = (0, 255, 0) if t > 0 else (255, 0, 0)
                m.throttle = t
                sleep(1.5)
                m.throttle = 0
                sleep(0.7)
        print("AMBOS ADELANTE (el robot deberia ir recto hacia adelante)")
        ib.motor_1.throttle = v
        ib.motor_2.throttle = v
        sleep(1.5)
    finally:
        ib.motor_1.throttle = 0
        ib.motor_2.throttle = 0
        ib.pixel = (0, 0, 0)
        print("motores detenidos")


def prueba_ir(segundos=15):
    ir = ib.DigitalIn(board.IO33)
    print("IR en IO33: pasa el sensor sobre BLANCO y NEGRO")
    t0 = monotonic()
    ultimo = None
    while monotonic() - t0 < segundos:
        v = ir.value
        ib.pixel = (255, 255, 255) if v else (0, 0, 40)
        if v != ultimo:
            print("%5.1f s  IR=%s" % (monotonic() - t0, v))
            ultimo = v
        sleep(0.02)
    ib.pixel = (0, 0, 0)
    print("fin IR")


def prueba_imu(n=10):
    i2c = board.I2C()
    while not i2c.try_lock():
        pass
    try:
        print("I2C encontrados:", [hex(a) for a in i2c.scan()])
    finally:
        i2c.unlock()
    from adafruit_lsm6ds.lsm6ds3trc import LSM6DS3TRC
    s = LSM6DS3TRC(i2c, 0x6b)
    print("Mueve / inclina el robot:")
    for _ in range(n):
        ax, ay, az = s.acceleration
        gx, gy, gz = s.gyro
        print("acc X=%6.2f Y=%6.2f Z=%6.2f m/s2 | gyro X=%5.2f Y=%5.2f Z=%5.2f rad/s"
              % (ax, ay, az, gx, gy, gz))
        sleep(0.5)


trig = digitalio.DigitalInOut(board.IO25)
trig.direction = digitalio.Direction.OUTPUT
trig.value = False
echo = digitalio.DigitalInOut(board.IO26)
echo.direction = digitalio.Direction.INPUT


def distancia_cm(max_cm=200):
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


def prueba_ultrasonico(segundos=10):
    print("Ultrasonico: acerca y aleja la mano")
    t0 = monotonic()
    while monotonic() - t0 < segundos:
        d = distancia_cm()
        print("%6.1f cm" % d if d != -1 else "   -1 (sin eco)")
        ib.pixel = (0, 255, 0) if 0 < d < 75 else (0, 0, 255)
        sleep(0.2)
    ib.pixel = (0, 0, 0)


# prueba_led()
# prueba_motores()
# prueba_ir()
# prueba_imu()
# prueba_ultrasonico()
