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
#include "../common/sim/map.h"
#include "../common/render/drawqueue.h"

int g_mode = APPMODE_LOADING;

double g_instantupdfps = 0;
double g_updfrinterval = 0;

void UpdateLoading()
{
	static int stage = 0;

	switch(stage)
	{
	case 0:
		if(!Load1Texture())
		{
			g_mode = APPMODE_MENU;
			//g_mode = APPMODE_PLAY;
			Click_NewGame();
			//Click_OpenEditor();
		}
		break;
	}
}

int g_reStage = 0;
void UpdateReloading()
{
	switch(g_reStage)
	{
	case 0:
		if(!Load1Texture())
		{
			g_mode = APPMODE_MENU;
		}
		break;
	}
}

void CalcUpdRate()
{
	static unsigned int frametime = 0;				// This stores the last frame's time
	static int framecounter = 0;
	static unsigned int lasttime;

	// Get the current time in seconds
	unsigned int currtime = timeGetTime();

	// We added a small value to the frame interval to account for some video
	// cards (Radeon's) with fast computers falling through the floor without it.

	// Here we store the elapsed time between the current and last frame,
	// then keep the current frame in our static variable for the next frame.
	g_updfrinterval = (currtime - frametime) / 1000.0f;	// + 0.005f;

	//g_instantdrawfps = 1.0f / (g_currentTime - frameTime);
	//g_instantdrawfps = 1.0f / g_drawfrinterval;

	frametime = currtime;

	// Increase the frame counter
	++framecounter;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if( currtime - lasttime > 1000 )
	{
		g_instantupdfps = framecounter;

		// Here we set the lastTime to the currentTime
		lasttime = currtime;

		// Reset the frames per second
		framecounter = 0;
	}
}

bool UpdNextFrame(int desiredFrameRate)
{
	static long long lastTime = GetTickCount64();
	static long long elapsedTime = 0;

	long long currentTime = GetTickCount64(); // Get the time (milliseconds = seconds * .001)
	long long deltaTime = currentTime - lastTime; // Get the slice of time
	int desireddelay = (int)(1000 / (float)desiredFrameRate); // Store 1 / desiredFrameRate

	elapsedTime += deltaTime; // Add to the elapsed time
	lastTime = currentTime; // Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desireddelay )
	{
		elapsedTime -= desireddelay; // Adjust the elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
	/*
	long long currentTime = GetTickCount();
	float desiredFPMS = 1000.0f/(float)desiredFrameRate;
	int deltaTime = currentTime - g_lasttime;

	if(deltaTime >= desiredFPMS)
	{
		g_lasttime = currentTime;
		return true;
	}

	return false;*/
}

void UpdateGameState()
{
	g_simframe ++;

	StartTimer(TIMER_UPDATEUNITS);
	UpdUnits();
	StopTimer(TIMER_UPDATEUNITS);
	StartTimer(TIMER_UPDATEBUILDINGS);
	UpdBls();
	StopTimer(TIMER_UPDATEBUILDINGS);
}

void UpdateEditor()
{
#if 0
	UpdateFPS();
#endif
}

void Update()
{
	//else if(g_mode == APPMODE_INTRO)
	//	UpdateIntro();
	if(g_mode == APPMODE_LOADING)
		UpdateLoading();
	else if(g_mode == APPMODE_RELOADING)
		UpdateReloading();
	else if(g_mode == APPMODE_PLAY)
		UpdateGameState();
	else if(g_mode == APPMODE_EDITOR)
		UpdateEditor();
}

void Draw()
{
	StartTimer(TIMER_DRAWSETUP);

	CHECKGLERROR();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CHECKGLERROR();

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;
	Camera* c = &py->camera;

	StopTimer(TIMER_DRAWSETUP);

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<endl;
    g_log.flush();
#endif

	CHECKGLERROR();

#if 1
	if(g_mode == APPMODE_PLAY || g_mode == APPMODE_EDITOR)
	{
		//glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);

		StartTimer(TIMER_DRAWSETUP);

        StopTimer(TIMER_DRAWSETUP);

#if 0
		UseS(SHADER_WORLDCOLOR2D);
		glUniform1f(g_shader[g_curS].m_slot[SSLOT_WIDTH], (float)g_drawwidth);
		glUniform1f(g_shader[g_curS].m_slot[SSLOT_HEIGHT], (float)g_drawheight);
		glUniform4f(g_shader[g_curS].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
		glUniform2f(g_shader[g_curS].m_slot[SSLOT_SCROLL], g_scroll.x, g_scroll.y);
		DrawSelection();
#endif

		UseS(SHADER_WORLDORTHO);
		glUniform1f(g_shader[g_curS].m_slot[SSLOT_WIDTH], (float)g_width);
		glUniform1f(g_shader[g_curS].m_slot[SSLOT_HEIGHT], (float)g_height);
		glUniform4f(g_shader[g_curS].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
		glUniform2f(g_shader[g_curS].m_slot[SSLOT_SCROLL], g_scroll.x, g_scroll.y);
		//DrawBuildings();
		//DrawUnits();
		DrawQueue();
		
		glEnable(GL_CULL_FACE);
		//glEnable(GL_BLEND);
	}
	CHECKGLERROR();
#endif

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<endl;
    g_log.flush();
#endif

	StartTimer(TIMER_DRAWGUI);
	gui->frameupd();

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<endl;
    g_log.flush();
#endif

	CHECKGLERROR();
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
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<endl;
    g_log.flush();
#endif

#if 0
	char fpsstr[256];
	sprintf(fpsstr, "draw fps: %lf (%lf s/frame), upd fps: %lf (%lf s/frame), zoom: %f, simframe: %lld", g_instantdrawfps, 1.0/g_instantdrawfps, g_instantupdfps, 1.0/g_instantupdfps, py->zoom, g_simframe);
	RichText fpsrstr(fpsstr);
	DrawShadowedText(MAINFONT8, 0, g_height-MINIMAP_SIZE-32-10, &fpsrstr);
	CHECKGLERROR();
	glEnable(GL_DEPTH_TEST);
	EndS();
	CHECKGLERROR();
#endif

#ifdef DEBUG
	g_log<<"draw "<<__FILE__<<" "<<__LINE__<<endl;
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
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	if(py->mouseout)
		return;

	bool moved = false;

	//const Uint8 *keys = SDL_GetKeyboardState(NULL);
	//SDL_BUTTON_LEFT;
	if((!py->keyintercepted && (py->keys[SDL_SCANCODE_UP] || py->keys[SDL_SCANCODE_W])) || (py->mouse.y <= SCROLL_BORDER))
	{
		//c->accelerate(CAMERA_SPEED / py->zoom * g_drawfrinterval);
		g_scroll.y -= CAMERA_SPEED / py->zoom * g_drawfrinterval;
		moved = true;
	}

	if((!py->keyintercepted && (py->keys[SDL_SCANCODE_DOWN] || py->keys[SDL_SCANCODE_S])) || (py->mouse.y >= g_height-SCROLL_BORDER))
	{
		//c->accelerate(-CAMERA_SPEED / py->zoom * g_drawfrinterval);
		g_scroll.y += CAMERA_SPEED / py->zoom * g_drawfrinterval;
		moved = true;
	}

	if((!py->keyintercepted && (py->keys[SDL_SCANCODE_LEFT] || py->keys[SDL_SCANCODE_A])) || (py->mouse.x <= SCROLL_BORDER))
	{
		//c->accelstrafe(-CAMERA_SPEED / py->zoom * g_drawfrinterval);
		g_scroll.x -= CAMERA_SPEED / py->zoom * g_drawfrinterval;
		moved = true;
	}

	if((!py->keyintercepted && (py->keys[SDL_SCANCODE_RIGHT] || py->keys[SDL_SCANCODE_D])) || (py->mouse.x >= g_width-SCROLL_BORDER))
	{
		//c->accelstrafe(CAMERA_SPEED / py->zoom * g_drawfrinterval);
		g_scroll.x += CAMERA_SPEED / py->zoom * g_drawfrinterval;
		moved = true;
	}

#if 0
	if(moved)
#endif
	{
#if 0
		if(c->zoompos().x < -g_hmap.m_widthx*TILE_SIZE)
		{
			float d = -g_hmap.m_widthx*TILE_SIZE - c->zoompos().x;
			c->move(Vec3f(d, 0, 0));
		}
		else if(c->zoompos().x > g_hmap.m_widthx*TILE_SIZE)
		{
			float d = c->zoompos().x - g_hmap.m_widthx*TILE_SIZE;
			c->move(Vec3f(-d, 0, 0));
		}

		if(c->zoompos().z < -g_hmap.m_widthz*TILE_SIZE)
		{
			float d = -g_hmap.m_widthz*TILE_SIZE - c->zoompos().z;
			c->move(Vec3f(0, 0, d));
		}
		else if(c->zoompos().z > g_hmap.m_widthz*TILE_SIZE)
		{
			float d = c->zoompos().z - g_hmap.m_widthz*TILE_SIZE;
			c->move(Vec3f(0, 0, -d));
		}
#else

		if(c->m_view.x < 0)
		{
			float d = 0 - c->m_view.x;
			c->move(Vec3f(d, 0, 0));
		}
		else if(c->m_view.x > g_hmap.m_widthx*TILE_SIZE)
		{
			float d = c->m_view.x - g_hmap.m_widthx*TILE_SIZE;
			c->move(Vec3f(-d, 0, 0));
		}

		if(c->m_view.z < 0)
		{
			float d = 0 - c->m_view.z;
			c->move(Vec3f(0, 0, d));
		}
		else if(c->m_view.z > g_hmap.m_widthz*TILE_SIZE)
		{
			float d = c->m_view.z - g_hmap.m_widthz*TILE_SIZE;
			c->move(Vec3f(0, 0, -d));
		}
#endif

#if 0
		UpdateMouse3D();

		if(g_mode == APPMODE_EDITOR && py->mousekeys[MOUSEKEY_LEFT])
		{
			EdApply();
		}

		if(!py->mousekeys[MOUSEKEY_LEFT])
		{
			g_vStart = g_vTile;
			g_vMouseStart = g_vMouse;
		}
#endif
	}

	Vec3f line[2];
	line[0] = c->zoompos();
	Camera oldcam = *c;
	c->frameupd();
	line[1] = c->zoompos();

	Vec3f ray = Normalize(line[1] - line[0]) * TILE_SIZE;
	//line[0] = line[0] - ray;
	line[1] = line[1] + ray;

	Vec3f clip;

#if 0
	if(GetMapIntersection(&g_hmap, line, &clip))
#else
	if(FastMapIntersect(&g_hmap, line, &clip))
	{
#endif
		*c = oldcam;
	}
	else
	{
		//CalcMapView();
	}

	c->friction2();
}

void LoadConfig()
{
	char cfgfull[MAX_PATH+1];
	FullPath(CONFIGFILE, cfgfull);

	ifstream f(cfgfull);

	if(!f)
		return;

	std::string line;
	char keystr[128];
	char actstr[128];

	Player* py = &g_player[g_curP];

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
		bool valueb = valuei != 0;

		if(stricmp(keystr, "fullscreen") == 0)					g_fullscreen = valueb;
		else if(stricmp(keystr, "client_width") == 0)			g_width = g_selectedRes.width = valuei;
		else if(stricmp(keystr, "client_height") == 0)			g_height = g_selectedRes.height = valuei;
		else if(stricmp(keystr, "screen_bpp") == 0)				g_bpp = valuei;
	}
}

int testfunc(ObjectScript::OS* os, int nparams, int closure_values, int need_ret_values, void * param)
{
	InfoMessage("os", "test");
	return 1;
}

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void SignalCallback(int signum)
{
    //printf("Caught signal %d\n",signum);
    // Cleanup and close up stuff here

    // Terminate program
    g_quit = true;
}

void Init()
{
#ifdef PLATFORM_LINUX
	signal(SIGINT, SignalCallback);
#endif

	SDL_Init(SDL_INIT_VIDEO);

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
	WriteProfiles(-1, 0);
	DestroyWindow(TITLE);
	// Clean up
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

	Player* py = &g_player[g_curP];
	GUI* gui = &py->gui;

	while (!g_quit)
	{
		StartTimer(TIMER_FRAME);
		StartTimer(TIMER_EVENT);

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InEv ev;
			ev.intercepted = false;

                        switch(e.type) {
                                case SDL_QUIT:
                                        g_quit = true;
                                        break;
                                case SDL_KEYDOWN:
                                        ev.type = INEV_KEYDOWN;
                                        ev.key = e.key.keysym.sym;
                                        ev.scancode = e.key.keysym.scancode;

                                        gui->inev(&ev);

                                        if(!ev.intercepted)
                                                py->keys[e.key.keysym.scancode] = true;

                                        py->keyintercepted = ev.intercepted;
                                        break;
                                case SDL_KEYUP:
                                        ev.type = INEV_KEYUP;
                                        ev.key = e.key.keysym.sym;
                                        ev.scancode = e.key.keysym.scancode;

                                        gui->inev(&ev);

                                        if(!ev.intercepted)
                                                py->keys[e.key.keysym.scancode] = false;

                                        py->keyintercepted = ev.intercepted;
                                        break;
                                case SDL_TEXTINPUT:
                                        //g_GUI.charin(e.text.text);	//UTF8
                                        ev.type = INEV_TEXTIN;
                                        ev.text = e.text.text;

                                        g_log<<"SDL_TEXTINPUT:";
                                        for(int i=0; i<strlen(e.text.text); i++)
                                        {
                                                g_log<<"[#"<<(unsigned int)(unsigned char)e.text.text[i]<<"]";
                                        }
                                        g_log<<endl;
                                        g_log.flush();

                                        gui->inev(&ev);
                                        break;
                                case SDL_TEXTEDITING:
                                        //g_GUI.charin(e.text.text);	//UTF8
                                        ev.type = INEV_TEXTED;
                                        ev.text = e.text.text;
                                        ev.cursor = e.edit.start;
                                        ev.sellen = e.edit.length;

                                        g_log<<"SDL_TEXTEDITING:";
                                        for(int i=0; i<strlen(e.text.text); i++)
                                        {
                                                g_log<<"[#"<<(unsigned int)(unsigned char)e.text.text[i]<<"]";
                                        }
                                        g_log<<endl;
                                        g_log.flush();

                                        g_log<<"texted: cursor:"<<ev.cursor<<" sellen:"<<ev.sellen<<endl;
                                        g_log.flush();

                                        gui->inev(&ev);
#if 0
                                        ev.intercepted = false;
                                        ev.type = INEV_TEXTIN;
                                        ev.text = e.text.text;

                                        gui->inev(&ev);
#endif
                                        break;
#if 0
                                case SDL_TEXTINPUT:
                                        /* Add new text onto the end of our text */
                                        strcat(text, event.text.text);
#if 0
                                        ev.type = INEV_CHARIN;
                                        ev.key = wParam;
                                        ev.scancode = 0;

                                        gui->inev(&ev);
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
                                        ev.type = INEV_MOUSEWHEEL;
                                        ev.amount = e.wheel.y;

                                        gui->inev(&ev);
                                        break;
                                case SDL_MOUSEBUTTONDOWN:
                                        switch (e.button.button) {
                                        case SDL_BUTTON_LEFT:
                                                py->mousekeys[MOUSE_LEFT] = true;
                                                py->moved = false;

                                                ev.type = INEV_MOUSEDOWN;
                                                ev.key = MOUSE_LEFT;
                                                ev.amount = 1;
                                                ev.x = py->mouse.x;
                                                ev.y = py->mouse.y;

                                                gui->inev(&ev);

                                                py->keyintercepted = ev.intercepted;
                                                break;
                                        case SDL_BUTTON_RIGHT:
                                                py->mousekeys[MOUSE_RIGHT] = true;

                                                ev.type = INEV_MOUSEDOWN;
                                                ev.key = MOUSE_RIGHT;
                                                ev.amount = 1;
                                                ev.x = py->mouse.x;
                                                ev.y = py->mouse.y;

                                                gui->inev(&ev);
                                                break;
                                        case SDL_BUTTON_MIDDLE:
                                                py->mousekeys[MOUSE_MIDDLE] = true;

                                                ev.type = INEV_MOUSEDOWN;
                                                ev.key = MOUSE_MIDDLE;
                                                ev.amount = 1;
                                                ev.x = py->mouse.x;
                                                ev.y = py->mouse.y;

                                                gui->inev(&ev);
                                                break;
                                        }
                                        break;
                                case SDL_MOUSEBUTTONUP:
                                        switch (e.button.button) {
                                                case SDL_BUTTON_LEFT:
                                                        py->mousekeys[MOUSE_LEFT] = false;

                                                        ev.type = INEV_MOUSEUP;
                                                        ev.key = MOUSE_LEFT;
                                                        ev.amount = 1;
                                                        ev.x = py->mouse.x;
                                                        ev.y = py->mouse.y;

                                                        gui->inev(&ev);
                                                        break;
                                                case SDL_BUTTON_RIGHT:
                                                        py->mousekeys[MOUSE_RIGHT] = false;

                                                        ev.type = INEV_MOUSEUP;
                                                        ev.key = MOUSE_RIGHT;
                                                        ev.amount = 1;
                                                        ev.x = py->mouse.x;
                                                        ev.y = py->mouse.y;

                                                        gui->inev(&ev);
                                                        break;
                                                case SDL_BUTTON_MIDDLE:
                                                        py->mousekeys[MOUSE_MIDDLE] = false;

                                                        ev.type = INEV_MOUSEUP;
                                                        ev.key = MOUSE_MIDDLE;
                                                        ev.amount = 1;
                                                        ev.x = py->mouse.x;
                                                        ev.y = py->mouse.y;

                                                        gui->inev(&ev);
                                                        break;
                                        }
                                        break;
                                case SDL_MOUSEMOTION:
                                        //py->mouse.x = e.motion.x;
                                        //py->mouse.y = e.motion.y;

                                        if(py->mouseout) {
                                                //TrackMouse();
                                                py->mouseout = false;
                                        }
                                        if(MousePosition()) {
                                                py->moved = true;

                                                ev.type = INEV_MOUSEMOVE;
                                                ev.x = py->mouse.x;
                                                ev.y = py->mouse.y;

                                                gui->inev(&ev);
                                        }
                                        break;
                        }
		}

		StopTimer(TIMER_EVENT);
#if 1
		if ((g_mode == APPMODE_LOADING || g_mode == APPMODE_RELOADING) || true /* DrawNextFrame(DRAW_FRAME_RATE) */ )
#endif
		{
			StartTimer(TIMER_DRAW);

#ifdef DEBUG
	g_log<<"main "<<__FILE__<<" "<<__LINE__<<endl;
    g_log.flush();
#endif
			CalcDrawRate();

			CHECKGLERROR();

#ifdef DEBUG
	g_log<<"main "<<__FILE__<<" "<<__LINE__<<endl;
    g_log.flush();
#endif

			Draw();
			CHECKGLERROR();

			if(g_mode == APPMODE_PLAY || g_mode == APPMODE_EDITOR)
			{
#ifdef DEBUG
	g_log<<"main "<<__FILE__<<" "<<__LINE__<<endl;
    g_log.flush();
#endif
				Scroll();
#ifdef DEBUG
	g_log<<"main "<<__FILE__<<" "<<__LINE__<<endl;
    g_log.flush();
#endif
				UpdateResTicker();
			}

			StopTimer(TIMER_DRAW);
		}

		if((g_mode == APPMODE_LOADING || g_mode == APPMODE_RELOADING) || UpdNextFrame(SIM_FRAME_RATE) )
		{
			StartTimer(TIMER_UPDATE);

#ifdef DEBUG
	g_log<<"main "<<__FILE__<<" "<<__LINE__<<endl;
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
	g_log << "Log start"    << endl; /* TODO, include date */
	g_log << "Init: "       << endl;
	g_log.flush();

	Init();

	g_log << "MakeWindow: " << endl;
	g_log.flush();

	MakeWindow(TITLE);

	g_log << "FillGUI: "    << endl;
	g_log.flush();

	FillGUI();

	g_log << "Queue: "      << endl;
	g_log.flush();

	SDL_ShowCursor(false);
	Queue();

	g_log << "EventLoop: "  << endl;
	g_log.flush();

	EventLoop();

	g_log << "Deinit: "     << endl;
	g_log.flush();

	Deinit();
	SDL_ShowCursor(true);

	return 0;
}
