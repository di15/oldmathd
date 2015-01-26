
#include "buildingtype.h"

BlType g_bltype[BL_TYPES];

BlType::BlType()
{
}

void DefB(int type, const char* name, Vec2i size, bool hugterr, const char* sprrelative, int foundation, int reqdeposit)
{
	BlType* t = &g_bltype[type];
	t->widthx = size.x;
	t->widthz = size.y;
	sprintf(t->name, name);

#if 1
	char texpath[MAX_PATH+1];
	sprintf(texpath, "%s_fr000.png", sprrelative);

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

	t->foundation = foundation;
	t->hugterr = hugterr;

	Zero(t->input);
	Zero(t->output);
	Zero(t->conmat);

	t->reqdeposit = reqdeposit;
}

void BConMat(int type, int res, int amt)
{
	BlType* t = &g_bltype[type];
	t->conmat[res] = amt;
}

void BInput(int type, int res, int amt)
{
	BlType* t = &g_bltype[type];
	t->input[res] = amt;
}

void BOutput(int type, int res, int amt)
{
	BlType* t = &g_bltype[type];
	t->output[res] = amt;
}

void BDesc(int type, const char* desc)
{
	BlType* t = &g_bltype[type];
	t->desc = desc;
}

void BEmitter(int type, int emitterindex, int ptype, Vec3f offset)
{
	BlType* t = &g_bltype[type];
	EmitterPlace* e = &t->emitterpl[emitterindex];
	*e = EmitterPlace(ptype, offset);
}
