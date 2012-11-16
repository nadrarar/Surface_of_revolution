#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 TexCoord;

out vec3 Position;
out vec3 Normal;
out vec2 tcoord;

flat out vec3 color;
flat out vec3 back_color;

uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;
uniform mat4 projection_matrix;
uniform mat4 mvp;
uniform bool do_flat_shading;
uniform sampler2D normal_map;
uniform sampler2D second_normal_map;
uniform bool do_normal_mapping;

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

vec3 ADS(vec4 eye_coords, vec3 normal)
{
	vec3 s = normalize(vec3(light.position)); // - eye_coords));
	vec3 v = normalize(vec3(-eye_coords));
	vec3 r = reflect(-s, normal);
	
	float s_dot_n = max(dot(s, normal), 0.0);

	vec3 ambient = light.la * material.ka;
	vec3 diffuse = light.ld * material.kd * s_dot_n;
	vec3 spec = vec3(0.0);
	if (s_dot_n > 0.0)
		spec = light.ls * material.ks * pow(max(dot(r, v), 0.0), material.shininess);
	return ambient + diffuse + spec;
}

void main()
{
	tcoord = TexCoord;

	if (do_flat_shading == false)
	{
		if (do_normal_mapping)
		{
			//Normal = normalize(normal_matrix * VertexNormal + texture(normal_map, tcoord * vec2(2.5, 1.5)).rgb - vec3(0.5, 0.5, 0.5));
			Normal = normalize(normal_matrix * VertexNormal + texture(second_normal_map, tcoord * vec2(4.0, 2.0)).rgb - vec3(0.5, 0.5, 0.5));
		}
		else
		{
			Normal = normalize(normal_matrix * VertexNormal);
		}
		Position = vec3(modelview_matrix * vec4(VertexPosition,1.0));
		gl_Position = mvp * vec4(VertexPosition,1.0);
	}
	else
	{
		vec3 tnorm = normalize(normal_matrix * VertexNormal);
		vec4 eye_coords = modelview_matrix * vec4(VertexPosition, 1.0);
		color = ADS(eye_coords, tnorm);
		back_color = ADS(eye_coords, -tnorm);
		gl_Position = mvp * vec4(VertexPosition, 1.0);
	}
}