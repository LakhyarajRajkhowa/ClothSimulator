#pragma once

#include <vector>

#include "particle.h"
#include "spring.h"
struct Cloth {
    std::vector<Particle> particles;
    std::vector<Spring> springs;
};