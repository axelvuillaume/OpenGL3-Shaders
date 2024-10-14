#version 430
in vec4 color;
in vec2 texcoord;
uniform sampler2D tex; 
out vec4 fColor; // final fragment color
void main()
{
fColor = texture(tex, texcoord)*color;
}