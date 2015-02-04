#ifndef SPRITE_H
#define SPRITE_H

#include "../texture.h"

class Sprite
{
public:

	Sprite();
	~Sprite();
	void free();

	bool on;
	unsigned int difftexi;
	unsigned int teamtexi;
	float offset[4];
	LoadedTex* pixels;
	std::string fullpath;
};

#define SPRITES	1024
extern Sprite g_sprite[SPRITES];

class SpriteToLoad
{
public:
	std::string relative;
	unsigned int* spindex;
	bool loadteam;
};

extern int g_lastLSp;

bool Load1Sprite();
void FreeSprites();
void LoadSprite(const char* relative, unsigned int* spindex, bool loadteam);
void QueueSprite(const char* relative, unsigned int* spindex, bool loadteam);
void ParseSprite(const char* relative, Sprite* s);
bool PlayAnimation(float& frame, int first, int last, bool loop, float rate);
bool PlayAnimationB(float& frame, int first, int last, bool loop, float rate);	//Play animation backwards

#endif
