#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "../platform.h"
#include "../math/vec2i.h"
#include "../math/vec2f.h"
#include "../math/vec2uc.h"
#include "../texture.h"
#include "vertexarray.h"
#include "tile.h"

#define TILE_SIZE			1000
#define TILE_Y_SCALE		(10.0f/255.0f)
#define MAPMINZ				(0)
#define MAPMAXZ				(m_widthz*TILE_SIZE)

#define MAX_MAP				256

class Vec2f;
class Vec3f;
class Shader;
class Matrix;
class Plane3f;

extern Vec2i g_mapview[2];

/*
Number of tiles, not heightpoints/corners.
Number of height points/corners is +1.
*/
extern Vec2uc g_mapsz;

class Heightmap
{
public:
	unsigned char *m_heightpoints;
	Vec3f *m_3dverts;
	Vec2f *m_texcoords0;
	Vec3f *m_normals;
	int *m_countryowner;
	bool *m_triconfig;
	Plane3f *m_tridivider;
	Tile *m_surftile;

	void alloc(int wx, int wz);
	void remesh();
	void draw();

	inline unsigned char getheight(int tx, int tz)
	{
		return m_heightpoints[ (tz)*(m_widthx+1) + tx ];
	}

	float accheight(int x, int z);
	void chheight(int x, int z, signed char change);
	void setheight(int x, int z, unsigned char height);
	void destroy();
	Vec3f getnormal(int x, int z);

	~Heightmap()
	{
		destroy();
	}
};

extern Heightmap g_hmap;

void AllocGrid(int wx, int wz);
void FreeGrid();

#endif
