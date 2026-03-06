#version 420 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D u_Texture;

void main()
{
	color = texture(u_Texture, v_TexCoord * 10.0) * vec4(1.0, 0.8, 0.8, 1.0);}