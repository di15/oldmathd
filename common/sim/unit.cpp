#include "unit.h"
#include "../render/shader.h"
#include "unittype.h"
#include "../texture.h"
#include "../utils.h"
#include "player.h"
#include "../math/hmapmath.h"
#include "unitmove.h"
#include "sim.h"
#include "labourer.h"
#include "../debug.h"
#include "../render/sprite.h"
#include "build.h"
#include "building.h"
#include "../math/isomath.h"
#include "map.h"

Unit g_unit[UNITS];

Unit::Unit()
{
	on = false;
	threadwait = false;
}

Unit::~Unit()
{
	destroy();
}

void Unit::destroy()
{
	on = false;
	threadwait = false;
}

void Unit::resetpath()
{
	path.clear();
	subgoal = cmpos;
	goal = cmpos;
	pathblocked = false;
}

void DrawUnits()
{
	StartTimer(TIMER_DRAWUNITS);
	glBindTexture(GL_TEXTURE_2D, g_screentex);

	for(int i=0; i<UNITS; i++)
	{
		StartTimer(TIMER_DRAWUMAT);

		Unit* u = &g_unit[i];

		if(!u->on)
			continue;

		UType* t = &g_utype[u->type];

		StopTimer(TIMER_DRAWUMAT);

#if 0
		Vec3i cmpos;
		cmpos.x = u->cmpos.x;
		cmpos.z = u->cmpos.y;
		//cmpos.y = g_hmap.accheight(cmpos.x, cmpos.z) * TILE_RISE;
		cmpos.y = Bilerp(&g_hmap, cmpos.x, cmpos.z) * TILE_RISE;

		Vec2i screenpos = CartToIso(cmpos);
		Sprite* sprite = &t->sprite;
		Texture* tex = &g_texture[sprite->texindex];

		DrawImage( tex->texname,
		screenpos.x + sprite->offset[0],
		screenpos.y + sprite->offset[1],
		screenpos.x + sprite->offset[2],
		screenpos.y + sprite->offset[3]);
#elif 1
		Vec2f screenpos = u->drawpos;
		Sprite* sprite = &t->sprite;
		Texture* tex = &g_texture[sprite->texindex];

#if 1
		DrawImage( tex->texname,
		screenpos.x + sprite->offset[0],
		screenpos.y + sprite->offset[1],
		screenpos.x + sprite->offset[2],
		screenpos.y + sprite->offset[3]);
#endif
#if 0
	for(int i=0; i<30; i++)
	{
		int x = rand()%g_width;
		int y = rand()%g_height;

		Blit(blittex, &blitscreen, Vec2i(x,y));
	}

	glDrawPixels(blitscreen.sizeX, blitscreen.sizeY, GL_RGB, GL_BYTE, blitscreen.data);
#endif

#if 0
    LoadedTex* pixels = sprite->pixels;
    //glDrawPixels(pixels->sizeX, pixels->sizeY, pixels->channels == 3 ? GL_RGB : GL_RGBA, GL_BYTE, pixels->data);
    glTexSubImage2D( GL_TEXTURE_2D, 0,
             0,0, pixels->sizeX,pixels->sizeY,
             GL_RGBA,GL_UNSIGNED_BYTE, pixels->data );
#endif
#endif
	}

	StopTimer(TIMER_DRAWUNITS);
}

int NewUnit()
{
	for(int i=0; i<UNITS; i++)
		if(!g_unit[i].on)
			return i;

	return -1;
}

void StartingBelongings(Unit* u)
{
	Zero(u->belongings);
	u->car = -1;
	u->home = -1;

	if(u->type == UNIT_LABOURER)
	{
		if(u->owner >= 0)
		{
			u->belongings[ RES_FUNDS ] = 100;
		}

		u->belongings[ RES_RETFOOD ] = STARTING_RETFOOD;
		u->belongings[ RES_LABOUR ] = STARTING_LABOUR;
	}
}

void FreeUnits()
{
	for(int i=0; i<UNITS; i++)
	{
		g_unit[i].destroy();
		g_unit[i].on = false;
	}
}

bool Unit::hidden() const
{
	return false;
}

void AnimUnit(Unit* u)
{
	UType* t = &g_utype[u->type];

	if(u->type == UNIT_ROBOSOLDIER || u->type == UNIT_LABOURER)
	{
		if(u->prevpos == u->cmpos)
		{
			u->frame[BODY_LOWER] = 0;
			return;
		}

		PlayAnimation(u->frame[BODY_LOWER], 0, 29, true, 1.0f);
	}
}

void UpdAI(Unit* u)
{
	if(u->type == UNIT_LABOURER)
		UpdLab(u);
}

void UpdUnits()
{
	for(int i = 0; i < UNITS; i++)
	{

		StartTimer(TIMER_UPDUONCHECK);

		Unit* u = &g_unit[i];

		if(!u->on)
		{
			StopTimer(TIMER_UPDUONCHECK);
			continue;
		}

		StopTimer(TIMER_UPDUONCHECK);

		StartTimer(TIMER_UPDUNITAI);
		UpdAI(u);
		StopTimer(TIMER_UPDUNITAI);
		StartTimer(TIMER_MOVEUNIT);
		MoveUnit(u);
		StopTimer(TIMER_MOVEUNIT);
		StartTimer(TIMER_ANIMUNIT);
		AnimUnit(u);
		StopTimer(TIMER_ANIMUNIT);
	}
}

// starting belongings for labourer
void StartBel(Unit* u)
{
	Zero(u->belongings);
	u->car = -1;
	u->home = -1;

	if(u->type == UNIT_LABOURER)
	{
		//if(u->owner >= 0)
		{
			//u->belongings[ RES_FUNDS ] = 100;
			//u->belongings[ RES_FUNDS ] = CYCLE_FRAMES * LABOURER_FOODCONSUM * 30;
			u->belongings[ RES_FUNDS ] = CYCLE_FRAMES/SIM_FRAME_RATE * LABOURER_FOODCONSUM * 10;
		}

		u->belongings[ RES_RETFOOD ] = STARTING_RETFOOD - 250;
		u->belongings[ RES_LABOUR ] = STARTING_LABOUR;
	}
}

bool PlaceUnit(int type, Vec2i cmpos, int owner, int *reti)
{
	int i = NewUnit();

	if(i < 0)
		return false;

	if(reti)
		*reti = i;

#if 0
	bool on;
	int type;
	int stateowner;
	int corpowner;
	int unitowner;

	/*
	The draw (floating-point) position vectory is used for drawing.
	*/
	Vec3f drawpos;

	/*
	The real position is stored in integers.
	*/
	Vec3i cmpos;
	Vec3f facing;
	Vec2f rotation;

	deque<Vec2i> path;
	Vec2i goal;

	int step;
	int target;
	int target2;
	bool targetu;
	bool underorder;
	int fuelstation;
	int belongings[RESOURCES];
	int hp;
	bool passive;
	Vec2i prevpos;
	int taskframe;
	bool pathblocked;
	int frameslookjobago;
	int supplier;
	int reqamt;
	int targtype;
	int home;
	int car;
	//std::vector<TransportJob> bids;

	float frame[2];
#endif

	Unit* u = &g_unit[i];
	UType* t = &g_utype[type];

	u->on = true;
	u->type = type;
	u->cmpos = cmpos;

	Vec3i cmpos3;
	cmpos3.x = u->cmpos.x;
	cmpos3.z = u->cmpos.y;
	//cmpos.y = g_hmap.accheight(cmpos.x, cmpos.z) * TILE_RISE;
	cmpos3.y = Bilerp(&g_hmap, u->cmpos.x, u->cmpos.y) * TILE_RISE;
	Vec2i screenpos = CartToIso(cmpos3);

	u->drawpos = Vec2f(screenpos.x, screenpos.y);
	u->owner = owner;
	u->path.clear();
	u->goal = cmpos;
	u->target = -1;
	u->target2 = -1;
	u->targetu = false;
	u->underorder = false;
	u->fuelstation = -1;
	//u->home = -1;
	StartBel(u);
	u->hp = t->starthp;
	u->passive = false;
	u->prevpos = u->cmpos;
	u->taskframe = 0;
	u->pathblocked = false;
	u->frameslookjobago = 0;
	u->supplier = -1;
	u->reqamt = 0;
	u->targtype = -1;
	u->frame[BODY_LOWER] = 0;
	u->frame[BODY_UPPER] = 0;
	u->subgoal = u->goal;

	u->mode = UMODE_NONE;
	u->pathdelay = 0;
	u->lastpath = g_simframe;

	u->cdtype = CONDUIT_NONE;
	u->driver = -1;
	//u->framesleft = 0;
	u->cyframes = WORK_DELAY-1;

	u->fillcollider();

	return true;
}

void ResetPath(Unit* u)
{
	u->path.clear();
}

void ResetGoal(Unit* u)
{
	u->goal = u->subgoal = u->cmpos;
	ResetPath(u);
}

void ResetMode(Unit* u)
{
	switch(u->mode)
	{
	case UMODE_BLJOB:
	case UMODE_CSTJOB:
	case UMODE_CDJOB:
	case UMODE_SHOPPING:
	case UMODE_RESTING:
	case UMODE_DRTRANSP:
	//case UMODE_REFUELING:
	//case UMODE_ATDEMB:
	//case UMODE_ATDEMCD:
		u->freecollider();
		PlaceUAb(u->type, u->cmpos, &u->cmpos);
		u->fillcollider();
	default:break;
	}

	//LastNum("resetmode 1");
	if(u->type == UNIT_LABOURER)
	{
		//LastNum("resetmode 1a");
		if(u->mode == UMODE_BLJOB)
			RemWorker(u);

		//if(hidden())
		//	relocate();
	}
	else if(u->type == UNIT_TRUCK)
	{
#if 0
		if(u->mode == UMODE_GOSUP
		                //|| mode == GOINGTOREFUEL
		                //|| mode == GOINGTODEMANDERB || mode == GOINGTODEMROAD || mode == GOINGTODEMPIPE || mode == GOINGTODEMPOWL
		  )
		{
			if(u->supplier >= 0)
			{
				Building* b = &g_building[u->supplier];
				b->transporter[u->transportRes] = -1;
			}
		}
		if((mode == GOINGTOSUPPLIER || mode == GOINGTODEMANDERB) && targtype == GOINGTODEMANDERB)
		{
			if(target >= 0)
			{
				CBuilding* b = &g_building[target];
				b->transporter[transportRes] = -1;
			}
		}
		else if((mode == GOINGTOSUPPLIER || mode == GOINGTODEMROAD) && targtype == GOINGTODEMROAD)
		{
			RoadAt(target, target2)->transporter[transportRes] = -1;
		}
		else if((mode == GOINGTOSUPPLIER || mode == GOINGTODEMPOWL) && targtype == GOINGTODEMPOWL)
		{
			PowlAt(target, target2)->transporter[transportRes] = -1;
		}
		else if((mode == GOINGTOSUPPLIER || mode == GOINGTODEMPIPE) && targtype == GOINGTODEMPIPE)
		{
			PipeAt(target, target2)->transporter[transportRes] = -1;
		}
#endif
		u->targtype = TARG_NONE;

		if(u->driver >= 0)
		{
			//LastNum("resetmode 1b");
			//g_unit[u->driver].Disembark();
			u->driver = -1;
		}
	}

	//LastNum("resetmode 2");

	//transportAmt = 0;
	u->target = u->target2 = -1;
	u->supplier = -1;
	u->mode = UMODE_NONE;
	ResetGoal(u);

	//LastNum("resetmode 3");
}

void ResetTarget(Unit* u)
{
	u->target = -1;
	ResetMode(u);
}
