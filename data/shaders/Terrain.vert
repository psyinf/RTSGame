varying vec3 v_normal;
varying vec3 v_light_dir;
varying vec3 v_view_dir;

void main(void)
{
	//gl_Position = ftransform();
	vec4 vertex = gl_Vertex;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	v_normal = gl_NormalMatrix * gl_Normal;
	
	vec3 v_view_dir = -vec3(gl_ModelViewMatrix * gl_Vertex);
	
	vec4 lpos = gl_LightSource[0].position;
	if (lpos.w == 0.0)
	{
		v_light_dir = lpos.xyz;
	}
    else
	{
		v_light_dir = lpos.xyz + v_view_dir;
	}
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vertex;
}