# Práctica 01 · LED RGB de la IdeaBoard

Tu primer programa en C para la placa: el LED RGB pasa por rojo, verde, azul y blanco, y luego hace
un arcoíris. Si mantienes **BOOT** presionado durante el arcoíris, el LED se apaga.

| | |
|---|---|
| **Nivel** | 🟢 Primer programa |
| **Tiempo** | 15 minutos (con todo instalado) |
| **Aprendes** | estructura de un programa ESP-IDF (`app_main`), funciones, `while`, `for`, esperas con `vTaskDelay`, `printf` por USB |

---

## 1. Explicación

La IdeaBoard trae un **LED RGB WS2812B** soldado y conectado al pin **IO2**. "RGB" significa que tiene
tres LEDs dentro (rojo, verde y azul): mezclándolos se forma cualquier color. Cada color va de
**0 a 255**, por ejemplo `(255, 0, 255)` es morado.

El WS2812B es un LED "inteligente": recibe el color por un solo cable con pulsos muy rápidos (~1 µs).
El ESP32 los genera con su periférico **RMT**, y el componente `led_strip` de Espressif se encarga
de los detalles.

Un microcontrolador **nunca termina** su programa: después de configurar todo, `app_main()` entra en
un `while (1)` que repite la secuencia de colores para siempre.

---

## 2. Materiales

| Material | Cantidad |
|---|---|
| IdeaBoard de CRCibernética | 1 |
| Cable USB-C de datos | 1 |

No hay que conectar nada: el LED y el botón BOOT ya vienen en la placa.

---

## 3. Conexión

Solo el cable USB entre la IdeaBoard y la computadora.

| Componente | Pin | Viene en la placa |
|---|---|---|
| LED RGB WS2812B | IO2 | ✅ |
| Botón BOOT | IO0 | ✅ |

---

## 4. El programa

Código completo: [`main/main.c`](main/main.c). Las partes importantes:

**Preparar el LED** (pin, cantidad y modelo):

```c
led_strip_config_t config = {
    .strip_gpio_num = PIN_LED,     // IO2
    .max_leds = 1,
    .led_model = LED_MODEL_WS2812,
    .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
};
```

**Poner un color:**

```c
static void led_color(uint8_t r, uint8_t g, uint8_t b)
{
    led_strip_set_pixel(led, 0, r * BRILLO, g * BRILLO, b * BRILLO);
    led_strip_refresh(led);
}
```

**El ciclo principal:**

```c
while (1) {
    printf("Rojo\n");  led_color(255, 0, 0);  esperar_ms(700);
    printf("Verde\n"); led_color(0, 255, 0);  esperar_ms(700);
    ...
}
```

La explicación línea por línea está en la [guía, parte 6](../../docs/guia-correr-c.md#6-entender-el-código-del-led-línea-por-línea).

---

## 5. Cómo correr el programa

Los pasos son los mismos de la [práctica 02, sección 5](../02-pulsador-digital/README.md#5-cómo-correr-el-programa),
abriendo esta carpeta (`practicas-aplicables/01-led-rgb`):
**Open Folder → ESP-IDF v6.1.0 en la barra → puerto COMx → ⚙️ Build → ⚡ Flash → 🖥️ Monitor**.

---

## 6. Resultado esperado

**En el Monitor:**

```
Hola desde la IdeaBoard! Mi primer programa en C
Rojo
Verde
Azul
Blanco
Arcoiris
```

**En la placa:** el LED pasa por rojo → verde → azul → blanco (0.7 s cada uno) y después hace un
arcoíris de unos 2.5 s. Se repite para siempre.

📷 **Foto y 🎥 video:** pendientes. ¿Lo grabaste? Agrégalo en `img/` con un Pull Request.

---

## 7. Retos

1. Cambia `BRILLO` a `0.05f` y compara.
2. Agrega el color **morado** (`255, 0, 255`) después del azul.
3. Haz que el arcoíris vaya el doble de rápido.
4. Haz que **presionar BOOT** cambie de color en vez de apagar el LED.

---

## 8. Estado y créditos

✅ **Probado** en una IdeaBoard real: el LED cambió de color y el Monitor mostró
`Hola desde la IdeaBoard!` y la secuencia de colores.

- IdeaBoard y ejemplo base de LED en ESP-IDF: [CRCibernética](https://github.com/CRCibernetica/espressif-ideaboard) (MIT).
- ESP-IDF y driver `led_strip`: [Espressif Systems](https://github.com/espressif/esp-idf) (Apache 2.0).
