#pragma once
#include "Types.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"

class aiNode;
class aiMesh;
class aiMaterial;
class aiScene;

class Model
{
	public:
		Model();

		bool Load(const string& path);
		void Draw(glm::vec3 const& camPos, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& lightPos, const glm::vec3& lightColor, bool bForceAmbient =false);
		const vector<Mesh>& getMeshs() const;

	private:
		vector<Mesh>			m_meshes;
		string					m_directory;
		map<string, Texture>	m_loaded_textures;
		Shader					m_ShaderColorPhong;
		Shader					m_ShaderColorAmbient;
		Shader					m_ShaderTextureDiffuse;
		Shader					m_ShaderTextureAmbient;

		void processNodes(const aiNode* node, const aiScene* scene);
		void processMesh(const aiMesh* mesh, const aiScene* scene);
		vector<Texture> loadMaterialTextures(aiMaterial* mat, int aiTexType, const string& type_name);
};
