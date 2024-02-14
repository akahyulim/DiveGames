#pragma once
#include "Dive.h"

//#include <assimp/postprocess.h>
//#include <assimp/version.h>
//#include <assimp/scene.h>
//#include <assimp/material.h>
//#include <assimp/Importer.hpp>
//#include <assimp/ProgressHandler.hpp>

// �ϴ� Ư���� static�� ��︰��.
// �� ����� �ܺ� ������ ���� ���Ϸ� ��ȯ�ϴ� ���̴�.
// model, material, animation �� ������ �����Ͽ� ����ȭ���Ѿ� �Ѵ�.
class ModelImporter
{
public:
	ModelImporter();
	~ModelImporter() = default;

	bool Load(const std::string& fileName);
	bool LoadExternalFile(const std::string& fileName);
	bool LoadEngineFile(const std::string& fileName);

	void Clear();

	Dive::Model* GetImportedModel() const { return m_pImportedModel; }

private:
	void processNode(aiNode* pNode, Dive::Transform* pParent);
	void processMesh(aiMesh* pMesh, Dive::GameObject* pMeshNodeObject);
	Dive::Material* loadMaterial(aiMesh* pMesh);
	Dive::Texture2D* loadEmbeddedTexture(const aiTexture* pEmbeddedTexture);

	void processAnimation();

private:
	std::string m_FileDirectory;
	std::string m_FileName;
	std::string m_ModelName;
	const aiScene* m_pScene;
	Dive::Model* m_pImportedModel;
};
