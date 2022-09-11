#include "Texture.h"
#include "CImage.h"

Texture::Texture()
{
	m_id = (GLuint)-1;
}

Texture::~Texture()
{
	// todo
}

void Texture::Bind(GLenum num)
{
	glActiveTexture(num);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

bool Texture::Load(const string& filename, GLenum wrap_s, GLenum wrap_t)
{
	CImage img;
	if (img.Load(filename) == false)
	{
		ConsoleWriteErr("load_texture(%s) failed !",filename.c_str());
		return false;
	}

	GLenum pixelType = (img.pixelSize == 4 ? GL_RGBA : GL_RGB);
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, pixelType, img.lenx, img.leny, 0, pixelType, GL_UNSIGNED_BYTE, img.data);

	// Sets how OpenGL filters mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Sets how OpenGL handles out-of-range texcoords
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

