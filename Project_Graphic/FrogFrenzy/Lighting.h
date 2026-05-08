#pragma once
#include <GL/gl.h>
#include <cmath>

class Lighting {
public:
    float sunAngle;      // زاوية الشمس للـ animation
    bool  nightMode;

    Lighting() : sunAngle(45.0f), nightMode(false) {}

    void setup() {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);   // شمس / ضوء رئيسي
        glEnable(GL_LIGHT1);   // ضوء محيط ثانوي
        glEnable(GL_NORMALIZE);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glShadeModel(GL_SMOOTH);
    }

    void update(float dt) {
        sunAngle += dt * 10.0f;   // الشمس تدور ببطء
        if (sunAngle > 360.0f) sunAngle -= 360.0f;
    }

    void apply() {
        float rad = sunAngle * 3.14159f / 180.0f;

        // LIGHT0 - الشمس الديناميكية
        float sunPos[4] = {
            20.0f * cosf(rad),
            20.0f * sinf(rad),
            10.0f,
            1.0f
        };

        float diffuse[4], ambient[4];
        if (nightMode) {
            diffuse[0]=0.1f; diffuse[1]=0.1f; diffuse[2]=0.3f; diffuse[3]=1.0f;
            ambient[0]=0.05f; ambient[1]=0.05f; ambient[2]=0.1f; ambient[3]=1.0f;
        } else {
            // لون الشمس يتغير حسب الزاوية (صباح = برتقالي، ظهر = أبيض)
            float t = (sinf(rad) + 1.0f) * 0.5f;
            diffuse[0] = 0.8f + 0.2f * t;
            diffuse[1] = 0.6f + 0.4f * t;
            diffuse[2] = 0.3f + 0.5f * t;
            diffuse[3] = 1.0f;
            ambient[0]=0.3f; ambient[1]=0.3f; ambient[2]=0.3f; ambient[3]=1.0f;
        }

        glLightfv(GL_LIGHT0, GL_POSITION, sunPos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);

        float spec[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

        // LIGHT1 - ضوء من تحت (fill light)
        float fillPos[4] = {0.0f, -5.0f, 0.0f, 1.0f};
        float fillDiff[4] = {0.2f, 0.3f, 0.2f, 1.0f};
        glLightfv(GL_LIGHT1, GL_POSITION, fillPos);
        glLightfv(GL_LIGHT1, GL_DIFFUSE,  fillDiff);

        // خصائص المواد
        float matSpec[4] = {0.5f, 0.5f, 0.5f, 1.0f};
        glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpec);
        glMaterialf (GL_FRONT, GL_SHININESS, 32.0f);
    }

    void toggleNight() { nightMode = !nightMode; }
};
