#include "Dive.h"

#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/Importer.hpp>
#include <assimp/ProgressHandler.hpp>
#include <yaml-cpp/yaml.h>

// scene, model, animation, material을 따로 파일화한다.

struct OutModel
{
	std::string name;
	aiNode* pRootNode;
	std::vector<aiNode*> meshNodes;
	std::vector<aiMesh*> meshes;
	uint32_t numVertices;
	uint32_t numIndices;
};

const aiScene* g_pScene = nullptr;

void ExportModel(const std::string& name, aiNode* pRoot);
void SaveModel(OutModel& model);

void ExportAnimation(const std::string& name);
void SaveAnimaiton();

void ExportScene(const std::string& name);
void SaveScene();

void CollectMeshes(OutModel& model, aiNode* pNode);
void CollectBones(OutModel& model);
void CollectAnimations(OutModel& model);
void CollectSceneModels(OutModel& model);

DirectX::XMFLOAT4X4 CovertMatrix(const aiMatrix4x4& transform);
uint32_t GetNumValidFaces(const aiMesh* pMesh);

int main(int argc, char** argv)
{
	Assimp::Importer importer;

	const auto flags =
		aiProcess_MakeLeftHanded |              // directx style.
		aiProcess_FlipUVs |                     // directx style.
		aiProcess_FlipWindingOrder |            // directx style.
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_OptimizeMeshes |              // reduce the number of meshes         
		aiProcess_ImproveCacheLocality |        // re-order triangles for better vertex cache locality.
		aiProcess_RemoveRedundantMaterials |    // remove redundant/unreferenced materials.
		aiProcess_LimitBoneWeights |
		aiProcess_SplitLargeMeshes |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_SortByPType |                 // splits meshes with more than one primitive type in homogeneous sub-meshes.
		aiProcess_FindDegenerates |             // convert degenerate primitives to proper lines or points.
		aiProcess_FindInvalidData |
		aiProcess_FindInstances |
		aiProcess_ValidateDataStructure |
		aiProcess_Debone;

	// Set normal smoothing angle
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
	// Set tangent smoothing angle
	importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 80.0f);
	// Maximum number of triangles in a mesh (before splitting)    
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 1000000);
	// Maximum number of vertices in a mesh (before splitting)    
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 1000000);
	// Remove points and lines.
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
	// Remove cameras and lights
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	// Enable progress tracking
	importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);

	std::string filePath = //"Assets/Models/dancing-stormtrooper/source/silly_dancing.fbx";
		"Assets/Models/Sponza-master/sponza.obj";

	if (const aiScene* pScene = importer.ReadFile(filePath, flags))
	{
		g_pScene = pScene;

		std::string outName = Dive::FileSystem::GetFileName(filePath) + ".mdl";
		ExportModel(outName, g_pScene->mRootNode);
	}

	return 0;
}

void ExportModel(const std::string& name, aiNode* pRoot)
{
	OutModel model;
	model.name = name;
	model.pRootNode = pRoot;

	CollectMeshes(model, pRoot);

	SaveModel(model);
}

void CollectMeshes(OutModel& model, aiNode* pNode)
{
	for (uint32_t i = 0; i < pNode->mNumMeshes; ++i)
	{
		aiMesh* pMesh = g_pScene->mMeshes[pNode->mMeshes[i]];
		for (size_t j = 0; j < model.meshes.size(); ++j)
		{
			if (pMesh == model.meshes[j])
			{
				break;
			}
		}

		model.meshNodes.emplace_back(pNode);
		model.meshes.emplace_back(pMesh);
		model.numVertices += pMesh->mNumVertices;
		model.numIndices += GetNumValidFaces(pMesh) * 3;
	}

	for (size_t i = 0; i < pNode->mNumChildren; ++i)
		CollectMeshes(model, pNode->mChildren[i]);
}

void SaveModel(OutModel& model)
{
	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "MeshNum" << YAML::Value << model.meshes.size();
	out << YAML::EndMap;

	for (size_t i = 0; i < model.meshes.size(); ++i)
	{
		const aiNode* pMeshNode = model.meshNodes[i];
		const aiMesh* pMesh = model.meshes[i];

		// node
		out << YAML::BeginMap;
		out << YAML::Key << "Transform";
		out << YAML::BeginMap;
		out << YAML::Key << "idx" << YAML::Value << i;
		out << YAML::Key << "name" << YAML::Value << pMeshNode->mName.C_Str();
		std::string parent = pMeshNode->mParent ? pMeshNode->mParent->mName.C_Str() : "root";
		out << YAML::Key << "parent" << YAML::Value << parent;

		DirectX::XMFLOAT4X4 transform = CovertMatrix(pMeshNode->mTransformation);
		DirectX::XMVECTOR pos, rot, scale;
		DirectX::XMMatrixDecompose(&scale, &rot, &pos, XMLoadFloat4x4(&transform));
		DirectX::XMFLOAT3 fPos, fScale;
		DirectX::XMFLOAT4 rotQuat;
		DirectX::XMStoreFloat3(&fPos, pos);
		DirectX::XMStoreFloat4(&rotQuat, rot);
		DirectX::XMStoreFloat3(&fScale, scale);
		
		out << YAML::Key << "pos" << YAML::Value << fPos;
		out << YAML::Key << "rot" << YAML::Value << rotQuat;
		out << YAML::Key << "scl" << YAML::Value << fScale;

		out << YAML::EndMap;
		out << YAML::EndMap;

		/*
		// mesh
		out << YAML::Key << "Mesh" << YAML::Value << YAML::BeginSeq;
		std::cout << "numVertices: " << pMesh->mNumVertices << std::endl;
		for (uint32_t i = 0; i < pMesh->mNumVertices; ++i)
		{
			auto pos = pMesh->mVertices[i];
			std::cout << "pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;

			auto normal = pMesh->mNormals[i];
			std::cout << "normal: " << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
		}
		std::cout << "numFaces: " << pMesh->mNumFaces << std::endl;
		for (uint32_t i = 0; i < pMesh->mNumFaces; ++i)
		{
			const aiFace& face = pMesh->mFaces[i];
			//const uint32_t index = i * 3;

			std::cout << "indices: " << face.mIndices[0] << ", " << face.mIndices[1] << ", " << face.mIndices[2] << std::endl;
		}
		*/
	}

	std::ofstream fout(model.name);
	fout << out.c_str();
}

DirectX::XMFLOAT4X4 CovertMatrix(const aiMatrix4x4& transform)
{
	return DirectX::XMFLOAT4X4(
		transform.a1, transform.b1, transform.c1, transform.d1,
		transform.a2, transform.b2, transform.c2, transform.d2,
		transform.a3, transform.b3, transform.c3, transform.d3,
		transform.a4, transform.b4, transform.c4, transform.d4
	);
}

uint32_t GetNumValidFaces(const aiMesh* pMesh)
{
	uint32_t ret = 0;

	for (uint32_t j = 0; j < pMesh->mNumFaces; ++j)
	{
		if (pMesh->mFaces[j].mNumIndices == 3)
			++ret;
	}

	return ret;
}