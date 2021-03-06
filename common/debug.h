#ifndef DEBUG_H
#define DEBUG_H

#include "platform.h"
#include "gui/richtext.h"

#define	TIMER_FRAME				0
#define TIMER_EVENT				1
#define TIMER_UPDATE			2
#define TIMER_DRAW				3
#define TIMER_DRAWSETUP			4
#define TIMER_DRAWGUI			5
#define TIMER_DRAWMINIMAP		6
#define TIMER_UPDATEUNITS		7
#define TIMER_UPDATEBUILDINGS	8
#define TIMER_UPDUONCHECK		9
#define TIMER_UPDUNITAI			10
#define TIMER_MOVEUNIT			11
#define TIMER_ANIMUNIT			12
#define TIMER_DRAWBL			13
#define TIMER_DRAWUNITS			14
#define TIMER_DRAWRIM			15
#define TIMER_DRAWWATER			16
#define TIMER_DRAWCRPIPES		17
#define TIMER_DRAWPOWLS			18
#define TIMER_DRAWFOLIAGE		19
#define TIMER_SORTPARTICLES		20
#define TIMER_DRAWPARTICLES		21
#define TIMER_DRAWMAP			22
#define TIMER_DRAWSCENEDEPTH	23
#define TIMER_DRAWSKY			24
#define TIMER_DRAWROADS			25
#define TIMER_DRAWMAPDEPTH		26
#define TIMER_DRAWUNITSDEPTH	27
#define TIMER_DRAWUMAT			28
#define TIMER_DRAWUTEXBIND		29
#define TIMER_DRAWUGL			30
#define TIMER_MANAGETRIPS		31
#define TIMER_UPDLAB			32
#define TIMER_UPDTRUCK			33
#define TIMER_FINDJOB			34
#define TIMER_JOBLIST			35
#define TIMER_JOBSORT			36
#define TIMER_JOBPATH			37
#define TIMER_RESETPATHNODES	38
#define TIMERS					39

class Timer
{
public:
	char name[64];
	double averagems;
	int lastframe;
	int frames;
	//double framems;
	unsigned long long starttick;
	//double timescountedperframe;
	//double lastframeaverage;
	int lastframeelapsed;
	int inside;

	Timer()
	{
		averagems = 0.0;
		lastframeelapsed = 0;
		lastframe = 0;
		frames = 0;
		inside = -1;
	}
};

extern Timer g_profile[TIMERS];
extern bool g_debuglines;

void StartTimer(int id);
void StopTimer(int id);
void DefTimer(int id, int inside, char* name);

void WriteProfiles(int in, int layer);
void InitProfiles();

void LogRich(const RichText* rt);
void LastNum(const char* l);
void CheckMem(const char* file, int line, const char* sep);

#ifdef GLDEBUG
void CheckGLError(const char* file, int line);
#endif

//#define MEMDEBUG
#ifndef MEMDEBUG
#define CheckMem(a,b,c) (void)0
#endif

#ifndef MATCHMAKER
GLvoid APIENTRY GLMessageHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);
#endif

#endif	//DEBUG_H
