#pragma once
#include <cmath>

// =============================================
// AABB Collision Detection (Axis-Aligned Bounding Box)
// =============================================

struct AABB {
    float x, y, z;     // مركز الـ box
    float hw, hh, hd;  // نص العرض، نص الارتفاع، نص العمق

    AABB() : x(0),y(0),z(0),hw(0.5f),hh(0.5f),hd(0.5f) {}
    AABB(float x,float y,float z,float hw,float hh,float hd)
        : x(x),y(y),z(z),hw(hw),hh(hh),hd(hd) {}
};

// هل في تصادم بين boxين؟
inline bool checkAABB(const AABB& a, const AABB& b) {
    return (fabsf(a.x - b.x) < (a.hw + b.hw)) &&
           (fabsf(a.y - b.y) < (a.hh + b.hh)) &&
           (fabsf(a.z - b.z) < (a.hd + b.hd));
}

// تصادم 2D (بنتجاهل Y - الأهم في الفروج)
inline bool checkAABB2D(const AABB& a, const AABB& b) {
    return (fabsf(a.x - b.x) < (a.hw + b.hw)) &&
           (fabsf(a.z - b.z) < (a.hd + b.hd));
}

// هل الضفدعة وصلت للهدف؟
inline bool reachedGoal(float fx, float fz, float gx, float gz, float radius=1.5f) {
    float dx = fx - gx;
    float dz = fz - gz;
    return (dx*dx + dz*dz) < radius*radius;
}
