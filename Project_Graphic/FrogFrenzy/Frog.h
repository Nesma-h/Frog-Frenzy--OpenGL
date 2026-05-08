#pragma once
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include "Collision.h"
#include "Texture.h"

// =============================================
// الضفدعة - بها animation للقفز
// =============================================

class Frog {
public:
    float x, y, z;
    float targetX, targetZ;
    float startX, startZ, startY;

    // Animation
    bool  isJumping;
    float jumpTime;        // 0..1
    float jumpDuration;    // ثواني
    float jumpHeight;      // ارتفاع القفزة
    float rotation;        // اتجاه الضفدعة

    // حالة
    bool  isDead;
    bool  isWinner;
    bool  onLog;
    float logRideSpeed;

    // Animation الضفدعة (أرجل)
    float legAnim;
    float legAnimSpeed;
    bool  legAnimDir;

    // Squish animation لما تموت
    float squishScale;
    float squishTime;

    int lives;
    int score;
    int level;

    Frog() {
        reset();
        lives = 3;
        score = 0;
        level = 1;
    }

    void reset() {
        x = 0.0f; y = 0.0f; z = 8.0f;
        targetX = 0.0f; targetZ = 8.0f;
        startX = 0.0f; startZ = 8.0f; startY = 0.0f;
        isJumping  = false;
        jumpTime   = 0.0f;
        jumpDuration = 0.25f;
        jumpHeight = 1.5f;
        rotation   = 0.0f;
        isDead     = false;
        isWinner   = false;
        onLog      = false;
        logRideSpeed = 0.0f;
        legAnim    = 0.0f;
        legAnimSpeed = 5.0f;
        legAnimDir = true;
        squishScale = 1.0f;
        squishTime  = 0.0f;
    }

    void moveForward()  { if(!isJumping && !isDead) startJump(x,        z - 2.0f,   0.0f); }
    void moveBackward() { if(!isJumping && !isDead) startJump(x,        z + 2.0f, 180.0f); }
    void moveLeft()     { if(!isJumping && !isDead) startJump(x - 2.0f, z,        270.0f); }
    void moveRight()    { if(!isJumping && !isDead) startJump(x + 2.0f, z,         90.0f); }

    void startJump(float tx, float tz, float rot) {
        if (isJumping) return;
        targetX = tx; targetZ = tz;
        startX  = x;  startZ  = z;  startY = y;
        rotation = rot;
        isJumping = true;
        jumpTime  = 0.0f;
    }

    void update(float dt) {
        if (isDead) {
            squishTime += dt;
            if (squishTime < 0.3f) squishScale = 1.0f - squishTime * 2.0f;
            else squishScale = 0.4f;
            return;
        }

        if (isJumping) {
            jumpTime += dt / jumpDuration;
            if (jumpTime >= 1.0f) {
                jumpTime = 1.0f;
                x = targetX; z = targetZ;
                y = startY;
                isJumping = false;
                score += 10;
            } else {
                // منحنى parabolic للقفزة
                float t = jumpTime;
                x = startX + (targetX - startX) * t;
                z = startZ + (targetZ - startZ) * t;
                y = startY + jumpHeight * (4.0f * t * (1.0f - t));
            }
        }

        // animation الأرجل
        if (isJumping) {
            legAnim += dt * 15.0f;
        } else {
            legAnim *= 0.85f;
        }

        // ركوب على السجلة
        if (onLog && !isJumping) {
            x += logRideSpeed * dt;
        }
    }

    void die() {
        if (isDead) return;
        isDead = true;
        squishTime = 0.0f;
        squishScale = 1.0f;
        lives--;
    }

    AABB getAABB() const {
        return AABB(x, y + 0.5f, z, 0.6f, 0.5f, 0.6f);
    }

    void draw(TextureManager& tex) {
        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(rotation, 0, 1, 0);

        if (isDead) {
            glScalef(1.5f + squishTime, squishScale, 1.5f + squishTime);
        }

        glEnable(GL_TEXTURE_2D);
        tex.bind(tex.texFrog);

        drawBody();
        drawHead();
        drawLegs();
        drawEyes();

        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }

private:
    void drawBody() {
        glColor3f(0.2f, 0.8f, 0.2f);
        // جسم بيضاوي
        glPushMatrix();
        glScalef(0.7f, 0.4f, 0.9f);
        drawSphere(0.7f, 12, 8);
        glPopMatrix();
    }

    void drawHead() {
        glColor3f(0.25f, 0.85f, 0.25f);
        glPushMatrix();
        glTranslatef(0, 0.2f, -0.55f);
        glScalef(0.55f, 0.35f, 0.45f);
        drawSphere(0.7f, 10, 8);
        glPopMatrix();
    }

    void drawLegs() {
        float kick = sinf(legAnim) * 0.4f;

        // رجلين أماميين
        glColor3f(0.15f, 0.7f, 0.15f);
        for(int side=-1; side<=1; side+=2) {
            glPushMatrix();
            glTranslatef(side * 0.6f, -0.15f, -0.3f);
            glRotatef(-20 + kick*30, 0,0,1);
            glScalef(0.15f, 0.12f, 0.5f);
            drawCube();
            glPopMatrix();
        }

        // رجلين خلفيين (أكبر)
        for(int side=-1; side<=1; side+=2) {
            glPushMatrix();
            glTranslatef(side * 0.65f, -0.2f, 0.4f);
            glRotatef(side*(30 - kick*50), 0,0,1);
            glScalef(0.18f, 0.15f, 0.7f);
            drawCube();
            glPopMatrix();
        }
    }

    void drawEyes() {
        glColor3f(1.0f, 1.0f, 0.0f);
        for(int side=-1; side<=1; side+=2) {
            glPushMatrix();
            glTranslatef(side * 0.25f, 0.32f, -0.7f);
            drawSphere(0.12f, 8, 6);
            // بؤبؤ العين
            glColor3f(0.0f, 0.0f, 0.0f);
            glTranslatef(0, 0, -0.08f);
            drawSphere(0.07f, 6, 4);
            glColor3f(1.0f, 1.0f, 0.0f);
            glPopMatrix();
        }
    }

    void drawSphere(float r, int slices, int stacks) {
        GLUquadric* q = gluNewQuadric();
        gluQuadricTexture(q, GL_TRUE);
        gluSphere(q, r, slices, stacks);
        gluDeleteQuadric(q);
    }

    void drawCube() {
        float v[8][3] = {
            {-0.5f,-0.5f,-0.5f},{0.5f,-0.5f,-0.5f},
            {0.5f, 0.5f,-0.5f},{-0.5f, 0.5f,-0.5f},
            {-0.5f,-0.5f, 0.5f},{0.5f,-0.5f, 0.5f},
            {0.5f, 0.5f, 0.5f},{-0.5f, 0.5f, 0.5f}
        };
        float tc[4][2] = {{0,0},{1,0},{1,1},{0,1}};
        int faces[6][4] = {
            {0,1,2,3},{4,5,6,7},{0,4,7,3},
            {1,5,6,2},{0,1,5,4},{3,2,6,7}
        };
        float normals[6][3] = {
            {0,0,-1},{0,0,1},{-1,0,0},
            {1,0,0},{0,-1,0},{0,1,0}
        };
        glBegin(GL_QUADS);
        for(int f=0;f<6;f++) {
            glNormal3fv(normals[f]);
            for(int i=0;i<4;i++) {
                glTexCoord2fv(tc[i]);
                glVertex3fv(v[faces[f][i]]);
            }
        }
        glEnd();
    }
};
