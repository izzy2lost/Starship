#pragma once

#include <cstdint>
#include "GeoCommand.h"

int16_t* read_vec3s_to_vec3f(Vec3f& dst, int16_t *src);
int16_t* read_vec3s(Vec3s& dst, int16_t *src);
int16_t* read_vec3s_angle(Vec3s& dst, int16_t *src);