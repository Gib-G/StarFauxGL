#pragma once
#include "Types.h"

class CTexture
{
	public:
		GLuint	m_id;
		string	m_type;

		 CTexture();
		~CTexture();

		bool Load(const string& filename, GLenum wrap_s, GLenum wrap_t);
		void Bind(GLenum num);
};
