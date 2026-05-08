#pragma once
#include <GL/gl.h>
#include <GL/glu.h>
#include "Collision.h"
#include "Texture.h"

// =============================================
// السيارات والسجلات (عوائق اللعبة)
// =============================================

enum ObstacleType { OBS_CAR, OBS_TRUCK, OBS_LOG };

class Obstacle {
public:
    float x, y, z;
    float speed;
    float width, height, depth;
    ObstacleType type;
    bool  active;

    // Animation الدواليب
    float wheelRot;

    // ألوان مختلفة للسيارات
    float colorR, colorG, colorB;

    Obstacle() : x(0),y(0),z(0),speed(4.0f),width(2.5f),height(1.0f),depth(1.5f),
                 type(OBS_CAR), active(true), wheelRot(0),
                 colorR(0.8f),colorG(0.1f),colorB(0.1f) {}

    void update(float dt) {
        if (!active) return;
        x += speed * dt;

        // دوران الدواليب
        wheelRot += fabsf(speed) * dt * 50.0f;
        if (wheelRot > 360.0f) wheelRot -= 360.0f;

        // لما يخرج من الشاشة، يرجع من الجهة التانية
        if (speed > 0 && x > 20.0f)  x = -20.0f;
        if (speed < 0 && x < -20.0f) x =  20.0f;
    }

    AABB getAABB() const {
        return AABB(x, y + height*0.5f, z, width*0.5f, height*0.5f, depth*0.5f);
    }

    void draw(TextureManager& tex) {
        if (!active) return;
        glPushMatrix();
        glTranslatef(x, y, z);

        if (type == OBS_LOG) {
            drawLog(tex);
        } else {
            drawCar(tex);
        }

        glPopMatrix();
    }

private:
    void drawCar(TextureManager& tex) {
        glEnable(GL_TEXTURE_2D);
        tex.bind(tex.texCar);

        // جسم السيارة الرئيسي
        glColor3f(colorR, colorG, colorB);
        glPushMatrix();
        glScalef(width, height*0.6f, depth);
        drawBox();
        glPopMatrix();

        // كابينة السيارة (علوية)
        glColor3f(colorR*0.8f, colorG*0.8f, colorB*0.8f);
        glPushMatrix();
        glTranslatef(0, height*0.55f, 0);
        glScalef(width*0.65f, height*0.5f, depth*0.85f);
        drawBox();
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);

        // دواليب
        drawWheels();

        // أضواء (نقاط برتقالية)
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.8f, 0.0f);
        float lx = (speed > 0) ? width*0.5f : -width*0.5f;
        for(int s=-1;s<=1;s+=2) {
            glPushMatrix();
            glTranslatef(lx, height*0.2f, s*depth*0.35f);
            GLUquadric* q = gluNewQuadric();
            gluSphere(q, 0.12f, 6, 4);
            gluDeleteQuadric(q);
            glPopMatrix();
        }
        glEnable(GL_LIGHTING);
    }

    void drawWheels() {
        glColor3f(0.15f, 0.15f, 0.15f);
        float wx[] = {width*0.4f, -width*0.4f, width*0.4f, -width*0.4f};
        float wz[] = {depth*0.45f, depth*0.45f, -depth*0.45f, -depth*0.45f};

        for(int i=0;i<4;i++) {
            glPushMatrix();
            glTranslatef(wx[i], 0, wz[i]);
            glRotatef(90, 0,1,0);
            glRotatef(wheelRot, 1,0,0);
            GLUquadric* q = gluNewQuadric();
            gluCylinder(q, 0.35f, 0.35f, 0.2f, 10, 2);
            gluDisk(q, 0, 0.35f, 10, 2);
            glTranslatef(0,0,0.2f);
            gluDisk(q, 0, 0.35f, 10, 2);
            gluDeleteQuadric(q);
            glPopMatrix();
        }
    }

    void drawLog(TextureManager& tex) {
        glEnable(GL_TEXTURE_2D);
        tex.bind(tex.texLog);

        glColor3f(0.6f, 0.4f, 0.2f);
        glPushMatrix();
        glRotatef(90, 0,1,0);
        GLUquadric* q = gluNewQuadric();
        gluQuadricTexture(q, GL_TRUE);
        gluCylinder(q, 0.5f, 0.5f, width, 12, 3);
        gluDisk(q, 0, 0.5f, 12, 2);
        glTranslatef(0,0,width);
        gluDisk(q, 0, 0.5f, 12, 2);
        gluDeleteQuadric(q);
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
    }

    void drawBox() {
        glBegin(GL_QUADS);
        // الوجوه الستة للمكعب
        float s = 0.5f;
        // الأمام
        glNormal3f(0,0,1); glTexCoord2f(0,0); glVertex3f(-s,-s, s);
                           glTexCoord2f(1,0); glVertex3f( s,-s, s);
                           glTexCoord2f(1,1); glVertex3f( s, s, s);
                           glTexCoord2f(0,1); glVertex3f(-s, s, s);
        // الخلف
        glNormal3f(0,0,-1); glTexCoord2f(0,0); glVertex3f( s,-s,-s);
                            glTexCoord2f(1,0); glVertex3f(-s,-s,-s);
                            glTexCoord2f(1,1); glVertex3f(-s, s,-s);
                            glTexCoord2f(0,1); glVertex3f( s, s,-s);
        // اليسار
        glNormal3f(-1,0,0); glTexCoord2f(0,0); glVertex3f(-s,-s,-s);
                            glTexCoord2f(1,0); glVertex3f(-s,-s, s);
                            glTexCoord2f(1,1); glVertex3f(-s, s, s);
                            glTexCoord2f(0,1); glVertex3f(-s, s,-s);
        // اليمين
        glNormal3f(1,0,0);  glTexCoord2f(0,0); glVertex3f( s,-s, s);
                            glTexCoord2f(1,0); glVertex3f( s,-s,-s);
                            glTexCoord2f(1,1); glVertex3f( s, s,-s);
                            glTexCoord2f(0,1); glVertex3f( s, s, s);
        // فوق
        glNormal3f(0,1,0);  glTexCoord2f(0,0); glVertex3f(-s, s, s);
                            glTexCoord2f(1,0); glVertex3f( s, s, s);
                            glTexCoord2f(1,1); glVertex3f( s, s,-s);
                            glTexCoord2f(0,1); glVertex3f(-s, s,-s);
        // تحت
        glNormal3f(0,-1,0); glTexCoord2f(0,0); glVertex3f(-s,-s,-s);
                            glTexCoord2f(1,0); glVertex3f( s,-s,-s);
                            glTexCoord2f(1,1); glVertex3f( s,-s, s);
                            glTexCoord2f(0,1); glVertex3f(-s,-s, s);
        glEnd();
    }
};
