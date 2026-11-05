# Cómo colaborar

¡Gracias por querer mejorar este proyecto! No hace falta ser experto: probar el código en tu
robot y contar qué pasó ya es una contribución valiosa.

## Formas de participar (de la más sencilla a la más avanzada)

| Nivel | Qué puedes hacer | Cómo |
|---|---|---|
| 🟢 Empezar | Probar el código en tu robot y contar el resultado | Abre un Issue con la plantilla **"Resultado de prueba"** |
| 🟢 Empezar | Avisar de un error o algo confuso en la documentación | Abre un Issue con la plantilla **"Reportar un error"** |
| 🟡 Intermedio | Agregar fotos, diagramas o corregir la documentación | Pull Request con cambios en `docs/` o `README.md` |
| 🟡 Intermedio | Proponer una táctica o un ajuste de parámetros | Issue con la plantilla **"Proponer una mejora"** |
| 🔴 Avanzado | Programar una mejora en CircuitPython o en C | Pull Request con cambios en `circuitpython/` o `firmware-c/` |

Si no sabes por dónde empezar, busca los Issues con la etiqueta `good first issue`.

## Proponer ideas o mejoras

1. Revisa que no exista ya un Issue parecido.
2. Abre un Issue nuevo con la plantilla **"Proponer una mejora"**.
3. Explica qué quieres cambiar, por qué y cómo sabremos que funcionó.

## Reportar errores

Abre un Issue con la plantilla **"Reportar un error"** e incluye:

- qué versión usaste (`circuitpython/sumobot_base.py`, `firmware-c`, ...);
- qué esperabas que pasara y qué pasó;
- lo que salió en la consola (Thonny o `idf.py monitor`) y el color del LED;
- cómo estaba el robot: sobre negro o blanco, con o sin batería, con o sin cable Qwiic.

## Contribuir con documentación o recursos

- La documentación está en [`docs/`](docs/) en Markdown. Los diagramas usan
  [Mermaid](https://mermaid.js.org/), que GitHub dibuja solo.
- Fotos e imágenes: súbelas a una carpeta `docs/img/` y enlázalas desde el documento.
  **Solo material propio** o con permiso de su autor.
- Si mediste algo en un robot (voltajes, tiempos, distancias), agrégalo a
  [`docs/06-estado-y-verificacion.md`](docs/06-estado-y-verificacion.md) diciendo **cómo** lo mediste.

## Solicitar la incorporación de cambios (Pull Request)

1. Haz un *fork* del repositorio (o una rama, si tienes permiso de escritura).
2. Crea una rama con un nombre que diga qué cambia: `docs/fotos-cableado`, `fix/umbral-ir`,
   `feat/espiral`.
3. Haz cambios pequeños y enfocados. Un tema por Pull Request.
4. Si tocaste `firmware-c/`, comprueba que **compila** (`idf.py build`) antes de enviar.
5. Abre el Pull Request hacia `main` y explica:
   - **qué** cambiaste,
   - **por qué**,
   - **cómo lo probaste**: en qué robot y con qué resultado. Si no lo probaste en un robot, dilo.
6. Enlaza el Issue que resuelve (`Cierra #3`).

## Reglas del proyecto

- **No inventar resultados.** Si algo no se probó en un robot, se escribe "sin probar".
- **Un ajuste a la vez:** al cambiar parámetros de pelea, cambia uno y documenta el efecto.
- Los valores ajustables van en `firmware-c/main/config.h`, no repartidos por el código.
- Escribe comentarios y documentación en español claro.
- No subas contraseñas, datos personales ni material de terceros sin permiso.
- Respeta a todas las personas: este es un espacio para aprender.

## Usar IA para contribuir

Puedes usar herramientas de IA (ChatGPT, Claude, Gemini...). Si lo haces, dilo en el Pull Request
y **verifica** lo que genere: que compile, que funcione en el robot y que no afirme cosas que no
probaste.
