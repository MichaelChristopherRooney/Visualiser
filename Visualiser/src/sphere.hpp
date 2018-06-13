#pragma once

#include <stdint.h>

#include "glm/glm.hpp"

// Velocity is given as metres per second
// Position is given in metres and is the center of the sphere.
// For now assuming all spheres have the same radius
struct sphere_s {
	glm::vec3 vel;
	glm::vec3 pos;
	float radius;
	float mass;
};

void update_sphere_position(struct sphere_s *s, float t);