#version 420 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D u_Texture;

void main()
{
	color = texture(u_Texture, v_TexCoord);
}