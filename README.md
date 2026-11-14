# Trabajo Final PEV 2 - PropHuntJOchoa
**Alumno:** Jesus Ochoa | **Legajo:** 45487149 | **Modo:** Prop Hunt (Multijugador Asimétrico) | **Versión de Unreal:**
UE 5.8.2
---
## Reglas y Mecánica Principal
- **Mecánica:** Juego multijugador por equipos (Cazadores vs. Props). Durante la fase inicial de escondite, los Cazadores quedan inmovilizados mientras los Props copian objetos interactuables del entorno (`BPI_Interactable`) y pueden anclarse (`F`) para rotar (`Q`/`E`) o deslizar (`W A S D`) su malla. Luego inicia la fase de cacería, donde los Cazadores disparan un rayo escáner para detectar y eliminar Props (`25 HP` de daño), recibiendo una penalización de salud (`-5 HP`) al fallar contra objetos del escenario.
- **Cómo se gana:** Los **Cazadores** ganan si eliminan a todos los Props antes de que termine el reloj de ronda. Los **Props** ganan si al menos uno sobrevive hasta que el temporizador llega a `00:00` (o si los Cazadores pierden toda su salud por penalizaciones).
---
## Ficha Técnica de Red
- **RepNotify:** `CurrentMatchState` en `APropHuntGameState` (y `CurrentRole` / `CurrentDisguiseMesh` / `bIsPropLocked` en `APropHuntJOchoaCharacter`) -> Actualiza en todos los clientes la fase de partida en el HUD, la visibilidad del personaje o malla copiada, el desplazamiento de cámara al hombro y la dimensión de la cápsula.
- **Server RPC:** `Server_ExecuteAttack` en `APropHuntJOchoaCharacter` (junto a `Server_RequestRole` y `Server_StartMatch` en `APropHuntPlayerController`) -> Valida en el servidor que la partida esté en `InProgress`, ejecuta el trazado de impacto autoritativo, aplica el daño o la penalización correspondiente y gestiona el viaje entre niveles mediante `ServerTravel`.
- **Client RPC:** `Client_NotifyAttackResult` en `APropHuntPlayerController` -> Envía el resultado del disparo exclusivamente al cliente del Cazador para reproducir el sonido 2D de acierto o fallo y cambiar el color de la mira (`Img_Crosshair`) en el HUD.
- **NetMulticast RPC:** `Multicast_PlayAttackEffects` y `Multicast_PlayTransformEffects` en `APropHuntJOchoaCharacter` -> Reproducen para todos los clientes conectados el rayo visual en Niagara (`NS_HunterBeam`), las partículas de transformación (`NS_PropTransform`) y los efectos de sonido posicionales 3D.
---
## Cómo Probar el Proyecto
1. Abrir el mapa: `Content/Maps/Lvl_MainMenuMap` (flujo completo de menú y lobby) o `Content/Maps/Lvl_MainPropHunt` (prueba directa en la arena de oficina).
2. En Play (PIE): `Number of Players = 2` (o 4+) y `Net Mode = Play as Listen Server`.
3. **Desde el ejecutable (LAN):** Abrir dos instancias de `PropHuntJOchoa.exe`, permitir el acceso en el Firewall de Windows, crear sesión desde **Lan->Crear Partida** y conectarse desde **Lan->Unirse** luego de colocar la IP del Host en respectivo lugar.

*(Nota: Los assets visuales de entorno en `Content/Fab/` fueron excluidos del repositorio en `.gitignore` por el límite de tamaño de GitHub, pero se incluyen compilados dentro del ejecutable de Windows).*]
*(Nota 2: Los plugins "SteamSubSytem", "SteamSockets" y "MultiplayerOW" fueron desactivados ya que daban un error que impedia iniciar el ejecutable, asi que el unico metodo de jugar *Multijugador* es solamente por **LAN**)*
