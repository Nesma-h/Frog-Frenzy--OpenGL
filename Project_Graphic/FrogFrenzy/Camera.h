#pragma once
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

enum CameraMode {
    CAM_FOLLOW,      // كاميرا بتتبع الضفدعة
    CAM_TOP,         // نظرة من فوق
    CAM_FREE         // كاميرا حرة
};

class Camera {
public:
    float eyeX, eyeY, eyeZ;
    float centerX, centerY, centerZ;
    float upX, upY, upZ;
    CameraMode mode;

    // للكاميرا الحرة
    float yaw, pitch;
    float freeX, freeY, freeZ;

    Camera() {
        mode = CAM_FOLLOW;
        eyeX = 0; eyeY = 8; eyeZ = 12;
        centerX = 0; centerY = 0; centerZ = 0;
        upX = 0; upY = 1; upZ = 0;
        yaw = -90.0f; pitch = -20.0f;
        freeX = 0; freeY = 10; freeZ = 15;
    }

    void setFollowTarget(float tx, float ty, float tz) {
        if (mode == CAM_FOLLOW) {
            eyeX = tx;
            eyeY = ty + 8.0f;
            eyeZ = tz + 12.0f;
            centerX = tx;
            centerY = ty;
            centerZ = tz;
        }
    }

    void setTopView(float tx, float ty, float tz) {
        if (mode == CAM_TOP) {
            eyeX = tx;
            eyeY = ty + 20.0f;
            eyeZ = tz;
            centerX = tx;
            centerY = ty;
            centerZ = tz;
        }
    }

    void applyFreeCamera() {
        float radYaw   = yaw   * 3.14159f / 180.0f;
        float radPitch = pitch * 3.14159f / 180.0f;
        float dx = cos(radPitch) * cos(radYaw);
        float dy = sin(radPitch);
        float dz = cos(radPitch) * sin(radYaw);
        centerX = freeX + dx;
        centerY = freeY + dy;
        centerZ = freeZ + dz;
        eyeX = freeX; eyeY = freeY; eyeZ = freeZ;
    }

    void apply() {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        if (mode == CAM_FREE) applyFreeCamera();
        gluLookAt(eyeX, eyeY, eyeZ,
                  centerX, centerY, centerZ,
                  upX, upY, upZ);
    }

    void switchMode() {
        if (mode == CAM_FOLLOW)      mode = CAM_TOP;
        else if (mode == CAM_TOP)    mode = CAM_FREE;
        else                          mode = CAM_FOLLOW;
    }

    void moveFree(float dx, float dy, float dz) {
        freeX += dx; freeY += dy; freeZ += dz;
    }

    void rotateFree(float dyaw, float dpitch) {
        yaw   += dyaw;
        pitch += dpitch;
        if (pitch >  89.0f) pitch =  89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }
};
