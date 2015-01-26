#ifndef SHADER_H
#define SHADER_H

#include "../platform.h"

#define SSLOT_TEXTURE0			0
#define SSLOT_COLOR				1
#define SSLOT_OWNCOLOR			2
#define SSLOT_WIDTH				3
#define SSLOT_HEIGHT			4
#define SSLOT_SCALE				5
#define SSLOT_OWNERMAP			6
#define SSLOT_SCROLL			7
#define SSLOTS					8

class Shader
{
public:
	Shader() {}
	~Shader()
	{
		release();
	}

	GLint getuniform(const char* strVariable);
	GLint getattrib(const char* strVariable);

	void mapuniform(int slot, const char* variable);
	void mapattrib(int slot, const char* variable);

	void release();

	bool m_hastexcoords;
	GLint m_slot[SSLOTS];

	GLhandleARB m_vertshader;
	GLhandleARB m_fragshader;
	GLhandleARB m_program;
};

#define SHADER_WORLDOWNED		1
#define SHADER_ORTHO			2
#define SHADER_COLOR2D			3
#define SHADER_WORLDORTHO		4
#define SHADERS					5

extern Shader g_shader[SHADERS];
extern int g_curS;

void UseS(int shader);
void EndS();
void InitGLSL();
void ReleaseShaders();
std::string LoadTextFile(char* strFile);
void LoadShader(int shader, const char* strVertex, const char* strFragment, bool hastexcoords);

#endif


