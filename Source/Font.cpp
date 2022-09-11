#include "Font.h"
#include "CImage.h"

static const float caractere[] = 
{
	0,0,0, 0,0, 
	0,1,0, 0,1, 
	1,1,0, 1,1, 
	0,0,0, 0,0, 
	1,1,0, 1,1, 
	1,0,0, 1,0
};

CFont::CFont()
{
	m_TexId		 = (GLuint)-1;
	m_VAO		 = (GLuint)-1;
	m_VBO		 = (GLuint)-1;
	m_Width		 = 0;
	m_Height	 = 0;
	m_CharWidth	 = 0;
	m_CharHeight = 0;
}

CFont::~CFont()
{
	// todo
}

bool CFont::Load(const string& filename)
{
	CImage img;
	if (img.Load(filename) == false)
	{
		ConsoleWriteErr("Font::Load(%s) failed !",filename.c_str());
		return false;
	}

	if (m_shader.Load("shaders/ambient_only.vert", "shaders/ambient_only.frag") == false)
	{
		ConsoleWriteErr("Font::Load(%s) failed ! (pb shader)",filename.c_str());
		return false;
	}

	GLenum pixelType = (img.pixelSize == 4 ? GL_RGBA : GL_RGB);
	glGenTextures(1, &m_TexId);
	glBindTexture(GL_TEXTURE_2D, m_TexId);
	glTexImage2D(GL_TEXTURE_2D, 0, pixelType, img.lenx, img.leny, 0, pixelType, GL_UNSIGNED_BYTE, img.data);

	// Sets how OpenGL filters mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Sets how OpenGL handles out-of-range texcoords
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Crée un mesh pour chaque caractère
	for (int y=0; y<16; y++)
	{
		for (int x=0; x<16; x++)
		{
			int charIndex = y*16+x;
			m_Coords[charIndex*6*5+0+0] = 0;
			m_Coords[charIndex*6*5+0+1] = 0;
			m_Coords[charIndex*6*5+0+2] = 0;
			m_Coords[charIndex*6*5+0+3] = (x+0)/16.0f;
			m_Coords[charIndex*6*5+0+4] = (y+0)/16.0f;

			m_Coords[charIndex*6*5+5+0] = 0;
			m_Coords[charIndex*6*5+5+1] = 1;
			m_Coords[charIndex*6*5+5+2] = 0;
			m_Coords[charIndex*6*5+5+3] = (x+0)/16.0f;
			m_Coords[charIndex*6*5+5+4] = (y+1)/16.0f;

			m_Coords[charIndex*6*5+10+0] = 1;
			m_Coords[charIndex*6*5+10+1] = 1;
			m_Coords[charIndex*6*5+10+2] = 0;
			m_Coords[charIndex*6*5+10+3] = (x+1)/16.0f;
			m_Coords[charIndex*6*5+10+4] = (y+1)/16.0f;

			m_Coords[charIndex*6*5+15+0] = 0;
			m_Coords[charIndex*6*5+15+1] = 0;
			m_Coords[charIndex*6*5+15+2] = 0;
			m_Coords[charIndex*6*5+15+3] = (x+0)/16.0f;
			m_Coords[charIndex*6*5+15+4] = (y+0)/16.0f;

			m_Coords[charIndex*6*5+20+0] = 1;
			m_Coords[charIndex*6*5+20+1] = 1;
			m_Coords[charIndex*6*5+20+2] = 0;
			m_Coords[charIndex*6*5+20+3] = (x+1)/16.0f;
			m_Coords[charIndex*6*5+20+4] = (y+1)/16.0f;

			m_Coords[charIndex*6*5+25+0] = 1;
			m_Coords[charIndex*6*5+25+1] = 0;
			m_Coords[charIndex*6*5+25+2] = 0;
			m_Coords[charIndex*6*5+25+3] = (x+1)/16.0f;
			m_Coords[charIndex*6*5+25+4] = (y+0)/16.0f;
		}
	}

	//  Crée un VAO pour un quadrilatère
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Coords), m_Coords, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(3*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	return true;
}

void CFont::Draw(const char* text, int x, int y, float size)
{
	glBindVertexArray(m_VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TexId);

	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 view  = glm::mat4(1.0);
	glm::mat4 proj  = glm::ortho(0,800,0,600,-100,100);

	glDisable(GL_DEPTH_TEST);
	m_shader.Use();
	m_shader.SetUniform("texture_ambient", 0);
	m_shader.SetUniform("view",  view );
	m_shader.SetUniform("proj",  proj );

	for (size_t i=0,j=strlen(text); i<j; i++)
	{
		model = glm::translate(glm::mat4(1.0),glm::vec3(x+i*size,y,10));
		model = glm::scale(model,glm::vec3(size,size,1));
		m_shader.SetUniform("model", model);
		glDrawArrays(GL_TRIANGLES, 30*(int)(text[i]), 6);
	}

	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);

}
