# Plan de sostenibilidad

Objetivo: que el proyecto **siga vivo y crezca sin depender de su autor original**.

## Principios de diseño que lo hacen posible

| Decisión | Por qué ayuda a que otros continúen |
|---|---|
| Licencia **libre, de dominio público** (The Unlicense) | Cualquiera puede copiarlo, adaptarlo o hacer su propia versión sin pedir permiso |
| **Un archivo por componente** y todos los ajustes en `config.h` | Se puede mejorar una parte sin entender todo; los módulos se reutilizan en otros robots |
| **Documentación de por qué**, no solo de cómo ([arquitectura](02-arquitectura.md), [historia](07-historia.md)) | Quien llega nuevo no necesita preguntarle al autor |
| **Separar lo probado de lo pendiente** ([estado](06-estado-y-verificacion.md)) | Cualquiera sabe qué falta y puede tomarlo |
| **Plantillas de Issues** y [CONTRIBUTING](../CONTRIBUTING.md) | Contribuir no depende de conocer a alguien |
| Ejercicios **sin solución publicada** en la [ruta de C](08-aprende-c.md) | Cada estudiante que resuelve uno deja material para el siguiente |

## Roles

| Rol | Responsabilidad | Cómo se llega |
|---|---|---|
| **Contribuidor/a** | Prueba, reporta, propone, resuelve ejercicios | Con el primer Issue o PR |
| **Mentor/a** | Responde Issues, revisa PRs de principiantes, etiqueta `good first issue` | Después de 3 contribuciones integradas, lo propone una persona mantenedora |
| **Mantenedor/a** | Integra PRs, cuida la calidad (que compile, que no se inventen resultados), decide el rumbo | Mentores activos invitados por los mantenedores actuales |

Meta: **al menos 2 personas mantenedoras** además del autor, para que nunca dependa de una sola.

## Cómo se toman decisiones

1. Toda propuesta empieza como **Issue** y queda escrita.
2. Cambios pequeños (documentación, correcciones): los integra cualquier mantenedor/a tras revisarlos.
3. Cambios grandes (nueva estrategia, cambio de hardware, nueva versión de ESP-IDF): se discuten en
   el Issue al menos una semana y se integran con el visto bueno de 2 mantenedores.
4. Si el proyecto queda sin mantenedores, la licencia permite que cualquiera haga un *fork* y lo
   continúe como nueva versión principal.

## Cómo puede crecer

| Horizonte | Meta |
|---|---|
| Corto plazo | Probar el firmware en C en robots reales y publicar resultados |
| | Publicar soluciones de los ejercicios de la ruta de C |
| Mediano plazo | Lecciones nuevas (WiFi, ESP-NOW entre robots, telemetría en el celular) |
| Largo plazo | Que el curso Sumobot lo use como material, y cada generación deje sus mejoras |

## Riesgos y cómo se cubren

| Riesgo | Respuesta |
|---|---|
| El autor deja el proyecto | Roles definidos, licencia libre, documentación completa |
| Cambia el kit (otra placa u otros pines) | Pines y ajustes centralizados en `config.h`; la historia documenta cómo se descubrieron |
| ESP-IDF cambia de versión | La versión usada (v6.1) está documentada; actualizar es un Issue más |
| Se publican resultados no verificados | Regla en CONTRIBUTING: "si no se probó en un robot, se escribe *sin probar*" |
