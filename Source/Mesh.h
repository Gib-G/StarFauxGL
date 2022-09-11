#pragma once
#include "Types.h"
#include "Shader.h"
#include "Texture.h"

struct SVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoords;
	glm::vec4 Colors;
};

class CMesh
{
	public:
		vector<SVertex>	m_vertices;
		vector<GLuint>	m_indices;
		vector<CTexture>	m_textures;
		glm::mat4		m_matColors;

		CMesh(const vector<SVertex>&  vertices,
			 const vector<GLuint>&  indices,
			 const vector<CTexture>& textures,
			 const glm::mat4& matColors,
			 bool bHasNormals,
			 bool bHasTexCoords,
			 bool bHasColors,
			 bool bHasAmbientTex,
			 bool bHasdiffuseTex,
			 bool bHasspecularTex,
			 const CShader& shaderColorPhong,
			 const CShader& shaderColorAmbient,
			 const CShader& shaderTextureDiffuse,
			 const CShader& shaderTextureAmbient);

		void Draw(glm::vec3 const& camPos, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& lightPos, const glm::vec3& lightColor, bool bForceAmbient);

	private:
		GLuint			m_VAO[1];
		GLuint			m_VBO[1];
		GLuint			m_EBO[1];
		bool			m_bHasNormals;
		bool			m_bHasTexCoords;
		bool			m_bHasColors;
		bool			m_bHasAmbientTex;
		bool			m_bHasDiffuseTex;
		bool			m_bHasSpecularTex;
		const CShader&	m_shaderColorPhong;
		const CShader&	m_shaderColorAmbient;
		const CShader&	m_shaderTextureDiffuse;
		const CShader&	m_shaderTextureAmbient;
};

