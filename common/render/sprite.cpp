#include "sprite.h"
#include "../utils.h"
#include "../texture.h"
#include "../gui/gui.h"

std::vector<SpriteToLoad> g_spriteload;
int g_lastLSp = -1;

Sprite::Sprite()
{
	difftexi = 0;
	teamtexi = 0;
	pixels = NULL;
}

Sprite::~Sprite()
{
    free();
}

void Sprite::free()
{
    if(pixels)
    {
        delete pixels;
        pixels = NULL;
    }
}

bool Load1Sprite()
{
	if(g_lastLSp+1 < g_spriteload.size())
		Status(g_spriteload[g_lastLSp+1].relative.c_str());

	CHECKGLERROR();

	if(g_lastLTex >= 0)
	{
		SpriteToLoad* s = &g_spriteload[g_lastLSp];
		LoadSprite(s->relative.c_str(), s->sprite);
	}

	g_lastLSp ++;

	if(g_lastLSp >= g_spriteload.size())
	{
		g_spriteload.clear();
		return false;	// Done loading all textures
	}

	return true;	// Not finished loading textures
}

void QueueSprite(const char* relative, Sprite* s, bool loadteam)
{
	SpriteToLoad stl;
	stl.relative = relative;
	stl.sprite = s;
	stl.loadteam = loadteam;
	g_spriteload.push_back(stl);
}

void LoadSprite(const char* relative, Sprite* s, bool loadteam)
{
	char reltxt[MAX_PATH+1];
	char relpng[MAX_PATH+1];
	char relteampng[MAX_PATH+1];
	sprintf(reltxt, "%s.txt", relative);
	sprintf(relpng, "%s.png", relative);
	sprintf(relteampng, "%s_team.png", relative);
	ParseSprite(reltxt, s);

	QueueTexture(&s->difftexi, relpng, true, false);
	if(loadteam)
		QueueTexture(&s->teamtexi, relteampng, true, false);
	
	char full[MAX_PATH+1];
	FullPath(relpng, full);
	s->pixels = LoadTexture(full);
}

void ParseSprite(const char* relative, Sprite* s)
{
	char fullpath[MAX_PATH+1];
	FullPath(relative, fullpath);

	FILE* fp = fopen(fullpath, "r");
	if(!fp) return;

	float centerx;
	float centery;
	float width;
	float height;

	fscanf(fp, "%f %f %f %f", &centerx, &centery, &width, &height);
	s->offset[0] = -centerx;
	s->offset[1] = -centery;
	s->offset[2] = s->offset[0] + width;
	s->offset[3] = s->offset[1] + height;

	fclose(fp);
}

void DefS(const char* relative, Sprite* s, int offx, int offy)
{
	CreateTexture(s->difftexi, relative, true, false);
	s->offset[0] = offx;
	s->offset[1] = offy;

	char full[1024];
	FullPath(relative, full);
    s->pixels = LoadTexture(full);
}

bool PlayAnimation(float& frame, int first, int last, bool loop, float rate)
{
    if(frame < first || frame > last+1)
    {
        frame = first;
        return false;
    }

    frame += rate;

    if(frame > last)
    {
        if(loop)
            frame = first;
		else
			frame = last;

        return true;
    }

    return false;
}

//Play animation backwards
bool PlayAnimationB(float& frame, int first, int last, bool loop, float rate)
{
    if(frame < first-1 || frame > last)
    {
        frame = last;
        return false;
    }

    frame -= rate;

    if(frame < first)
    {
        if(loop)
            frame = last;
		else
			frame = first;

        return true;
    }

    return false;
}
