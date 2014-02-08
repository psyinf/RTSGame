varying vec4 color;

uniform sampler2D base_tex;
void main() 
{
	color = gl_Color;

	gl_Position = ftransform();
}