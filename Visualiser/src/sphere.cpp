#include "sphere.hpp"

// Updates the spheres position.
// 't' is the time since the last position update, not the time since the start
// of the simulation. 
void update_sphere_position(struct sphere_s *s, float t) {
	s->pos.x = s->pos.x + (s->vel.x * t);
	s->pos.y = s->pos.y + (s->vel.y * t);
	s->pos.z = s->pos.z + (s->vel.z * t);
}