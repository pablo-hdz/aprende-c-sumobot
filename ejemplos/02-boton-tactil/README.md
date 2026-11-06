# Ejemplo 02 · Botón o sensor táctil

Cada vez que presionas el botón, el LED RGB de la IdeaBoard cambia de color. Mientras lo mantienes
presionado, el LED brilla fuerte.

**Aprendes:** entradas digitales (`gpio_get_level`), resistencias pull-up / pull-down,
anti-rebote y detección de flancos ("justo se presionó").

## Materiales

- IdeaBoard
- 1 módulo digital de 3 pines: probado con el **botón Keyestudio** (capuchón amarillo). También
  sirve el táctil, de inclinación, reed o Hall de los kits.
- 3 cables hembra-hembra

## Conexión

| Pin del módulo | Pin de la IdeaBoard |
|---|---|
| **S** (señal) | **IO27** |
| **V** / + | **3.3V** de la misma fila que IO27 |
| **G** / − | **GND** de la misma fila que IO27 |

⚠️ Usa **3.3V**, no V+ ni Vin: la IdeaBoard funciona a 3.3 V.

## Correrlo

Igual que el [ejemplo 01](../01-rgb-ideaboard/) (ver la [guía](../../docs/guia-correr-c.md#5-tu-primer-programa-en-la-ideaboard-el-led-rgb)):
**Build → Flash → Monitor**. **No toques el botón durante el primer segundo**: la placa está midiendo
el reposo.

En el Monitor verás:

```
No toques el sensor: midiendo el reposo...
Reposo = 1. Listo: toca o presiona el sensor.
Toque #1 -> verde
Soltado
Toque #2 -> azul
Soltado
```

## Cómo funciona

1. **Reposo automático:** al arrancar lee el pin 20 veces. El botón Keyestudio da **1 suelto y 0
   presionado**, porque su resistencia R1 es un *pull-up*; otros módulos dan lo contrario. El programa
   toma como "activado" **cualquier cambio respecto al reposo**, así funciona con los dos tipos.
2. **Anti-rebote:** al presionar, el contacto metálico "rebota" y da varios 0 y 1 en milisegundos.
   El cambio solo se acepta si se repite **3 lecturas seguidas** (30 ms).
3. **Flanco:** el color cambia solo en el instante en que pasa de suelto a presionado, no mientras
   se mantiene presionado.

## Retos

1. Cambia `ESTABLE` a 1. ¿Aparecen toques "fantasma" al presionar rápido?
2. Haz que **mantener presionado 1 segundo** apague el LED.
3. Conecta el **sensor táctil** (rojo, de OSOYOO; pin **DO** a IO27) sin cambiar el código. ¿Qué
   reposo mide?

## Estado

✅ Probado en una IdeaBoard real con el botón Keyestudio: 17 pulsaciones seguidas, cada una
registrada una sola vez y con su "Soltado".
