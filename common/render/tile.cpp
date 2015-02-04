#include "../texture.h"
#include "tile.h"
#include "../utils.h"
#include "../sim/map.h"
#include "../render/heightmap.h"

TlType g_tiletype[TILE_TYPES];

void DefTl(int tiletype, const char* texpath, Vec2i spriteoffset, Vec2i spritesz)
{
	TlType* t = &g_tiletype[tiletype];
	//QueueTexture(&t->sprite.texindex, texpath, true);
	//CreateTexture(t->sprite.texindex, texpath, true, false);
	QueueSprite(texpath, &t->sprite, false);
#if 0
	t->sprite.offset[0] = spriteoffset.x;
	t->sprite.offset[1] = spriteoffset.y;
	t->sprite.offset[2] = t->sprite.offset[0] + spritesz.x;
	t->sprite.offset[3] = t->sprite.offset[1] + spritesz.y;
#endif
}

Tile &SurfTile(int tx, int tz)
{
	return g_hmap.m_surftile[ tz * g_mapsz.x + tx ];
}
