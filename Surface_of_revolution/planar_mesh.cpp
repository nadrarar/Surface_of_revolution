#include <GL/freeglut.h>
#include "planar_mesh.h"

#ifndef	BAD_GL_VALUE
#define	BAD_GL_VALUE	((GLuint) -1)
#endif

/*	Perry Kivolowitz - University of Wisconsin CS 559
	
	A class to draw a planar mesh  grid  triangulated in one
	of two ways. The mesh is drawn  over +/- 0.5 so scale to
	taste. Vector3s can be drawn  to assist in the debugging
	of displacement maps.

	Vector3 arrays are used.  The  caller is given access to
	both the original data (the planar  mesh which should be
	kept pristine) and the output array (which the user must
	fill in). When the mesh is asked to draw itself the user
	specifies which mesh (in or out) to draw from.
*/

// A constructur which sets the settings and builds the
// mesh right away.
PlanarMesh::PlanarMesh(int xDensity, int yDensity, bool open, MeshStyle meshStyle)
{
	this->open = open;
	this->xDensity = xDensity;
	this->yDensity = yDensity;

	this->ReleaseMemory();
	this->AllocateMemory();
	this->MakeMesh(meshStyle);
}

PlanarMesh::~PlanarMesh()
{
	this->ReleaseMemory();
}

// ReleaseMemory is abstracted from the destructor because it
// can then be shared with ResizeMesh().
void PlanarMesh::ReleaseMemory()
{
	this->VInArray.clear();
	this->VTInArray.clear();
	this->VTOutArray.clear();
	this->VOutArray.clear();
	this->TArray.clear();
}

// AllocateMemory is similarly abstracted from the constructor allowing
// it to be shared with ResizeMesh.
void PlanarMesh::AllocateMemory()
{
	this->VInArray.resize(xDensity * yDensity);
	this->VTInArray.resize(xDensity * yDensity);
	this->TArray.resize((xDensity - 1) * (yDensity - 1) * 2);
	this->NInArray.resize(xDensity * yDensity);
	this->NInArrayDx.resize(xDensity * yDensity);
	this->NInArrayDy.resize(xDensity * yDensity);
}

void PlanarMesh::ResizeMesh(int xDensity, int yDensity, MeshStyle meshStyle)
{
	ReleaseMemory();
	this->xDensity = xDensity;
	this->yDensity = yDensity;
	AllocateMemory();
	MakeMesh(meshStyle);
}

// These two inline functions help in defining the "flexible" mesh.
inline bool EVEN(int n)
{
	return ((n % 2) == 0);
}

inline bool ODD(int n)
{
	return ((n & 1) == 1);
}

void PlanarMesh::MakeMesh(MeshStyle meshStyle)
{
	int		y , x , i;
	float		HalfW;
	float		HalfH;
	glm::uvec3 * t;

	// assigns values to vertices

	HalfW = (xDensity - 1) / 2.0f;
	HalfH = (yDensity - 1) / 2.0f;

	// Define a grid of Vector3es with Vector3s that point down the Z axis.
	// The Vector3 grid ranges over +/- 0.5.

	// We also define a grid of texture coordinates. These simply range
	// from zero to one.

	for (y = 0; y < yDensity; y++)
	{
		for (x = 0; x < xDensity; x++)
		{
			i = y * xDensity + x;

			VInArray[i].x = (((float) x) - HalfW) / ((float) xDensity - 1);
			VInArray[i].y = (((float) y) - HalfH) / ((float) yDensity - 1);
			VInArray[i].z = 0.0;

			NInArray[i].x = 0.0;
			NInArray[i].y = 0.0;
			NInArray[i].z = 1.0;
			
			NInArrayDx[i].x = 0.0;
			NInArrayDx[i].y = 0.0;
			NInArrayDx[i].z = 1.0;
			
			NInArrayDy[i].x = 0.0;
			NInArrayDy[i].y = 0.0;
			NInArrayDy[i].z = 1.0;

			VTInArray[i].s = ((float) x) / ((float) xDensity - 1);
			VTInArray[i].t = ((float) y) / ((float) yDensity - 1);
		}
	}

	// To use vector arrays, we must define triangles by indicating which
	// Vector3es live in the same triangle. With more complicated code
	// TRIANGLE_STRIPS could have been drawn force large changes to the
	// following code.

	// This code provides two means of assigning Vector3es to triangles.
	// The two methods produce grids conforming to two schools of thought.
	// For a planar mesh, once a texture is placed on them it won't really
	// make a difference. However, if you start manipulating the components
	// of the mesh (in the VOutArray) one triangulation method might 
	// produce better results than another.

	for (y = 0; y < yDensity - 1; y++)
	{
		for (x = 0; x < xDensity - 1; x++)
		{
			t = &TArray[(y * (xDensity - 1) + x) * 2];

			if (meshStyle == Flexible)
			{
				if ((EVEN(x) && EVEN(y)) || (ODD(x) && ODD(y)))
				{
					t->x = (y + 0) * xDensity + x + 0;
					t->y = (y + 0) * xDensity + x + 1;
					t->z = (y + 1) * xDensity + x + 0;
					t++;
					t->x = (y + 0) * xDensity + x + 1;
					t->y = (y + 1) * xDensity + x + 1;
					t->z = (y + 1) * xDensity + x + 0;
				}
				else 
				{
					t->x = (y + 0) * xDensity + x + 0;
					t->y = (y + 0) * xDensity + x + 1;
					t->z = (y + 1) * xDensity + x + 1;
					t++;
					t->x = (y + 0) * xDensity + x + 0;
					t->y = (y + 1) * xDensity + x + 1;
					t->z = (y + 1) * xDensity + x + 0;
				}
			}
			else // Regular
			{
				// "top" triangle
				t->x = (y + 0) * xDensity + x + 0;
				t->y = (y + 0) * xDensity + x + 1;
				t->z = (y + 1) * xDensity + x + 0;
				t++;
				// "bottom" triangle
				t->x = (y + 0) * xDensity + x + 1;
				t->y = (y + 1) * xDensity + x + 1;
				t->z = (y + 1) * xDensity + x + 0;
			}
		}
	}

	this->NOutArray.insert(this->NOutArray.begin(), this->NInArray.begin(), this->NInArray.end());
	this->NOutArrayDx.insert(this->NOutArrayDx.begin(), this->NInArrayDx.begin(), this->NInArrayDx.end());
	this->NOutArrayDy.insert(this->NOutArrayDy.begin(), this->NInArrayDy.begin(), this->NInArrayDy.end());
	this->VTOutArray.insert(this->VTOutArray.begin(), this->VTInArray.begin(), this->VTInArray.end());
	this->VOutArray.insert(this->VOutArray.begin(), this->VInArray.begin(), this->VInArray.end());
}

// When you ask the mesh to draw itself, you must specify which array
// to draw. There are two arrays. The InArray should always be kept
// pristine. The OutArray might contain new vertices after some type
// of displacement map operation is performed by the calling code.
void PlanarMesh::Draw(WhichArray whichArray, bool drawNormals,bool ndx_n)
{
	// We will specify array pointers for Vector3es, Vector3s and texture
	// coordinates. Therefore, we must enable OpenGL to process these
	// arrays.
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// These functions specify the array pointers of the various types.
	// Notice we can point to either the IN arrays or the OUT arrays.
	// This permits the use of the OUT array after processing using
	// a displacement map.
	//glVertexPointer(3 , GL_FLOAT , sizeof(glm::vec3) , (whichArray == InArray) ? &VInArray[0] : &VOutArray[0]);
	//glNormalPointer(GL_FLOAT, sizeof(glm::vec3), (whichArray == InArray) ? &NInArray[0] : &NOutArray[0]);
	//glTexCoordPointer(2 , GL_FLOAT , sizeof(glm::vec2) , (whichArray == InArray) ? &VTInArray[0].x : &VTOutArray[0].x);

	// Here is the actual drawing is made.
	//glDrawElements(GL_TRIANGLES , (xDensity - 1) * (yDensity - 1) * 2 * 3 , GL_UNSIGNED_INT , &TArray[0]);


	// For debugging purposes you might wish to draw the Vector3s at each Vector3.
	if (drawNormals)
	{
		float line_width;
		glGetFloatv(GL_LINE_WIDTH, &line_width);
		glLineWidth(1.0f);

		GLboolean lightingState = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);

		GLboolean textureState = glIsEnabled(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);

		glm::vec3 * v = (whichArray == InArray) ? &VInArray[0] : &VOutArray[0];
		glm::vec3 * n = (whichArray == InArray) ? &NInArray[0] : &NOutArray[0];
		glm::vec3 * ndx = (whichArray == InArray) ? &NInArrayDx[0] : &NOutArrayDx[0];
		glm::vec3 * ndy = (whichArray == InArray) ? &NInArrayDy[0] : &NOutArrayDy[0];

		int maxDensity = (xDensity > yDensity) ? xDensity : yDensity;
		maxDensity = maxDensity / 4;

		glBegin(GL_LINES);
		for (int i = 0; i < xDensity * yDensity; i++)
		{
			glColor3d(1,1,1);

			if ((i % xDensity) == 1)
				glColor3d(1,0,0);
			else if ((i % xDensity) == xDensity - 1)
			{
				v++;
				n++;
				ndx++;
				ndy++;

				continue;
			}

			if (i < xDensity)
				glColor3d(0,1,0);

			if(ndx_n){
				glVertex3d(v->x + n->x / 400.0, v->y + n->y / 400.0, v->z + n->z / 400.0);
				glVertex3d(v->x + n->x / maxDensity, v->y + n->y / maxDensity, v->z + n->z / maxDensity);
			}else{
				glVertex3d(v->x + ndy->x / 400.0, v->y + ndy->y / 400.0, v->z + ndy->z / 400.0);
				glVertex3d(v->x + ndy->x / maxDensity, v->y + ndy->y / maxDensity, v->z + ndy->z / maxDensity);
			}// By scaling the length of the Vector3s by the maximum axis density, the Vector3s
			// will get short when the mesh gets dense. This way, if you are doing displacement
			// mapping, the Vector3 vectors will tend to get in each other's way less.
			v++;
			n++;
			ndx++;
			ndy++;
		}
		glEnd();

		glLineWidth(line_width);

		if (lightingState)
			glEnable(GL_LIGHTING);

		if (textureState)
			glEnable(GL_TEXTURE_2D);
	}
}

// All these Get function assist in the creation of displacement
// map code. I acknowledge that returning pointers to the actual
// data breaks the object oriented model and permits destructive
// operations. However, they are liste as "protected".

int PlanarMesh::GetNumberOfElements()
{
	return (xDensity - 1) * (yDensity - 1) * 2 * 3;
}

void PlanarMesh::GetDensity(int & x, int & y)
{
	x = xDensity;
	y = yDensity;
}

glm::vec3 * PlanarMesh::GetInArray()
{
	return &VInArray[0];
}

glm::vec3 * PlanarMesh::GetOutArray()
{
	return &VOutArray[0];
}

glm::vec3 * PlanarMesh::GetInNormals()
{
	return &NInArray[0];
}
glm::vec3 * PlanarMesh::GetInNormalsDx()
{
	return &NInArrayDx[0];
}
glm::vec3 * PlanarMesh::GetInNormalsDy()
{
	return &NInArrayDy[0];
}

glm::vec3 * PlanarMesh::GetOutNormals()
{
	return &NOutArray[0];
}

glm::vec3 * PlanarMesh::GetOutNormalsDx()
{
	return &NOutArrayDx[0];
}

glm::vec3 * PlanarMesh::GetOutNormalsDy()
{
	return &NOutArrayDy[0];
}

glm::vec2 * PlanarMesh::GetInTextureCoordinates()
{
	return &VTInArray[0];
}

glm::vec2 * PlanarMesh::GetOutTextureCoordinates()
{
	return &VTOutArray[0];
}

glm::uvec3 * PlanarMesh::GetTriangleIndexArray()
{
	return &TArray[0];
}

