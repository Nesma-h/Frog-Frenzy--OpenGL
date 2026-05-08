# 🐸 FROG FRENZY 3D
## OpenGL C++ Game Project

--

## 📋 المتطلبات المحققة

| المتطلب | التفاصيل |
|---------|---------|
| ✅ 3D | بيئة ثلاثية الأبعاد كاملة (طريق، ماء، أشجار، سيارات) |
| ✅ Animation | قفز الضفدعة (parabolic arc)، أرجل متحركة، دواليب السيارات، ماء متموج، شمس دوارة |
| ✅ Keyboard & Mouse | WASD + Arrow Keys لتحريك الضفدعة، Mouse Drag للكاميرا الحرة |
| ✅ Camera | 3 modes: Follow / Top-Down / Free Camera |
| ✅ Lighting | شمس ديناميكية (LIGHT0) + Fill light (LIGHT1)، وضع ليل/نهار |
| ✅ Texture | Procedural textures: طريق، عشب، ضفدعة، سيارة، ماء، سجلة، سماء |
| ✅ Collision | AABB 3D/2D collision لكل العوائق + Goal detection |

---

## 🗂️ هيكل الملفات

```
FrogFrenzy/
├── main.cpp         ← الملف الرئيسي (اللعبة + GLUT callbacks)
├── Camera.h         ← نظام الكاميرا (Follow / Top / Free)
├── Lighting.h       ← الإضاءة الديناميكية (شمس + ليل)
├── Texture.h        ← Procedural Texture Manager
├── Collision.h      ← AABB Collision Detection
├── Frog.h           ← كلاس الضفدعة + animation
├── Car.h            ← السيارات والسجلات + animation
├── stb_image.h      ← (مش بنستخدمه لأن textures procedural)
├── Makefile         ← Linux/Mac
└── README.md
```

---

## 🖥️ كيفية التشغيل

### Linux (Ubuntu/Debian)
```bash
# 1. تثبيت المكتبات
sudo apt-get install freeglut3-dev

# 2. بناء اللعبة
make

# 3. تشغيل
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
# freeglut موجود مع Xcode Command Line Tools
make
./FrogFrenzy
```

### Windows (MinGW / Code::Blocks)
```bash
# تحميل freeglut من: https://www.transmissionzero.co.uk/software/freeglut-devel/
# ثم compile:
g++ main.cpp -o FrogFrenzy.exe -lopengl32 -lglu32 -lfreeglut -std=c++11
```

### Windows (Visual Studio)
1. حمّل **freeglut** أو استخدم **NuGet: freeglut**
2. أضف كل الملفات للمشروع
3. في Project Properties:
   - Additional Include Dirs: مجلد freeglut include
   - Additional Lib Dirs: مجلد freeglut lib
   - Additional Dependencies: `opengl32.lib; glu32.lib; freeglut.lib`
4. Build & Run

---

## 🎮 تحكمات اللعبة

| مفتاح | الوظيفة |
|-------|---------|
| ↑ ↓ ← → | تحريك الضفدعة |
| W A S D | تحريك الضفدعة (بديل) |
| **C** | تبديل الكاميرا (Follow → Top → Free) |
| **N** | ليل / نهار |
| **R** | إعادة اللعب |
| **ESC** | خروج |
| Mouse Drag | تدوير الكاميرا الحرة |
| I J K L | تحريك الكاميرا الحرة |

---

## 🎯 هدف اللعبة

1. حرّك الضفدعة من **الأسفل للأعلى** (من Z=+8 إلى Z=-15)
2. **تجنب السيارات** في طريق (5 lanes) - تصادم = خسارة حياة
3. **اركب السجلات** عبر النهر (3 lanes) - لو وقعت في الماء = غرق
4. **اوصل لـ 5 أماكن** في الجانب الآخر ← تنتهي المرحلة
5. عند إكمال مرحلة → **Level Up** والسرعة تزيد
6. لديك **3 أرواح** - لو خلصت = Game Over

---

## 🔧 التفاصيل التقنية

### 3D Scene
- OpenGL Fixed Pipeline (GL_LIGHTING, GL_TEXTURE_2D)
- Z-buffer depth testing
- Alpha blending للماء الشفاف

### Animation System
- **Frog Jump**: منحنى Parabolic `y = height * 4t(1-t)` 
- **Leg Animation**: `sinf(legAnim)` للأرجل
- **Car Wheels**: دوران مستمر بناءً على السرعة
- **Water**: موجات `sinf(x*0.5 + time)` + تمرير UV offset
- **Sun**: دوران ديناميكي يغير لون الضوء
- **Death**: Squish scale animation

### Collision Detection
- **AABB** (Axis-Aligned Bounding Box) لكل كائن
- Road obstacles → موت فوري
- Water obstacles (logs) → يركب عليها + يتحرك معها
- Water lanes بدون سجلة → غرق
- Goal zones → فوز

### Texture System
- **Procedural Generation** - لا توجد ملفات خارجية مطلوبة
- كل texture تُولَّد بالكود (noise, patterns, gradients)
- `glGenerateMipmap` للجودة من مسافات بعيدة

### Lighting
- `GL_LIGHT0`: الشمس الديناميكية (تتحرك، تغير لون)
- `GL_LIGHT1`: Fill light من الأسفل
- `GL_COLOR_MATERIAL`: ألوان الكائنات تتأثر بالضوء
- Night mode: يغير ambient + diffuse لألوان ليلية

### Camera Modes
1. **Follow**: كاميرا تتبع الضفدعة من الخلف وفوق
2. **Top-Down**: نظرة عشائية من فوق
3. **Free**: تحريك حر مع Mouse + IJKL

---

## 📦 المكتبات المستخدمة

- `OpenGL (GL)` - الرسم ثلاثي الأبعاد
- `GLU` - gluPerspective, gluLookAt, gluQuadric
- `GLUT / freeglut` - نافذة، input، main loop

**لا يوجد أي مكتبة خارجية أخرى!**

---

## 🎨 لقطة الشاشة (وصف)

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
