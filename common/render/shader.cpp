#include "../utils.h"
#include "shader.h"
#include "../platform.h"
#include "../window.h"
#include "../sim/player.h"
#include "../debug.h"

Shader g_shader[SHADERS];
int g_curS = 0;

GLint Shader::getuniform(const char* strVariable)
{
	if(!m_program)
		return -1;

	return glGetUniformLocation(m_program, strVariable);
}

GLint Shader::getattrib(const char* strVariable)
{
	g_log<<"shader "<<(int)(this-g_shader)<<" attrib "<<strVariable<<" = ";

	if(!m_program)
		return -1;

	g_log<<glGetAttribLocation(m_program, strVariable)<<endl;

	return glGetAttribLocation(m_program, strVariable);
}

void Shader::mapuniform(int slot, const char* variable)
{
	m_slot[slot] = getuniform(variable);
	//g_log<<"\tmap uniform "<<variable<<" = "<<(int)m_slot[slot]<<endl;
}

void Shader::mapattrib(int slot, const char* variable)
{
	m_slot[slot] = getattrib(variable);
	//g_log<<"\tmap attrib "<<variable<<" = "<<(int)m_slot[slot]<<endl;
}

void GetGLVersion(int* major, int* minor)
{
	// for all versions
	char* ver = (char*)glGetString(GL_VERSION); // ver = "3.2.0"

	char vermaj[4];

	for(int i=0; i<4; i++)
	{
		if(ver[i] != '.')
			vermaj[i] = ver[i];
		else
			vermaj[i] = '\0';
	}

	//*major = ver[0] - '0';
	*major = StrToInt(vermaj);
	if( *major >= 3)
	{
		// for GL 3.x
		glGetIntegerv(GL_MAJOR_VERSION, major); // major = 3
		glGetIntegerv(GL_MINOR_VERSION, minor); // minor = 2
	}
	else
	{
		*minor = ver[2] - '0';
	}

	// GLSL
	ver = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION); // ver = "1.50 NVIDIA via Cg compiler"
}

void InitGLSL()
{
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if( glewError != GLEW_OK )
	{
		ErrorMessage("Error initializing GLEW!", (const char*)glewGetErrorString( glewError ));
		return;
	}

	g_log<<"Renderer: "<<(char*)glGetString(GL_RENDERER)<<endl;
	g_log<<"GL_VERSION = "<<(char*)glGetString(GL_VERSION)<<endl;

#if 0
	if( !GLEW_VERSION_1_4 )
	{
		ErrorMessage("Error", "OpenGL 1.4 not supported!\n" );
		g_quit = true;
		return;
	}
#endif

#if 1
	char* szGLExtensions = (char*)glGetString(GL_EXTENSIONS);

	g_log<<szGLExtensions<<endl;
	g_log.flush();

	if(!strstr(szGLExtensions, "GL_ARB_debug_output"))
	{
		//ErrorMessage("Error", "GL_ARB_debug_output extension not supported!");
		//g_quit = true;
		//return;
		g_log<<"GL_ARB_debug_output extension not supported"<<std::endl;
	}
	else
	{
		g_log<<"Reging debug handler"<<std::endl;
		g_log.flush();
		glDebugMessageCallbackARB(&GLMessageHandler, 0);
		CHECKGLERROR();
	}

	if(!strstr(szGLExtensions, "GL_ARB_shader_objects"))
	{
		ErrorMessage("Error", "GL_ARB_shader_objects extension not supported!");
		//g_quit = true;
		//return;
	}

	if(!strstr(szGLExtensions, "GL_ARB_shading_language_100"))
	{
		ErrorMessage("Error", "GL_ARB_shading_language_100 extension not supported!");
		//g_quit = true;
		//return;
	}
#endif

#if 1
	//might still be 1.4 if card says 2.0 etc.
	int major, minor;
	GetGLVersion(&major, &minor);

	if(major < 1 || ( major == 1 && minor < 4 ))
	{
		char msg[128];
		sprintf(msg, "OpenGL 1.4 is not supported! GL version = %d.%d.", major, minor);
		ErrorMessage("Error", msg);
		//g_quit = true;
	}
#endif

	CHECKGLERROR();
	LoadShader(SHADER_ORTHO, "shaders/ortho.vert", "shaders/ortho.frag", true);
	CHECKGLERROR();
	LoadShader(SHADER_COLOR2D, "shaders/color2d.vert", "shaders/color2d.frag", false);
	CHECKGLERROR();
	LoadShader(SHADER_WORLDOWNED, "shaders/worldowned.vert", "shaders/worldowned.frag", true);
	CHECKGLERROR();
	LoadShader(SHADER_WORLDORTHO, "shaders/worldortho.vert", "shaders/worldortho.frag", true);
	CHECKGLERROR();
}

std::string LoadTextFile(const char* strFile)
{
	ifstream fin(strFile);

	if(!fin)
	{
		g_log<<"Failed to load file "<<strFile<<endl;
		return "";
	}

	std::string strLine = "";
	std::string strText = "";

	while(getline(fin, strLine))
		strText = strText + "\n" + strLine;

	fin.close();

	return strText;
}

void LoadShader(int shader, const char* strVertex, const char* strFragment, bool hastexcoords)
{
	Shader* s = &g_shader[shader];
	std::string strVShader, strFShader;

	if(s->m_vertshader || s->m_fragshader || s->m_program)
		s->release();

    s->m_hastexcoords = hastexcoords;
	s->m_vertshader = glCreateShader(GL_VERTEX_SHADER);
	s->m_fragshader = glCreateShader(GL_FRAGMENT_SHADER);

	strVShader = LoadTextFile(strVertex);
	strFShader = LoadTextFile(strFragment);

	const char* szVShader = strVShader.c_str();
	const char* szFShader = strFShader.c_str();

	glShaderSource(s->m_vertshader, 1, &szVShader, NULL);
	glShaderSource(s->m_fragshader, 1, &szFShader, NULL);

	glCompileShader(s->m_vertshader);
	GLint logLength;
	glGetShaderiv(s->m_vertshader, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		GLchar* log = (GLchar*)malloc(logLength);

		if(!log)
		{
			OutOfMem(__FILE__, __LINE__);
			return;
		}

		glGetShaderInfoLog(s->m_vertshader, logLength, &logLength, log);
		g_log<<"Shader "<<strVertex<<" compile log: "<<endl<<log<<endl;
		free(log);
	}

	glCompileShader(s->m_fragshader);
	glGetShaderiv(s->m_fragshader, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		GLchar* log = (GLchar*)malloc(logLength);

		if(!log)
		{
			OutOfMem(__FILE__, __LINE__);
			return;
		}

		glGetShaderInfoLog(s->m_fragshader, logLength, &logLength, log);
		g_log<<"Shader "<<strFragment<<" compile log: "<<endl<<log<<endl;
		free(log);
	}

	s->m_program = glCreateProgram();
	glAttachShader(s->m_program, s->m_vertshader);
	glAttachShader(s->m_program, s->m_fragshader);
	glLinkProgram(s->m_program);

	//glUseProgramObject(s->m_program);

	//g_log<<"shader "<<strVertex<<","<<strFragment<<endl;

	g_log<<"Program "<<strVertex<<" / "<<strFragment<<" :";

	glGetProgramiv(s->m_program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar* log = (GLchar*)malloc(logLength);
		glGetProgramInfoLog(s->m_program, logLength, &logLength, log);
		g_log<<"Program link log:"<<endl<<log<<endl;
		free(log);
	}

	GLint status;
	glGetProgramiv(s->m_program, GL_LINK_STATUS, &status);
	if (status == 0)
	{
		g_log<<"link status 0"<<endl;
	}
	else
	{
		g_log<<"link status ok"<<endl;
	}

	g_log<<endl<<endl;

	s->mapuniform(SSLOT_TEXTURE0, "texture0");
	s->mapuniform(SSLOT_COLOR, "color");
	s->mapuniform(SSLOT_OWNCOLOR, "owncolor");
	s->mapuniform(SSLOT_WIDTH, "width");
	s->mapuniform(SSLOT_HEIGHT, "height");
	s->mapuniform(SSLOT_SCALE, "scale");
	s->mapuniform(SSLOT_OWNERMAP, "ownermap");
	s->mapuniform(SSLOT_SCROLL, "scroll");
}

void UseS(int shader)
{
	CHECKGLERROR();
	g_curS = shader;

	Shader* s = &g_shader[g_curS];

	//glUseProgramObject(g_shader[shader].m_program);
	glUseProgram(s->m_program);
	CHECKGLERROR();

	Player* py = &g_player[g_curP];

	glEnableClientState(GL_VERTEX_ARRAY);
	if(s->m_hastexcoords)	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void EndS()
{
	CHECKGLERROR();

	if(g_curS < 0)
		return;

	Shader* s = &g_shader[g_curS];

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glUseProgram(0);

	g_curS = -1;
}

void Shader::release()
{
	if(m_vertshader)
	{
		glDetachShader(m_program, m_vertshader);
		glDeleteShader(m_vertshader);
		m_vertshader = 0;
	}

	if(m_fragshader)
	{
		glDetachShader(m_program, m_fragshader);
		glDeleteShader(m_fragshader);
		m_fragshader = 0;
	}

	if(m_program)
	{
		glDeleteProgram(m_program);
		m_program = 0;
	}
}

void ReleaseShaders()
{
	for(int i=0; i<SHADERS; i++)
		g_shader[i].release();
}

