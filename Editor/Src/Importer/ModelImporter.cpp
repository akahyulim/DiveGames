#include "Dive.h"
#include "ModelImporter.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>


ModelImporter::ModelImporter(Dive::Scene * scene)
	: m_scene(scene)
{
}

ModelImporter::~ModelImporter()
{
}

// Model�̶�� ���ҽ� �������� ����ȭ �Ǿ�� �Ѵ�.
// Model�� ���� ����ȭ�� GameObject�� Mesh, Material, Animation ������ �����ȴ�.
bool ModelImporter::LoadFromFile(const std::string & filepath)
{
	// file Ȯ��
	if (!Dive::File::IsExistFile(filepath))
	{
		APP_ERROR("�ش� ������ �������� �ʽ��ϴ�. - {:s}", filepath);
		return false;
	}

	const auto importer_flags =
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

	// root scene load
	Assimp::Importer importer;
	const float maxNormalSmoothingAngle		= 80.0f;
	const float maxTangentSmoothingAngle	= 80.0f;
	const uint32_t triangleLimit			= 1000000;
	const uint32_t vertexLimit				= 1000000;
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, maxNormalSmoothingAngle);
	importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, maxTangentSmoothingAngle);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, triangleLimit);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, vertexLimit);
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);
	
	if (const aiScene* scene = importer.ReadFile(filepath, importer_flags))
	{
		// test
		APP_TRACE("num meshs: {:d}", scene->mNumMeshes);

		printModelData(scene->mRootNode);

		// �ִϸ��̼� ���� ���� Ȯ��

		// Scene���� ���� Root GameObject ����
		//auto rootGameObject = m_scene->CreateGameObject().get();

		// ��� ������ �Ľ�
		//m_aiScene = const_cast<aiScene*>(scene);
		//parseNode(scene->mRootNode, rootGameObject);
		// ��Ʈ���� Ŀ���� ������ �����ؾ� �Ѵ�.

		// �ִϸ��̼� �Ľ�: mesh�ʹ� �����̱� ������ �����Ѵ�.
		// �ٸ� bone�� mesh�� ����Ǿ� �ִ�?
	}
	else
	{
		APP_ERROR("{:s}", importer.GetErrorString());
		importer.FreeScene();
		return false;
	}

	importer.FreeScene();

	// GameObject ���������� �ϼ��� �� Prefab���� ����� ����

	return true;
}

// root node���� ��� child node
void ModelImporter::parseNode(const aiNode * node, Dive::GameObject* object, Dive::GameObject* parentObject)
{
	if (parentObject)
	{
		object->SetName(node->mName.C_Str());
	}

	object->GetTransform()->SetParent(parentObject ? parentObject->GetTransform() : nullptr);

	// node���� �ҷ��ͼ� transform set

	parseMesh(node, object);

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		auto newObject = m_scene->CreateGameObject().get();
		parseNode(node->mChildren[i], object, newObject);
	}
}

// �׷��� �� Mesh�� ���� Renderable�ϱ�?
// Animation�� Mesh�� �����ϴ���...
// ase, rigid�� ��� animation�� mesh�� ���Խ��״�. �ٸ� ����ȭ �������� ������ �޶���.
// �ݸ� skinned�� mesh�� 2���� Ȯ �پ� ���ȴ�...
// => test�غ��� node�� mesh�� ������ �ٸ���...
void ModelImporter::parseMesh(const aiNode * node, Dive::GameObject * object)
{
	// sub mesh�� �����ؾ� �Ѵ�.
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		auto targetObject = object;
		std::string name = node->mName.C_Str();
		auto mesh = m_aiScene->mMeshes[node->mMeshes[i]];

		// 2�� �̻��̸� ���� �� �����Ѵ�.
		if (node->mNumMeshes > 1)
		{
			targetObject = m_scene->CreateGameObject().get();
			name += "_" + std::to_string(i + 1);
			// �ڽĸ��� �ٸ� ����� ������? Mesh�� sub mesh�� ��������?
			targetObject->GetTransform()->SetParent(object->GetTransform());
		}

		object->SetName(name);
		// mesh�� ���� �ε��Ѵ�. �̴� �����ϱ� �����ε� �ϴ�.
		// ���� ����� �޽� �ε����� �̿��� ��Ʈ ��忡�� ã�ƾ� �Ѵ�.
		loadMesh(mesh, targetObject);
	}
}

void ModelImporter::loadMesh(aiMesh * mesh, Dive::GameObject * object)
{
	if (!mesh || !object)
	{
		APP_ERROR("");
		return;
	}

	const auto vertexCount = mesh->mNumVertices;
	const auto indexCount = mesh->mNumFaces * 3;

	APP_TRACE("num vertices: {0:d}, num indices: {1:d}", vertexCount, indexCount);

	// vertices
	std::vector<Dive::DIVE_VERTEX> vertices = std::vector<Dive::DIVE_VERTEX>(vertexCount);
	for (unsigned int i = 0; i < vertexCount; i++)
	{
		auto& vertex = vertices[i];

		// position
		const auto& pos = mesh->mVertices[i];
		vertex.positions[0] = pos.x;
		vertex.positions[1] = pos.y;
		vertex.positions[2] = pos.z;

		// color
		// color ���� �������� �� �ִ�.
		const size_t colorIndex = 0;
		if (mesh->HasVertexColors(colorIndex))
		{
			const auto& color = mesh->mColors[colorIndex][i];
		
			vertex.colors[0] = color.r;
			vertex.colors[1] = color.g;
			vertex.colors[2] = color.b;
			vertex.colors[3] = color.a;
		}


		// texCoord
		// ���� �� �����Ϸ��� 8������ �����ؾ� �Ѵ�...
		const size_t channel = 0;
		if (mesh->HasTextureCoords(channel))
		{
			const auto& texCoords = mesh->mTextureCoords[channel][i];

			vertex.texCoords[0] = texCoords.x;
			vertex.texCoords[1] = texCoords.y;
		}

		// normal
		if (mesh->mNormals)
		{
			const auto& normal = mesh->mNormals[i];

			vertex.normals[0] = normal.x;
			vertex.normals[1] = normal.y;
			vertex.normals[2] = normal.z;
		}
		
		// tangent
		if (mesh->mTangents)
		{
			const auto& tangent = mesh->mTangents[i];

			vertex.tangnts[0] = tangent.x;
			vertex.tangnts[1] = tangent.y;
			vertex.tangnts[2] = tangent.z;
		}

		// bone weights

		// bone index
	}
	
	// indices
	std::vector<unsigned int> indices = std::vector<unsigned int>(indexCount);
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const auto& face = mesh->mFaces[i];
		const auto index = (i * 3);

		indices[index + 0] = face.mIndices[0];
		indices[index + 1] = face.mIndices[1];
		indices[index + 2] = face.mIndices[2];
	}

	// material�� �ִٸ� base mateial�� �����ؾ� �ϳ�...?
	// aabb�� bone�� ���⿡�� �����Ѵ�.
	
	// ������ ��� �����ʹ� Renderable Mesh���� �����Ѵ�.
	// �׷��� Mesh�� Material�� Resource���� ���� ���ƾ� �Ѵ�.
	// ��, �����ͷ� �����ϸ�, ���� �� ������ �ٸ� ������� �ؾ� �Ѵ�.
	// �׷��ٸ� ResourceManager�� ���� ������ �� �����͸� �־� �����ϰ�
	// �̸� �����ͷ� �޾� MeshRenderer�� ����ϴ� ������ ���̴�.
}

void ModelImporter::printModelData(const aiNode * node)
{
	static int num = 0;
	APP_TRACE("{0:d} : {1:s}, {2:d}, {3:d}", num++, node->mName.C_Str(), node->mNumMeshes, node->mNumChildren);

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		printModelData(node->mChildren[i]);
	}
}
