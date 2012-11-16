#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*	Perry Kivolowitz - Computer Sciences Department - University of Wisconsin
*/

class PlanarMesh
{
public:

	enum WhichArray
	{
		InArray,
		OutArray
	};

	enum MeshStyle
	{
		Regular,
		Flexible
	};

	PlanarMesh(int xDensity, int yDensity, bool open = false, MeshStyle meshStyle = Regular);
	~PlanarMesh();
	void Draw(WhichArray whichArray = InArray, bool drawNormals = false);
	void ResizeMesh(int xDensity, int yDensity, MeshStyle meshStyle = Regular);
	void GetDensity(int & x, int & y);

	glm::vec3 * GetInArray();
	glm::vec3 * GetOutArray();
	glm::vec2 * GetInTextureCoordinates();
	glm::vec2 * GetOutTextureCoordinates();
	glm::vec3 * GetInNormals();
	glm::vec3 * GetOutNormals();
	glm::uvec3 * GetTriangleIndexArray();

	bool open;

	int GetSizeOfArray()
	{
		return (int) VInArray.size();
	}

	int GetNumberOfElements();

private:
	std::vector<glm::vec3>	VInArray;			// A pristine grid.
	std::vector<glm::vec2>  VTInArray;			// The unchanging texture coordinate grid.
	std::vector<glm::vec2>  VTOutArray;			// A changable texture coordinate array.
	std::vector<glm::vec3>	VOutArray;			// A potentially modified grid.
	std::vector<glm::uvec3>	TArray;				// An array defining the triangles in the grid.
	std::vector<glm::vec3>	NInArray;			// A printing grid of NORMALS
	std::vector<glm::vec3>	NOutArray;			// A potentially modified grid of NORMALS

	int xDensity;
	int yDensity;

	void MakeMesh(MeshStyle meshStyle);
	void ReleaseMemory();
	void AllocateMemory();
};
