#pragma once
#include "Types.h"
#include "Shader.h"

class Font
{
	private:
		GLuint	m_TexId;
		GLuint	m_VAO;
		GLuint	m_VBO;
		int		m_Width;
		int		m_Height;
		int		m_CharWidth;
		int		m_CharHeight;
		Shader	m_shader;
		float	m_Coords[16*16*6*5];

	public:
		 Font();
		~Font();

		bool Load(const string& filename);
		void Draw(const char* text, int x, int y, float size);
};
