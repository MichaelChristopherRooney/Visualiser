#pragma once

#include <stdint.h>

#include "sphere.hpp"

extern float grid_x_start;
extern float grid_x_end;
extern float grid_y_start;
extern float grid_y_end;
extern float grid_z_start;
extern float grid_z_end;

extern uint64_t num_spheres;
extern float simulation_time_of_last_event;
extern float simulation_time_of_next_event;
extern uint64_t iteration_num;
extern struct sphere_s *spheres;