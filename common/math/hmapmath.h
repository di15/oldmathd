#ifndef HMAPMATH_H
#define HMAPMATH_H

#include "../math/vec3f.h"

class Heightmap;

float Bilerp(Heightmap* hmap, float x, float z);

#endif
