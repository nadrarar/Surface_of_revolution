#version 400
in vec3 Position;
in vec3 Normal;
in vec3 NormalDx;
in vec3 NormalDy;

flat in vec3 color;
flat in vec3 back_color;

struct LightInfo
{
	vec4 position;
	vec3 la;
	vec3 ld;
	vec3 ls;
};

struct MaterialInfo
{
	vec3 ka;	
	vec3 kd;
	vec3 ks;
	float shininess;
};

uniform LightInfo light;
uniform MaterialInfo material;
uniform bool do_flat_shading;
uniform bool do_cartoon_mode;
uniform vec3 camera_position;
layout (location = 0) out vec4 FragColor;
in vec2 tcoord;

vec4 ads( )
{
    vec3 n = normalize( Normal );

    if (!gl_FrontFacing)
	n = -n;

    vec3 s = normalize( vec3(light.position) - Position );
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect(-s, n);
    float s_dot_n = max(dot(s, n), 0.0);
	vec3 ambient =light.la * material.ka;
    vec3 defuse =light.ld * material.kd * s_dot_n;
    vec3 specular =(s_dot_n > 0 ? light.ls * material.ks * pow(max(dot(r,v), 0.0), material.shininess) : vec3(0.0));
	vec4 ads=vec4(ambient + defuse + specular,1.0f);
    //cartoon mode
	if(do_cartoon_mode){
		bool drawLine=false;
		vec3 c = normalize( camera_position-Position);
		float c_dot_n = max(dot(c, n), 0.0);
    	float c_n_theta=acos(c_dot_n);
		float resolution=.15f;

		if(length(defuse)<0.05f)
			defuse=vec3(0.0f);
		else if (length(defuse)<0.07f)
			drawLine=true;
		else
			defuse=light.ld * material.kd;

		if(length(specular)<0.07f)
			specular=vec3(0.0f);
		else if (length(specular)<0.08f)
			drawLine=true;
		else
			specular=vec3(light.ls * material.ks);

		ads=vec4(ambient+defuse+specular,1.0f);

		ads=vec4(vec3(mix(mix(ads.x,ads.y,.5),ads.z,.66)),1.0f);
		if(drawLine){
			//distance from center of line, from 0-1
			float lerp_val=(abs(abs(c_n_theta)-3.1415f/2))/resolution;
			//squared in order to make a cleaner line
			ads=mix(ads,vec4(0.0f,0.0f,0.0f,0.0f),lerp_val);
			

			//ads=vec4(0.0f,0.0f,0.0f,1.0f);
		}		
		//else ads=vec4(1.0f,1.0f,1.0f,0.0f);
		
    }
	return ads;
}

void main()
{
	if (do_flat_shading)
		FragColor = vec4(gl_FrontFacing ? color : back_color, 1.0);
	else
		FragColor = ads();
}
