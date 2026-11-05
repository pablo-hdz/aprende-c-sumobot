# 4. Instalar y cargar en un sumobot

> ⚠️ **Flashear este firmware borra CircuitPython** y todos los archivos de la placa (`code.py`, `/lib`).
> Haz un respaldo antes (paso 3) y guarda tu `code.py` en la computadora.

## 1. Instalar ESP-IDF v6.1 (una sola vez, ~4 GB, 10–25 min)

Guía oficial de CRCibernética para Linux, macOS y Windows:
[espressif-ideaboard/setup.md](https://github.com/CRCibernetica/espressif-ideaboard/blob/master/skills/ideaboard-esp-idf/references/setup.md).

En Windows por línea de comandos (así se instaló para este proyecto):

```bat
mkdir C:\esp && cd C:\esp
git clone -b v6.1 --recursive --depth 1 --shallow-submodules https://github.com/espressif/esp-idf.git esp-idf
C:\esp\esp-idf\install.bat esp32
```

Reglas para Windows: rutas **cortas, sin espacios ni tildes y fuera de OneDrive**
(por ejemplo `C:\esp\sumobot-ideaboard`). Con rutas largas la compilación falla.

En cada terminal nueva hay que activar el entorno:

```bat
C:\esp\esp-idf\export.bat
```

## 2. Compilar

```bat
cd C:\esp\sumobot-ideaboard\firmware-c
idf.py set-target esp32
idf.py build
```

La primera vez descarga el driver del LED (necesita internet) y compila todo ESP-IDF (1–5 min).
Al final debe decir `Project build complete` y crear `build\sumo_fw.bin`.

## 3. Respaldar la placa (recomendado)

Cierra Thonny o cualquier programa que use el puerto. Busca el puerto en
**Administrador de dispositivos → Puertos (COM y LPT) → USB-SERIAL CH340 (COMx)**.

```bat
python -m esptool --chip esp32 --port COM5 --baud 115200 read-flash 0 0x800000 respaldo_sumo.bin
```

Guarda 8 MB: CircuitPython + tus archivos. Para volver exactamente a ese estado:
`python -m esptool --chip esp32 --port COM5 write-flash 0x0 respaldo_sumo.bin`.
Si falla a mitad con "Packet content transfer stopped", baja `--baud` (115200) y desconecta la batería.

## 4. Flashear

```bat
idf.py -p COM5 flash monitor
```

`monitor` muestra lo que imprime el robot (salir con **Ctrl+]**). Solo un programa puede usar el
puerto a la vez: cierra Thonny antes.

## 5. Usarlo en el dojo

1. Jumper en **SELECT–Vin** y baterías puestas (por USB solo, los motores no tienen fuerza).
2. Encender. El LED muestra el **autodiagnóstico**:
   - verde = IMU y ultrasónico responden
   - 2 parpadeos amarillos = no hay IMU (conectar el cable Qwiic; el robot funciona igual sin ella)
   - 2 parpadeos cian = el ultrasónico no responde (revisar jumper SELECT–Vin y cables)
3. LED **naranja** = esperando. Poner el robot **dentro del dojo, sobre el negro**.
4. **Presionar y soltar BOOT** y no tocarlo: 5 s de cuenta (LED rojo) mientras se calibra.
   3 parpadeos **morados** = un sensor IR está dañado o sin 3V3/GND (el robot lo ignora).
5. Pelea. **Presionar BOOT** = detener y volver a esperar.

## 6. Volver a CircuitPython

Opción A, con tu respaldo del paso 3:

```bat
python -m esptool --chip esp32 --port COM5 write-flash 0x0 respaldo_sumo.bin
```

Opción B, CircuitPython limpio: guía oficial
[restore-circuitpython.md](https://github.com/CRCibernetica/espressif-ideaboard/blob/master/skills/ideaboard-esp-idf/references/restore-circuitpython.md)
(descargar el `.bin` de https://circuitpython.org/board/crcibernetica-ideaboard/, `erase-flash`,
`write-flash 0x0`, y copiar `/lib` de nuevo).

## Problemas comunes

| Síntoma | Causa probable |
|---|---|
| `could not open port` / `Acceso denegado` | Thonny u otro programa tiene el puerto abierto |
| `fatal error: driver/...h: No such file` | falta un componente en `main/CMakeLists.txt` |
| La compilación falla con rutas raras | proyecto en ruta larga, con espacios o en OneDrive |
| Se reinicia al arrancar motores (`Brownout`) | baterías bajas o solo USB |
| Siempre retrocede como si viera blanco | se calibró sobre una superficie clara, o los IR no tienen 3V3/GND |
