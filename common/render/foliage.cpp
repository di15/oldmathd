

#include "../texture.h"
#include "foliage.h"
#include "water.h"
#include "shader.h"
#include "../math/camera.h"
#include "../math/frustum.h"
#include "vertexarray.h"
#include "heightmap.h"
#include "../utils.h"
#include "../window.h"
#include "../sim/player.h"

FoliageT g_foltype[FOLIAGE_TYPES];
Foliage g_foliage[FOLIAGES];
int g_folonsw[FOLIAGES];

Foliage::Foliage()
{
	on = false;
}

void DefF(int type, const char* modelrelative, Vec3f scale, Vec3f translate, Vec3i size)
{
	FoliageT* t = &g_foltype[type];
	//QueueTexture(&t->texindex, texrelative, true);
	//QueueModel(&t->model, modelrelative, scale, translate);
	t->size = size;
}

int NewFoliage()
{
	for(int i=0; i<FOLIAGES; i++)
	{
		if(!g_foliage[i].on)
			return i;
	}

	return -1;
}

#if 0
void PlaceFoliage()
{
	if(g_scT < 0)
		return;

	int i = NewFoliage();

	if(i < 0)
		return;

	Foliage* s = &g_foliage[i];
	s->on = true;
	s->pos = g_vMouse;
	s->type = g_scT;
	s->yaw = DEGTORAD((rand()%360));

	if(s->pos.y <= WATER_LEVEL)
		s->on = false;
}
#endif

void DrawFoliage(Vec3f zoompos, Vec3f vertical, Vec3f horizontal)
{
	//return;

#if 0
	Vec3f a, b, c, d;
	Vec3f vert, horiz;

	Shader* s = &g_shader[g_curS];

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(s->m_slot[SSLOT_TEXTURE0], 0);

	//glUniform1f(s->m_slot[SSLOT_MIND], MIN_DISTANCE);
	//glUniform1f(s->m_slot[SSLOT_MAXD], MAX_DISTANCE / py->zoom);

	FoliageT* t = &g_foltype[FOLIAGE_TREE1];
	Vec3i* size = &t->size;
	Model* m = &g_model[t->model];
	VertexArray* va = &m->m_va[0];
	m->usetex();
	Matrix im;

	Player* py = &g_player[g_curP];
	Camera* cam = &py->camera;

	Vec3f viewdir = Normalize(cam->m_view - zoompos);
	Vec3f horizontal2 = horizontal;

	for(int i=0; i<FOLIAGES; i++)
	{
		Foliage* f = &g_foliage[i];

		if(!f->on)
			continue;

		Vec3f vmin(f->pos.x - t->size.x/2, f->pos.y, f->pos.z - t->size.x/2);
		Vec3f vmax(f->pos.x + t->size.x/2, f->pos.y + t->size.y, f->pos.z + t->size.x/2);

		if(!g_frustum.boxin2(vmin.x, vmin.y, vmin.z, vmax.x, vmax.y, vmax.z))
			continue;

		glUniformMatrix4fv(s->m_slot[SSLOT_MODELMAT], 1, 0, g_folmodmat[i].m_matrix);

		Matrix modelview;
#ifdef SPECBUMPSHADOW
   		 modelview.set(g_camview.m_matrix);
#endif
    	modelview.postmult(g_folmodmat[i]);
		glUniformMatrix4fv(s->m_slot[SSLOT_MODELVIEW], 1, 0, modelview.m_matrix);

		Matrix mvp;
#if 0
		mvp.set(modelview.m_matrix);
		mvp.postmult(g_camproj);
#elif 0
		mvp.set(g_camproj.m_matrix);
		mvp.postmult(modelview);
#else
		mvp.set(g_camproj.m_matrix);
		mvp.postmult(g_camview);
		mvp.postmult(g_folmodmat[i]);
#endif
		glUniformMatrix4fv(s->m_slot[SSLOT_MVP], 1, 0, mvp.m_matrix);

		Matrix modelviewinv;
		Transpose(modelview, modelview);
		Inverse2(modelview, modelviewinv);
		//Transpose(modelviewinv, modelviewinv);
		glUniformMatrix4fv(s->m_slot[SSLOT_NORMALMAT], 1, 0, modelviewinv.m_matrix);

		glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, va->vertices);
		glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, 0, va->texcoords);
		if(s->m_slot[SSLOT_NORMAL] != -1)
			glVertexAttribPointer(s->m_slot[SSLOT_NORMAL], 3, GL_FLOAT, GL_FALSE, 0, va->normals);
		glDrawArrays(GL_TRIANGLES, 0, va->numverts);
	}
#endif
}

bool PlaceFoliage(int type, Vec3i ipos)
{
	int i = NewFoliage();

	if(i < 0)
		return false;

	Foliage* f = &g_foliage[i];
	f->on = true;
	f->type = type;
	f->pos = Vec3f(ipos.x, ipos.y, ipos.z);

	return true;
}

void ClearFol(int minx, int minz, int maxx, int maxz)
{
	for(int i=0; i<FOLIAGES; i++)
	{
		Foliage* f = &g_foliage[i];

		if(!f->on)
			continue;

		if(f->pos.x >= minx && f->pos.x <= maxx && f->pos.z >= minz && f->pos.z <= maxz)
		{
			f->on = false;
		}
	}
}

void FreeFoliage()
{
	for(int i=0; i<FOLIAGES; i++)
	{
		g_foliage[i].on = false;
	}
}
