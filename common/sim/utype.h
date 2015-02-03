#ifndef UNITTYPE_H
#define UNITTYPE_H

#include "../math/vec3i.h"
#include "../math/vec2i.h"
#include "../render/heightmap.h"
#include "resources.h"
#include "../math/vec3f.h"

class UType
{
public:
	Sprite sprite;
	Vec3i size;
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
};

#if 0
#define UNIT_LABOURER		0
#if 0
#define UNIT_BLIZZAIAMECH	1
#define UNIT_918132634MECH	2
#define UNIT_CALMMECH		3
#define UNIT_GIZAMECH		4
#define UNIT_ZENITHMECH		5
#define UNIT_M1A2TANK		6
#define UNIT_OPLOTTANK		7
#define UNIT_LAVAPC			8
#define UNIT_GEPARDAA		9
#define UNIT_TYPES			10
#else
#define UNIT_TRUCK			1
#define UNIT_SOLDIER		2
#define UNIT_CAR			3
#define UNIT_CONCTRUCK		4
#define UNIT_TANKERTRUCK	5
#define UNIT_CARGOSHIP		6
#define UNIT_LOGHARVESTER	7
#define UNIT_LOGFORWARDER	8
#define UNIT_TYPES			9
#endif
#endif

#define UNIT_ROBOSOLDIER	0
#define UNIT_LABOURER		1
#define UNIT_TRUCK			2
#define UNIT_TYPES			3

extern UType g_utype[UNIT_TYPES];

void DefU(int type, const char* sprrelative, Vec3i size, const char* name, int starthp, bool landborne, bool walker, bool roaded, bool seaborne, bool airborne, int cmspeed, bool military);

#endif
