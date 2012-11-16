#pragma once
#include <iostream>
#include <sstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader();
	void TakeDown();
	void Use();
	virtual bool Initialize(char * vertex_shader_file, char * fragment_shader_file);

	GLuint modelview_matrix_handle;
	GLuint projection_matrix_handle;
	GLuint normal_matrix_handle;
	GLuint mvp_handle;
	GLboolean do_flat_shading_handle;
	GLboolean do_cartoon_mode_handle;
	
	GLuint camera_position_handle;

	GLuint vertex_shader_id;
	GLuint fragment_shader_id;
	GLuint program_id;
	bool LoadShader(const char * file_name, GLuint shader_id);
	std::stringstream GetShaderLog(GLuint shader_id);
};

class ADSShader : public Shader
{
public:
	ADSShader();
	virtual bool Initialize(char * vertex_shader_file, char * fragment_shader_file);

	void SetLight(glm::vec4 position, glm::vec3 la, glm::vec3 ld, glm::vec3 ls);
	void SetMaterial(glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, float shininess);

	GLuint position_handle;
	GLuint la_handle;
	GLuint ld_handle;
	GLuint ls_handle;
	GLuint ka_handle;
	GLuint kd_handle;
	GLuint ks_handle;
	GLuint shininess_handle;
	GLuint normal_map_handle;
	GLuint second_normal_map_handle;
	GLuint do_normal_mapping_handle;
};

