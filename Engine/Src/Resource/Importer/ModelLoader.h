#pragma once
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>

namespace Dive
{
	class Model;
	class Scene;
	class GameObject;
	class Material;
	class DvTexture2D;

	class ModelLoader
	{
	public:
		static bool Load(Model* pModel, const std::string& fileName);

	private:
		static void parseNode(const aiNode* pNode, GameObject* pParent = nullptr);
		static void parseMesh(const aiMesh* pMesh, GameObject* pMeshNodeObject);
		static Material* loadMaterial(const aiMesh* pMesh);
		static DvTexture2D* loadEmbeddedTexture(const aiTexture* pEmbeddedTex);

	private:
		static const aiScene* s_aiScene;

		static std::string s_FileName;
		static std::string s_ModelName;

		static Scene* s_pActiveScene;
		static Model* s_pModel;
	};
}