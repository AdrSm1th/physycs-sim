#include "Collision.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <variant>

namespace Collision {
bool circleCircle(const Body& a, const Body& b, Manifold& m) {
    auto* ra = std::get_if<CircleShape>(&a.shape);
    auto* rb = std::get_if<CircleShape>(&b.shape);
    assert(ra && rb && "circleCircle called with non-circle body");

    Vec2 delta = b.position - a.position;

    float distSq = delta.lengthSquared();

    float radiusSum = ra->radius + rb->radius;

    if (distSq >= radiusSum * radiusSum) {
        m.valid = false;
        return false;
    }

    float dist = sqrt(distSq);

    if (dist < 1e-6) {
        m.normal = {0, 1};
    } else {
        m.normal = delta * (1 / dist);
    }

    m.points.push_back(a.position + m.normal * ra->radius);
    m.points[0].penetration = radiusSum - dist;
    m.valid = true;
    return true;
}

/* Вычисление происходят в локальных координатах бокса. В circleBox будут
вычисляться мировые координаты. Координаты бокса считаются (0, 0) */
bool circleBoxImpl(Vec2 posA, float radius, Vec2 halfExterns, Manifold& m) {
    Vec2 closest;
    closest.x = std::clamp(posA.x, -halfExterns.x, halfExterns.x);
    closest.y = std::clamp(posA.y, -halfExterns.y, halfExterns.y);

    Vec2 delta = closest - posA;

    float distSq = delta.lengthSquared();

    if (distSq >= radius * radius) {
        m.valid = false;
        return false;
    }

    float dist = sqrt(distSq);

    if (dist < 1e-6) {
        float distLeft = posA.x - (-halfExterns.x);
        float distRight = (halfExterns.x) - posA.x;
        float distBottom = (halfExterns.y) - posA.y;
        float distTop = posA.y - (-halfExterns.y);

        auto minDist = std::min({distLeft, distRight, distBottom, distTop});

        if (minDist == distLeft) {
            m.normal = {1.0f, 0.0f};
            m.points.push_back(Vec2{-halfExterns.x, posA.y});
        } else if (minDist == distRight) {
            m.normal = {-1.0f, 0.0f};
            m.points.push_back(Vec2{halfExterns.x, posA.y});
        } else if (minDist == distBottom) {
            m.normal = {0.0f, -1.0f};
            m.points.push_back(Vec2{posA.x, halfExterns.y});
        } else {
            m.normal = {0.0, 1.0f};
            m.points.push_back(Vec2{posA.x, -halfExterns.y});
        }

        m.points[0].penetration = radius + minDist;
    } else {
        m.normal = delta * (1 / dist);
        m.points.push_back(posA + m.normal * radius);
        m.points[0].penetration = radius - dist;
    }

    m.valid = true;
    return true;
}

bool circleBox(const Body& a, const Body& b, Manifold& m) {
    auto* ca = std::get_if<CircleShape>(&a.shape);
    auto* cb = std::get_if<CircleShape>(&b.shape);
    auto* ba = std::get_if<BoxShape>(&a.shape);
    auto* bb = std::get_if<BoxShape>(&b.shape);

    bool hit = false;
    float theta;

    // a - круг, b — бокс. Нормаль смотрит от a к b.
    if (ca && bb) {
        theta = b.angleOfRotation;
        // Вычисление позиции круга относительно позиции наклоненного бокса
        Vec2 circleLocalPosition = (a.position - b.position).rotated(-theta);

        hit =
            circleBoxImpl(circleLocalPosition, ca->radius, bb->halfExtents, m);
    }

    // b - круг, a - бокс
    if (ba && cb) {
        theta = a.angleOfRotation;
        Vec2 circleLocalPosition = (b.position - a.position).rotated(-theta);

        hit =
            circleBoxImpl(circleLocalPosition, cb->radius, ba->halfExtents, m);
        if (hit) m.normal = -m.normal;
    }

    // Перевод в мировые координаты
    if (hit) {
        m.normal.rotate(theta);
        m.points[0].position.rotate(theta);
        m.points[0].position += ba ? a.position : b.position;
    }

    return hit;
}

bool cut(Vec2 &z1, Vec2 &z2, float &x1, float &x2, float hTR) {
    float f1 = x1 - hTR;
    float f2 = x2 - hTR;

    if (f1 > 0.0f && f2 > 0.0f) {
        return false;
    }

    if ((f1 > 0.0f && f2 <= 0.0f) || (f1 <= 0.0f && f2 > 0.0f)) {
        float a = f1 / (f1 - f2);
        Vec2 cutPoint = z1 + (z2 - z1) * a;

        if (f1 > 0.0f) {
            z1 = cutPoint;
        } else {
            z2 = cutPoint;
        }
    }

    return true;
}

bool boxBox(const Body& a, const Body& b, Manifold& m) {
    auto* ca = std::get_if<BoxShape>(&a.shape);
    auto* cb = std::get_if<BoxShape>(&b.shape);

    assert(ca && cb && "box_box called with non-box body");

    /* Проверка пересечения прямоугольников будет производится по теореме о
     * разделяющей оси */

    // Получение направления сторон прямоугольников
    Vec2 uA = {cos(a.angleOfRotation), sin(a.angleOfRotation)};
    Vec2 vA = {-sin(a.angleOfRotation), cos(a.angleOfRotation)};
    Vec2 uB = {cos(b.angleOfRotation), sin(b.angleOfRotation)};
    Vec2 vB = {-sin(b.angleOfRotation), cos(b.angleOfRotation)};
    Vec2 axes[] = {uA, vA, uB, vB};

    float minDelta = FLT_MAX;
    Vec2 minAxis;

    Vec2 d = b.position - a.position;

    int supportingFaceIdx = 0;
    int idx = 0;

    for (const Vec2& s : axes) {
        // Размеры проекции
        float rA = ca->halfExtents.x * std::abs(uA.dot(s)) +
                   ca->halfExtents.y * std::abs(vA.dot(s));
        float rB = cb->halfExtents.x * std::abs(uB.dot(s)) +
                   cb->halfExtents.y * std::abs(vB.dot(s));

        float distanceBetweenProjections = std::abs(d.dot(s));

        float delta = rA + rB - distanceBetweenProjections;

        if (delta <= 0.0f) {
            m.valid = false;
            return false;
        }

        if (minDelta > delta) {
            minDelta = delta;
            minAxis = s;
            supportingFaceIdx = idx;
        }

        idx++;
    }

    m.normal = d.dot(minAxis) >= 0 ? minAxis : -minAxis;

    Body R;   // Опорное тело
    Body I;   // Тело, которому будет обрезана грань
    Vec2 nR;  // Нормаль опорной грани

    if (supportingFaceIdx == 0 || supportingFaceIdx == 1) {
        R = a;
        I = b;
        nR = m.normal;
    } else {
        R = b;
        I = a;
        nR = -m.normal;
    }

    float hNR;  // Полуразмер поперек опорной грани
    float hTR;  // Полуразмер вдоль опорной грани
    Vec2 tR;   // Единичное направление вдоль грани

    if (supportingFaceIdx == 0 || supportingFaceIdx == 2) {
        auto* b = std::get_if<BoxShape>(&R.shape);
        hNR = b->halfExtents.x;
        hTR = b->halfExtents.y;
        tR = supportingFaceIdx == 0 ? vA : vB;
    } else {
        auto* b = std::get_if<BoxShape>(&R.shape);
        hNR = b->halfExtents.y;
        hTR = b->halfExtents.x;
        tR = supportingFaceIdx == 1 ? uA : uB;
    }

    Vec2 cR = R.position + nR * hNR;  // Центр опорной грани
    // Концы опорной грани
    Vec2 q1 = cR - tR * hTR;
    Vec2 q2 = cR + tR * hTR;

    float hNI;
    float hTI;
    Vec2 tI;
    Vec2 uI, vI;
    if (supportingFaceIdx < 2) {
        vI = vB;
        uI = uB;
    } else {
        uI = uA;
        vI = vA;
    }

    Vec2 normals[] = {uI, -uI, vI, -vI};
    Vec2 nI;  // Нормаль с минимальным скалярным произведением с nR
    int incidentFaceIdx = 0;

    float minDot = FLT_MAX;
    for (int i = 0; i < 4; i++) {
        float dot = normals[i].dot(nR);
        if (dot < minDot) {
            minDot = dot;
            nI = normals[i];
            incidentFaceIdx = i;
        }
    }

    if (incidentFaceIdx < 2) {
        auto* b = std::get_if<BoxShape>(&I.shape);
        hNI = b->halfExtents.x;
        hTI = b->halfExtents.y;
        tI = vI;
    } else {
        auto* b = std::get_if<BoxShape>(&I.shape);
        hNI = b->halfExtents.y;
        hTI = b->halfExtents.x;
        tI = uI;
    }

    Vec2 cI = I.position + nI * hNI;
    Vec2 z1 = cI - tI * hTI;
    Vec2 z2 = cI + tI * hTI;

    float x1 = -(z1 - cR).dot(tR);
    float x2 = -(z2 - cR).dot(tR);

    if (!cut(z1, z2, x1, x2, hTR)) {
        m.valid = false;
        return false;
    }

    x1 = (z1 - cR).dot(tR);
    x2 = (z2 - cR).dot(tR);

    if (!cut(z1, z2, x1, x2, hTR)) {
        m.valid = false;
        return false;
    }

    float s1 = (z1 - cR).dot(nR);
    if (s1 <= 0.0f) {
        ContactPoint point(z1);
        point.penetration = -s1;

        m.points.push_back(point);
    }

    float s2 = (z2 - cR).dot(nR);
    if (s2 <= 0.0f) {
        ContactPoint point(z2);
        point.penetration = -s2;

        m.points.push_back(point);
    }

    m.valid = m.points.size() > 0;
    return m.valid;
}
}  // namespace Collision
