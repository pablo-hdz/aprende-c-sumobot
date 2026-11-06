# Firmware en C (ESP-IDF v6.1)

Firmware nativo para la IdeaBoard del Sumobot CENFOTEC 2025.

> ⚠️ Compila sin errores, pero **todavía no se ha probado en un robot**. Flashearlo borra CircuitPython.

```bat
. C:\Espressif\tools\Microsoft.v6.1.PowerShell_profile.ps1
cd C:\esp\sumobot-ideaboard\firmware-c
idf.py set-target esp32
idf.py build
idf.py -p COM5 flash monitor
```

- Ajustes del robot: [`main/config.h`](main/config.h)
- Cómo funciona: [`../docs/02-arquitectura.md`](../docs/02-arquitectura.md) y [`../docs/03-modulos.md`](../docs/03-modulos.md)
- Instalación completa y cómo volver a CircuitPython: [`../docs/04-instalar-y-flashear.md`](../docs/04-instalar-y-flashear.md)
