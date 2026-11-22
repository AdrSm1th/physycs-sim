#include "PhysycsMath.h"

void computeRot(Vec2 &vRotA, Vec2 &vRotB, float wa, float wb, Vec2 rA,
                Vec2 rB) {
    vRotA = {-wa * rA.y, wa * rA.x};
    vRotB = {-wb * rB.y, wb * rB.x};
}