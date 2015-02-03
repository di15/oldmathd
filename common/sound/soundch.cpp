

#include "soundch.h"

SoundCh g_soundch[SOUND_CHANNELS];

int NewChan()
{
	int best = 0;
	unsigned long long bestd = 0;
	unsigned long long now = GetTickCount64();

	for(unsigned char i=0; i<SOUND_CHANNELS; i++)
	{
		SoundCh* c = &g_soundch[i];
		unsigned long long d = now - c->last;

		if(d < bestd)
			continue;

		bestd = d;
		best = i;
	}

	return best;
}