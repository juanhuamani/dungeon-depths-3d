# Dungeon Depths 3D

Videojuego 3D en OpenGL con generación aleatoria de mapas. Proyecto final del curso de Computación Gráfica, UNSA (profesor Percy Maldonado Quispe).

## Descripción

El jugador recorre 3 niveles generados de forma procedural. Cada nivel es un conjunto de 10 a 20 salas conectadas entre sí, con un camino garantizado desde la entrada hasta la salida. Hay que derrotar enemigos, recolectar tesoros y encontrar la llave dorada para poder pasar al siguiente nivel.


## Qué incluye

- Generación procedural de mapas, siempre resolubles
- Progresión: llave dorada, tesoros, ítems de vida, llaves de tesoro
- 3 tipos de enemigos con comportamiento distinto (perseguidor, disparador aleatorio, tipo bola)
- Disparo en 4 direcciones con rebote de proyectiles contra paredes
- Iluminación y sombras por sala, texturas en jugador/enemigos/paredes/piso/puertas
- Audio (disparos, muerte de enemigos, recolección de ítems, música de fondo)
- HUD con vida, nivel, llaves de tesoro y estado de la llave dorada


## Stack

C++ con OpenGL (GLFW), CMake para build. GLM para matemáticas, stb_image para texturas. *(ajustar si usan otra cosa)*

## Estructura

```
dungeon-depths-3d/
├── src/
│   ├── main.cpp
│   ├── world/          # generación de mapas (Persona A)
│   ├── player/          # jugador y proyectiles (Persona B)
│   ├── enemies/         # IA de enemigos (Persona C)
│   ├── render/          # iluminación, texturas, modelos (Persona D)
│   ├── audio/           # sonido y música (Persona D)
│   └── ui/              # HUD y menús (Persona D)
├── assets/
│   ├── models/
│   ├── textures/
│   └── sounds/
├── docs/
│   └── informe.pdf
├── CMakeLists.txt
└── README.md
```

## Build

```bash
git clone https://github.com/<usuario>/dungeon-depths-3d.git
cd dungeon-depths-3d
mkdir build && cd build
cmake ..
make
./dungeon-depths-3d
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

## Contacto

Profesor: Percy Maldonado Quispe — pmaldonado@unsa.edu.pe
