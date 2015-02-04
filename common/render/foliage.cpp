#include "../texture.h"
#include "foliage.h"
#include "water.h"
#include "shader.h"
#include "../math/camera.h"
#include "../math/frustum.h"
#include "vertexarray.h"
#include "heightmap.h"
#include "../utils.h"
#include "../window.h"
#include "../sim/player.h"
#include "../path/pathnode.h"
#include "../path/collidertile.h"
#include "../phys/collision.h"
#include "../math/hmapmath.h"
#include "../sim/simflow.h"
#include "../path/fillbodies.h"

FlType g_fltype[FL_TYPES];
Foliage g_foliage[FOLIAGES];

Foliage::Foliage()
{
	on = false;
	lastdraw = 0;
}

void DefF(int type, const char* sprelative, Vec3f scale, Vec3f translate, Vec2s size)
{
	FlType* t = &g_fltype[type];
	//QueueTexture(&t->texindex, texrelative, true);
	//QueueModel(&t->model, sprel, scale, translate);
	QueueSprite(sprelative, &t->sprite, false);
	t->size = size;
}

int NewFoliage()
{
	for(int i=0; i<FOLIAGES; i++)
	{
		if(!g_foliage[i].on)
			return i;
	}

	return -1;
}

#if 0
void PlaceFol()
{
	if(g_scT < 0)
		return;

	int i = NewFoliage();

	if(i < 0)
		return;

	Foliage* s = &g_foliage[i];
	s->on = true;
	s->pos = g_vMouse;
	s->type = g_scT;
	s->yaw = DEGTORAD((rand()%360));

	if(s->pos.y <= WATER_LEVEL)
		s->on = false;
}
#endif

void DrawFol(Vec3f zoompos, Vec3f vertical, Vec3f horizontal)
{
	//return;

}

bool PlaceFol(int type, Vec3i ipos)
{
	int i = NewFoliage();

	if(i < 0)
		return false;

	int nx = ipos.x / PATHNODE_SIZE;
	int nz = ipos.z / PATHNODE_SIZE;
	ColliderTile* c = ColliderAt(nx, nz);

	if(c->foliage != USHRT_MAX)
		return false;

	Foliage* f = &g_foliage[i];
	f->on = true;
	f->type = type;
	f->cmpos = Vec2i(ipos.x, ipos.z);
	f->drawpos = Vec3f(ipos.x, ipos.y, ipos.z);
	f->yaw = rand()%360;
	f->fillcollider();

	return true;
}

void ClearFol(int cmminx, int cmminy, int cmmaxx, int cmmaxy)
{
#if 0
	for(int i=0; i<FOLIAGES; i++)
	{
		Foliage* f = &g_foliage[i];

		if(!f->on)
			continue;

		if(f->pos.x >= minx && f->pos.x <= maxx && f->pos.z >= minz && f->pos.z <= maxz)
		{
			f->on = false;
			f->reinstance();
		}
	}
#endif

	//c = cell position
	int cminx = cmminx / PATHNODE_SIZE;
	int cminy = cmminy / PATHNODE_SIZE;
	int cmaxx = cmmaxx / PATHNODE_SIZE;
	int cmaxy = cmmaxy / PATHNODE_SIZE;

	for(int ny = cminy; ny <= cmaxy; ny++)
		for(int nx = cminx; nx <= cmaxx; nx++)
		{
			ColliderTile* c = ColliderAt(nx, ny);

			if(c->foliage < 0)
				continue;

			Foliage* f = &g_foliage[c->foliage];
			FlType* ft = &g_fltype[f->type];
			
			int cmminx2 = f->cmpos.x - ft->size.x/2;
			int cmminy2 = f->cmpos.y - ft->size.x/2;
			int cmmaxx2 = cmminx2 + ft->size.x - 1;
			int cmmaxy2 = cmminy2 + ft->size.x - 1;

			//if(f->cmpos.x >= cmminx && f->cmpos.x <= cmmaxx && f->cmpos.y >= cmminz && f->cmpos.y <= cmmaxz)
			if(cmminx2 <= cmmaxx && cmminy2 <= cmmaxy && cmmaxx2 >= cmminx && cmmaxy2 >= cmminy)
			{
				f->on = false;
				f->freecollider();
			}
		}

	FillBodies();
}

void FillForest()
{
}

void FreeFol()
{
	for(int i=0; i<FOLIAGES; i++)
	{
		g_foliage[i].on = false;
	}
}
