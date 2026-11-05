# 5. Tácticas y ajustes

## La estrategia base (la que ganó una pelea en CircuitPython)

1. **Nunca salir del dojo.** La línea blanca se revisa antes que cualquier otra cosa, en cada ciclo.
2. **Buscar girando en el lugar** a velocidad baja, para no pasarse del rival.
3. **Atacar recto y a fondo** en cuanto el ultrasónico ve algo a menos de 60 cm.
4. **Escapar del borde según dónde se vio la línea**:
   - línea adelante → retroceder 350 ms y girar hacia el lado contrario (180° si la ven los dos
     sensores delanteros);
   - línea atrás (el rival nos empuja) → avanzar a fondo 250 ms.

## Lo que agrega el firmware en C

| Mejora | Por qué ayuda |
|---|---|
| Ciclo fijo de 5 ms | ve la línea blanca hasta 6 veces más seguido que la versión en CircuitPython |
| Ultrasónico en segundo plano | el robot nunca queda "ciego" esperando el eco |
| Giros por grados con IMU | el giro de escape queda igual aunque bajen las baterías |
| "Empuja tras choque" con IMU | cuando está pegado al rival el ultrasónico falla; el choque mantiene el empuje 300 ms |
| Autocalibración por robot | los umbrales se adaptan a cada sumo y a cada dojo |

## Parámetros de `firmware-c/main/config.h`

| Parámetro | Valor | Subirlo | Bajarlo |
|---|---|---|---|
| `DIST_ATTACK_CM` | 60 | ataca desde más lejos, pero puede perseguir cosas fuera del dojo | más conservador |
| `SPEED_SEARCH` | 0.35 | gira más rápido buscando (subir si no tiene fuerza para girar) | se pasa menos del rival |
| `SPEED_ATTACK` | 1.0 | — | menos empuje, más control |
| `SPEED_ESCAPE` | 0.8 | escapa más rápido del borde | escape más suave |
| `T_BACKUP_MS` | 350 | se aleja más del borde | vuelve antes a pelear |
| `TURN_DEG` | 120 | queda mirando más hacia el centro | gira menos |
| `ATTACK_HOLD_MS` | 0 | sigue atacando aunque pierda al rival (una versión con 600 ms se cayó del dojo una vez) | — |
| `CONTACT_HOLD_MS` | 300 | empuja más tiempo tras un choque | suelta antes |
| `IMU_CONTACT_MS2` | 6.0 | solo choques fuertes cuentan | detecta roces |
| `MOTOR_FWD` | 1 | poner −1 si "adelante" va hacia atrás | |

## Cómo afinar con datos

1. Conectar por USB con batería y abrir `idf.py monitor`.
2. Hacer una pelea de prueba. Cada 50 ms sale una línea `T,ms,estado,S1,S2,S3,S4,dist,yaw,acc,choque`.
3. Copiar las líneas a un archivo `.csv` y graficarlas (Excel, Google Sheets o Python).
4. Mirar qué pasó justo antes de cada error: ¿la línea se vio tarde? ¿perdió al rival y giró?
5. Cambiar **un solo parámetro**, compilar, flashear y repetir.

## Ideas tácticas para probar (no implementadas)

- **Esquivar al inicio:** girar 90° al arrancar para que el rival pase de largo.
- **Búsqueda en espiral** en lugar de girar en el lugar.
- **Detector de empuje:** si los motores van hacia adelante pero el acelerómetro dice que el robot
  retrocede, girar de golpe para que el rival se siga de largo.
- **Recordar el lado** donde se vio al rival por última vez y buscar primero hacia ese lado.
