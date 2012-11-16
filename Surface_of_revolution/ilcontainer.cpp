#include <assert.h>
#include "ilcontainer.h"

/*	Using IL can be this simple. The most difficult part of using IL is not getting stung by the wide character support.
	IL is very much worth exploring more deeply.
*/

bool ILContainer::Initialize(const char * file_name)
{
	assert(this->il_handle == BAD_IL_VALUE);
	if ((this->il_handle = ilGenImage()) == BAD_IL_VALUE)
		return false;
	ilBindImage(this->il_handle);
	if (!ilLoadImage(file_name))
		return false;

	glGenTextures(1, &this->il_texture_handle);
	glBindTexture(GL_TEXTURE_2D, this->il_texture_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), this->width = ilGetInteger(IL_IMAGE_WIDTH), this->height = ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
	return true;
}

void ILContainer::Bind(GLuint texture_unit)
{
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_2D, this->il_texture_handle);
}

ILContainer::~ILContainer()
{
	if (this->il_texture_handle != BAD_GL_VALUE)
		glDeleteTextures(1, &this->il_texture_handle);
	if (this->il_handle != BAD_IL_VALUE)
		ilDeleteImage(this->il_handle);

	this->il_handle = BAD_IL_VALUE;
	this->il_texture_handle = BAD_GL_VALUE;
}
