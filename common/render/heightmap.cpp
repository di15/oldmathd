#include "../platform.h"
#include "heightmap.h"
#include "../render/shader.h"
#include "../texture.h"
#include "../gui/gui.h"
#include "../gui/icon.h"
#include "../math/vec4f.h"
#include "../math/3dmath.h"
#include "../math/vec3f.h"
#include "../math/vec2f.h"
#include "../utils.h"
#include "../math/camera.h"
#include "../window.h"
#include "../math/polygon.h"
#include "../math/physics.h"
#include "../sim/road.h"
#include "../sim/powl.h"
#include "../sim/crpipe.h"
#include "../math/plane3f.h"
#include "../path/collidertile.h"
#include "foliage.h"
#include "../save/savemap.h"
#include "../texture.h"
#include "water.h"
#include "../sim/player.h"
#include "../debug.h"

Vec2i g_mapview[2];
Heightmap g_hmap;

/*
Number of tiles, not heightpoints/corners.
Number of height points/corners is +1.
*/
Vec2uc g_mapsz(0,0);

void AllocGrid(int wx, int wy)
{
	g_log<<"allocating class arrays "<<wx<<","<<wy<<std::endl;
	g_log.flush();

	if( !(g_cdtype[CONDUIT_ROAD].cdtiles[0] = new CdTile [ (wx * wy) ]) ) OutOfMem(__FILE__, __LINE__);
	if( !(g_cdtype[CONDUIT_ROAD].cdtiles[1] = new CdTile [ (wx * wy) ]) ) OutOfMem(__FILE__, __LINE__);
	if( !(g_cdtype[CONDUIT_POWL].cdtiles[0] = new CdTile [ (wx * wy) ]) ) OutOfMem(__FILE__, __LINE__);
	if( !(g_cdtype[CONDUIT_POWL].cdtiles[1] = new CdTile [ (wx * wy) ]) ) OutOfMem(__FILE__, __LINE__);
	if( !(g_cdtype[CONDUIT_CRPIPE].cdtiles[0] = new CdTile [ (wx * wy) ]) ) OutOfMem(__FILE__, __LINE__);
	if( !(g_cdtype[CONDUIT_CRPIPE].cdtiles[1] = new CdTile [ (wx * wy) ]) ) OutOfMem(__FILE__, __LINE__);
}

/*
allocate room for a number of tiles, not height points.
*/
void Heightmap::alloc(int wx, int wy)
{
	destroy();

	// Vertices aren't shared between tiles or triangles.
	int numverts = wx * wy * 3 * 2;

	g_mapsz.x = wx;
	g_mapsz.y = wy;

	m_heightpoints = new unsigned char [ (wx+1) * (wy+1) ];
	m_3dverts = new Vec3f [ numverts ];
	m_normals = new Vec3f [ numverts ];
	m_texcoords0 = new Vec2f [ numverts ];
	m_triconfig = new bool [ wx * wy ];
	m_tridivider = new Plane3f [ wx * wy ];
	m_surftile = new Tile [ wx * wy ];

	if(!m_heightpoints) OutOfMem(__FILE__, __LINE__);
	if(!m_3dverts) OutOfMem(__FILE__, __LINE__);
	if(!m_normals) OutOfMem(__FILE__, __LINE__);
	if(!m_texcoords0) OutOfMem(__FILE__, __LINE__);
	if(!m_triconfig) OutOfMem(__FILE__, __LINE__);
	if(!m_tridivider) OutOfMem(__FILE__, __LINE__);
	if(!m_surftile) OutOfMem(__FILE__, __LINE__);

	//g_log<<"setting heights to 0"<<std::endl;
	//g_log.flush();

	// Set to initial height.
	for(int x=0; x<=wx; x++)
		for(int y=0; y<=wy; y++)
			//m_heightpoints[ y*(wx+1) + x ] = rand()%1000;
			m_heightpoints[ y*(wx+1) + x ] = 0;

	remesh();
	//retexture();
}

void FreeGrid()
{
	for(unsigned char ctype=0; ctype<CONDUIT_TYPES; ctype++)
	{
		CdType* ct = &g_cdtype[ctype];
		//nice
		CdTile*& actual = ct->cdtiles[(int)false];
		CdTile*& planned = ct->cdtiles[(int)true];

		if(planned)
		{
			delete [] planned;
			planned = NULL;
		}

		if(actual)
		{
			delete [] actual;
			actual = NULL;
		}
	}
}

void Heightmap::destroy()
{
	if(g_mapsz.x <= 0 || g_mapsz.y <= 0)
		return;

	g_log<<"deleting [] g_open"<<std::endl;
	g_log.flush();
	/*
		delete [] g_open;

		g_log<<"deleting [] g_road"<<std::endl;
		g_log.flush();
		*/

	delete [] m_heightpoints;
	delete [] m_3dverts;
	delete [] m_normals;
	delete [] m_texcoords0;
	delete [] m_surftile;

	g_mapsz.x = 0;
	g_mapsz.y = 0;

	FreePathGrid();
}

void Heightmap::adjheight(int x, int y, signed char change)
{
	m_heightpoints[ (y)*(g_mapsz.x+1) + x ] += change;
}

void Heightmap::setheight(int x, int y, unsigned char height)
{
	m_heightpoints[ (y)*(g_mapsz.x+1) + x ] = height;
}

float Heightmap::accheight(int x, int y)
{
	int tx = x / TILE_SIZE;
	int ty = y / TILE_SIZE;

	if(tx < 0)
		tx = 0;

	if(ty < 0)
		ty = 0;

	if(tx >= g_mapsz.x)
		tx = g_mapsz.x-1;

	if(ty >= g_mapsz.y)
		ty = g_mapsz.y-1;

	int tileindex = ty*g_mapsz.x + tx;
	int tileindex6v = tileindex * 6;

	Vec3f point = Vec3f(x, 0, y);

	Vec3f tri[3];

	if(PointBehindPlane(point, m_tridivider[tileindex]))
	{
		tri[0] = m_3dverts[ tileindex6v + 0 ];
		tri[1] = m_3dverts[ tileindex6v + 1 ];
		tri[2] = m_3dverts[ tileindex6v + 2 ];
	}
	else
	{
		tri[0] = m_3dverts[ tileindex6v + 3 ];
		tri[1] = m_3dverts[ tileindex6v + 4 ];
		tri[2] = m_3dverts[ tileindex6v + 5 ];
	}

	Plane3f plane;

	Vec3f trinorm = Normal(tri);

	MakePlane(&plane.m_normal, &plane.m_d, tri[0], trinorm);

	float z = - ( x*plane.m_normal.x + y*plane.m_normal.y + plane.m_d ) / plane.m_normal.y;

	return z;
}

Vec3f Heightmap::getnormal(int x, int y)
{
	return m_normals[ (y * g_mapsz.x + x) * 6 ];
}

/*
Regenerate the mesh vertices (m_3dverts) and normals (m_normals) from the height points.
Texture coordinates (m_texcoords0) will also be generated.
*/
void Heightmap::remesh()
{
#if 1
	bool changed = false;

	do
	{
		changed = false;

		for(int x=0; x<g_mapsz.x; x++)
			for(int y=0; y<g_mapsz.y; y++)
			{
				unsigned short h0 = getheight(x, y);
				unsigned short h1 = getheight(x+1, y);
				unsigned short h2 = getheight(x+1, y+1);
				unsigned short h3 = getheight(x, y+1);

				unsigned int minh = imin(h0, imin(h1, imin(h2, h3)));

				if(h0 > minh+1)
				{
					changed = true;
					adjheight(x, y, -1);
				}

				if(h1 > minh+1)
				{
					changed = true;
					adjheight(x+1, y, -1);
				}

				if(h2 > minh+1)
				{
					changed = true;
					adjheight(x+1, y+1, -1);
				}

				if(h3 > minh+1)
				{
					changed = true;
					adjheight(x, y+1, -1);
				}
			}

	}while(changed);
#endif

	for(int x=0; x<g_mapsz.x; x++)
		for(int y=0; y<g_mapsz.y; y++)
		{
			unsigned char h0 = getheight(x, y);
			unsigned char h1 = getheight(x+1, y);
			unsigned char h2 = getheight(x+1, y+1);
			unsigned char h3 = getheight(x, y+1);


			//g_log<<"height after "<<(unsigned int)h0<<std::endl;

			g_log<<"("<<(int)h0<<","<<(int)h1<<","<<(int)h2<<","<<(int)h3<<")"<<std::endl;

			unsigned char minh = imin(h0, imin(h1, imin(h2, h3)));

			bool u0 = h0 > minh;
			bool u1 = h1 > minh;
			bool u2 = h2 > minh;
			bool u3 = h3 > minh;

			if(!u0 && !u1 && !u2 && !u3)	SurfTile(x,y).tiletype = TILE_0000;
			if(!u0 && !u1 && !u2 && u3)		SurfTile(x,y).tiletype = TILE_0001;
			if(!u0 && !u1 && u2 && !u3)		SurfTile(x,y).tiletype = TILE_0010;
			if(!u0 && !u1 && u2 && u3)		SurfTile(x,y).tiletype = TILE_0011;
			if(!u0 && u1 && !u2 && !u3)		SurfTile(x,y).tiletype = TILE_0100;
			if(!u0 && u1 && !u2 && u3)		SurfTile(x,y).tiletype = TILE_0101;
			if(!u0 && u1 && u2 && !u3)		SurfTile(x,y).tiletype = TILE_0110;
			if(!u0 && u1 && u2 && u3)		SurfTile(x,y).tiletype = TILE_0111;
			if(u0 && !u1 && !u2 && !u3)		SurfTile(x,y).tiletype = TILE_1000;
			if(u0 && !u1 && !u2 && u3)		SurfTile(x,y).tiletype = TILE_1001;
			if(u0 && !u1 && u2 && !u3)		SurfTile(x,y).tiletype = TILE_1010;
			if(u0 && !u1 && u2 && u3)		SurfTile(x,y).tiletype = TILE_1011;
			if(u0 && u1 && !u2 && !u3)		SurfTile(x,y).tiletype = TILE_1100;
			if(u0 && u1 && !u2 && u3)		SurfTile(x,y).tiletype = TILE_1101;
			if(u0 && u1 && u2 && !u3)		SurfTile(x,y).tiletype = TILE_1110;

			SurfTile(x,y).tilepos = Vec3i(x,minh,y);
		}
}

void Heightmap::draw()
{
	if(g_mapsz.x <= 0 || g_mapsz.y <= 0)
		return;
	//return;
	Shader* s = &g_shader[g_curS];

}
