uniform sampler2D base;
uniform sampler2D ambient;
varying vec3 v_normal;
varying vec3 v_light_dir;
varying vec3 v_view_dir;


void main(void)
{
	vec4 ambient_term = texture2D(ambient, gl_TexCoord[0].ts);
	vec3 lightDir = normalize(v_light_dir);
	vec3 normal = normalize(v_normal );
	float intensity = max(dot(lightDir, normal),0.0);
	vec4 color = gl_FrontLightModelProduct.sceneColor;
	
	color += gl_FrontLightProduct[0].ambient;
    color += gl_FrontLightProduct[0].diffuse;// * intensity;
	vec4 base = vec4(1,1,1,1);//texture2D(base, gl_TexCoord[0].st);
	
	base *= ambient_term.a * ambient_term.a;
	color *= base;
	gl_FragData[0] = color;
}