uniform sampler2D base;
varying vec3 v_normal;
varying vec3 v_light_dir;
varying vec3 v_view_dir;
uniform float ConstructionProgress;
uniform bool ProductionPaused;
uniform float osg_SimulationTime;

void main(void)
{
	vec3 lightDir = normalize(v_light_dir);
	vec3 normal = normalize(v_normal);
	float intensity = max(dot(lightDir, normal),0.0);
	vec4 color = gl_FrontLightModelProduct.sceneColor;
	
	color += gl_FrontLightProduct[0].ambient;
    color += gl_FrontLightProduct[0].diffuse * intensity;
	vec4 base = texture2D(base, gl_TexCoord[0].st);
	color *= base;
	if (ConstructionProgress < 1.0)
	{
		color*= vec4(0,1,0,1);
	}
	else if (ProductionPaused)
	{
		color = sin(osg_SimulationTime) * vec4(1,0,0,1);
	}
	
	gl_FragData[0] = color;
}