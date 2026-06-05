# 🐸 FROG FRENZY
## OpenGL C++ Game Project

---

## 📋 Implemented Requirements

| Requirement | Details |
|-------------|---------|
| ✅ 3D | Full 3D environment (road, water, trees, cars) |
| ✅ Animation | Frog jumping (parabolic arc), moving legs, car wheels, rippling water, rotating sun |
| ✅ Keyboard & Mouse | WASD + Arrow Keys to move the frog, Mouse Drag for free camera |
| ✅ Camera | 3 modes: Follow / Top-Down / Free Camera |
| ✅ Lighting | Dynamic sun (LIGHT0) + Fill light (LIGHT1), day/night mode |
| ✅ Texture | Procedural textures: road, grass, frog, car, water, log, sky |
| ✅ Collision | AABB 3D/2D collision for all obstacles + Goal detection |

---

## 🗂️ File Structure

```
FrogFrenzy/
├── main.cpp         ← Main file (game + GLUT callbacks)
├── Camera.h         ← Camera system (Follow / Top / Free)
├── Lighting.h       ← Dynamic lighting (sun + night)
├── Texture.h        ← Procedural Texture Manager
├── Collision.h      ← AABB Collision Detection
├── Frog.h           ← Frog class + animation
├── Car.h            ← Cars and logs + animation
├── stb_image.h      ← (unused since textures are procedural)
├── Makefile         ← Linux/Mac
└── README.md
```

---

## 🖥️ How to Run

### Linux (Ubuntu/Debian)
```bash
# 1. Install libraries
sudo apt-get install freeglut3-dev

# 2. Build the game
make

# 3. Run
./FrogFrenzy
```

### Linux (Fedora/CentOS)
```bash
sudo dnf install freeglut-devel
make
./FrogFrenzy
```

### macOS
```bash
# freeglut comes with Xcode Command Line Tools
make
./FrogFrenzy
```

### Windows (MinGW / Code::Blocks)
```bash
# Download freeglut from: https://www.transmissionzero.co.uk/software/freeglut-devel/
# Then compile:
g++ main.cpp -o FrogFrenzy.exe -lopengl32 -lglu32 -lfreeglut -std=c++11
```

### Windows (Visual Studio)
1. Download **freeglut** or use **NuGet: freeglut**
2. Add all files to the project
3. In Project Properties:
   - Additional Include Dirs: freeglut include folder
   - Additional Lib Dirs: freeglut lib folder
   - Additional Dependencies: `opengl32.lib; glu32.lib; freeglut.lib`
4. Build & Run

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| ↑ ↓ ← → | Move the frog |
| W A S D | Move the frog (alternative) |
| **C** | Switch camera (Follow → Top → Free) |
| **N** | Toggle night / day |
| **R** | Restart game |
| **ESC** | Quit |
| Mouse Drag | Rotate free camera |
| I J K L | Move free camera |

---

## 🎯 Game Objective

1. Move the frog from **bottom to top** (from Z=+8 to Z=-15)
2. **Avoid cars** on the road (5 lanes) — collision = lose a life
3. **Ride logs** across the river (3 lanes) — falling in water = drowning
4. **Reach 5 goal spots** on the other side → stage complete
5. On stage completion → **Level Up** and speed increases
6. You have **3 lives** — lose them all = Game Over

---

## 🔧 Technical Details

### 3D Scene
- OpenGL Fixed Pipeline (GL_LIGHTING, GL_TEXTURE_2D)
- Z-buffer depth testing
- Alpha blending for transparent water

### Animation System
- **Frog Jump**: Parabolic curve `y = height * 4t(1-t)`
- **Leg Animation**: `sinf(legAnim)` for legs
- **Car Wheels**: Continuous rotation based on speed
- **Water**: Waves `sinf(x*0.5 + time)` + UV offset scrolling
- **Sun**: Dynamic rotation that changes light color
- **Death**: Squish scale animation

### Collision Detection
- **AABB** (Axis-Aligned Bounding Box) for every object
- Road obstacles → instant death
- Water obstacles (logs) → frog rides and moves with them
- Water lanes without a log → drowning
- Goal zones → win condition

### Texture System
- **Procedural Generation** — no external files required
- Every texture is generated in code (noise, patterns, gradients)
- `glGenerateMipmap` for quality at far distances

### Lighting
- `GL_LIGHT0`: Dynamic sun (moves, changes color)
- `GL_LIGHT1`: Fill light from below
- `GL_COLOR_MATERIAL`: Object colors are affected by lighting
- Night mode: changes ambient + diffuse to nighttime colors

### Camera Modes
1. **Follow**: Camera tracks the frog from behind and above
2. **Top-Down**: Top-down overhead view
3. **Free**: Free movement with Mouse + IJKL

---

## 📦 Libraries Used

- `OpenGL (GL)` — 3D rendering
- `GLU` — gluPerspective, gluLookAt, gluQuadric
- `GLUT / freeglut` — window, input, main loop

**No other external libraries required!**

---

## 📸 Screenshots

### Main Gameplay
![Gameplay](screenshots/MainGame.png)

### Free Camera View
![Top Camera](screenshots/FreeView.png)

### Night Mode
![Night Mode](screenshots/DarkMode.png)

### Squash--lose
![Free Camera](screenshots/Squash.png)

```
┌─────────────────────────────────────┐
│ Score: 350    Level: 2   Cam: Follow│
│ Lives: ❤❤❤   Goals: 2/5           │
│                                     │
│  🌳   [GOAL][GOAL][----][----][----]│
│       ~~~~~~~~~~~~~~~~~~~~ (WATER) │
│  🚗← ← ← ← ← ← ← ← ← ← ← ←    │
│       → → → 🐸 → → → → → →      │
│  🚕→ → → → → → → → → → → →       │
│  🌳                                 │
│                                     │
│ Arrow/WASD: Move  C: Camera  N: Night│
└─────────────────────────────────────┘
```
