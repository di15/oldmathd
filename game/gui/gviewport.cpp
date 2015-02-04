

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
#include "../../common/sim/simdef.h"
#include "gviewport.h"
#include "../../common/math/hmapmath.h"
#include "../../common/render/water.h"
#include "../../common/save/savemap.h"
#include "../../common/gui/widgets/spez/botpan.h"
#include "../../common/sim/bltype.h"
#include "../../common/sim/road.h"
#include "../../common/sim/powl.h"
#include "../../common/sim/crpipe.h"
#include "../../common/sim/utype.h"
#include "../../common/sim/player.h"
#include "../../common/debug.h"
#include "../../common/sim/conduit.h"

VpType g_vptype[VIEWPORT_TYPES];
Viewport g_viewport[4];
//Vec3f g_focus;

VpType::VpType(Vec3f offset, Vec3f up, const char* label, bool axial)
{
	m_offset = offset;
	m_up = up;
	strcpy(m_label, label);
	m_axial = axial;
}

#if 0
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
	Player* py = &g_player[g_localP];
	Camera* c = &g_cam;

	Vec3f upvec = c->m_up;
	VpType* t = &g_vptype[m_type];

	if(t->m_axial)
		upvec = t->m_up;

	return upvec;
}

Vec3f Viewport::up2()
{
	Player* py = &g_player[g_localP];
	Camera* c = &g_cam;

	Vec3f upvec = c->up2();
	VpType* t = &g_vptype[m_type];

	if(t->m_axial)
		upvec = t->m_up;

	return upvec;
}

Vec3f Viewport::strafe()
{
	Vec3f upvec = up();
	VpType* t = &g_vptype[m_type];
	Vec3f sidevec = Normalize(Cross(Vec3f(0,0,0)-t->m_offset, upvec));

	//if(!t->m_axial)
	//	sidevec = c->m_strafe;

	return sidevec;
}

Vec3f Viewport::focus()
{
	Player* py = &g_player[g_localP];
	Camera* c = &g_cam;

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
	Player* py = &g_player[g_localP];
	Camera* c = &g_cam;

	Vec3f posvec = c->m_pos;

#if 0
	if(g_projtype == PROJ_PERSP && !t->m_axial)
	{
		Vec3f dir = Normalize( c->m_view - c->m_pos );
		posvec = c->m_view - dir * 1000.0f / g_zoom;
	}
#endif

	VpType* t = &g_vptype[m_type];

	if(t->m_axial)
		posvec = c->m_view + t->m_offset;

	return posvec;
}
#endif

void DrawMinimap()
{
	//g_frustum.construct(projection.m_matrix, viewmat.m_matrix);

	CHECKGLERROR();

}

void DrawPreview()
{
	Shader* s = &g_shader[g_curS];
	Player* py = &g_player[g_localP];
	float* color = py->color;
	glUniform4f(s->m_slot[SSLOT_OWNCOLOR], color[0], color[1], color[2], color[3]);
	//glUniform4f(s->m_slot[SSLOT_OWNCOLOR], 1, 0, 0, 0);

	Sprite* sp = NULL;

	if(g_bptype >= 0 && g_bptype < BL_TYPES)
	{
		BlType* t = &g_bltype[g_bptype];
		sp = &g_sprite[t->sprite];
	}
	else if(g_bptype == BL_ROAD)
	{
		CdType* ct = &g_cdtype[CONDUIT_ROAD];
		sp = &g_sprite[ct->sprite[CONNECTION_EASTWEST][1]];
	}
	else if(g_bptype == BL_POWL)
	{
		CdType* ct = &g_cdtype[CONDUIT_POWL];
		sp = &g_sprite[ct->sprite[CONNECTION_EASTWEST][1]];
	}
	else if(g_bptype == BL_CRPIPE)
	{
		CdType* ct = &g_cdtype[CONDUIT_CRPIPE];
		sp = &g_sprite[ct->sprite[CONNECTION_EASTWEST][1]];
	}

	if(!sp)
		return;

}


void DrawViewport(int which, int x, int y, int width, int height)
{
	//return;

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	Viewport* v = &g_viewport[which];
	VpType* t = &g_vptype[v->m_type];
	Player* py = &g_player[g_localP];

	if(which == VIEWPORT_ENTVIEW)
	{
		CHECKGLERROR();
	}

	if(which == VIEWPORT_MINIMAP)
	{
		StartTimer(TIMER_DRAWMINIMAP);

		CHECKGLERROR();

		StopTimer(TIMER_DRAWMINIMAP);
	}

	glDisable(GL_DEPTH_TEST);
	glFlush();
	CHECKGLERROR();
}

#if 0
bool ViewportLDown(int which, int relx, int rely, int width, int height)
{
	//return false;

	Viewport* v = &g_viewport[which];
	v->m_ldown = true;
	v->m_lastmouse = Vec2i(relx, rely);
	v->m_curmouse = Vec2i(relx, rely);


	VpType* t = &g_vptype[v->m_type];

	//g_log<<"vp["<<which<<"] l down"<<std::endl;
	//g_log.flush();

	float aspect = fabsf((float)width / (float)height);
	Matrix projection;

	bool persp = false;

#if 0
	if(v->!t->m_axial && g_projtype == PROJ_PERSP)
	{
		projection = PerspProj(FIELD_OF_VIEW, aspect, MIN_DISTANCE, MAX_DISTANCE);
		persp = true;
	}
	else
	{
		projection = OrthoProj(-PROJ_RIGHT*aspect/g_zoom, PROJ_RIGHT*aspect/g_zoom, PROJ_RIGHT/g_zoom, -PROJ_RIGHT/g_zoom, MIN_DISTANCE, MAX_DISTANCE);
	}
#endif

	//Vec3f viewvec = g_focus; //c->m_view;
	//Vec3f viewvec = c->m_view;
	Vec3f focusvec = v->focus();
	//Vec3f posvec = g_focus + t->m_offset;
	//Vec3f posvec = c->m_pos;
	Vec3f posvec = v->pos();

	//if(v->t->m_axial)
	{
		//	posvec = c->m_view + t->m_offset;
		//viewvec = posvec + Normalize(c->m_view-posvec);
	}

	//viewvec = posvec + Normalize(viewvec-posvec);
	//Vec3f posvec2 = c->lookpos() + t->m_offset;
	//Vec3f upvec = t->m_up;
	//Vec3f upvec = c->m_up;
	Vec3f upvec = v->up();

	//if(v->t->m_axial)
	//	upvec = t->m_up;

	Matrix viewmat = LookAt(posvec.x, posvec.y, posvec.z, focusvec.x, focusvec.y, focusvec.z, upvec.x, upvec.y, upvec.z);
	Matrix mvpmat;
	mvpmat.set(projection.m_matrix);
	mvpmat.postmult(viewmat);

	return true;
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
	VpType* t = &g_vptype[v->m_type];

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
	VpType* t = &g_vptype[v->m_type];

	v->m_rdown = false;

	return false;
}

bool NULL(int which, int delta)
{
	Viewport* v = &g_viewport[which];
	VpType* t = &g_vptype[v->m_type];

#if 0
	//if(v->!t->m_axial)
	{
		g_zoom *= 1.0f + (float)delta / 10.0f;
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
#endif