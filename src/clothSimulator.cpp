#include "clothSimulator.h"

ClothSimulator::ClothSimulator() {}
ClothSimulator::~ClothSimulator() {}

void ClothSimulator::applySpringForces(Cloth& c) {
    for (int i = 0; i < c.springs.size(); ) {
        Spring& s = c.springs[i];
        Particle& A = c.particles[s.a];
        Particle& B = c.particles[s.b];

        Vec2 delta = B.pos - A.pos;
        float dist = length(delta);
        if (dist < 1e-6f ) {
            ++i;
            continue;
        }

        // --- Break spring if stretched too far ---
        if (dist > s.maxLen) {
            c.springs.erase(c.springs.begin() + i);
            continue; // skip incrementing i, next spring now has same index
        }

        // Compute spring force normally
        Vec2 n = delta / dist;
        float stretch = dist - s.restLen;
        Vec2 F = n * (s.k * stretch);

        if (!A.fixed) A.force = A.force + F;
        if (!B.fixed) B.force = B.force - F;


        ++i;
    }
}

void ClothSimulator::integrate(Cloth& c, float dt) {
    for (auto& p : c.particles) {
        if (p.fixed) {
            p.force = { 0,0 }; // clear forces but don't move
            continue;
        }
		p.force = p.force + gravityStrength * p.mass; // apply gravity
        Vec2 acc = p.force / p.mass;
        p.vel = p.vel + acc * dt;
        p.pos = p.pos + p.vel * dt;

        // damping
        p.vel = p.vel * 0.98f;


        // reset force for next step
        p.force = { 0,0 };
    }
}


Cloth  ClothSimulator::makeCloth(int rows, int cols, float spacing, float yPos) {
    Cloth c;
    // Create particles
    for (int y = 0; y < rows; y++){
        for (int x = 0; x < cols; x++) {
            Particle p;
            p.pos = { x * spacing , yPos };   // row spread out across screen
            p.vel = { 0,0 };
            p.force = { 0,0 };
            p.mass = m_particle_mass;
            p.color = m_nail_color;   // red to mark "nails"
            p.radius = m_nail_radius;
            p.fixed = true;          // mark as fixed so they don’t move

            c.particles.push_back(p);
        }
    }

    // Connect neighbors with springs
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            int idx = y * cols + x;
            // right neighbor
            if (x < cols - 1) {
                Spring s{ idx, idx + 1, m_spring_restLen, m_spring_k, m_spring_max_len };
                c.springs.push_back(s);
            }
            // down neighbor
            if (y < rows - 1) {
                Spring s{ idx, idx + cols, m_spring_restLen, m_spring_k, m_spring_max_len };
                c.springs.push_back(s);
            }
        }
    }
    return c;
}

void ClothSimulator::addInvertedPyramid(Cloth& c, int rows, int baseCols, float spacing, float xPos, float yPos) {
    for (int y = 0; y < rows; y++) {
        int cols = baseCols - y;  // decrease number of particles per row
        float offsetX = xPos + (y * spacing * 0.5f); // shift rows inward to center

        for (int x = 0; x < cols; x++) {
            Particle p;
            p.pos = { offsetX + x * spacing, yPos + y * spacing * 0.75f }; // x & y placement
            p.vel = { 0,0 };
            p.force = { 0,0 };
            p.mass = m_particle_mass;
            p.color = m_particle_color;
            p.radius = m_particle_radius;
            

            int newIndex = c.particles.size();
            c.particles.push_back(p);

            // check all existing particles for proximity
            for (int i = 0; i < newIndex; i++) {
                Particle& other = c.particles[i];
                float dx = other.pos.x - p.pos.x;
                float dy = other.pos.y - p.pos.y;
                float dist = sqrt(dx * dx + dy * dy);

                if (dist <= spacing) {
                    // connect with a spring
                    Spring s{ newIndex, i, dist, m_spring_k, m_spring_max_len };
                    c.springs.push_back(s);
                }
            }
        }
    }
}
void ClothSimulator::applyRepulsionForces(Cloth& c ) {
    int n = c.particles.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            Particle& A = c.particles[i];
            Particle& B = c.particles[j];

            Vec2 delta = B.pos - A.pos;
            float dist = length(delta);
            if (dist < 1e-6) continue;

            if (dist < m_repulsion_minDist) {
                Vec2 nrm = delta / dist;
                float overlap = m_repulsion_minDist - dist;

                // Repulsion force (Hooke-like)
                Vec2 F = nrm * (m_repulsion_k * overlap);

                A.force = A.force - F;
                B.force = B.force + F;


            }
        }
    }
}

void ClothSimulator::addParticleAt(Cloth& c, float x, float y, bool isFixed) {
    Particle p;
    p.pos = { x, y };
    p.vel = { 0,0 };
    p.force = { 0,0 };
    p.mass = m_particle_mass;
    p.color = m_particle_color;
    p.radius = m_particle_radius;
    p.fixed = isFixed;

    int newIndex = c.particles.size();
    c.particles.push_back(p);

    // check all existing particles for proximity
    for (int i = 0; i < newIndex; i++) {
        Particle& other = c.particles[i];
        float dx = other.pos.x - p.pos.x;
        float dy = other.pos.y - p.pos.y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist <= 20.0f) {
            // connect with a spring
            Spring s{ newIndex, i, dist, m_spring_k, m_spring_max_len };
            c.springs.push_back(s);
        }
    }
}

void ClothSimulator::satisfyConstraints(Cloth& c) {
    for (auto& s : c.springs) {
        Particle& A = c.particles[s.a];
        Particle& B = c.particles[s.b];

        Vec2 delta = B.pos - A.pos;
        float dist = length(delta);
        if (dist < 1e-6f) continue;

        float diff = (dist - s.restLen) / dist;

        if (A.fixed && B.fixed) {
            // both fixed: do nothing
            continue;
        }
        else if (A.fixed) {
            // only B moves
            B.pos = B.pos - delta * diff;
        }
        else if (B.fixed) {
            // only A moves
            A.pos = A.pos + delta * diff;
        }
        else {
            // both free: move half each
            A.pos = A.pos + delta * (0.5f * diff);
            B.pos = B.pos - delta * (0.5f * diff);
        }
    }
}

void ClothSimulator::applyRuntimeParameters(Cloth& cloth) {
    // Update particle radius and mass
    for (auto& p : cloth.particles) {
        p.radius = m_particle_radius;
        p.mass = m_particle_mass;
    }

    // Update spring stiffness
    for (auto& s : cloth.springs) {
        s.k = m_spring_k;
    }
}

void ClothSimulator::applyGravity(Cloth& c) {
    for (auto& p : c.particles) {
        if (!p.fixed) {
            p.force = p.force + gravityStrength * p.mass;
        }
    }
}

void ClothSimulator::removeOffscreenParticles(Cloth& cloth, int screenWidth, int screenHeight) {
    if (cloth.particles.empty()) return;

    // Mark which particles should stay
    std::vector<int> indexMap(cloth.particles.size(), -1);
    std::vector<Particle> newParticles;
    newParticles.reserve(cloth.particles.size());

    for (int i = 0; i < (int)cloth.particles.size(); ++i) {
        const auto& p = cloth.particles[i];
        bool inside = !(p.pos.x < -50 || p.pos.x > screenWidth + 50 ||
            p.pos.y < -50 || p.pos.y > screenHeight + 50);
        if (inside) {
            indexMap[i] = (int)newParticles.size();
            newParticles.push_back(p);
        }
    }

    // Rebuild spring list with valid indices
    std::vector<Spring> newSprings;
    newSprings.reserve(cloth.springs.size());
    for (auto& s : cloth.springs) {
        int a = indexMap[s.a];
        int b = indexMap[s.b];
        if (a != -1 && b != -1) {
            s.a = a;
            s.b = b;
            newSprings.push_back(s);
        }
    }

    // Replace old data
    cloth.particles = std::move(newParticles);
    cloth.springs = std::move(newSprings);
}



