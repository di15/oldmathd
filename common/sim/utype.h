#ifndef UNITTYPE_H
#define UNITTYPE_H

#include "../math/vec3i.h"
#include "../math/vec2i.h"
#include "../math/vec2s.h"
#include "../render/heightmap.h"
#include "resources.h"
#include "../math/vec3f.h"
#include "../path/pathnode.h"

class UType
{
public:
#if 0
	unsigned int texindex;
	Vec2i bilbsize;
#endif
	unsigned int* sprite[DIRS];
	int nframes;
	Vec2s size;
	char name[64];
	int starthp;
	int cmspeed;
	int cost[RESOURCES];
	bool walker;
	bool landborne;
	bool roaded;
	bool seaborne;
	bool airborne;
	bool military;

	UType()
	{
		//TODO move all bl, u, cd constructors to Init();
		for(int s=0; s<DIRS; s++)
		{
			sprite[s] = NULL;
		}
		
		nframes = 0;
	}

	~UType()
	{
		free();
	}

	void free()
	{
		
		for(int s=0; s<DIRS; s++)
		{
			if(!sprite[s])
				continue;

			delete [] sprite[s];
			sprite[s] = NULL;
		}
		
		nframes = 0;
	}
};

#define UNIT_LABOURER		0
#define UNIT_BATTLECOMP		1
#define UNIT_TRUCK			2
#define UNIT_CARLYLE		3
#define UNIT_TYPES			4

extern UType g_utype[UNIT_TYPES];

void DefU(int type, const char* sprel, int nframes, Vec2s size, const char* name, int starthp, bool landborne, bool walker, bool roaded, bool seaborne, bool airborne, int cmspeed, bool military);
void UCost(int type, int res, int amt);

#endif
