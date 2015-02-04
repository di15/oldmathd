#ifndef CONDUIT_H
#define CONDUIT_H

#include "connectable.h"
#include "resources.h"
#include "../render/vertexarray.h"
#include "../math/vec3i.h"
#include "../render/heightmap.h"

#define CONDUIT_ROAD	0
#define CONDUIT_POWL	1
#define CONDUIT_CRPIPE	2
#define CONDUIT_TYPES	3

class CdTile
{
public:
	bool on;
	unsigned char conntype;
	bool finished;
	unsigned char owner;
	int conmat[RESOURCES];
	short netw;	//network
	//bool inaccessible;
	short transporter[RESOURCES];
	Vec3f drawpos;
	//int maxcost[RESOURCES];
	int conwage;

	CdTile();
	~CdTile();

	//virtual unsigned char cdtype();
	int netreq(int res, unsigned char cdtype);
	void destroy();
	void allocate(unsigned char cdtype);
	bool checkconstruction(unsigned char cdtype);
	virtual void fillcollider();
	virtual void freecollider();
};

class CdType
{
public:
	int conmat[RESOURCES];
	unsigned short netwoff;	//offset to network list in Building class
	unsigned short seloff;	//offset to selection list in Selection class
	//TO DO elevation inclines
	unsigned int sprite[CONNECTION_TYPES][2];	//0 = not finished, 1 = finished/constructed
	unsigned short maxforwincl;
	unsigned short maxsideincl;
	bool blconduct;	//do buildings conduct this resource (also act as conduit in a network?)
	Vec2i physoff;	//offset in cm
	Vec3f drawoff;	//offset in cm
	CdTile* cdtiles[2];	//0 = actual placed, 1 = plan proposed
	bool cornerpl;	//is the conduit centered on corners or tile centers?
	char name[32];
	std::string desc;
	unsigned int lacktex;

	CdType()
	{
		Zero(conmat);
		blconduct = false;
		cdtiles[0] = NULL;
		cdtiles[1] = NULL;
		cornerpl = false;
		name[0] = '\0';
		lacktex = 0;
	}

	~CdType()
	{
		for(int i=0; i<2; i++)
			if(cdtiles[i])
			{
				delete [] cdtiles[i];
				cdtiles[i] = NULL;
			}
	}
};

extern CdType g_cdtype[CONDUIT_TYPES];

inline CdTile* GetCd(unsigned char ctype, int tx, int tz, bool plan)
{
	CdType* ct = &g_cdtype[ctype];
	CdTile* tilesarr = ct->cdtiles[(int)plan];
	return &tilesarr[ tx + tz*g_mapsz.x ];
}

class Building;

void DefCo(unsigned char ctype,
			const char* name,
           unsigned short netwoff,
           unsigned short seloff,
           unsigned short maxforwincl,
           unsigned short maxsideincl,
           bool blconduct,
           bool cornerpl,
           Vec2i physoff,
           Vec3f drawoff,
		   const char* lacktex);
void CoDesc(unsigned char ctype, const char* desc);
void CoConMat(unsigned char ctype, unsigned char rtype, short ramt);
void UpdCoPlans(unsigned char ctype, char owner, Vec3f start, Vec3f end);
void ClearCoPlans(unsigned char ctype);
void ReNetw(unsigned char ctype);
void ResetNetw(unsigned char ctype);
bool ReNetwB(unsigned char ctype);
void MergeNetw(unsigned char ctype, int A, int B);
bool ReNetwTiles(unsigned char ctype);
bool CompareCo(unsigned char ctype, CdTile* ctile, int tx, int tz);
bool BAdj(unsigned char ctype, int i, int tx, int tz);
bool CompareB(unsigned char ctype, Building* b, CdTile* ctile);
bool CoLevel(unsigned char ctype, float iterx, float iterz, float testx, float testz, float dx, float dz, int i, float d, bool plantoo);
void RemeshCd(unsigned char ctype, int tx, int tz, bool plan);
void PlaceCo(unsigned char ctype);
void PlaceCo(unsigned char ctype, int tx, int tz, int owner, bool plan);
void Repossess(unsigned char ctype, int tx, int tz, int owner);
void DrawCo(unsigned char ctype);
void CdXZ(unsigned char ctype, CdTile* ctile, bool plan, int& tx, int& tz);
void DefConn(unsigned char conduittype, unsigned char connectiontype, bool finished, const char* modelfile, const Vec3f scale, Vec3f transl);
void PruneCo(unsigned char ctype);

#endif
