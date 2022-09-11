#pragma once
#include "Types.h"

class Texture
{
	public:
		GLuint	m_id;
		string	m_type;

		 Texture();
		~Texture();

		bool Load(const string& filename, GLenum wrap_s, GLenum wrap_t);
		void Bind(GLenum num);
};
