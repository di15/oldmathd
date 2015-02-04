#ifndef TILE_H
#define TILE_H

#include "sprite.h"
#include "../phys/collidable.h"
#include "../math/vec2i.h"

class TlType
{
public:
	unsigned int sprite;
};

#define TILE_0000		0
#define TILE_0001		1
#define TILE_0010		2
#define TILE_0011		3
#define TILE_0100		4
#define TILE_0101		5
#define TILE_0110		6
#define TILE_0111		7
#define TILE_1000		8
#define TILE_1001		9
#define TILE_1010		10
#define TILE_1011		11
#define TILE_1100		12
#define TILE_1101		13
#define TILE_1110		14
#define TILE_TYPES		15

extern TlType g_tiletype[TILE_TYPES];

class Tile : public Collidable
{
public:
	int tiletype;
	Vec3i tilepos;
};

void DefTl(int tiletype, const char* texpath, Vec2i spriteoffset, Vec2i spritesz);
Tile &SurfTile(int tx, int tz);

#endif
