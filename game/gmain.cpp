#include "gmain.h"
#include "../common/gui/gui.h"
#include "keymap.h"
#include "../common/render/shader.h"
#include "gres.h"
#include "../common/gui/font.h"
#include "../common/texture.h"
#include "../common/math/frustum.h"
#include "gui/ggui.h"
#include "../common/gui/gui.h"
#include "../common/debug.h"
#include "../common/render/heightmap.h"
#include "../common/sim/map.h"
#include "../common/math/camera.h"
#include "../common/window.h"
#include "../common/utils.h"
#include "../common/sim/simdef.h"
#include "../common/math/hmapmath.h"
#include "../common/sim/unit.h"
#include "../common/sim/building.h"
#include "../common/sim/build.h"
#include "../common/sim/bltype.h"
#include "../common/render/foliage.h"
#include "../common/render/water.h"
#include "../common/sim/road.h"
#include "../common/sim/crpipe.h"
#include "../common/sim/powl.h"
#include "../common/sim/deposit.h"
#include "../common/sim/selection.h"
#include "../common/sim/player.h"
#include "../common/sim/order.h"
#include "../common/render/transaction.h"
#include "../common/path/collidertile.h"
#include "../common/path/pathdebug.h"
#include "gui/playgui.h"
#include "../common/gui/widgets/spez/botpan.h"
#include "../common/texture.h"
#include "../common/script/script.h"
#include "../common/ai/ai.h"
#include "../common/net/lockstep.h"
#include "../common/sim/simflow.h"
#include "../common/sim/transport.h"
#include "../common/render/infoov.h"
#include "../common/sound/soundch.h"
#include "../common/net/netconn.h"
#include "../common/econ/demand.h"
#include "../common/save/savemap.h"
#include "../common/render/graph.h"
#include "../common/path/tilepath.h"

int g_mode = APPMODE_LOADING;

//static long long g_lasttime = GetTickCount();

#ifdef DEMO
static long long g_demostart = GetTickCount64();
#endif

void SkipLogo()
{
	g_mode = APPMODE_LOADING;
	Player* py = &g_player[g_localP];
	GUI* gui = &g_gui;
	gui->closeall();
	gui->open("load");
}

void UpdLogo()
{
	static int stage = 0;

	Player* py = &g_player[g_localP];
	GUI* gui = &g_gui;

	if(stage < 60)
	{
		float a = (float)stage / 60.0f;
		gui->get("logo")->get("logo")->m_rgba[3] = a;
	}
	else if(stage < 120)
	{
		float a = 1.0f - (float)(stage-60) / 60.0f;
		gui->get("logo")->get("logo")->m_rgba[3] = a;
	}
	else
		SkipLogo();

	stage++;
}

int g_restage = 0;
void UpdLoad()
{
	Player* py = &g_player[g_localP];
	GUI* gui = &g_gui;

	switch(g_restage)
	{
	case 0:
		if(!Load1Sprite()) g_restage++;
		break;
	case 1:
		if(!Load1Texture())
		{
			g_mode = APPMODE_MENU;
			gui->closeall();
			gui->open("main");
			//g_mode = APPMODE_PLAY;
			//Click_NewGame();
			//Click_OpenEditor();
		}
		break;
	}
}

void UpdReload()
{
#if 0
	switch(g_reStage)
	{
	case 0:
		if(!Load1Texture())
		{
			g_mode = APPMODE_MENU;
		}
		break;
	}
#else
	g_restage = 0;
	g_lastLTex = -1;
	//g_lastmodelload = -1;
	g_lastLSp = -1;
	g_gui.freech();
	//FreeModels();
	FreeSprites();
	FreeTextures();
	DestroyWindow(TITLE);
	//ReloadTextures();
	MakeWindow(TITLE);
	//ReloadModels();	//Important - VBO only possible after window GL context made.
	g_mode = APPMODE_LOADING;
	LoadFonts();
	FillGUI();
	//ReloadTextures();
	Queue();
	char exepath[MAX_PATH+1];
	ExePath(exepath);
	g_log<<"ExePath "<<exepath<<std::endl;
	g_log.flush();
#endif
}

void UpdSim()
{
	if(!CanTurn())
		return;

#ifdef FREEZE_DEBUG
		g_log<<"updturn"<<std::endl;
		g_log.flush();
#endif

#if 1
	//if(g_netframe%100==0)
	//	g_log<<"numlab: "<<CountU(UNIT_LABOURER)<<std::endl;
	if(CountU(UNIT_LABOURER) <= 0 && !g_gameover)
	{
		g_gameover = true;
		char msg[128];
		sprintf(msg, "lasted %u simframes / %fs / %fm / %fh",
			g_simframe,
			(float)g_simframe / (float)SIM_FRAME_RATE,
			(float)g_simframe / (float)SIM_FRAME_RATE / 60.0f,
			(float)g_simframe / (float)SIM_FRAME_RATE / 60.0f / 60.0f);
		InfoMess("Game Over", msg);
#if 0
		EndSess();
		FreeMap();
		Player* py = &g_player[g_localP];
		GUI* gui = &g_gui;
		gui->closeall();
		gui->open("main");
		g_mode = APPMODE_MENU;
		//g_quit = true;
#endif
	}
#endif

	if(g_speed == SPEED_PAUSE)
	{
		UpdTurn();
		g_netframe ++;
		return;
	}

	UpdTurn();

	UpdJams();

#ifdef FREEZE_DEBUG
		g_log<<"updai"<<std::endl;
		g_log.flush();
#endif

	UpdAI();

#ifdef FREEZE_DEBUG
		g_log<<"managetrips"<<std::endl;
		g_log.flush();
#endif

	ManageTrips();
	StartTimer(TIMER_UPDATEUNITS);

#ifdef FREEZE_DEBUG
		g_log<<"updunits"<<std::endl;
		g_log.flush();
#endif

	UpdUnits();
	StopTimer(TIMER_UPDATEUNITS);
	StartTimer(TIMER_UPDATEBUILDINGS);

#ifdef FREEZE_DEBUG
		g_log<<"updbls"<<std::endl;
		g_log.flush();
#endif

	UpdBls();
	StopTimer(TIMER_UPDATEBUILDINGS);

	Tally();

#ifdef FREEZE_DEBUG
		g_log<<"/updsim"<<std::endl;
		g_log.flush();
#endif

	static long long tick = GetTickCount64();

	//must do this after UpdTurn
	g_simframe ++;
	g_netframe ++;

	if(g_simframe == 4344000)
	{
		long long off = GetTickCount64() - tick;
		char msg[128];
		sprintf(msg, "atf4,344,000 \n min %f \n s %f", (float)(off)/(float)1000/(float)60, (float)(off)/(float)1000);
		InfoMess(msg, msg);
		//1.7m
	}

#if 1
	if(g_simframe % (SIM_FRAME_RATE * 60 * 30) == 0 && g_simframe > 0)
	{
		char fn[MAX_PATH+1];
		sprintf(fn, "saves/%s autosave.sav", FileDateTime().c_str());
		SaveMap(fn);
	}
#endif
}

void UpdEd()
{
#if 0
	UpdateFPS();
#endif
}

void Update()
{
	//if(g_netmode != NETM_SINGLE)
	if(g_sock)
		UpdNet();

	if(g_mode == APPMODE_LOGO)
		UpdLogo();
	//else if(g_mode == APPMODE_INTRO)
	//	UpdateIntro();
	else if(g_mode == APPMODE_LOADING)
		UpdLoad();
	else if(g_mode == APPMODE_RELOADING)
		UpdReload();
	else if(g_mode == APPMODE_PLAY)
		UpdSim();
	else if(g_mode == APPMODE_EDITOR)
		UpdEd();

#ifdef DEMO
	if(GetTickCount64() - g_demostart > DEMOTIME)
		g_quit = true;
#endif
}

void DrawScene(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float lightpos[3], float lightdir[3])
{
	//return;	//temp

}


void Draw()
{
	StartTimer(TIMER_DRAWSETUP);

	CHECKGLERROR();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CHECKGLERROR();

	Player* py = &g_player[g_localP];
	GUI* gui = &g_gui;
	Camera* c = &g_cam;

	StopTimer(TIMER_DRAWSETUP);

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<std::endl;
	g_log.flush();
#endif

#if 2
	if(g_mode == APPMODE_PLAY || g_mode == APPMODE_EDITOR)
	{
		StartTimer(TIMER_DRAWSETUP);

		StopTimer(TIMER_DRAWSETUP);
	}
	CHECKGLERROR();
#endif

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<std::endl;
	g_log.flush();
#endif

	StartTimer(TIMER_DRAWGUI);
	gui->frameupd();

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<std::endl;
	g_log.flush();
#endif

	CHECKGLERROR();
	//if(!(g_mode == APPMODE_PLAY && g_speed == SPEED_FAST))
	gui->draw();
	StopTimer(TIMER_DRAWGUI);

#if 0
	for(int i=0; i<30; i++)
	{
		int x = rand()%g_width;
		int y = rand()%g_height;

		Blit(blittex, &blitscreen, Vec2i(x,y));
	}

	glDrawPixels(blitscreen.sizeX, blitscreen.sizeY, GL_RGB, GL_BYTE, blitscreen.data);
#endif

	CHECKGLERROR();
	Ortho(g_width, g_height, 1, 1, 1, 1);
	CHECKGLERROR();
	glDisable(GL_DEPTH_TEST);
	CHECKGLERROR();

#if 0
	RichText uni;

	for(int i=16000; i<19000; i++)
		//for(int i=0; i<3000; i++)
	{
		uni.m_part.push_back(RichPart(i));
	}

	float color[] = {1,1,1,1};
	DrawBoxShadText(MAINFONT8, 0, 0, g_width, g_height, &uni, color, 0, -1);
#endif

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<std::endl;
	g_log.flush();
#endif

#if 1
	if(g_debuglines)
	{
		char fpsstr[256];
		sprintf(fpsstr, "drw:%lf (%lf s/frame), upd:%lf (%lf s/frame), simfr:%s", g_instantdrawfps, 1.0/g_instantdrawfps, g_instantupdfps, 1.0/g_instantupdfps, iform(g_simframe).c_str());
		RichText fpsrstr(fpsstr);
		//fpsrstr = ParseTags(fpsrstr, NULL);
		DrawShadowedText(MAINFONT8, 0, g_height-MINIMAP_SIZE-32, &fpsrstr);
		CHECKGLERROR();
	}
#endif

#ifdef DEMO
	{
		unsigned int msleft = DEMOTIME - (GetTickCount64() - g_demostart);
		char msg[128];
		sprintf(msg, "Demo time %d:%02d", msleft / 1000 / 60, (msleft % (1000 * 60)) / 1000);
		RichText msgrt(msg);
		float color[] = {0.5f,1.0f,0.5f,1.0f};
		DrawShadowedText(MAINFONT16, g_width - 130, g_height-16, &msgrt, color);
	}
#endif

	glEnable(GL_DEPTH_TEST);
	EndS();
	CHECKGLERROR();

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<std::endl;
	g_log.flush();
#endif

	SDL_GL_SwapWindow(g_window);

	//CheckNum("post draw");
}

bool OverMinimap()
{
	return false;
}

void Scroll()
{
	//return;	//disable for now

	Player* py = &g_player[g_localP];
	Camera* c = &g_cam;

	if(g_mouseout)
		return;

	bool moved = false;

	//const Uint8 *keys = SDL_GetKeyboardState(NULL);
	//SDL_BUTTON_LEFT;
	if((!g_keyintercepted && (g_keys[SDL_SCANCODE_UP] || g_keys[SDL_SCANCODE_W])) || (g_mouse.y <= SCROLL_BORDER))
	{
		g_scroll.y -= 40;
		moved = true;
	}

	if((!g_keyintercepted && (g_keys[SDL_SCANCODE_DOWN] || g_keys[SDL_SCANCODE_S])) || (g_mouse.y >= g_height-SCROLL_BORDER))
	{
		g_scroll.y += 40;
		moved = true;
	}

	if((!g_keyintercepted && (g_keys[SDL_SCANCODE_LEFT] || g_keys[SDL_SCANCODE_A])) || (g_mouse.x <= SCROLL_BORDER))
	{
		g_scroll.x -= 40;
		moved = true;
	}

	if((!g_keyintercepted && (g_keys[SDL_SCANCODE_RIGHT] || g_keys[SDL_SCANCODE_D])) || (g_mouse.x >= g_width-SCROLL_BORDER))
	{
		g_scroll.x += 40;
		moved = true;
	}

	//TO DO check in map
}

void LoadConfig()
{
	char cfgfull[MAX_PATH+1];
	FullPath(CONFIGFILE, cfgfull);

	std::ifstream f(cfgfull);

	if(!f)
		return;

	std::string line;
	char keystr[128];
	char actstr[128];

	Player* py = &g_player[g_localP];

	while(!f.eof())
	{
		strcpy(keystr, "");
		strcpy(actstr, "");

		getline(f, line);

		if(line.length() > 127)
			continue;

		sscanf(line.c_str(), "%s %s", keystr, actstr);

		float valuef = StrToFloat(actstr);
		int valuei = StrToInt(actstr);
		bool valueb = valuei ? true : false;

		if(stricmp(keystr, "fullscreen") == 0)					g_fullscreen = valueb;
		else if(stricmp(keystr, "client_width") == 0)			g_width = g_selectedRes.width = valuei;
		else if(stricmp(keystr, "client_height") == 0)			g_height = g_selectedRes.height = valuei;
		else if(stricmp(keystr, "screen_bpp") == 0)				g_bpp = valuei;
	}

	f.close();
}

void WriteConfig()
{
	char cfgfull[MAX_PATH+1];
	FullPath(CONFIGFILE, cfgfull);
	FILE* fp = fopen(cfgfull, "w");
	if(!fp)
		return;
	fprintf(fp, "fullscreen %d \r\n\r\n", g_fullscreen ? 1 : 0);
	fprintf(fp, "client_width %d \r\n\r\n", g_selectedRes.width);
	fprintf(fp, "client_height %d \r\n\r\n", g_selectedRes.height);
	fprintf(fp, "screen_bpp %d \r\n\r\n", g_bpp);
	fclose(fp);
}

int testfunc(ObjectScript::OS* os, int nparams, int closure_values, int need_ret_values, void * param)
{
	InfoMess("os", "test");
	return 1;
}

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void SignalCallback(int signum)
{
	//printf("Caught signal %d\n",signum);
	// Cleanup and close up stuff here

	// Terminate program
	//g_quit = true;
	exit(0);	//force quit NOW
}

void Init()
{
#ifdef PLATFORM_LINUX
	signal(SIGINT, SignalCallback);
#endif

	if(SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		char msg[1280];
		sprintf(msg, "SDL_Init: %s\n", SDL_GetError());
		ErrMess("Error", msg);
	}

	if(SDLNet_Init() == -1)
	{
		char msg[1280];
		sprintf(msg, "SDLNet_Init: %s\n", SDLNet_GetError());
		ErrMess("Error", msg);
	}

	SDL_version compile_version;
	const SDL_version *link_version=Mix_Linked_Version();
	SDL_MIXER_VERSION(&compile_version);
	printf("compiled with SDL_mixer version: %d.%d.%d\n",
			compile_version.major,
			compile_version.minor,
			compile_version.patch);
	printf("running with SDL_mixer version: %d.%d.%d\n",
			link_version->major,
			link_version->minor,
			link_version->patch);

	// load support for the OGG and MOD sample/music formats
	//int flags=MIX_INIT_OGG|MIX_INIT_MOD|MIX_INIT_MP3;
	int flags=MIX_INIT_OGG|MIX_INIT_MP3;
	int initted=Mix_Init(flags);
	if( (initted & flags) != flags)
	{
		char msg[1280];
		sprintf(msg, "Mix_Init: Failed to init required ogg and mod support!\nMix_Init: %s", Mix_GetError());
		//ErrMess("Error", msg);
		// handle error
	}

	// start SDL with audio support
	if(SDL_Init(SDL_INIT_AUDIO)==-1) {
		char msg[1280];
		sprintf(msg, "SDL_Init: %s\n", SDL_GetError());
		ErrMess("Error", msg);
		// handle error
		//exit(1);
	}
	// open 44.1KHz, signed 16bit, system byte order,
	//      stereo audio, using 1024 byte chunks
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
		char msg[1280];
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		ErrMess("Error", msg);
		// handle error
		//exit(2);
	}

	Mix_AllocateChannels(SOUND_CHANNELS);

	OpenLog("log.txt", VERSION);

	srand((unsigned int)GetTickCount64());

	LoadConfig();

	//g_os = ObjectScript::OS::create();
	//g_os->pushCFunction(testfunc);
	//g_os->setGlobal("testfunc");
	//os->eval("testfunc();");
	//os->eval("function require(){ /* if(relative == \"called.os\") */ { testfunc(); } }");
	char autoexecpath[MAX_PATH+1];
	FullPath("scripts/autoexec.os", autoexecpath);
	//g_os->require(autoexecpath);
	//g_os->release();

	//EnumerateMaps();
	//EnumerateDisplay();
	MapKeys();

	InitProfiles();
}

void Deinit()
{
	g_gui.freech();

	WriteProfiles(-1, 0);
	DestroyWindow(TITLE);

	const unsigned long long start = GetTickCount64();
	//After quit, wait to send out quit packet to make sure host/clients recieve it.
	while (GetTickCount64() - start < QUIT_DELAY)
	{
		if(NetQuit())
			break;
		if(g_sock)
			UpdNet();
	}

	// Clean up

	if(g_sock)
	{
		SDLNet_UDP_Close(g_sock);
		g_sock = NULL;
	}

	FreeSounds();

	Mix_CloseAudio();

	// force a quit
	//while(Mix_Init(0))
	//	Mix_Quit();
	Mix_Quit();

	SDLNet_Quit();
	SDL_Quit();
}

void EventLoop()
{
#if 0
	key->keysym.scancode
	SDLMod  e.key.keysym.mod
	key->keysym.unicode

	if( mod & KMOD_NUM ) printf( "NUMLOCK " );
	if( mod & KMOD_CAPS ) printf( "CAPSLOCK " );
	if( mod & KMOD_LCTRL ) printf( "LCTRL " );
	if( mod & KMOD_RCTRL ) printf( "RCTRL " );
	if( mod & KMOD_RSHIFT ) printf( "RSHIFT " );
	if( mod & KMOD_LSHIFT ) printf( "LSHIFT " );
	if( mod & KMOD_RALT ) printf( "RALT " );
	if( mod & KMOD_LALT ) printf( "LALT " );
	if( mod & KMOD_CTRL ) printf( "CTRL " );
	if( mod & KMOD_SHIFT ) printf( "SHIFT " );
	if( mod & KMOD_ALT ) printf( "ALT " );
#endif

	//SDL_EnableUNICODE(SDL_ENABLE);

	Player* py = &g_player[g_localP];
	GUI* gui = &g_gui;

	while (!g_quit)
	{
		StartTimer(TIMER_FRAME);
		StartTimer(TIMER_EVENT);

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			InEv ie;
			ie.intercepted = false;
			ie.curst = CU_DEFAULT;

			switch(e.type)
			{
			case SDL_QUIT:
				g_quit = true;
				break;
			case SDL_KEYDOWN:
				ie.type = INEV_KEYDOWN;
				ie.key = e.key.keysym.sym;
				ie.scancode = e.key.keysym.scancode;

				//Handle copy
				if( e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL )
				{
					//SDL_SetClipboardText( inputText.c_str() );
					ie.type = INEV_COPY;
				}
				//Handle paste
				if( e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
				{
					//inputText = SDL_GetClipboardText();
					//renderText = true;
					ie.type = INEV_PASTE;
				}
				//Select all
				if( e.key.keysym.sym == SDLK_a && SDL_GetModState() & KMOD_CTRL )
				{
					//inputText = SDL_GetClipboardText();
					//renderText = true;
					ie.type = INEV_SELALL;
				}

				gui->inev(&ie);

				if(!ie.intercepted)
					g_keys[e.key.keysym.scancode] = true;

				g_keyintercepted = ie.intercepted;
				break;
			case SDL_KEYUP:
				ie.type = INEV_KEYUP;
				ie.key = e.key.keysym.sym;
				ie.scancode = e.key.keysym.scancode;

				gui->inev(&ie);

				if(!ie.intercepted)
					g_keys[e.key.keysym.scancode] = false;

				g_keyintercepted = ie.intercepted;
				break;
			case SDL_TEXTINPUT:
				//g_GUI.charin(e.text.text);	//UTF8
				ie.type = INEV_TEXTIN;
				ie.text = e.text.text;

#if 0
				g_log<<"SDL_TEXTINPUT:";
				for(int i=0; i<strlen(e.text.text); i++)
				{
					g_log<<"[#"<<(unsigned int)(unsigned char)e.text.text[i]<<"]";
				}
				g_log<<std::endl;
				g_log.flush();
#endif

				gui->inev(&ie);
				break;
			case SDL_TEXTEDITING:
				//g_GUI.charin(e.text.text);	//UTF8
				ie.type = INEV_TEXTED;
				ie.text = e.text.text;
				ie.cursor = e.edit.start;
				ie.sellen = e.edit.length;

#if 0
				g_log<<"SDL_TEXTEDITING:";
				for(int i=0; i<strlen(e.text.text); i++)
				{
					g_log<<"[#"<<(unsigned int)(unsigned char)e.text.text[i]<<"]";
				}
				g_log<<std::endl;
				g_log.flush();

				g_log<<"texted: cursor:"<<ie.cursor<<" sellen:"<<ie.sellen<<std::endl;
				g_log.flush();
#endif

				gui->inev(&ie);
#if 0
				ie.intercepted = false;
				ie.type = INEV_TEXTIN;
				ie.text = e.text.text;

				gui->inev(&ie);
#endif
				break;
#if 0
			case SDL_TEXTINPUT:
				/* Add new text onto the end of our text */
				strcat(text, event.text.text);
#if 0
				ie.type = INEV_CHARIN;
				ie.key = wParam;
				ie.scancode = 0;

				gui->inev(&ie);
#endif
				break;
			case SDL_TEXTEDITING:
				/*
				Update the composition text.
				Update the cursor position.
				Update the selection length (if any).
				*/
				composition = event.edit.text;
				cursor = event.edit.start;
				selection_len = event.edit.length;
				break;
#endif
			//else if(e.type == SDL_BUTTONDOWN)
			//{
			//}
			case SDL_MOUSEWHEEL:
				ie.type = INEV_MOUSEWHEEL;
				ie.amount = e.wheel.y;

				gui->inev(&ie);
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch (e.button.button) {
				case SDL_BUTTON_LEFT:
					g_mousekeys[MOUSE_LEFT] = true;
					g_moved = false;

					ie.type = INEV_MOUSEDOWN;
					ie.key = MOUSE_LEFT;
					ie.amount = 1;
					ie.x = g_mouse.x;
					ie.y = g_mouse.y;

					gui->inev(&ie);

					g_keyintercepted = ie.intercepted;
					break;
				case SDL_BUTTON_RIGHT:
					g_mousekeys[MOUSE_RIGHT] = true;

					ie.type = INEV_MOUSEDOWN;
					ie.key = MOUSE_RIGHT;
					ie.amount = 1;
					ie.x = g_mouse.x;
					ie.y = g_mouse.y;

					gui->inev(&ie);
					break;
				case SDL_BUTTON_MIDDLE:
					g_mousekeys[MOUSE_MIDDLE] = true;

					ie.type = INEV_MOUSEDOWN;
					ie.key = MOUSE_MIDDLE;
					ie.amount = 1;
					ie.x = g_mouse.x;
					ie.y = g_mouse.y;

					gui->inev(&ie);
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (e.button.button) {
				case SDL_BUTTON_LEFT:
					g_mousekeys[MOUSE_LEFT] = false;

					ie.type = INEV_MOUSEUP;
					ie.key = MOUSE_LEFT;
					ie.amount = 1;
					ie.x = g_mouse.x;
					ie.y = g_mouse.y;

					gui->inev(&ie);
					break;
				case SDL_BUTTON_RIGHT:
					g_mousekeys[MOUSE_RIGHT] = false;

					ie.type = INEV_MOUSEUP;
					ie.key = MOUSE_RIGHT;
					ie.amount = 1;
					ie.x = g_mouse.x;
					ie.y = g_mouse.y;

					gui->inev(&ie);
					break;
				case SDL_BUTTON_MIDDLE:
					g_mousekeys[MOUSE_MIDDLE] = false;

					ie.type = INEV_MOUSEUP;
					ie.key = MOUSE_MIDDLE;
					ie.amount = 1;
					ie.x = g_mouse.x;
					ie.y = g_mouse.y;

					gui->inev(&ie);
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				//g_mouse.x = e.motion.x;
				//g_mouse.y = e.motion.y;

				if(g_mouseout) {
					//TrackMouse();
					g_mouseout = false;
				}
				if(MousePosition()) {
					g_moved = true;

					ie.type = INEV_MOUSEMOVE;
					ie.x = g_mouse.x;
					ie.y = g_mouse.y;

					gui->inev(&ie);

					g_curst = ie.curst;
				}
				break;
			}
		}

		StopTimer(TIMER_EVENT);
#if 1
		//if ((g_mode == APPMODE_LOADING || g_mode == APPMODE_RELOADING) || true /* DrawNextFrame(DRAW_FRAME_RATE) */ )
		if ((g_mode == APPMODE_LOADING || g_mode == APPMODE_RELOADING) || DrawNextFrame(DRAW_FRAME_RATE) )
#endif
		{
			StartTimer(TIMER_DRAW);

#ifdef DEBUG
			g_log<<"main "<<__FILE__<<" "<<__LINE__<<std::endl;
			g_log.flush();
#endif
			CalcDrawRate();

			CHECKGLERROR();

#ifdef DEBUG
			g_log<<"main "<<__FILE__<<" "<<__LINE__<<std::endl;
			g_log.flush();
#endif

			Draw();
			CHECKGLERROR();

			if(g_mode == APPMODE_PLAY || g_mode == APPMODE_EDITOR)
			{
#ifdef DEBUG
				g_log<<"main "<<__FILE__<<" "<<__LINE__<<std::endl;
				g_log.flush();
#endif
				Scroll();
#ifdef DEBUG
				g_log<<"main "<<__FILE__<<" "<<__LINE__<<std::endl;
				g_log.flush();
#endif
				UpdResTicker();
			}

			StopTimer(TIMER_DRAW);
		}

		//if((g_mode == APPMODE_LOADING || g_mode == APPMODE_RELOADING) || true /* UpdNextFrame(SIM_FRAME_RATE) */ )
		if((g_mode == APPMODE_LOADING || g_mode == APPMODE_RELOADING) || UpdNextFrame(SIM_FRAME_RATE) )
		{
			StartTimer(TIMER_UPDATE);

#ifdef DEBUG
			g_log<<"main "<<__FILE__<<" "<<__LINE__<<std::endl;
			g_log.flush();
#endif
			CalcUpdRate();
			Update();

			StopTimer(TIMER_UPDATE);
		}

		StopTimer(TIMER_FRAME);
	}
}

#ifdef PLATFORM_WIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
	//g_log << "Log start"    << std::endl; /* TODO, include date */
	//g_log << "Init: "       << std::endl;
	//g_log.flush();

	Init();

	g_log << "MakeWindow: " << std::endl;
	g_log.flush();

	MakeWindow(TITLE);

	g_log << "FillGUI: "    << std::endl;
	g_log.flush();

	FillGUI();

	g_log << "Queue: "      << std::endl;
	g_log.flush();

	SDL_ShowCursor(false);
	Queue();

	g_log << "EventLoop: "  << std::endl;
	g_log.flush();

	EventLoop();

	g_log << "Deinit: "     << std::endl;
	g_log.flush();

	Deinit();
	SDL_ShowCursor(true);

	return 0;
}
