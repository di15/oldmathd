//
// gviewport.cpp
//
//

#include "../../common/render/shader.h"
#include "../../common/gui/gui.h"
#include "../../common/math/3dmath.h"
#include "../../common/window.h"
#include "../../common/platform.h"
#include "../../common/gui/font.h"
#include "../../common/math/camera.h"
#include "../../common/math/matrix.h"
#include "../../common/render/heightmap.h"
#include "../../common/math/vec4f.h"
#include "../../common/math/brush.h"
#include "../../common/math/frustum.h"
#include "../../common/sim/sim.h"
#include "gviewport.h"
#include "../../common/math/hmapmath.h"
#include "../../common/render/water.h"
#include "../../common/save/savemap.h"
#include "../../common/gui/widgets/spez/bottompanel.h"
#include "../../common/sim/buildingtype.h"
#include "../../common/sim/road.h"
#include "../../common/sim/powl.h"
#include "../../common/sim/crpipe.h"
#include "../../common/sim/unittype.h"
#include "../../common/sim/player.h"
#include "../../common/debug.h"

ViewportT g_viewportT[VIEWPORT_TYPES];
Viewport g_viewport[4];
//Vec3f g_focus;

ViewportT::ViewportT(Vec3f offset, Vec3f up, const char* label, bool axial)
{
	m_offset = offset;
	m_up = up;
	strcpy(m_label, label);
	m_axial = axial;
}

Viewport::Viewport()
{
	m_drag = false;
	m_ldown = false;
	m_rdown = false;
}

Viewport::Viewport(int type)
{
	m_drag = false;
	m_ldown = false;
	m_rdown = false;
	m_mdown = false;
	m_type = type;
}

Vec3f Viewport::up()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	Vec3f upvec = c->m_up;
	ViewportT* t = &g_viewportT[m_type];

	if(t->m_axial)
		upvec = t->m_up;

	return upvec;
}

Vec3f Viewport::up2()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	Vec3f upvec = c->up2();
	ViewportT* t = &g_viewportT[m_type];

	if(t->m_axial)
		upvec = t->m_up;

	return upvec;
}

Vec3f Viewport::strafe()
{
	Vec3f upvec = up();
	ViewportT* t = &g_viewportT[m_type];
	Vec3f sidevec = Normalize(Cross(Vec3f(0,0,0)-t->m_offset, upvec));

	//if(!t->m_axial)
	//	sidevec = c->m_strafe;

	return sidevec;
}

Vec3f Viewport::focus()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	Vec3f viewvec = c->m_view;
	return viewvec;
}

Vec3f Viewport::viewdir()
{
	Vec3f focusvec = focus();
	Vec3f posvec = pos();
	//Vec3f viewvec = posvec + Normalize(focusvec-posvec);
	//return viewvec;
	return focusvec-posvec;
}

Vec3f Viewport::pos()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	Vec3f posvec = c->m_pos;

#if 0
	if(g_projtype == PROJ_PERSP && !t->m_axial)
	{
		Vec3f dir = Normalize( c->m_view - c->m_pos );
		posvec = c->m_view - dir * 1000.0f / py->zoom;
	}
#endif

	ViewportT* t = &g_viewportT[m_type];

	if(t->m_axial)
		posvec = c->m_view + t->m_offset;

	return posvec;
}

void DrawMMFrust()
{
	Player* py = &g_player[g_curP];
	Camera* c = &py->camera;

	Vec3f campos = c->zoompos();
	Vec3f camside = c->m_strafe;
	Vec3f camup2 = c->up2();
	Vec3f viewdir = Normalize(c->m_view - c->m_pos);

	int minx = 0;
	int maxx = g_width;
	int miny = 0;
	int maxy = g_height;

	//Vec3f campos = c->m_pos;
	//Vec3f camside = c->m_strafe;
	//Vec3f camup2 = c->up2();
	//Vec3f viewdir = Normalize( c->m_view - c->m_pos );

	Vec3f topLeftRay = ScreenPerspRay(minx, miny, g_width, g_height, campos, camside, camup2, viewdir, FIELD_OF_VIEW);
	Vec3f lineTopLeft[2];
	lineTopLeft[0] = campos;
	lineTopLeft[1] = campos + (topLeftRay * 1000000.0f);

	Vec3f topRightRay = ScreenPerspRay(maxx, miny, g_width, g_height, campos, camside, camup2, viewdir, FIELD_OF_VIEW);
	Vec3f lineTopRight[2];
	lineTopRight[0] = campos;
	lineTopRight[1] = campos + (topRightRay * 1000000.0f);

	Vec3f bottomLeftRay = ScreenPerspRay(minx, maxy, g_width, g_height, campos, camside, camup2, viewdir, FIELD_OF_VIEW);
	Vec3f lineBottomLeft[2];
	lineBottomLeft[0] = campos;
	lineBottomLeft[1] = campos + (bottomLeftRay * 1000000.0f);

	Vec3f bottomRightRay = ScreenPerspRay(maxx, maxy, g_width, g_height, campos, camside, camup2, viewdir, FIELD_OF_VIEW);
	Vec3f lineBottomRight[2];
	lineBottomRight[0] = campos;
	lineBottomRight[1] = campos + (bottomRightRay * 1000000.0f);

	Vec3f interTopLeft;
	Vec3f interTopRight;
	Vec3f interBottomLeft;
	Vec3f interBottomRight;

	if(!FastMapIntersect(&g_hmap, lineTopLeft, &interTopLeft))
		GetMapIntersection2(&g_hmap, lineTopLeft, &interTopLeft);
	if(!FastMapIntersect(&g_hmap, lineTopRight, &interTopRight))
		GetMapIntersection2(&g_hmap, lineTopRight, &interTopRight);
	if(!FastMapIntersect(&g_hmap, lineBottomLeft, &interBottomLeft))
		GetMapIntersection2(&g_hmap, lineBottomLeft, &interBottomLeft);
	if(!FastMapIntersect(&g_hmap, lineBottomRight, &interBottomRight))
		GetMapIntersection2(&g_hmap, lineBottomRight, &interBottomRight);

	float mmxscale = (float)MINIMAP_SIZE / (g_hmap.m_widthx*TILE_SIZE);
	float mmzscale = (float)MINIMAP_SIZE / (g_hmap.m_widthz*TILE_SIZE);

	interTopLeft.x = interTopLeft.x * mmxscale;
	interTopRight.x = interTopRight.x * mmxscale;
	interBottomLeft.x = interBottomLeft.x * mmxscale;
	interBottomRight.x = interBottomRight.x * mmxscale;

	interTopLeft.z = interTopLeft.z * mmzscale;
	interTopRight.z = interTopRight.z * mmzscale;
	interBottomLeft.z = interBottomLeft.z * mmzscale;
	interBottomRight.z = interBottomRight.z * mmzscale;

	float* color = g_player[g_localP].colorcode;

	glUniform4f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_COLOR], color[0], color[1], color[2], 0.3f);

	float vertices[] =
	{
		//posx, posy
		interTopLeft.x, interTopLeft.z,0,
		interTopRight.x, interTopRight.z,0,
		interBottomRight.x, interBottomRight.z,0,
		interBottomLeft.x, interBottomLeft.z,0,
		interTopLeft.x, interTopLeft.z,0
	};

	glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
	//glVertexAttribPointer(g_shader[SHADER_COLOR2D].m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, &vertices[0]);

	glDrawArrays(GL_LINE_STRIP, 0, 5);
}

void DrawMinimap(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float lightpos[3], float lightdir[3])
{
}

void DrawViewport(int which, int x, int y, int width, int height)
{
}

bool ViewportLDown(int which, int relx, int rely, int width, int height)
{
	return false;
}

bool ViewportRDown(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	v->m_rdown = true;
	v->m_lastmouse = Vec2i(relx, rely);
	v->m_curmouse = Vec2i(relx, rely);

	return true;
}

bool ViewportLUp(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	if(v->m_ldown)
	{
		//return true;
		v->m_ldown = false;

	}

	//g_sel1b = NULL;
	//g_dragV = -1;
	//g_dragS = -1;

	return false;
}

bool ViewportRUp(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	v->m_rdown = false;

	return false;
}

bool ViewportMousewheel(int which, int delta)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

#if 0
	//if(v->!t->m_axial)
	{
		py->zoom *= 1.0f + (float)delta / 10.0f;
		return true;
	}
#endif

	return false;
}

bool ViewportMousemove(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];

	v->m_lastmouse = Vec2i(relx, rely);
	v->m_curmouse = Vec2i(relx, rely);

	return false;
}
