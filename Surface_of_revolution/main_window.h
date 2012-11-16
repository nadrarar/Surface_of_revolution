#pragma once
#include "ilcontainer.h"
#include "window.h"
#include "planar_mesh.h"
#include "shader.h"

struct MainWindowStruct
{
	glm::ivec2 grid_size;
	float (* function)(float);
	bool open;
};


class MainWindow : public Window
{
public:
	MainWindow(GLsizei w, GLsizei h) : Window(w, h)
	{
		this->planar_mesh = NULL;
		this->wireframe = false;
		this->draw_normals = false;
		this->do_flat_shading = false;
		this->force_open = false;
		this->InitializeGLHandles();
		this->do_normal_mapping = false;
	}

	~MainWindow()
	{
		this->TakeDown();
		if (this->planar_mesh != NULL)
			delete this->planar_mesh;
		this->planar_mesh = NULL;
		this->fragment_shader.TakeDown();
		this->vertex_shader.TakeDown();
	}

	virtual void DisplayFunc();
	virtual void KeyboardFunc(unsigned char c, int x, int y);
	virtual void SpecialFunc(int c, int x, int y);
	virtual void TimerFunc(int value);
	virtual bool PostGLInitialize(void * data);
	virtual void TakeDown();

	void ApplyCustomization(float (* function)(float));
	void DrawSkybox();

	PlanarMesh * planar_mesh;
	ADSShader fragment_shader;
	ADSShader vertex_shader;
	ADSShader * shader;

	GLuint vertex_coordinate_handle;
	GLuint normal_value_handle;
	GLuint texture_coordinate_handle;
	GLuint vertex_array_handle;

	GLuint endcap_coordinate_handle;
	GLuint endcap_normal_handle;
	GLuint endcap_vertex_array_handle;

	ILContainer skybox_container;
	ILContainer normal_map_container;
	ILContainer second_normal_map_container;

	std::vector<glm::vec3> endcap_coordinates;
	std::vector<glm::vec3> endcap_normals;
	std::vector<glm::uint> endcap_triangle_indices;

	bool wireframe;
	bool draw_normals;
	bool do_flat_shading;
	bool force_open;
	bool do_normal_mapping;
	bool do_cartoon_mode;
	bool shiftDown;

	glm::vec3 camera_position;

private:

	float (*function)(float);

	void InitializeGLHandles()
	{
		this->vertex_array_handle = BAD_GL_VALUE;
		this->vertex_coordinate_handle = BAD_GL_VALUE;
		this->texture_coordinate_handle = BAD_GL_VALUE;
		this->normal_value_handle = BAD_GL_VALUE;
		this->endcap_coordinate_handle = BAD_GL_VALUE;
		this->endcap_normal_handle = BAD_GL_VALUE;
		this->endcap_vertex_array_handle = BAD_GL_VALUE;

	}

	void MakeEndCap();
};

