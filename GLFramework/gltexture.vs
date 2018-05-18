////////////////////////////////////////////////////////////////////////////////
// Filename: texture.vs
////////////////////////////////////////////////////////////////////////////////
#version 400

/////////////////////
// INPUT VARIABLES //
/////////////////////
in vec3 v_position;
in vec2 v_texCoord;
in vec4 v_color;

//////////////////////
// OUTPUT VARIABLES //
//////////////////////
out vec2 texCoord;
out vec4 color;  

///////////////////////
// UNIFORM VARIABLES //
///////////////////////
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
	gl_Position = worldMatrix * vec4(v_position, 1.0f);
	gl_Position = viewMatrix * gl_Position;
	gl_Position = projectionMatrix * gl_Position;

	texCoord = v_texCoord;
	color = v_color;
}