#include "Mesh.h"

CMesh::CMesh
(
			const vector<SVertex>& vertices,
			const vector<GLuint>& indices,
			const vector<CTexture>& textures,
			const glm::mat4& matColors,
			bool bHasNormals, bool bHasTexCoords, bool bHasColors,
			bool bHasAmbientTex, bool bHasDiffuseTex, bool bHasSpecularTex,
			const CShader& shaderColorPhong,
			const CShader& shaderColorAmbient,
			const CShader& shaderTextureDiffuse,
			const CShader& shaderTextureAmbient
)
	: m_vertices(vertices)
	, m_indices(indices)
	, m_textures(textures)
	, m_matColors(matColors)
	, m_bHasNormals(bHasNormals)
	, m_bHasTexCoords(bHasTexCoords)
	, m_bHasColors(bHasColors)
	, m_bHasAmbientTex (bHasAmbientTex)
	, m_bHasDiffuseTex (bHasDiffuseTex)
	, m_bHasSpecularTex(bHasSpecularTex)
	, m_shaderColorPhong    (shaderColorPhong)
	, m_shaderColorAmbient  (shaderColorAmbient)
	, m_shaderTextureDiffuse(shaderTextureDiffuse)
	, m_shaderTextureAmbient(shaderTextureAmbient)
{
	glGenVertexArrays(1, m_VAO);
	glGenBuffers(1, m_VBO);
	glGenBuffers(1, m_EBO);

	glBindVertexArray(m_VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(SVertex), m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

	// vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*)offsetof(SVertex, Position));
	glEnableVertexAttribArray(0);

	// vertex normals
	if (m_bHasNormals)
	{
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*)offsetof(SVertex, Normal));
		glEnableVertexAttribArray(1);
	}
	else
	{
		glDisableVertexAttribArray(1);
	}

	// vertex texture coordinates
	if (m_bHasTexCoords)
	{
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*)offsetof(SVertex, TexCoords));
		glEnableVertexAttribArray(2);
	}
	else
	{
		glDisableVertexAttribArray(2);
	}

	// vertex colors
	if (m_bHasColors)
	{
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), (GLvoid*)offsetof(SVertex, Colors));
		glEnableVertexAttribArray(3);
	}
	else
	{
		glDisableVertexAttribArray(3);
	}

	glBindVertexArray(0);
}

void CMesh::Draw(glm::vec3 const& camPos, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& lightPos, const glm::vec3& lightColor, bool bForceAmbient)
{
	// Note : Il faut utiliser bForceAmbient à true pour les modeles 3D ayant des normales incohérentes

	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

	if (m_textures.size() > 0)
	{
		// L'algo est basique, quand on texture, on ignore les combinaisons ambiant+diffus+spéculaire, on prend que le 1er venu.
		size_t index = 0;
		if (m_bHasAmbientTex || bForceAmbient)
		{
			for (size_t i = 0, iLen = m_textures.size(); i < iLen; i++)
			{
				if (m_textures[i].m_type == "texture_ambient" ||
					(bForceAmbient && m_textures[i].m_type == "texture_diffuse"))
				{
					index = i;
					break;
				}
			}
			m_shaderTextureAmbient.Use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textures[index].m_id);
			m_shaderTextureAmbient.SetUniform("texture_ambient", 0);
			m_shaderTextureAmbient.SetUniform("model", model);
			m_shaderTextureAmbient.SetUniform("view", view);
			m_shaderTextureAmbient.SetUniform("proj", proj);
			m_shaderTextureAmbient.SetUniform("lightColor", lightColor);
		}
		else if (m_bHasDiffuseTex)
		{
			for (size_t i = 0, iLen = m_textures.size(); i < iLen; i++)
			{
				if (m_textures[i].m_type == "texture_diffuse")
				{
					index = i;
					break;
				}
			}
			m_shaderTextureDiffuse.Use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textures[index].m_id);
			m_shaderTextureDiffuse.SetUniform("texture_diffuse", 0);
			m_shaderTextureDiffuse.SetUniform("model", model);
			m_shaderTextureDiffuse.SetUniform("view", view);
			m_shaderTextureDiffuse.SetUniform("proj", proj);
			m_shaderTextureDiffuse.SetUniform("normalMatrix", normalMatrix);
			m_shaderTextureDiffuse.SetUniform("lightColor", lightColor);
			m_shaderTextureDiffuse.SetUniform("lightPosition", lightPos);
		}
		else
		{
			// m_bHasSpecularTex, unsupported 
		}
	}
	else
	{
		if (bForceAmbient)
		{
			m_shaderColorAmbient.Use();
			m_shaderColorAmbient.SetUniform("material", m_matColors);
			m_shaderColorAmbient.SetUniform("model", model);
			m_shaderColorAmbient.SetUniform("view", view);
			m_shaderColorAmbient.SetUniform("proj", proj);
			m_shaderColorAmbient.SetUniform("lightColor", lightColor);
		}
		else
		{
			m_shaderColorPhong.Use();
			m_shaderColorPhong.SetUniform("material", m_matColors);
			m_shaderColorPhong.SetUniform("model", model);
			m_shaderColorPhong.SetUniform("view", view);
			m_shaderColorPhong.SetUniform("proj", proj);
			m_shaderColorPhong.SetUniform("normalMatrix", normalMatrix);
			m_shaderColorPhong.SetUniform("cameraPosition", camPos);
			m_shaderColorPhong.SetUniform("lightColor", lightColor);
			m_shaderColorPhong.SetUniform("lightPosition", lightPos);
		}
	}

	// Draw
	glBindVertexArray(m_VAO[0]);
	glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}