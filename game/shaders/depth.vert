#version 130


in vec4 position;

//uniform mat4 projection;
//uniform mat4 model;
//uniform mat4 view;
uniform mat4 mvp;

in vec2 texCoordIn0;
out vec2 texCoordOut0;

void main(void)
{
	//gl_Position = projection * (view * (model * position));	//why doesn't this work?
	//gl_Position = projection * view * model * position;
	gl_Position = mvp * position;
	gl_Position.w = 1;	// sun light is orthographic directional light
	texCoordOut0 = texCoordIn0;
}
