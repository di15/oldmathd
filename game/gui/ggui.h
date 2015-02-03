#ifndef GGUI_H
#define GGUI_H

#include "../../common/math/vec3f.h"

class ViewLayer;
class Widget;

#if 0
extern bool g_canselect;
#endif

#define LEFT_PANEL_WIDTH	200

extern char g_lastsave[MAX_PATH+1];

void Resize_Fullscreen(Widget* thisw);
void Click_NewGame();
void Click_OpenEditor();
void FillGUI();
void Click_LoadMapButton();
void Click_SaveMapButton();
void Click_QSaveMapButton();
void Resize_MenuItem(Widget* thisw);

#endif	//GGUI_H
