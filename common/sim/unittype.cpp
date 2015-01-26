#include "unittype.h"
#include "../texture.h"
#include "resources.h"
#include "../render/sprite.h"
#include "../utils.h"

UType g_utype[UNIT_TYPES];

void DefU(int type, const char* sprrelative, Vec3i size, const char* name, int starthp, bool landborne, bool walker, bool roaded, bool seaborne, bool airborne, int cmspeed, bool military)
{
	UType* t = &g_utype[type];

#if 1
	char texpath[MAX_PATH+1];
	sprintf(texpath, "%s_si0_fr000.png", sprrelative);

	CreateTexture(t->sprite.texindex, texpath, true, false);
#endif

#if 1
	char infopath[MAX_PATH+1];
	strcpy(infopath, texpath);
	StripExtension(infopath);
	strcat(infopath, ".txt");

	std::ifstream infos(infopath);

	if(!infos)
		return;

	int centeroff[2];
	int imagesz[2];
	int clipsz[2];

	infos>>centeroff[0]>>centeroff[1];
	infos>>imagesz[0]>>imagesz[1];
	infos>>clipsz[0]>>clipsz[1];

	t->sprite.offset[0] = -centeroff[0];
	t->sprite.offset[1] = -centeroff[1];
	t->sprite.offset[2] = t->sprite.offset[0] + imagesz[0];
	t->sprite.offset[3] = t->sprite.offset[1] + imagesz[1];
#endif

	t->size = size;
	strcpy(t->name, name);
	t->starthp = starthp;
	Zero(t->cost);
	t->landborne = landborne;
	t->walker = walker;
	t->roaded = roaded;
	t->seaborne = seaborne;
	t->airborne = airborne;
	t->cmspeed = cmspeed;
	t->military = true;
}
