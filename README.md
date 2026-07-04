# Dungeon Depths 3D

> Videojuego 3D con generación aleatoria de mapas — Proyecto Final 2, Computación Gráfica (UNSA)

**Nombre del repositorio sugerido:** `dungeon-depths-3d`
*(alternativas: `roguelike-3d-opengl`, `procedural-dungeon-gl`, `mazehunter3d`)*

**Descripción corta (para GitHub, 1 línea):**
> Videojuego 3D en OpenGL con mazmorras generadas proceduralmente, combate contra enemigos e iluminación dinámica. Proyecto final del curso de Computación Gráfica.

**Topics/tags sugeridos:** `opengl` `cpp` `3d-game` `procedural-generation` `computer-graphics` `university-project`

---

## 📖 Descripción

Explora mazmorras generadas aleatoriamente a través de 3 niveles, derrota enemigos, recolecta tesoros y encuentra la llave dorada para avanzar. Cada nivel garantiza un camino jugable desde la entrada hasta la salida, pero el contenido y el layout cambian en cada partida.

Desarrollado con **OpenGL** como proyecto final del curso de Computación Gráfica (UNSA), a cargo del profesor Percy Maldonado Quispe.

## ✨ Características principales

- 🗺️ Generación procedural de mapas (10–20 salas por nivel, siempre resolubles)
- 🔑 Sistema de progresión: llave dorada, tesoros, ítems de vida, llaves de tesoro
- 👾 3+ tipos de enemigos con comportamientos distintos (perseguidor, disparador aleatorio, tipo bola)
- 🔫 Sistema de disparo en 4 direcciones con física de rebote de proyectiles
- 💡 Iluminación y sombras por sala + texturas en todos los elementos clave
- 🔊 Audio: disparos, muerte de enemigos, recolección de ítems, música de fondo
- 🖥️ HUD en tiempo real (vida, nivel, llaves, estado de llave dorada)

## 🎮 Controles

| Tecla | Acción |
|-------|--------|
| `W A S D` | Movimiento |
| `Flechas` / `Click` | Disparo en 4 direcciones |
| `ESC` | Menú / Pausa |

## 👥 Equipo y módulos

| Módulo | Responsable | Carpeta |
|--------|-------------|---------|
| Generación de mapas y niveles | Persona A | `src/world/` |
| Jugador y física de disparos | Persona B | `src/player/` |
| Enemigos e IA | Persona C | `src/enemies/` |
| Render, audio y UI | Persona D | `src/render/`, `src/audio/`, `src/ui/` |

## 🛠️ Tecnologías

- **Lenguaje:** C++
- **Gráficos:** OpenGL (+ GLFW / GLUT — *ajustar según lo que usen*)
- **Librerías sugeridas:** GLM (matemáticas), stb_image (texturas), irrKlang / OpenAL (audio) — *ajustar*
- **Build:** CMake

## 📁 Estructura del proyecto

```
dungeon-depths-3d/
├── src/
│   ├── main.cpp
│   ├── world/          # Generación de mapas (Persona A)
│   ├── player/          # Jugador y proyectiles (Persona B)
│   ├── enemies/         # IA de enemigos (Persona C)
│   ├── render/          # Iluminación, texturas, modelos (Persona D)
│   ├── audio/           # Sonido y música (Persona D)
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

## 🚀 Instalación y ejecución

```bash
git clone https://github.com/<usuario>/dungeon-depths-3d.git
cd dungeon-depths-3d
mkdir build && cd build
cmake ..
make
./dungeon-depths-3d
```

*(Ajustar comandos según el sistema de build final que elijan.)*

## ✅ Estado del proyecto

- [ ] Generación procedural de mapas
- [ ] Movimiento y disparo del jugador
- [ ] Física de rebote de proyectiles
- [ ] 3 tipos de enemigos
- [ ] Iluminación y texturas
- [ ] Audio
- [ ] HUD
- [ ] Funcionalidad extra: _______
- [ ] Informe PDF
- [ ] Video de demostración

## 📄 Licencia / Curso

Proyecto académico — Computación Gráfica, UNSA. Uso educativo.

## 📬 Contacto

Profesor: Percy Maldonado Quispe — pmaldonado@unsa.edu.pe
