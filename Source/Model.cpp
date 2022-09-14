#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Model.h"
#include "Texture.h"
#include "StringUtil.h"
#include <reactphysics3d/reactphysics3d.h>

rp3d::Vector3 SAABB::GetLength() const
{
	assert(XMin <= XMax && YMin <= YMax && ZMin <= ZMax);
	return rp3d::Vector3(XMax - XMin, YMax - YMin, ZMax - ZMin);
}

// Computing it all the times but no one cares...
float SAABB::GetMaxLength() const
{
	return GetLength().getMaxValue();
}

CModel::CModel(const string& path) { Load(path); }

SAABB const& CModel::GetAABB() const { return AABB; }

void CModel::Draw(glm::vec3 const& CameraPosition, glm::mat4 const& ModelMatrix, glm::mat4 const& ViewMatrix, glm::mat4 const& ProjectionMatrix, glm::vec3 const& LightPosition, glm::vec3 const& LightColor, bool const ForceAmbient)
{
	for (auto& m : m_meshes)
	{
		m.Draw(CameraPosition, ModelMatrix, ViewMatrix, ProjectionMatrix, LightPosition, LightColor, ForceAmbient);
	}
}

bool CModel::Load(const string& path)
{
	string const curratedPath = stringReplaceAllTokens(path, "\\", "/");
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(curratedPath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		ConsoleWriteErr("CModel::loadModel(%s) : failed to load. %s", path.c_str(), importer.GetErrorString());
		Loaded = false;
		return false;
	}
	m_directory = curratedPath.substr(0, curratedPath.find_last_of('/'));

	if (m_ShaderColorPhong.Load(ROOT_DIR"Resources\\Shaders\\phong.vert", ROOT_DIR"Resources\\Shaders\\phong.frag") == false)
	{
		ConsoleWriteErr("Failed to load shader");
	}
	if (m_ShaderColorAmbient.Load(ROOT_DIR"Resources\\Shaders\\ambient_col.vert", ROOT_DIR"Resources\\Shaders\\ambient_col.frag") == false)
	{
		ConsoleWriteErr("Failed to load shader");
	}
	if (m_ShaderTextureDiffuse.Load(ROOT_DIR"Resources\\Shaders\\diffuse_tex.vert", ROOT_DIR"Resources\\Shaders\\diffuse_tex.frag") == false)
	{
		ConsoleWriteErr("Failed to load shader");
	}
	if (m_ShaderTextureAmbient.Load(ROOT_DIR"Resources\\Shaders\\ambient_tex.vert", ROOT_DIR"Resources\\Shaders\\ambient_tex.frag") == false)
	{
		ConsoleWriteErr("Failed to load shader");
	}
	processNodes(scene->mRootNode, scene);

	Loaded = true;
	return true;
}

void CModel::processNodes(const aiNode* node, const aiScene* scene)
{
	for (GLuint i=0; i<node->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}
	for (GLuint i=0; i<node->mNumChildren; ++i)
	{
		processNodes(node->mChildren[i], scene);
	}
}

void CModel::processMesh(const aiMesh* mesh, const aiScene* scene)
{
	vector<SVertex>	vertices;
	vector<GLuint>	indices;
	vector<CTexture>	textures;

	bool bHasNormals	= (mesh->mNormals != NULL);
	bool bHasTexCoords	= (mesh->HasTextureCoords(0));
	bool bHasColors		= (mesh->mColors && mesh->mColors[0]);

	// Vertices
	for (GLuint i=0; i<mesh->mNumVertices; ++i)
	{
		SVertex vertex;
		float const x = mesh->mVertices[i].x;
		float const y = mesh->mVertices[i].y;
		float const z = mesh->mVertices[i].z;
		vertex.Position.x = x; vertex.Position.y = y; vertex.Position.z = z;
		AABB.XMin = std::fmin(AABB.XMin, x); AABB.XMax = std::fmax(AABB.XMax, x);
		AABB.YMin = std::fmin(AABB.YMin, y); AABB.YMax = std::fmax(AABB.YMax, y);
		AABB.ZMin = std::fmin(AABB.ZMin, z); AABB.ZMax = std::fmax(AABB.ZMax, z);

		if (bHasNormals)
		{
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}
		else
		{
			vertex.Normal.x = 1.0f;
			vertex.Normal.y = 0.0f;
			vertex.Normal.z = 0.0f;
		}

		// if model contains texcoords
		if (bHasTexCoords)
		{
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.TexCoords.x = 0.0f;
			vertex.TexCoords.y = 0.0f;
		}

		if (bHasColors)
		{
			vertex.Colors.r = mesh->mColors[0][i].r;
			vertex.Colors.g = mesh->mColors[0][i].g;
			vertex.Colors.b = mesh->mColors[0][i].b;
			vertex.Colors.a = mesh->mColors[0][i].a;
		}
		else
		{
			vertex.Colors.r = 1.0f;
			vertex.Colors.g = 1.0f;
			vertex.Colors.b = 1.0f;
			vertex.Colors.a = 1.0f;
		}

		vertices.push_back(vertex);
	}
	// Faces (triangles indices)
	for (GLuint i=0; i<mesh->mNumFaces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];
		for (GLuint j=0; j<face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	vector<CTexture> ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient");
	textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());

	vector<CTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	vector<CTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	bool bHasAmbientTex  = !ambientMaps.empty();
	bool bHasDiffuseTex  = !diffuseMaps.empty();
	bool bHasSpecularTex = !specularMaps.empty();

	glm::mat4 colors(0);
	if (ambientMaps.empty() && diffuseMaps.empty() && specularMaps.empty())
	{
		// No texture, color material
		aiColor3D colorAmbi(0.25f,0.25f,0.25f);
		aiColor3D colorDiff(0.75f,0.75f,0.75f);
		aiColor3D colorSpec(0.00f,0.00f,0.00f);
		float     shininess(0.0f);	// exposant 
		material->Get(AI_MATKEY_COLOR_AMBIENT,	colorAmbi);
		material->Get(AI_MATKEY_COLOR_DIFFUSE,	colorDiff);
		material->Get(AI_MATKEY_COLOR_SPECULAR,	colorSpec);
		material->Get(AI_MATKEY_SHININESS,		shininess);

		// On stocke toutes ces couleurs dans une matrice, c'est plus simple à manipuler.
		colors[0][0] = colorAmbi.r; colors[0][1] = colorAmbi.g; colors[0][2] = colorAmbi.b; colors[0][3] = 1.0f;
		colors[1][0] = colorDiff.r; colors[1][1] = colorDiff.g; colors[1][2] = colorDiff.b; colors[1][3] = 1.0f;
		colors[2][0] = colorSpec.r; colors[2][1] = colorSpec.g; colors[2][2] = colorSpec.b; colors[2][3] = 1.0f;
		colors[3][0] = shininess;
	}

	m_meshes.push_back(CMesh(vertices, indices, textures, colors, 
							bHasNormals, bHasTexCoords, bHasColors, 
							bHasAmbientTex, bHasDiffuseTex, bHasSpecularTex,
							m_ShaderColorPhong, m_ShaderColorAmbient, m_ShaderTextureDiffuse, m_ShaderTextureAmbient));
}

vector<CTexture> CModel::loadMaterialTextures(aiMaterial* mat, int aiTexType, const string& type_name)
{
	aiTextureType type = (aiTextureType)aiTexType;
	vector<CTexture> textures;
	for (GLuint i=0; i<mat->GetTextureCount(type); ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		auto iter = m_loaded_textures.find(str.C_Str());
		if (iter != m_loaded_textures.end())
		{
			textures.push_back(iter->second);
			continue;
		}

		CTexture texture;
		texture.Load(m_directory + '/' + str.C_Str(), GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T);
		texture.m_type = type_name;
		textures.push_back(texture);
		m_loaded_textures[str.C_Str()] = texture;
	}
	return textures;
}

const vector<CMesh>& CModel::getMeshs() const
{
	return m_meshes;
}
