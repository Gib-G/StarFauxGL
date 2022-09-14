#pragma once
#include "Axes.h"
#include "Mesh.h"
#include "Shader.h"
#include <reactphysics3d/reactphysics3d.h>

class aiNode;
class aiMesh;
class aiMaterial;
class aiScene;

struct SAABB 
{
	float XMin = 0.f, XMax = 0.f, YMin = 0.f, YMax = 0.f, ZMin = 0.f, ZMax = 0.f;
	rp3d::Vector3 GetLength() const;
	float GetMaxLength() const;
};

class CModel
{
	public:
		CModel() = default;
		CModel(const string& path);

		bool Load(const string& path);
		bool IsLoaded() const { return Loaded; }

		void Draw(glm::vec3 const& CameraPosition, glm::mat4 const& ModelMatrix, glm::mat4 const& ViewMatrix, glm::mat4 const& ProjectionMatrix, glm::vec3 const& LightPosition, glm::vec3 const& LightColor, bool const ForceAmbient = false);
		
		SAABB const& GetAABB() const;
		const vector<CMesh>& getMeshs() const; // Should be private.

	private:
		bool Loaded = false;

		vector<CMesh>			m_meshes;
		string					m_directory;
		map<string, CTexture>	m_loaded_textures;
		CShader					m_ShaderColorPhong;
		CShader					m_ShaderColorAmbient;
		CShader					m_ShaderTextureDiffuse;
		CShader					m_ShaderTextureAmbient;

		SAABB AABB;

		void processNodes(const aiNode* node, const aiScene* scene);
		void processMesh(const aiMesh* mesh, const aiScene* scene);
		vector<CTexture> loadMaterialTextures(aiMaterial* mat, int aiTexType, const string& type_name);
};
