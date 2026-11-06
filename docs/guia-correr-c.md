# Guía: cómo correr C (en tu computadora y en la IdeaBoard)

Guía paso a paso para estudiantes que nunca han programado en C. Al final vas a tener **tu primer
programa corriendo en la IdeaBoard**: el LED RGB cambiando de colores.

| Parte | Qué logras | Tiempo |
|---|---|---|
| [1. ¿Qué es C?](#1-qué-es-c) | entender qué es compilar | 5 min |
| [2. Qué necesitas](#2-qué-necesitas) | revisar materiales | 2 min |
| [3. Instalar las herramientas](#3-instalar-las-herramientas) | VS Code, gcc y ESP-IDF | 30–45 min (una sola vez) |
| [4. Tu primer programa en la PC](#4-tu-primer-programa-en-la-pc) | "Hola" en la pantalla | 5 min |
| [5. Tu primer programa en la IdeaBoard](#5-tu-primer-programa-en-la-ideaboard-el-led-rgb) | el LED RGB de colores | 10 min |
| [6. Entender el código del LED](#6-entender-el-código-del-led-línea-por-línea) | qué hace cada línea | 10 min |
| [7. Si algo falla](#7-si-algo-falla) | soluciones a errores comunes | — |

---

## 1. ¿Qué es C?

C es un lenguaje de programación **compilado**. Eso significa que el código que escribes no se
ejecuta directamente: primero un programa llamado **compilador** lo traduce a instrucciones que
entiende la máquina.

```
  hola.c   ──(compilador)──►   hola.exe / firmware.bin   ──►   se ejecuta
  lo que escribes              lo que entiende la máquina
```

| | Python (CircuitPython) | C |
|---|---|---|
| Cómo corre | un intérprete lo lee línea por línea | se compila antes y corre directo |
| Velocidad | más lento | muy rápido |
| Dónde se usa | aprender, prototipos | casi todo lo embebido: autos, drones, electrodomésticos, robots |

**C puede correr en dos lugares**, y en esta guía haces los dos:

| | En tu computadora | En la IdeaBoard |
|---|---|---|
| Compilador | `gcc` | ESP-IDF (`xtensa-esp32-elf-gcc`) |
| Resultado | un `.exe` de Windows | un `.bin` que se carga en la placa por USB |
| Sirve para | practicar el lenguaje rápido | controlar LED, motores y sensores |

---

## 2. Qué necesitas

- Computadora con **Windows 10 u 11** e internet.
- Unos **12 GB libres** en el disco (ESP-IDF ocupó ~11 GB en nuestra prueba).
- Para la parte 5: una **IdeaBoard** (ESP32) y un **cable USB-C de datos**. Algunos cables solo cargan
  y no transmiten datos; si la placa no aparece, prueba otro cable.

---

## 3. Instalar las herramientas

Se hace **una sola vez**.

> **Regla de oro en Windows:** usa rutas **cortas, sin espacios, sin tildes y fuera de OneDrive**,
> por ejemplo `C:\esp\...`. Con rutas largas o con espacios, ESP-IDF no compila.

### 3.1 Visual Studio Code

1. Descárgalo de https://code.visualstudio.com e instálalo.
2. Ábrelo, ve a **Extensiones** (los cuadritos de la barra izquierda) e instala **C/C++** de Microsoft.

### 3.2 Compilador `gcc` (para C en la PC)

Abre **PowerShell** (botón Inicio → escribe "PowerShell") y pega:

```bash
winget install --id BrechtSanders.WinLibs.POSIX.UCRT -e --accept-package-agreements --accept-source-agreements
```

Cierra PowerShell, abre uno nuevo y comprueba:

```bash
gcc --version
```

Debe mostrar algo como `gcc.exe (MinGW-W64 ...) 15.x`.

### 3.3 ESP-IDF con el instalador oficial (EIM)

**ESP-IDF** es el kit oficial del fabricante del ESP32 para programarlo en C. Se instala con el
**ESP-IDF Installation Manager (EIM)**, el instalador oficial de Espressif, que según su documentación
es *"la forma predeterminada y recomendada"* de instalar ESP-IDF v6.0 y versiones nuevas
([guía de Windows de ESP-IDF v6.1](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32/get-started/windows-setup.html)).

No necesitas instalar Git ni Python antes: el EIM revisa tu computadora e instala lo que falte
([requisitos del EIM](https://docs.espressif.com/projects/idf-im-ui/en/latest/prerequisites.html)).

**1. Instalar el EIM.** En PowerShell:

```bash
winget install Espressif.EIM
```

(También se puede descargar desde https://dl.espressif.com/dl/eim/ o desde las
[versiones en GitHub](https://github.com/espressif/idf-im-ui/releases)).

**2. Instalar ESP-IDF.** Abre la aplicación **ESP-IDF Installation Manager** desde el menú Inicio y sigue
estos pasos ([documentación oficial](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32/get-started/windows-setup.html)):

1. En **New Installation**, haz clic en **Start Installation**.
2. Elige **Easy Installation → Start Easy Installation**. Instala la última versión estable con la
   configuración recomendada, e incluye la integración con VS Code
   ([instalación fácil](https://docs.espressif.com/projects/idf-im-ui/en/latest/simple_installation.html)).
3. En **Ready to Install**, haz clic en **Start Installation** y espera la barra de progreso. Tarda
   según tu internet.
4. Al final aparece **Installation Complete**. ✅

Si algo falla, el botón **Logs** muestra qué pasó, y **Try Again** lo reintenta.

> **En nuestra prueba** (Windows 11, noviembre 2026), el EIM instaló ESP-IDF **v6.1** en
> `C:\esp\v6.1\esp-idf` y sus herramientas en `C:\Espressif\tools`. Entre las dos carpetas ocuparon
> **unos 11 GB**, así que revisa que tengas espacio libre.

### 3.4 Extensión ESP-IDF en VS Code

1. En VS Code → **Extensiones** (Ctrl+Shift+X) → busca e instala **ESP-IDF** de *Espressif Systems*.
2. La extensión **detecta sola** lo que instaló el EIM, leyendo el archivo
   `C:\Espressif\tools\eim_idf.json`
   ([documentación de la extensión](https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/installation.html)).
3. Abre la carpeta de tu proyecto (parte 5), presiona **Ctrl+Shift+P** y elige
   **ESP-IDF: Select Current ESP-IDF Version**. Escoge **v6.1**.
4. La barra azul de abajo debe decir **ESP-IDF v6.1.0**. ✅
5. Si algo no cuadra, **Ctrl+Shift+P → ESP-IDF: Doctor Command** muestra un diagnóstico.

> ⚠️ No pongas `IDF_PATH`, `IDF_TOOLS_PATH` ni `IDF_PYTHON_ENV_PATH` en la configuración de VS Code:
> esas variables **tienen prioridad** sobre la versión elegida y harían que la extensión ignore el EIM
> ([nota oficial](https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/installation.html)).

### 3.5 ¿Dónde está "Run Code"?

Si haces clic derecho en el código y buscas **Run Code**, no aparece: esa opción la agrega una extensión
aparte (*Code Runner*), y además **no sirve para la IdeaBoard**. Code Runner corre el programa en tu
computadora, y el código de la IdeaBoard solo puede correr en la placa.

| Para… | Usa |
|---|---|
| Código de la **PC** (parte 4) | **Terminal → Run Build Task** (Ctrl+Shift+B), o los comandos `gcc` |
| Código de la **IdeaBoard** (parte 5) | la barra azul: ⚙️ **Build** → ⚡ **Flash** → 🖥️ **Monitor**, o **Ctrl+Shift+P → ESP-IDF: Build, Flash and Monitor** |

---

## 4. Tu primer programa en la PC

1. Crea la carpeta `C:\esp\practica` y ábrela en VS Code (**File → Open Folder**).
2. Crea un archivo nuevo `hola.c` con:

```c
#include <stdio.h>

int main(void)
{
    printf("Hola, estoy aprendiendo C\n");
    return 0;
}
```

3. Abre la terminal: menú **Terminal → New Terminal**.
4. **Compila** (traduce `hola.c` a `hola.exe`):

```bash
gcc hola.c -o hola.exe
```

5. **Corre** el programa:

```bash
.\hola.exe
```

Debe aparecer `Hola, estoy aprendiendo C`. 🎉

| Parte del código | Qué hace |
|---|---|
| `#include <stdio.h>` | trae `printf()` para escribir en pantalla |
| `int main(void)` | el programa empieza aquí |
| `printf("...\n");` | escribe el texto; `\n` es un salto de línea |
| `return 0;` | termina el programa (0 = todo bien) |

---

## 5. Tu primer programa en la IdeaBoard: el LED RGB

> ⚠️ **Cargar un programa en C borra CircuitPython** y todos los archivos de la placa (`code.py`,
> `/lib`). Si la placa tiene algo importante, haz primero el respaldo de la parte 7. Se puede volver
> a CircuitPython cuando quieras.

El ejemplo está en [`ejemplos/01-rgb-ideaboard/`](../ejemplos/01-rgb-ideaboard/). Hace que el LED
de la placa muestre rojo, verde, azul y blanco, y luego un arcoíris. Si mantienes presionado **BOOT**
durante el arcoíris, el LED se apaga.

1. **Descarga el proyecto:** copia la carpeta `ejemplos\01-rgb-ideaboard` a `C:\esp\01-rgb-ideaboard`.
2. **Ábrelo en VS Code:** **File → Open Folder** → `C:\esp\01-rgb-ideaboard`. Si la barra de abajo no
   dice **ESP-IDF v6.1.0**, haz **Ctrl+Shift+P → ESP-IDF: Select Current ESP-IDF Version** (paso 3.4).
3. **Conecta la IdeaBoard** por USB. Cierra **Thonny** si está abierto: solo un programa puede usar
   la placa a la vez.
4. **Elige el puerto:** en la barra de abajo, clic en el 🔌 y elige **USB-SERIAL CH340 (COMx)**.
5. **Elige el chip:** en la barra de abajo debe decir **esp32**. Si no, haz clic y elige `esp32`.
6. **Compila:** clic en ⚙️ **Build**. La primera vez tarda 1–3 minutos y descarga el driver del LED
   (necesita internet). Debe terminar con `Project build complete`.
7. **Carga a la placa:** clic en ⚡ **Flash** (si pregunta, elige **UART**). Espera a que diga
   `Hash of data verified`.
8. **Mira el resultado:**
   - El **LED de la placa** cambia: rojo → verde → azul → blanco → arcoíris, y se repite.
   - Clic en 🖥️ **Monitor**: verás `Hola desde la IdeaBoard!` y el nombre de cada color.
     Para salir del monitor: **Ctrl+]**.

¿Prefieres la terminal? Abre la **terminal ESP-IDF** (ícono `>_` de la barra de abajo de VS Code), o en
PowerShell activa ESP-IDF con el script que creó el EIM:

```bash
. C:\Espressif\tools\Microsoft.v6.1.PowerShell_profile.ps1
```

y después escribe:

```bash
idf.py set-target esp32
```

```bash
idf.py build
```

```bash
idf.py -p COM5 flash monitor
```

(cambia `COM5` por tu puerto).

---

## 6. Entender el código del LED, línea por línea

Abre [`main/main.c`](../ejemplos/01-rgb-ideaboard/main/main.c) y sigue esta tabla:

| Código | Qué hace |
|---|---|
| `#include "led_strip.h"` | trae el driver del LED WS2812B (lo baja ESP-IDF solo, por `idf_component.yml`) |
| `#define PIN_LED 2` | el LED está conectado al pin **IO2** de la IdeaBoard |
| `#define BRILLO 0.2f` | brillo al 20 %; al 100 % el LED encandila |
| `static led_strip_handle_t led;` | una "manija" para controlar el LED |
| `led_iniciar()` | configura el LED: pin, cantidad (1) y modelo (WS2812) |
| `led_color(r, g, b)` | pone un color; cada componente va de 0 a 255 |
| `led_arcoiris(pos)` | convierte un número de 0 a 255 en un color del arcoíris |
| `vTaskDelay(pdMS_TO_TICKS(ms))` | espera `ms` milisegundos sin trabar el procesador |
| `void app_main(void)` | en ESP-IDF el programa empieza aquí, no en `main()` |
| `while (1) { ... }` | repite para siempre (un microcontrolador nunca "termina") |
| `gpio_get_level(PIN_BOOT) == 0` | ¿está presionado el botón BOOT? |
| `printf(...)` | manda texto por USB; se ve en el Monitor |

### Retos para practicar

1. Cambia `BRILLO` a `0.05f`. Compila, flashea y compara.
2. Agrega un color **morado** (`255, 0, 255`) después del azul.
3. Haz que el arcoíris vaya el doble de rápido.
4. Haz que **presionar BOOT** cambie de color (en vez de apagar el LED).

Cuando domines esto, sigue con la [ruta de 11 lecciones de C en el robot](08-aprende-c.md).

---

## 7. Si algo falla

| Problema | Solución |
|---|---|
| `gcc` no se reconoce | cierra y vuelve a abrir PowerShell o VS Code después de instalarlo |
| VS Code dice **ESP-IDF InvalidSetup** o no encuentra ESP-IDF | **Ctrl+Shift+P → ESP-IDF: Select Current ESP-IDF Version** y elige v6.1; si sigue, **ESP-IDF: Doctor Command** |
| Clic derecho no muestra **Run Code** | es normal: ver [3.5](#35-dónde-está-run-code) |
| No aparece ningún puerto COM | cable que solo carga, o falta el driver CH340: https://www.wch-ic.com/downloads/CH341SER_EXE.html |
| `could not open port` / `Acceso denegado` | Thonny u otro programa tiene la placa abierta: ciérralo |
| `Connecting......` y falla | mantén **BOOT**, presiona y suelta **RESET**, suelta **BOOT** y repite Flash |
| La compilación falla con rutas raras | el proyecto está en una ruta larga, con espacios o en OneDrive |
| El LED no enciende | revisa que el monitor diga `Hola desde la IdeaBoard!`; si no aparece, el programa no se cargó |

### Respaldar y volver a CircuitPython

**Antes** de flashear, para guardar todo lo que tiene la placa (tarda unos 12 minutos):

```bash
python -m esptool --chip esp32 --port COM5 --baud 115200 read-flash 0 0x800000 C:\esp\respaldo_placa.bin
```

Para **volver** a dejarla exactamente como estaba:

```bash
python -m esptool --chip esp32 --port COM5 write-flash 0x0 C:\esp\respaldo_placa.bin
```

Para instalar un CircuitPython limpio en lugar del respaldo:
[guía oficial de CRCibernética](https://github.com/CRCibernetica/espressif-ideaboard/blob/master/skills/ideaboard-esp-idf/references/restore-circuitpython.md).

---

**Estado de esta guía:** probada en Windows 11 con ESP-IDF v6.1 instalado por el EIM y gcc 15; el ejemplo
del LED compila sin errores ni advertencias. **Probado en una IdeaBoard real** (ESP32-D0WD-V3,
8 MB): se respaldó la placa, se flasheó a 115200 baudios, el monitor mostró `Hola desde la IdeaBoard!` y la
secuencia de colores, y el LED cambió de color. Si lo pruebas en otra placa, cuéntalo en un Issue con la
plantilla "Resultado de prueba".

> **Nota:** en esa placa, respaldar a 460800 o 921600 baudios corrompía los datos; a **115200** funcionó.
> Si `read-flash` falla con "Corrupt data" o "Packet content transfer stopped", baja la velocidad a 115200.

---

## Créditos y fuentes

Esta guía se apoya en documentación oficial. Todo el software mencionado es de sus autores:

| Herramienta | Autor | Licencia | Documentación usada |
|---|---|---|---|
| **ESP-IDF** | Espressif Systems | Apache 2.0 | [Instalación en Windows (v6.1)](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32/get-started/windows-setup.html) |
| **ESP-IDF Installation Manager (EIM)** | Espressif Systems | Apache 2.0 | [Documentación del EIM](https://docs.espressif.com/projects/idf-im-ui/en/latest/), [repositorio](https://github.com/espressif/idf-im-ui) |
| **Extensión ESP-IDF para VS Code** | Espressif Systems | Apache 2.0 | [Instalación](https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/installation.html), [problemas comunes](https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/troubleshooting.html) |
| **IdeaBoard** y ejemplos en ESP-IDF | CRCibernética | MIT | [espressif-ideaboard](https://github.com/CRCibernetica/espressif-ideaboard) |
| **Driver `led_strip`** | Espressif Systems | Apache 2.0 | [Component Registry](https://components.espressif.com/components/espressif/led_strip) |
| **WinLibs (gcc para Windows)** | Brecht Sanders | ver [winlibs.com](https://winlibs.com) | [winlibs.com](https://winlibs.com) |
| **Visual Studio Code** | Microsoft | — | [code.visualstudio.com](https://code.visualstudio.com) |
