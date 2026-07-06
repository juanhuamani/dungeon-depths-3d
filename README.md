# Dungeon Depths 3D

Videojuego 3D en OpenGL con generación aleatoria de mazmorras. Proyecto final del curso de Computación Gráfica.

## Descripción

El jugador recorre **3 niveles** generados de forma procedural. Cada nivel es un conjunto de 10 a 20 salas conectadas entre sí, con un camino garantizado desde la entrada hasta la salida. Hay que derrotar enemigos, recolectar tesoros y encontrar la **llave dorada** para poder pasar al siguiente nivel.

## Características

- Generación procedural de mapas, siempre resolubles
- Progresión: llave dorada, tesoros, ítems de vida, llaves de tesoro
- 3 tipos de enemigos con comportamiento distinto (perseguidor, disparador aleatorio, tipo bola)
- Disparo en 4 direcciones con rebote de proyectiles contra paredes
- Iluminación y sombras por sala, texturas en jugador/enemigos/paredes/piso/puertas
- Audio (disparos, muerte de enemigos, recolección de ítems, música de fondo)
- HUD con vida, nivel, llaves de tesoro y estado de la llave dorada

## Controles

| Tecla | Acción |
|-------|--------|
| `W` `A` `S` `D` | Movimiento |
| `Flechas` / `Click` | Disparo en 4 direcciones |
| `ESC` | Menú / pausa |

## Requisitos previos

### Arch Linux / Manjaro

```bash
sudo pacman -S base-devel cmake glfw glm
```

### Ubuntu / Debian

```bash
sudo apt install build-essential cmake libglfw3-dev libglm-dev
```

### Fedora

```bash
sudo dnf install gcc-c++ cmake glfw-devel glm-devel
```

> GLAD y stb_image están incluidos en `vendor/`.

## Compilación

```bash
git clone https://github.com/<usuario>/dungeon-depths-3d.git
cd dungeon-depths-3d
mkdir build && cd build
cmake ..
make -j$(nproc)
./dungeon-depths-3d
```

Recompilar después de cambios:

```bash
cd build && make -j$(nproc) && ./dungeon-depths-3d
```

Compilar desde cero:

```bash
rm -rf build && mkdir build && cd build && cmake .. && make -j$(nproc)
```

## Stack

| Tecnología | Uso |
|------------|-----|
| C++17 | Lenguaje principal |
| OpenGL 3.3 Core | Renderizado gráfico |
| GLFW | Ventana y manejo de input |
| GLAD | Loader de funciones OpenGL |
| GLM | Matemáticas (vectores, matrices, transformaciones) |
| stb_image | Carga de texturas (PNG, JPG) |
| CMake | Sistema de build |

## Estructura del proyecto

```
dungeon-depths-3d/
├── src/                          # Código fuente (.cpp)
│   ├── main.cpp                  # Entry point y game loop
│   ├── engine/                   # Core del engine (shader, window, input, recursos)
│   ├── game/                     # Estado del juego, lógica de niveles
│   ├── world/                    # Generación procedural de mapas
│   ├── player/                   # Jugador y proyectiles
│   ├── enemies/                  # IA de enemigos
│   ├── render/                   # Pipeline de renderizado, iluminación, texturas
│   ├── audio/                    # Sistema de audio
│   └── ui/                      # HUD y menús
├── include/                      # Headers (.h) — espeja la estructura de src/
│   ├── engine/
│   ├── game/
│   ├── world/
│   ├── player/
│   ├── enemies/
│   ├── render/
│   ├── audio/
│   └── ui/
├── assets/
│   ├── shaders/                  # Shaders GLSL
│   ├── models/                   # Modelos 3D
│   ├── textures/                 # Texturas
│   └── sounds/                   # Efectos de sonido y música
├── vendor/                       # Dependencias de terceros (GLAD, stb_image)
├── docs/                         # Informe PDF
├── CMakeLists.txt
└── README.md
```

## Estado

- [ ] Generación procedural de mapas
- [ ] Movimiento y disparo del jugador
- [ ] Rebote de proyectiles
- [ ] 3 tipos de enemigos
- [ ] Iluminación y texturas
- [ ] Audio
- [ ] HUD
- [ ] Funcionalidad extra: _______
- [ ] Informe PDF
- [ ] Video de demostración
