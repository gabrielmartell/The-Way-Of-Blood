#ifndef PARTICLES_H_
#define PARTICLES_H_

#include "geometry.h"

#define NUM_PARTICLES 1000

namespace game {

    // A set of particles that can be rendered
    class Particles : public Geometry {

        public:
            Particles(int);
            int type;

            // Create the geometry (called once)
            void CreateGeometry();

            // Use the geometry
            void SetGeometry(GLuint shader_program);

    }; // class Particles
} // namespace game

#endif // PARTICLES_H_
