#pragma once
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

// =============================================
// Texture Manager - بيولّد Procedural Textures
// بدون ما نحتاج ملفات صور خارجية
// =============================================

class TextureManager {
public:
    GLuint texRoad;
    GLuint texGrass;
    GLuint texFrog;
    GLuint texCar;
    GLuint texWater;
    GLuint texLog;
    GLuint texSky;

    void init() {
        texRoad  = makeRoadTexture();
        texGrass = makeGrassTexture();
        texFrog  = makeFrogTexture();
        texCar   = makeCarTexture();
        texWater = makeWaterTexture();
        texLog   = makeLogTexture();
        texSky   = makeSkyTexture();
    }

    void bind(GLuint tex) {
        glBindTexture(GL_TEXTURE_2D, tex);
    }

private:
    GLuint createTexture(unsigned char* data, int w, int h) {
        GLuint id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);
        free(data);
        return id;
    }

    unsigned char* allocTex(int w, int h) {
        return (unsigned char*)malloc(w * h * 3);
    }

    void setPixel(unsigned char* d, int w, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
        int idx = (y * w + x) * 3;
        d[idx] = r; d[idx+1] = g; d[idx+2] = b;
    }

    // ---- طريق رمادي مع خطوط صفراء ----
    GLuint makeRoadTexture() {
        int W=128, H=128;
        unsigned char* d = allocTex(W,H);
        for(int y=0;y<H;y++) for(int x=0;x<W;x++) {
            unsigned char grey = 60 + (rand()%15);
            // خط أصفر في المنتصف
            if(abs(x - W/2) < 4) {
                setPixel(d,W,x,y, 220,200,30);
            } else {
                setPixel(d,W,x,y, grey,grey,grey);
            }
        }
        return createTexture(d,W,H);
    }

    // ---- عشب أخضر ----
    GLuint makeGrassTexture() {
        int W=64, H=64;
        unsigned char* d = allocTex(W,H);
        for(int y=0;y<H;y++) for(int x=0;x<W;x++) {
            int noise = rand()%30;
            setPixel(d,W,x,y, 30+noise, 100+noise, 20+noise);
        }
        return createTexture(d,W,H);
    }

    // ---- ضفدعة خضراء ----
    GLuint makeFrogTexture() {
        int W=64, H=64;
        unsigned char* d = allocTex(W,H);
        for(int y=0;y<H;y++) for(int x=0;x<W;x++) {
            // نقش بقع الضفدعة
            int bx = x/8, by = y/8;
            bool spot = ((bx+by)%3 == 0);
            if(spot)
                setPixel(d,W,x,y, 20, 130, 20);
            else
                setPixel(d,W,x,y, 50, 180, 50);
        }
        return createTexture(d,W,H);
    }

    // ---- سيارة ملونة ----
    GLuint makeCarTexture() {
        int W=64, H=32;
        unsigned char* d = allocTex(W,H);
        for(int y=0;y<H;y++) for(int x=0;x<W;x++) {
            // جسم السيارة
            if(y < H*0.7f)
                setPixel(d,W,x,y, 200,50,50);
            else
                setPixel(d,W,x,y, 40,40,40);
            // نوافذ
            if(x>W*0.2f && x<W*0.8f && y>H*0.1f && y<H*0.5f)
                setPixel(d,W,x,y, 150,200,255);
        }
        return createTexture(d,W,H);
    }

    // ---- ماء ----
    GLuint makeWaterTexture() {
        int W=128, H=128;
        unsigned char* d = allocTex(W,H);
        for(int y=0;y<H;y++) for(int x=0;x<W;x++) {
            int wave = (int)(20 * sin(x*0.3f + y*0.1f));
            setPixel(d,W,x,y, 20, 80+wave, 160+wave/2);
        }
        return createTexture(d,W,H);
    }

    // ---- جذع شجرة ----
    GLuint makeLogTexture() {
        int W=64, H=32;
        unsigned char* d = allocTex(W,H);
        for(int y=0;y<H;y++) for(int x=0;x<W;x++) {
            int grain = (int)(10*sin(y*1.5f)) + rand()%10;
            setPixel(d,W,x,y, 120+grain, 70+grain/2, 30);
        }
        return createTexture(d,W,H);
    }

    // ---- سماء ----
    GLuint makeSkyTexture() {
        int W=128, H=64;
        unsigned char* d = allocTex(W,H);
        for(int y=0;y<H;y++) for(int x=0;x<W;x++) {
            float t = (float)y/H;
            unsigned char r = (unsigned char)(135 - 50*t);
            unsigned char g = (unsigned char)(206 - 80*t);
            unsigned char b = (unsigned char)(235 - 50*t);
            setPixel(d,W,x,y, r,g,b);
        }
        return createTexture(d,W,H);
    }
};
