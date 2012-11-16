#pragma once
#undef	_UNICODE
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "utilities.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#define	BAD_IL_VALUE	IL_INVALID_VALUE

class ILContainer
{
public:
	ILContainer()
	{
		this->il_handle = BAD_IL_VALUE;
		this->il_texture_handle = BAD_GL_VALUE;
	}

	~ILContainer();
	
	bool Initialize(const char * file_name);
	void Bind(GLuint texture_unit = 0);

	ILint width;
	ILint height;
	ILuint il_handle;
	GLuint il_texture_handle;
};
