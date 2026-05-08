// =============================================
//   FROG FRENZY - OpenGL 3D Game
//   Using: OpenGL + GLUT only (no extra libs)
// =============================================
// Controls:
//   Arrow Keys / WASD  = Move frog
//   C                  = Switch camera mode
//   N                  = Toggle night mode
//   R                  = Restart
//   ESC                = Quit
//   Mouse drag (free cam) = Rotate camera
// =============================================

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>

#include "Camera.h"
#include "Lighting.h"
#include "Texture.h"
#include "Collision.h"
#include "Frog.h"
#include "Car.h"

// =============================================
// Game State
// =============================================
static int   g_windowW = 900, g_windowH = 700;
static float g_lastTime = 0.0f;
static float g_dt       = 0.0f;

// Water animation
static float g_waterAnim = 0.0f;

// Mouse (للكاميرا الحرة)
static int   g_lastMouseX = 0, g_lastMouseY = 0;
static bool  g_mouseDown = false;

// Message display
static char  g_message[64] = "";
static float g_messageTimer = 0.0f;

// Game objects
static Frog          g_frog;
static Camera        g_camera;
static Lighting      g_light;
static TextureManager g_tex;

// Obstacles
static std::vector<Obstacle> g_obstacles;

// Goal positions (حمامات الماء في نهاية المسار)
struct Goal { float x, z; bool reached; };
static Goal g_goals[5];
static int  g_goalsReached = 0;

// Particle system (للاحتفال)
struct Particle {
    float x,y,z, vx,vy,vz;
    float r,g,b;
    float life;
    bool active;
};
static Particle g_particles[200];

// =============================================
// Lane definitions
// =============================================
// Z < 0  = اتجاه الهدف (الماء)
// الممر: عشب -> طريق (سيارات) -> عشب -> ماء (سجلات)
// z= 8   = بداية الضفدعة (عشب)
// z= 0.. -12 = طريق (5 lanes)
// z=-14..-18 = ماء (سجلات)
// z=-20  = الهدف

struct Lane {
    float z;
    bool  isWater;   // true = lane ماء (سجلات)
    bool  isRoad;    // true = lane طريق (سيارات)
    bool  isSafe;    // true = عشب أو رصيف
};

static Lane g_lanes[] = {
    {  8.0f, false, false, true  },   // البداية - عشب
    {  6.0f, false, false, true  },   // عشب
    {  4.0f, false, true,  false },   // طريق
    {  2.0f, false, true,  false },   // طريق
    {  0.0f, false, true,  false },   // طريق
    { -2.0f, false, true,  false },   // طريق
    { -4.0f, false, true,  false },   // طريق
    { -6.0f, false, false, true  },   // رصيف وسط
    { -8.0f, true,  false, false },   // ماء
    {-10.0f, true,  false, false },   // ماء
    {-12.0f, true,  false, false },   // ماء
    {-14.0f, false, false, true  },   // بر (الهدف)
};
static const int NUM_LANES = 12;

// =============================================
// Helper: رسم الأرض مع texture
// =============================================
void drawTexturedPlane(float cx, float cz, float w, float d,
                       float r, float gg, float b, GLuint tex) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor3f(r, gg, b);
    float hw = w*0.5f, hd = d*0.5f;
    float tx = w/4.0f, tz = d/4.0f;
    glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glTexCoord2f(0,  0  ); glVertex3f(cx-hw, 0, cz-hd);
        glTexCoord2f(tx, 0  ); glVertex3f(cx+hw, 0, cz-hd);
        glTexCoord2f(tx, tz ); glVertex3f(cx+hw, 0, cz+hd);
        glTexCoord2f(0,  tz ); glVertex3f(cx-hw, 0, cz+hd);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// =============================================
// رسم الماء المتحرك
// =============================================
void drawWater(float z, float w, float d) {
    glEnable(GL_TEXTURE_2D);
    g_tex.bind(g_tex.texWater);
    glColor4f(0.3f, 0.6f, 0.9f, 0.85f);

    float hw=w*0.5f, hd=d*0.5f;
    float offset = g_waterAnim * 0.1f;

    glBegin(GL_QUADS);
    for(int ix=0; ix<10; ix++) {
        for(int iz=0; iz<4; iz++) {
            float x0 = -hw + ix*w/10.0f;
            float x1 = x0 + w/10.0f;
            float z0 = z - hd + iz*d/4.0f;
            float z1 = z0 + d/4.0f;
            float wave = sinf(x0*0.5f + g_waterAnim)*0.05f;
            glNormal3f(0,1,0);
            glTexCoord2f(ix/10.0f + offset, iz/4.0f + offset);
            glVertex3f(x0, wave, z0);
            glTexCoord2f((ix+1)/10.0f + offset, iz/4.0f + offset);
            glVertex3f(x1, wave, z0);
            glTexCoord2f((ix+1)/10.0f + offset, (iz+1)/4.0f + offset);
            glVertex3f(x1, wave, z1);
            glTexCoord2f(ix/10.0f + offset, (iz+1)/4.0f + offset);
            glVertex3f(x0, wave, z1);
        }
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// =============================================
// Particle System
// =============================================
void spawnParticles(float px, float py, float pz) {
    for(int i=0;i<200;i++) {
        Particle& p = g_particles[i];
        p.x=px; p.y=py; p.z=pz;
        p.vx = ((float)(rand()%200)-100)/100.0f * 3.0f;
        p.vy = ((float)(rand()%200))/200.0f * 5.0f + 2.0f;
        p.vz = ((float)(rand()%200)-100)/100.0f * 3.0f;
        p.r  = (float)(rand()%100)/100.0f;
        p.g  = (float)(rand()%100)/100.0f;
        p.b  = (float)(rand()%100)/100.0f;
        p.life = 1.5f;
        p.active = true;
    }
}

void updateParticles(float dt) {
    for(auto& p : g_particles) {
        if(!p.active) continue;
        p.x += p.vx*dt; p.y += p.vy*dt; p.z += p.vz*dt;
        p.vy -= 5.0f*dt;
        p.life -= dt;
        if(p.life <= 0) p.active = false;
    }
}

void drawParticles() {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for(auto& p : g_particles) {
        if(!p.active) continue;
        float alpha = p.life > 1.0f ? 1.0f : p.life;
        glColor4f(p.r, p.g, p.b, alpha);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
    glPointSize(1.0f);
    glEnable(GL_LIGHTING);
}

// =============================================
// رسم الشجر والديكور
// =============================================
void drawTree(float tx, float tz) {
    glDisable(GL_TEXTURE_2D);
    // جذع
    glColor3f(0.4f, 0.25f, 0.1f);
    glPushMatrix();
    glTranslatef(tx, 0, tz);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, 0.2f, 0.15f, 1.5f, 8, 3);
    gluDeleteQuadric(q);
    // ورق
    glColor3f(0.1f, 0.6f, 0.1f);
    glTranslatef(0, 1.5f, 0);
    glutSolidCone(1.0f, 2.5f, 10, 5);
    glPopMatrix();
}

void drawGoalPad(float gx, float gz, bool reached) {
    glDisable(GL_TEXTURE_2D);
    if(reached) {
        glColor3f(0.2f, 1.0f, 0.2f);
        // فيها ضفدعة صغيرة
        glPushMatrix();
        glTranslatef(gx, 0.1f, gz);
        glColor3f(0.0f, 0.8f, 0.0f);
        glutSolidSphere(0.4f, 8, 6);
        glPopMatrix();
    } else {
        glColor3f(0.1f, 0.5f, 0.1f);
    }
    glPushMatrix();
    glTranslatef(gx, 0.05f, gz);
    glScalef(1.5f, 0.1f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// =============================================
// Setup Obstacles
// =============================================
void setupObstacles() {
    g_obstacles.clear();

    // ===== طريق (سيارات) =====
    // Lane z=4
    for(int i=0;i<3;i++) {
        Obstacle o; o.type=OBS_CAR;
        o.z=4.0f; o.x=-15.0f + i*10.0f;
        o.speed=4.5f*(g_frog.level);
        o.colorR=0.8f; o.colorG=0.1f; o.colorB=0.1f;
        g_obstacles.push_back(o);
    }
    // Lane z=2 (عكس)
    for(int i=0;i<4;i++) {
        Obstacle o; o.type=OBS_CAR;
        o.z=2.0f; o.x=15.0f - i*8.0f;
        o.speed=-3.5f*(g_frog.level);
        o.width=3.5f; o.colorR=0.2f; o.colorG=0.2f; o.colorB=0.8f;
        o.type=OBS_TRUCK;
        g_obstacles.push_back(o);
    }
    // Lane z=0
    for(int i=0;i<3;i++) {
        Obstacle o; o.type=OBS_CAR;
        o.z=0.0f; o.x=-10.0f + i*12.0f;
        o.speed=5.5f*(g_frog.level);
        o.colorR=0.9f; o.colorG=0.5f; o.colorB=0.0f;
        g_obstacles.push_back(o);
    }
    // Lane z=-2 (عكس)
    for(int i=0;i<3;i++) {
        Obstacle o; o.type=OBS_CAR;
        o.z=-2.0f; o.x=12.0f - i*9.0f;
        o.speed=-6.0f*(g_frog.level);
        o.colorR=0.8f; o.colorG=0.0f; o.colorB=0.8f;
        g_obstacles.push_back(o);
    }
    // Lane z=-4
    for(int i=0;i<2;i++) {
        Obstacle o; o.type=OBS_TRUCK;
        o.z=-4.0f; o.x=-8.0f + i*14.0f;
        o.speed=3.0f*(g_frog.level);
        o.width=4.0f;
        o.colorR=0.2f; o.colorG=0.6f; o.colorB=0.1f;
        g_obstacles.push_back(o);
    }

    // ===== ماء (سجلات) =====
    // Lane z=-8
    for(int i=0;i<3;i++) {
        Obstacle o; o.type=OBS_LOG;
        o.z=-8.0f; o.x=-12.0f + i*10.0f;
        o.speed=2.5f; o.width=4.0f; o.y=0.0f;
        o.depth=1.0f;
        g_obstacles.push_back(o);
    }
    // Lane z=-10 (عكس)
    for(int i=0;i<3;i++) {
        Obstacle o; o.type=OBS_LOG;
        o.z=-10.0f; o.x=10.0f - i*11.0f;
        o.speed=-3.0f; o.width=5.0f; o.y=0.0f;
        o.depth=1.0f;
        g_obstacles.push_back(o);
    }
    // Lane z=-12
    for(int i=0;i<2;i++) {
        Obstacle o; o.type=OBS_LOG;
        o.z=-12.0f; o.x=-6.0f + i*13.0f;
        o.speed=2.0f; o.width=6.0f; o.y=0.0f;
        o.depth=1.0f;
        g_obstacles.push_back(o);
    }

    // Goals
    for(int i=0;i<5;i++) {
        g_goals[i].x = -8.0f + i*4.0f;
        g_goals[i].z = -15.0f;
        g_goals[i].reached = false;
    }
    g_goalsReached = 0;
}

// =============================================
// HUD (2D text overlay)
// =============================================
void drawText2D(float x, float y, const char* txt, float r, float gg, float b) {
    glColor3f(r, gg, b);
    glRasterPos2f(x, y);
    for(const char* c = txt; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

void drawHUD() {
    // التبديل لـ 2D
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, g_windowW, 0, g_windowH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    char buf[128];

    // Score
    snprintf(buf, sizeof(buf), "Score: %d", g_frog.score);
    drawText2D(10, g_windowH-30, buf, 1,1,0);

    // Lives
    snprintf(buf, sizeof(buf), "Lives: %d", g_frog.lives);
    drawText2D(10, g_windowH-55, buf, 1,0.3f,0.3f);

    // Level
    snprintf(buf, sizeof(buf), "Level: %d", g_frog.level);
    drawText2D(10, g_windowH-80, buf, 0.3f,1,0.3f);

    // Goals
    snprintf(buf, sizeof(buf), "Goals: %d/5", g_goalsReached);
    drawText2D(10, g_windowH-105, buf, 0.3f,0.8f,1);

    // Camera mode
    const char* camNames[] = {"Follow","Top","Free"};
    snprintf(buf, sizeof(buf), "Cam: %s [C]", camNames[(int)g_camera.mode]);
    drawText2D(g_windowW-150, g_windowH-30, buf, 0.8f,0.8f,0.8f);

    // Night toggle
    snprintf(buf, sizeof(buf), "Night: %s [N]", g_light.nightMode?"ON":"OFF");
    drawText2D(g_windowW-150, g_windowH-55, buf, 0.6f,0.6f,1);

    // Controls
    drawText2D(10, 55, "Arrow/WASD: Move", 0.7f,0.7f,0.7f);
    drawText2D(10, 35, "C: Camera | N: Night | R: Restart", 0.7f,0.7f,0.7f);
    drawText2D(10, 15, "ESC: Quit", 0.7f,0.7f,0.7f);

    // Message center
    if(g_messageTimer > 0) {
        float alpha = g_messageTimer > 1.0f ? 1.0f : g_messageTimer;
        glColor4f(1,1,0,alpha);
        int len = strlen(g_message) * 10;
        glRasterPos2f((g_windowW - len)*0.5f, g_windowH*0.55f);
        for(const char* c = g_message; *c; c++)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    // Game Over
    if(g_frog.lives <= 0) {
        drawText2D(g_windowW/2.0f-80, g_windowH/2.0f+20, "GAME OVER!", 1,0.1f,0.1f);
        snprintf(buf,sizeof(buf),"Score: %d  Press R to restart", g_frog.score);
        drawText2D(g_windowW/2.0f-130, g_windowH/2.0f-10, buf, 1,1,1);
    }

    // Win!
    if(g_goalsReached >= 5) {
        drawText2D(g_windowW/2.0f-70, g_windowH/2.0f+20, "YOU WIN!", 0.1f,1,0.1f);
        snprintf(buf,sizeof(buf),"Score: %d  Level %d! Press R", g_frog.score, g_frog.level);
        drawText2D(g_windowW/2.0f-120, g_windowH/2.0f-10, buf, 1,1,1);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// =============================================
// Draw Sky Background
// =============================================
void drawSkybox() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    g_tex.bind(g_tex.texSky);

    // سماء بسيطة (quad كبير في الخلف)
    if(g_light.nightMode) {
        glColor3f(0.02f, 0.02f, 0.1f);
    } else {
        float t = sinf(g_light.sunAngle * 3.14159f/180.0f);
        glColor3f(0.5f + 0.3f*t, 0.7f + 0.2f*t, 0.9f);
    }

    glPushMatrix();
    float skySize = 80.0f;
    // سطح علوي
    glBegin(GL_QUADS);
        glTexCoord2f(0,0); glVertex3f(-skySize, 30,  skySize);
        glTexCoord2f(1,0); glVertex3f( skySize, 30,  skySize);
        glTexCoord2f(1,1); glVertex3f( skySize, 30, -skySize);
        glTexCoord2f(0,1); glVertex3f(-skySize, 30, -skySize);
    glEnd();
    // الجوانب
    glColor3f(0.6f, 0.7f, 0.85f);
    glBegin(GL_QUADS);
        // أمام
        glTexCoord2f(0,0); glVertex3f(-skySize, -5, -skySize);
        glTexCoord2f(1,0); glVertex3f( skySize, -5, -skySize);
        glTexCoord2f(1,1); glVertex3f( skySize, 30, -skySize);
        glTexCoord2f(0,1); glVertex3f(-skySize, 30, -skySize);
        // خلف
        glTexCoord2f(0,0); glVertex3f( skySize, -5,  skySize);
        glTexCoord2f(1,0); glVertex3f(-skySize, -5,  skySize);
        glTexCoord2f(1,1); glVertex3f(-skySize, 30,  skySize);
        glTexCoord2f(0,1); glVertex3f( skySize, 30,  skySize);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// =============================================
// Draw World
// =============================================
void drawWorld() {
    float worldW = 30.0f;

    // ارسم كل lane
    for(int i=0; i<NUM_LANES; i++) {
        Lane& ln = g_lanes[i];

        if(ln.isWater) {
            drawWater(ln.z, worldW, 2.2f);
        } else if(ln.isRoad) {
            drawTexturedPlane(0, ln.z, worldW, 2.2f, 1,1,1, g_tex.texRoad);
        } else {
            drawTexturedPlane(0, ln.z, worldW, 2.2f, 1,1,1, g_tex.texGrass);
        }
    }

    // رصيف (border)
    drawTexturedPlane(0, -15.0f, worldW, 2.0f, 0.7f,0.7f,0.7f, g_tex.texGrass);

    // الحدود الجانبية (عشب)
    drawTexturedPlane(-16.0f, -4.0f, 2.0f, 32.0f, 1,1,1, g_tex.texGrass);
    drawTexturedPlane( 16.0f, -4.0f, 2.0f, 32.0f, 1,1,1, g_tex.texGrass);

    // أشجار جانبية
    for(int i=-3;i<=3;i++) {
        drawTree(-17.0f, i*4.0f);
        drawTree( 17.0f, i*4.0f - 2.0f);
    }

    // Goal pads
    for(int i=0;i<5;i++) {
        drawGoalPad(g_goals[i].x, g_goals[i].z, g_goals[i].reached);
    }
}

// =============================================
// Collision Checks
// =============================================
void checkCollisions() {
    if(g_frog.isDead || g_frog.isJumping) return;

    AABB fAABB = g_frog.getAABB();
    bool onAnyLog = false;

    for(auto& obs : g_obstacles) {
        if(!obs.active) continue;
        AABB oAABB = obs.getAABB();

        if(checkAABB2D(fAABB, oAABB)) {
            if(obs.type == OBS_CAR || obs.type == OBS_TRUCK) {
                // تصادم مع سيارة -> موت
                g_frog.die();
                snprintf(g_message, sizeof(g_message), "SQUASH!");
                g_messageTimer = 2.0f;
                spawnParticles(g_frog.x, 0.5f, g_frog.z);
                return;
            } else if(obs.type == OBS_LOG) {
                // على سجلة -> آمن
                onAnyLog = true;
                g_frog.onLog = true;
                g_frog.logRideSpeed = obs.speed;
            }
        }
    }

    // في منطقة الماء؟
    bool inWater = (g_frog.z <= -7.5f && g_frog.z >= -13.5f);
    if(inWater && !onAnyLog) {
        // غرق!
        g_frog.die();
        snprintf(g_message, sizeof(g_message), "SPLASH!");
        g_messageTimer = 2.0f;
        spawnParticles(g_frog.x, 0.5f, g_frog.z);
        return;
    }

    if(!inWater) {
        g_frog.onLog = false;
        g_frog.logRideSpeed = 0;
    }

    // وصل للهدف؟
    if(g_frog.z <= -14.5f && g_frog.z >= -16.0f) {
        for(int i=0;i<5;i++) {
            if(!g_goals[i].reached &&
               reachedGoal(g_frog.x, g_frog.z, g_goals[i].x, g_goals[i].z, 2.0f)) {
                g_goals[i].reached = true;
                g_goalsReached++;
                g_frog.score += 200;
                snprintf(g_message, sizeof(g_message), "NICE! +200");
                g_messageTimer = 2.0f;
                spawnParticles(g_frog.x, 1.0f, g_frog.z);

                // أعد الضفدعة للبداية
                g_frog.x = 0; g_frog.z = 8.0f;
                g_frog.targetX=0; g_frog.targetZ=8.0f;

                // لو وصل كل الـ goals -> next level
                if(g_goalsReached >= 5) {
                    g_frog.level++;
                    g_frog.score += 500;
                    snprintf(g_message, sizeof(g_message), "LEVEL UP!");
                    g_messageTimer = 3.0f;
                    setupObstacles(); // إعادة الإعداد مع سرعة أكبر
                }
                return;
            }
        }
    }

    // تأكد أن الضفدعة مش تخرج من الحدود
    if(g_frog.x < -14.0f) g_frog.x = g_frog.targetX = -14.0f;
    if(g_frog.x >  14.0f) g_frog.x = g_frog.targetX =  14.0f;
    if(g_frog.z >  10.0f) g_frog.z = g_frog.targetZ =  10.0f;
    if(g_frog.z < -16.0f) g_frog.z = g_frog.targetZ = -16.0f;
}

// =============================================
// Respawn after death
// =============================================
void respawnFrog() {
    if(g_frog.isDead && g_frog.squishTime > 1.5f) {
        if(g_frog.lives > 0) {
            g_frog.x = 0; g_frog.y = 0; g_frog.z = 8.0f;
            g_frog.targetX=0; g_frog.targetZ=8.0f;
            g_frog.isDead = false;
            g_frog.squishScale = 1.0f;
            g_frog.onLog = false;
        }
    }
}

// =============================================
// GLUT Callbacks
// =============================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // كاميرا
    switch(g_camera.mode) {
        case CAM_FOLLOW: g_camera.setFollowTarget(g_frog.x, g_frog.y, g_frog.z); break;
        case CAM_TOP:    g_camera.setTopView(g_frog.x, g_frog.y, g_frog.z); break;
        case CAM_FREE:   break;
    }
    g_camera.apply();

    // إضاءة
    g_light.apply();

    // سكاي
    drawSkybox();

    // العالم
    drawWorld();

    // العوائق
    for(auto& obs : g_obstacles) {
        obs.draw(g_tex);
    }

    // الضفدعة
    g_frog.draw(g_tex);

    // Particles
    drawParticles();

    // HUD
    drawHUD();

    glutSwapBuffers();
}

void idle() {
    float now = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    g_dt = now - g_lastTime;
    if(g_dt > 0.05f) g_dt = 0.05f;
    g_lastTime = now;

    if(g_frog.lives > 0 && g_goalsReached < 5) {
        // تحديث الضفدعة
        g_frog.update(g_dt);

        // تحديث العوائق
        for(auto& obs : g_obstacles) obs.update(g_dt);

        // الماء
        g_waterAnim += g_dt;

        // إضاءة
        g_light.update(g_dt);

        // تصادم
        checkCollisions();

        // إعادة ولادة
        respawnFrog();

        // message timer
        if(g_messageTimer > 0) g_messageTimer -= g_dt;
    }

    updateParticles(g_dt);

    glutPostRedisplay();
}

void reshape(int w, int h) {
    g_windowW = w; g_windowH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)w/h, 0.1f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'w': case 'W': g_frog.moveForward();  break;
        case 's': case 'S': g_frog.moveBackward(); break;
        case 'a': case 'A': g_frog.moveLeft();     break;
        case 'd': case 'D': g_frog.moveRight();    break;
        case 'c': case 'C': g_camera.switchMode(); break;
        case 'n': case 'N': g_light.toggleNight(); break;
        case 'r': case 'R':
            g_frog.reset();
            g_frog.lives = 3;
            g_frog.score = 0;
            g_frog.level = 1;
            setupObstacles();
            break;
        case 27: exit(0); break; // ESC
        // كاميرا حرة
        case 'i': g_camera.moveFree(0,0,-0.5f); break;
        case 'k': g_camera.moveFree(0,0, 0.5f); break;
        case 'j': g_camera.moveFree(-0.5f,0,0); break;
        case 'l': g_camera.moveFree( 0.5f,0,0); break;
    }
}

void specialKeys(int key, int x, int y) {
    switch(key) {
        case GLUT_KEY_UP:    g_frog.moveForward();  break;
        case GLUT_KEY_DOWN:  g_frog.moveBackward(); break;
        case GLUT_KEY_LEFT:  g_frog.moveLeft();     break;
        case GLUT_KEY_RIGHT: g_frog.moveRight();    break;
    }
}

void mouse(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON) {
        g_mouseDown = (state == GLUT_DOWN);
        g_lastMouseX = x;
        g_lastMouseY = y;
    }
}

void mouseMotion(int x, int y) {
    if(g_mouseDown && g_camera.mode == CAM_FREE) {
        float dx = (x - g_lastMouseX) * 0.3f;
        float dy = (y - g_lastMouseY) * 0.3f;
        g_camera.rotateFree(dx, -dy);
        g_lastMouseX = x;
        g_lastMouseY = y;
        glutPostRedisplay();
    }
}

// =============================================
// main
// =============================================
int main(int argc, char** argv) {
    // Init GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(g_windowW, g_windowH);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("Frog Frenzy 3D - OpenGL");

    // OpenGL setup
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.4f, 0.6f, 0.8f, 1.0f);

    // Init subsystems
    g_tex.init();
    g_light.setup();
    setupObstacles();

    // Callbacks
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

    printf("=== FROG FRENZY 3D ===\n");
    printf("Arrow Keys / WASD = Move\n");
    printf("C = Switch Camera (Follow/Top/Free)\n");
    printf("N = Toggle Night Mode\n");
    printf("R = Restart\n");
    printf("ESC = Quit\n");
    printf("In Free Camera: I/J/K/L + Mouse Drag\n");

    glutMainLoop();
    return 0;
}
