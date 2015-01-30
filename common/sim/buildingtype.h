
#ifndef BUILDINGTYPE_H
#define BUILDINGTYPE_H

#include "../math/vec3f.h"
#include "../math/vec2i.h"
#include "resources.h"
#include "../render/billboard.h"
#include "../render/particle.h"
#include "infrastructure.h"
#include "../render/sprite.h"

#define MAX_B_EMITTERS	10

class BlType
{
public:
	// width in tiles
	int widthx;
	int widthz;

	int model;
	int cmodel;

	char name[64];

	int foundation;

	int input[RESOURCES];
	int output[RESOURCES];

	int conmat[RESOURCES];

	int reqdeposit;

	EmitterPlace emitterpl[MAX_B_EMITTERS];

	bool hugterr;

	std::string desc;
	Sprite sprite;

	BlType();
};

#define FOUNDATION_LAND			0
#define FOUNDATION_COASTAL		1
#define FOUNDATION_SEA			2

#define BL_NONE			-1
#define BL_APARTMENT	0
#define BL_HOUSE1		1
#define BL_FACTORY		2
#define BL_REFINERY		3
#define BL_NUCPOW		4
#define BL_FARM			5
#define BL_STORE		6
#define BL_HARBOUR		7
#define BL_OILWELL		8
#define BL_SHMINE		9
#define BL_GASSTATION	10
#define BL_CHEMPL		11
#define BL_ELECPL		12
#define BL_IRONSM		13
#define BL_TYPES		14

#define BL_ROAD			(BL_TYPES+CONDUIT_ROAD)
#define BL_POWL			(BL_TYPES+CONDUIT_POWL)
#define BL_CRPIPE			(BL_TYPES+CONDUIT_CRPIPE)
//#define BL_WATERPIPE		(BL_TYPES+4)

#define TOTAL_BUILDABLES		(BL_TYPES+CONDUIT_TYPES)

extern BlType g_bltype[BL_TYPES];

void DefB(int type, const char* name, Vec2i size, bool hugterr, const char* sprrelative, int foundation, int reqdeposit);
void BConMat(int type, int res, int amt);
void BInput(int type, int res, int amt);
void BOutput(int type, int res, int amt);
void BEmitter(int type, int emitterindex, int ptype, Vec3f offset);
void BDesc(int type, const char* desc);

#endif
