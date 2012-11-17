#include <iostream>
#include "main_window.h"
#include "planar_mesh.h"

using namespace std;

extern double elapsed_time;
extern double total_time;

void MainWindow::DisplayFunc()
{
	float theta = ((float) this->x) / ((float) this->width) * 360.0f - 180.0f;
	float rho = ((float) this->y) / ((float) this->height) * 360.0f - 180.0f;

	this->ClearWindow();
	this->DrawSkybox();	
	this->DisplayWindowName("Main Window");


	glEnable(GL_DEPTH_TEST);

	if (this->endcap_coordinates.size() > 0 && !this->force_open)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	glm::vec3 raxis=glm::vec3(cos(theta*3.1415f/180),0.0f,sin(theta*3.1415f/180));
	raxis=glm::normalize(raxis);
	glm::mat4 projection_matrix = glm::perspective(20.0f, ((float) this->width) / ((float) this->height), 1.0f, 10.0f);
	//changed modelview matrix and normals so that roation is relative to the camera position
	camera_position=glm::vec3(0.0f, 0.0f, 5.5f);
	glm::vec3 scaleVec=glm::vec3(1.0f);
	//glm::mat4 modelview_matrix = glm::rotate(glm::rotate(glm::lookAt(glm::vec3(0.0f, 0.0f, 5.5f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)), theta, glm::vec3(0.0f, 1.0f, 0.0f)), rho, glm::vec3(0,0,1));
	glm::mat4 modelview_matrix = glm::scale(glm::rotate(glm::rotate(glm::lookAt(camera_position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),theta,glm::vec3(0,1,0)), rho, glm::vec3(raxis.x, raxis.y, raxis.z)),scaleVec);
	glm::mat3 normal_matrix = glm::inverse(glm::transpose(glm::mat3(modelview_matrix)));
	glm::mat4 mvp_matrix = projection_matrix * modelview_matrix;

	glPolygonMode(GL_FRONT_AND_BACK, this->wireframe ? GL_LINE : GL_FILL);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	shader->Use();
	glUniform1i(shader->normal_map_handle, 0);
	this->normal_map_container.Bind(0);
	glUniform1i(shader->second_normal_map_handle, 1);
	this->second_normal_map_container.Bind(1);
	glUniform1i(shader->do_normal_mapping_handle, (GLint) this->do_normal_mapping);
	glUniform1i(shader->do_flat_shading_handle, (GLint) this->do_flat_shading);
	glUniform1i(shader->do_cartoon_mode_handle, (GLint) this->do_cartoon_mode);
	glUniform3f(shader->camera_position_handle, this->camera_position.x,this->camera_position.y,this->camera_position.z);
	glUniformMatrix4fv(shader->projection_matrix_handle, 1, GL_FALSE, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(shader->modelview_matrix_handle, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
	glUniformMatrix4fv(shader->mvp_handle, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
	glUniformMatrix3fv(shader->normal_matrix_handle, 1, GL_FALSE, glm::value_ptr(normal_matrix));

	shader->SetLight(glm::vec4(100.0f, 100.0f, 100.0f,1.0f), glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f));
	shader->SetMaterial(glm::vec3(0.5f,0.2f,0.2f), glm::vec3(.3f, .1f, .1f), glm::vec3(0.4f,0.1f,0.1f), 60.0f);

	glBindVertexArray(this->vertex_array_handle);
	glDrawElements(GL_TRIANGLES , this->planar_mesh->GetNumberOfElements(), GL_UNSIGNED_INT , this->planar_mesh->GetTriangleIndexArray());

	glUniform1i(shader->do_normal_mapping_handle, (GLint) 0);

	/* This endcap code is an afterthought and not well considered.
	*/
	if (this->endcap_coordinates.size() > 0 && !this->force_open)
	{
		glBindVertexArray(this->endcap_vertex_array_handle);
		float radius;
		glm::mat4 endcap_modelview_matrix(1.0);

		if ((radius = this->function(0)) > 0)
		{
			endcap_modelview_matrix = glm::rotate(glm::translate(modelview_matrix, glm::vec3(0.0f, -0.5f, 0.0f)), 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			endcap_modelview_matrix = glm::scale(endcap_modelview_matrix, glm::vec3(radius, 1.0f, radius));
			normal_matrix = glm::inverse(glm::transpose(glm::mat3(endcap_modelview_matrix)));
			mvp_matrix = projection_matrix * endcap_modelview_matrix;
			glUniformMatrix4fv(shader->mvp_handle, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
			glUniformMatrix4fv(shader->modelview_matrix_handle, 1, GL_FALSE, glm::value_ptr(endcap_modelview_matrix));
			glUniformMatrix3fv(shader->normal_matrix_handle, 1, GL_FALSE, glm::value_ptr(normal_matrix));
			glDrawElements(GL_TRIANGLE_FAN, this->endcap_coordinates.size(), GL_UNSIGNED_INT, &this->endcap_triangle_indices[0]);
		}

		if ((radius = this->function(1)) > 0)
		{
			endcap_modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.5f, 0.0f));
			endcap_modelview_matrix = glm::scale(endcap_modelview_matrix, glm::vec3(radius, 1.0f, radius));
			normal_matrix = glm::inverse(glm::transpose(glm::mat3(endcap_modelview_matrix)));
			mvp_matrix = projection_matrix * endcap_modelview_matrix;
			glUniformMatrix4fv(shader->mvp_handle, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
			glUniformMatrix4fv(shader->modelview_matrix_handle, 1, GL_FALSE, glm::value_ptr(endcap_modelview_matrix));
			glUniformMatrix3fv(shader->normal_matrix_handle, 1, GL_FALSE, glm::value_ptr(normal_matrix));
			glDrawElements(GL_TRIANGLE_FAN, this->endcap_coordinates.size(), GL_UNSIGNED_INT, &this->endcap_triangle_indices[0]);
		}

	}

	glBindVertexArray(0);
	glUseProgram(0);

	if (this->draw_normals)
	{
		glDisable(GL_LIGHTING);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(20, ((double) this->width) / ((double) this->height), 1, 10);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 0.0, 5.5, 0, 0, 0, 0, 1, 0);
		glRotated(theta, 0, 1, 0);
		//changed modelview matrix and normals so that roation is relative to the camera position
		glRotated(rho, raxis.x,raxis.y,raxis.z);
		glPushMatrix();
		glScalef(0.5f, 0.5f, 0.5f);
		this->Axes();
		glPopMatrix();
		glScalef(scaleVec.x,scaleVec.y,scaleVec.z);
		this->planar_mesh->Draw(PlanarMesh::OutArray, true,this->do_cartoon_mode);
	}

//	glDisable(GL_CULL_FACE);

	glutSwapBuffers();
}

void MainWindow::KeyboardFunc(unsigned char c, int x, int y)
{
	this->shiftDown=false;
	switch (c)
	{
	case 'o':
		this->force_open = !this->force_open;
		break;

	case 's':
		this->shader = (this->shader == &this->fragment_shader) ? &this->vertex_shader : &this->fragment_shader;
		break;

	case 'f':
		this->do_flat_shading = !this->do_flat_shading;
		break;

	case 'c':
		this->do_cartoon_mode = !this->do_cartoon_mode;
		break;

	case 'n':
		this->draw_normals = !this->draw_normals;
		break;

	case 'N':
		this->do_normal_mapping = !this->do_normal_mapping;
		break;

	case 'w':
		this->wireframe = !this->wireframe;
		break;

	case 'x':
	case 27:
		glutLeaveMainLoop();
		break;
	case 15:
		this->shiftDown=true;
		break;
	}
}

void MainWindow::SpecialFunc(int c, int x, int y)
{
}

void MainWindow::TimerFunc(int value)
{
	extern void BaseTimerFunc(int value);
	glutTimerFunc(this->period, BaseTimerFunc, value);
	glutPostRedisplay();
}

void MainWindow::TakeDown()
{
	if (this->vertex_coordinate_handle != BAD_GL_VALUE)
		glDeleteBuffers(1, &this->vertex_coordinate_handle);
	if (this->texture_coordinate_handle != BAD_GL_VALUE)
		glDeleteBuffers(1, &this->texture_coordinate_handle);
	if (this->normal_value_handle != BAD_GL_VALUE)
		glDeleteBuffers(1, &this->normal_value_handle);
	if (this->normal_dx_handle != BAD_GL_VALUE)
		glDeleteBuffers(1, &this->normal_dx_handle);
	if (this->normal_dy_handle != BAD_GL_VALUE)
		glDeleteBuffers(1, &this->normal_dy_handle);

	if (this->vertex_array_handle != BAD_GL_VALUE)
		glDeleteVertexArrays(1, &this->vertex_array_handle);

	this->InitializeGLHandles();
}

bool MainWindow::PostGLInitialize(void * data)
{
	// TO PERMIT THIS TO BE CALLED MORE THAN ONCE, THE GL STATE MUST BE FREED IF FOUND ALLOCATED.

	MainWindowStruct * mws = (MainWindowStruct *) data;

	this->function = mws->function;
	this->planar_mesh = new	PlanarMesh(mws->grid_size.x, mws->grid_size.y, mws->open);
	this->ApplyCustomization(mws->function);

	if (!this->fragment_shader.Initialize("per_fragment.vert", "per_fragment.frag"))
		return false;

	if (!this->vertex_shader.Initialize("per_vertex.vert", "per_vertex.frag"))
		return false;

	if (this->skybox_container.Initialize("skybox_texture.jpg") == false)
	{
		cerr << "Failed to load sky box texture." << endl;
		return false;
	}

	if (this->normal_map_container.Initialize("NMStripes.png") == false)
	{
		cerr << "Failed to load normal map texture." << endl;
		return false;
	}

	if (this->second_normal_map_container.Initialize("NMBumpsOut.png") == false)
	{
		cerr << "Failed to load second normal map texture." << endl;
		return false;
	}

	this->shader = &this->fragment_shader;

	glGenBuffers(1, &this->vertex_coordinate_handle);
	assert(this->vertex_coordinate_handle != BAD_GL_VALUE);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_coordinate_handle);
	glBufferData(GL_ARRAY_BUFFER, this->planar_mesh->GetSizeOfArray() * sizeof(glm::vec3), this->planar_mesh->GetOutArray(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->texture_coordinate_handle);
	assert(this->texture_coordinate_handle != BAD_GL_VALUE);
	glBindBuffer(GL_ARRAY_BUFFER, this->texture_coordinate_handle);
	glBufferData(GL_ARRAY_BUFFER, this->planar_mesh->GetSizeOfArray() * sizeof(glm::vec2), this->planar_mesh->GetOutTextureCoordinates(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->normal_value_handle);
	assert(this->normal_value_handle != BAD_GL_VALUE);
	glBindBuffer(GL_ARRAY_BUFFER, this->normal_value_handle);
	glBufferData(GL_ARRAY_BUFFER, this->planar_mesh->GetSizeOfArray() * sizeof(glm::vec3), this->planar_mesh->GetOutNormals(), GL_STATIC_DRAW);
	
	//pass in the change in normals, relative to x and y
	glGenBuffers(1, &this->normal_dx_handle);
	assert(this->normal_dx_handle != BAD_GL_VALUE);
	glBindBuffer(GL_ARRAY_BUFFER, this->normal_dx_handle);
	glBufferData(GL_ARRAY_BUFFER, this->planar_mesh->GetSizeOfArray() * sizeof(glm::vec3), this->planar_mesh->GetOutNormalsDx(), GL_STATIC_DRAW);

	glGenBuffers(1, &this->normal_dy_handle);
	assert(this->normal_dy_handle != BAD_GL_VALUE);
	glBindBuffer(GL_ARRAY_BUFFER, this->normal_dy_handle);
	glBufferData(GL_ARRAY_BUFFER, this->planar_mesh->GetSizeOfArray() * sizeof(glm::vec3), this->planar_mesh->GetOutNormalsDy(), GL_STATIC_DRAW);


	glGenVertexArrays(1, &this->vertex_array_handle);
	glBindVertexArray(this->vertex_array_handle);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, this->vertex_coordinate_handle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *) NULL);
	
	glBindBuffer(GL_ARRAY_BUFFER, this->normal_value_handle);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *) NULL);

	glBindBuffer(GL_ARRAY_BUFFER, this->texture_coordinate_handle);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *) NULL);

	glBindBuffer(GL_ARRAY_BUFFER, this->normal_dy_handle);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *) NULL);

	glBindBuffer(GL_ARRAY_BUFFER, this->normal_dx_handle);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *) NULL);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (mws->open == false)
	{
		this->MakeEndCap();

		glGenBuffers(1, &this->endcap_coordinate_handle);
		assert(this->endcap_coordinate_handle != BAD_GL_VALUE);
		glBindBuffer(GL_ARRAY_BUFFER, this->endcap_coordinate_handle);
		glBufferData(GL_ARRAY_BUFFER, this->endcap_coordinates.size() * sizeof(glm::vec3), &this->endcap_coordinates[0], GL_STATIC_DRAW);

		glGenBuffers(1, &this->endcap_normal_handle);
		assert(this->endcap_normal_handle != BAD_GL_VALUE);
		glBindBuffer(GL_ARRAY_BUFFER, this->endcap_normal_handle);
		glBufferData(GL_ARRAY_BUFFER, this->endcap_normals.size() * sizeof(glm::vec3), &this->endcap_normals[0], GL_STATIC_DRAW);

		glGenVertexArrays(1, &this->endcap_vertex_array_handle);
		glBindVertexArray(this->endcap_vertex_array_handle);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, this->endcap_coordinate_handle);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *) NULL);
		glBindBuffer(GL_ARRAY_BUFFER, this->endcap_normal_handle);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *) NULL);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	return true;
}

inline bool EVEN(int n)
{
	return ((n % 2) == 0);
}

inline bool ODD(int n)
{
	return ((n & 1) == 1);
}

/* Making end caps was an afterthought. The endcaps are triangle fans.
*/

void MainWindow::MakeEndCap()
{
	int x_density, y_density;
	this->planar_mesh->GetDensity(x_density, y_density);
	float tr = 360.0f / ((float) x_density - 1);

	glm::mat4 m(1.0f);
	glm::vec4 p(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 q;

	this->endcap_coordinates.push_back(glm::vec3(0.0f));
	this->endcap_normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	this->endcap_triangle_indices.push_back(0);

	for (int i = 0; i < x_density; ++i)
	{
		q = m * p;
		this->endcap_coordinates.push_back(glm::vec3(q));
		this->endcap_normals.push_back(this->endcap_normals[0]);
		this->endcap_triangle_indices.push_back(i + 1);
		m = glm::rotate(m, tr, glm::vec3(0.0f, 1.0f, 0.0f));
	}
}

void MainWindow::ApplyCustomization(float (* function)(float))
{
	int x_density, y_density;
	this->planar_mesh->GetDensity(x_density, y_density);
	float tr =  360.0f / ((float) x_density - 1);

	glm::vec4 p;
	glm::vec4 q;

	int y_start = 0;
	int y_end = y_density;

	for (int y = y_start; y < y_end; ++y)
	{
		glm::mat4 m(1.0f);
		glm::vec3 * outPointer = (this->planar_mesh->GetOutArray() + x_density * y);
		p = glm::vec4((*function)(((float) y - y_start) / ((float) y_end - 1)), ((float) y - y_start) / ((float) y_end - 1) - 0.5f, 0.0f, 0.0f);
		for (int x = 0; x < x_density; ++x)
		{
			q = m * p;
			*(outPointer++) = glm::vec3(q.x, q.y, q.z);
			m = glm::rotate(m, tr, glm::vec3(0.0f, 1.0f, 0.0f));
		}
	}

	// Make normals
	int left = -1;
	int right;
	int up = x_density;
	int down = -x_density;

	float top_radius = (*function)(0.0f);
	float bot_radius = (*function)(1.0f);
	

	for (int y = 0; y < y_density; y++)
	{
		glm::vec3 * outVertexPointer = (this->planar_mesh->GetOutArray() + x_density * y + 1);
		glm::vec3 * outNormalPointer = (this->planar_mesh->GetOutNormals() + x_density * y + 1);
		glm::vec3 * firstNormalPointer = (this->planar_mesh->GetOutNormals() + x_density * y);
		//calculate the normal dx, dy
		glm::vec3 * outNormalPointerDx = (this->planar_mesh->GetOutNormalsDx() + x_density * y + 1);
		glm::vec3 * outNormalPointerDy = (this->planar_mesh->GetOutNormalsDy() + x_density * y + 1);
		glm::vec3 * firstNormalPointerDx = (this->planar_mesh->GetOutNormalsDx() + x_density * y);
		glm::vec3 * firstNormalPointerDy = (this->planar_mesh->GetOutNormalsDy() + x_density * y);
		

		for (int x = 1; x < x_density; x++)
		{
			right = (x < x_density - 1) ? 1 : -(x_density - 2);

			glm::vec3 normal(0.0f);

			if (y == 0 && top_radius == 0.0f)
				normal = glm::vec3(0.0f, 1.0f, 0.0f);
			else if (y == y_density - 1 && bot_radius == 0.0f)
				normal = glm::vec3(0.0f, -1.0f, 0.0f);
			else
			{
				// Row above
				if (y < y_density - 1)
				{
					if (y > 0)
						normal += glm::cross(glm::normalize(*(outVertexPointer + left) - *outVertexPointer) , glm::normalize(*(outVertexPointer + left + up) - *outVertexPointer));
					normal += glm::cross(glm::normalize(*(outVertexPointer + left + up) - *outVertexPointer) , glm::normalize(*(outVertexPointer + up) - *outVertexPointer));
					normal += glm::cross(glm::normalize(*(outVertexPointer + up) - *outVertexPointer) , glm::normalize(*(outVertexPointer + right) - *outVertexPointer));
				}
				if (y > 0)
				{
					normal += glm::cross(glm::normalize(*(outVertexPointer + down) - *outVertexPointer) , glm::normalize(*(outVertexPointer + left) - *outVertexPointer));
					normal += glm::cross(glm::normalize(*(outVertexPointer + down + right) - *outVertexPointer) , glm::normalize(*(outVertexPointer + down) - *outVertexPointer));
					if (y < y_density - 1)
						normal += glm::cross(glm::normalize(*(outVertexPointer + right) - *outVertexPointer) , glm::normalize(*(outVertexPointer + down + right) - *outVertexPointer));
				}
			}

			normal = -1.0f*glm::normalize(normal);
			//dx
			glm::vec3 prevNormalDx=*(outNormalPointer+left);
			if (x == x_density - 1)
			{
				*firstNormalPointer = normal;
				*firstNormalPointerDx = (prevNormalDx-normal)*20.0f;
				*(outNormalPointerDx-x_density+2) = (prevNormalDx-normal)*20.0f;
			}
			if(x!=1)
				*outNormalPointerDx = (prevNormalDx-normal)*20.0f;
			//dy
			glm::vec3 prevNormalDy;
			if (y == y_start){
				prevNormalDy= normal;
			} else{
				prevNormalDy= *(outNormalPointer+down);
			}
			*outNormalPointerDy = (prevNormalDy-normal)*20.0f;
			
			*outNormalPointer = normal;
			outNormalPointer++;
			outNormalPointerDx++;
			outNormalPointerDy++;
			outVertexPointer++;
		}
	}
}

//http://www.keithlantz.net/wp-content/uploads/2011/10/skybox_texture.jpg

/*	This turned out not to be an actual skybox as the camera is fixed.
	As such, only the front face is drawn.
*/

void MainWindow::DrawSkybox()
{
	double one_third = 1.0 / 3.0;
	double one_quarter = 1.0 / 4.0;

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(20.0, double(this->width) / double(this->height), 1, 10);
	glViewport(0, 0, this->width, this->height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.5f);
	this->skybox_container.Bind();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glScalef(3.0f, 3.0f, 3.0f);
	glBegin(GL_QUADS);
	glTexCoord2d(1.0 * one_quarter, 1 * one_third);
	glVertex3d(1, 1, -1);
	glTexCoord2d(2.0 * one_quarter, 1 * one_third);
	glVertex3d(-1, 1, -1);
	glTexCoord2d(2.0 * one_quarter, 2 * one_third);
	glVertex3d(-1, -1, -1);
	glTexCoord2d(1.0 * one_quarter, 2 * one_third);
	glVertex3d(1, -1, -1);
	glEnd();
}
