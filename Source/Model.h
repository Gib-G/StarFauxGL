#pragma once
#include "Types.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"

class aiNode;
class aiMesh;
class aiMaterial;
class aiScene;

class CModel
{
	public:
		CModel();

		bool Load(const string& path);
		void Draw(glm::vec3 const& CameraPosition, glm::mat4 const& ModelMatrix, glm::mat4 const& ViewMatrix, glm::mat4 const& ProjectionMatrix, glm::vec3 const& LightPosition, glm::vec3 const& LightColor, bool const ForceAmbient = false);
		const vector<CMesh>& getMeshs() const;

	private:
		vector<CMesh>			m_meshes;
		string					m_directory;
		map<string, CTexture>	m_loaded_textures;
		CShader					m_ShaderColorPhong;
		CShader					m_ShaderColorAmbient;
		CShader					m_ShaderTextureDiffuse;
		CShader					m_ShaderTextureAmbient;

		void processNodes(const aiNode* node, const aiScene* scene);
		void processMesh(const aiMesh* mesh, const aiScene* scene);
		vector<CTexture> loadMaterialTextures(aiMaterial* mat, int aiTexType, const string& type_name);
};
