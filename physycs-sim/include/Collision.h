#pragma once

#include "Body.h"
#include "Manifold.h"

namespace Collision {
bool circleCircle(const Body &a, const Body &b, Manifold &m);
bool circleBox(const Body &a, const Body &b, Manifold &m);
}