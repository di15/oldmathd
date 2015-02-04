#include "collision.h"
#include "../sim/building.h"
#include "../sim/bltype.h"
#include "../sim/unit.h"
#include "../sim/utype.h"
#include "../render/heightmap.h"
#include "../sim/crpipe.h"
#include "../sim/powl.h"

int g_lastcollider = -1;
int g_collidertype;
bool g_ignored;

bool BlAdj(int i, int j)
{
	Building* bi = &g_building[i];
	Building* bj = &g_building[j];

	BlType* ti = &g_bltype[bi->type];
	BlType* tj = &g_bltype[bj->type];

	Vec2i tpi = bi->tilepos;
	Vec2i tpj = bj->tilepos;

	Vec2i mini;
	Vec2i minj;
	Vec2i maxi;
	Vec2i maxj;

	mini.x = tpi.x - ti->widthx/2;
	mini.y = tpi.y - ti->widthy/2;
	minj.x = tpj.x - tj->widthx/2;
	minj.y = tpj.y - tj->widthy/2;
	maxi.x = mini.x + ti->widthx;
	maxi.y = mini.y + ti->widthy;
	maxj.x = minj.x + tj->widthx;
	maxj.y = minj.y + tj->widthy;

	if(maxi.x >= minj.x && maxi.y >= minj.y && mini.x <= maxj.x && mini.y <= maxj.y)
		return true;

	return false;
}

// Is conduit x,z adjacent to building i?
bool CoAdj(unsigned char ctype, int i, int x, int z)
{
	Building* b = &g_building[i];
	BlType* t = &g_bltype[b->type];

	Vec2i tp = b->tilepos;

	Vec2i tmin;
	Vec2i tmax;

	tmin.x = tp.x - t->widthx/2;
	tmin.y = tp.y - t->widthy/2;
	tmax.x = tmin.x + t->widthx;
	tmax.y = tmin.y + t->widthy;

	Vec2i cmmin = Vec2i(tmin.x*TILE_SIZE, tmin.y*TILE_SIZE);
	Vec2i cmmax = Vec2i(tmax.x*TILE_SIZE, tmax.y*TILE_SIZE);

	CdType* ct = &g_cdtype[ctype];
	Vec2i ccmp2 = ct->physoff + Vec2i(x, z)*TILE_SIZE;

	Vec2i cmmin2 = Vec2i(ccmp2.x-TILE_SIZE/2, ccmp2.y-TILE_SIZE/2);
	Vec2i cmmax2 = Vec2i(ccmp2.x+TILE_SIZE/2, ccmp2.y+TILE_SIZE/2);

	if(cmmax.x >= cmmin2.x && cmmax.y >= cmmin2.y && cmmin.x <= cmmax2.x && cmmin.y <= cmmax2.y)
		return true;

	return false;
}

bool CollidesWithBuildings(int minx, int minz, int maxx, int maxz, int ignore)
{
	g_ignored = false;

	for(int i=0; i<BUILDINGS; i++)
	{
		Building* b = &g_building[i];

		if(!b->on)
			continue;

		BlType* t = &g_bltype[ b->type ];

		Vec2i tmin;
		Vec2i tmax;

		tmin.x = b->tilepos.x - t->widthx/2;
		tmin.y = b->tilepos.y - t->widthy/2;
		tmax.x = tmin.x + t->widthx - 1;
		tmax.y = tmin.y + t->widthy - 1;

		Vec2i cmmin2;
		Vec2i cmmax2;

		cmmin2.x = tmin.x * TILE_SIZE;
		cmmin2.y = tmin.y * TILE_SIZE;
		cmmax2.x = cmmin2.x + t->widthx*TILE_SIZE - 1;
		cmmax2.y = cmmin2.y + t->widthy*TILE_SIZE - 1;

		if(maxx >= cmmin2.x && maxz >= cmmin2.y && minx <= cmmax2.x && minz <= cmmax2.y)
		{
			if(i == ignore)
			{
				g_ignored = true;
				continue;
			}
			/*
			 if(g_debug1)
			 {
			 g_log<<"fabs((p.x)"<<p.x<<"-(x)"<<x<<") < (hwx)"<<hwx<<"+(hwx2)"<<hwx2<<" && fabs((p.z)"<<p.z<<"- (z)"<<z<<") < (hwz)"<<hwz<<"+(hwz2)"<<hwz2<<std::endl;
			 g_log<<fabs(p.x-x)<<" < "<<(hwx+hwx2)<<" && "<<fabs(p.z-z)<<" < "<<(hwz+hwz2)<<std::endl;
			 }*/

			g_lastcollider = i;
			g_collidertype = COLLIDER_BUILDING;
			return true;
		}
	}

	return false;
}

//TODO circular unit radius
bool CollidesWithUnits(int minx, int miny, int maxx, int maxy, bool isunit, Unit* thisu, Unit* ignore)
{
	for(int i=0; i<UNITS; i++)
	{
		Unit* u = &g_unit[i];

		if(!u->on)
			continue;

		if(u == ignore)
			continue;

		if(u == thisu)
			continue;

		if(u->hidden())
			continue;

		UType* t = &g_utype[u->type];

		int minx2 = u->cmpos.x - t->size.x/2;
		int miny2 = u->cmpos.y - t->size.x/2;
		int maxx2 = minx2 + t->size.x - 1;
		int maxy2 = miny2 + t->size.x - 1;

		if(maxx >= minx2 && maxy >= miny2 && minx <= maxx2 && miny <= maxy2)
		{
			g_lastcollider = i;
			g_collidertype = COLLIDER_UNIT;
			return true;
		}
	}

	return false;
}

bool OffMap(int cmminx, int cmminz, int cmmaxx, int cmmaxz)
{
	if(cmminx < 0)
		return true;

	if(cmmaxx >= g_mapsz.x*TILE_SIZE)
		return true;

	if(cmminz < 0)
		return true;

	if(cmmaxz >= g_mapsz.y*TILE_SIZE)
		return true;

	return false;
}

