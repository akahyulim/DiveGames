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

// Model이라는 리소스 포멧으로 파일화 되어야 한다.
// Model은 계층 구조화된 GameObject와 Mesh, Material, Animation 등으로 구성된다.
bool ModelImporter::LoadFromFile(const std::string & filepath)
{
	// file 확인
	if (!Dive::File::IsExistFile(filepath))
	{
		APP_ERROR("해당 파일이 존재하지 않습니다. - {:s}", filepath);
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

		// 애니메이션 존재 여부 확인

		// Scene으로 부터 Root GameObject 생성
		//auto rootGameObject = m_scene->CreateGameObject().get();

		// 루드 노드부터 파싱
		//m_aiScene = const_cast<aiScene*>(scene);
		//parseNode(scene->mRootNode, rootGameObject);
		// 루트노드는 커스텀 네임을 전달해야 한다.

		// 애니메이션 파싱: mesh와는 별개이기 때문에 따로한다.
		// 다만 bone은 mesh에 저장되어 있다?
	}
	else
	{
		APP_ERROR("{:s}", importer.GetErrorString());
		importer.FreeScene();
		return false;
	}

	importer.FreeScene();

	// GameObject 계층구조를 완성한 후 Prefab으로 만들어 리턴

	return true;
}

// root node부터 모든 child node
void ModelImporter::parseNode(const aiNode * node, Dive::GameObject* object, Dive::GameObject* parentObject)
{
	if (parentObject)
	{
		object->SetName(node->mName.C_Str());
	}

	object->GetTransform()->SetParent(parentObject ? parentObject->GetTransform() : nullptr);

	// node에서 불러와서 transform set

	parseMesh(node, object);

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		auto newObject = m_scene->CreateGameObject().get();
		parseNode(node->mChildren[i], object, newObject);
	}
}

// 그런데 이 Mesh가 전부 Renderable일까?
// Animation도 Mesh가 존재하던데...
// ase, rigid의 경우 animation도 mesh로 포함시켰다. 다만 최적화 영향인지 개수가 달랐다.
// 반면 skinned는 mesh가 2개로 확 줄어 버렸다...
// => test해보니 node와 mesh의 개수가 다르다...
void ModelImporter::parseMesh(const aiNode * node, Dive::GameObject * object)
{
	// sub mesh를 생각해야 한다.
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		auto targetObject = object;
		std::string name = node->mName.C_Str();
		auto mesh = m_aiScene->mMeshes[node->mMeshes[i]];

		// 2개 이상이면 따로 또 생성한다.
		if (node->mNumMeshes > 1)
		{
			targetObject = m_scene->CreateGameObject().get();
			name += "_" + std::to_string(i + 1);
			// 자식말고 다른 방법이 없을까? Mesh가 sub mesh를 가지도록?
			targetObject->GetTransform()->SetParent(object->GetTransform());
		}

		object->SetName(name);
		// mesh를 따로 로드한다. 이는 복잡하기 때문인듯 하다.
		// 현재 노드의 메시 인덱스를 이용해 루트 노드에서 찾아야 한다.
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
		// color 역시 여러개일 수 있다.
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
		// 전부 다 적용하려면 8개까지 지원해야 한다...
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

	// material이 있다면 base mateial에 저장해야 하나...?
	// aabb와 bone도 여기에서 추출한다.
	
	// 추출한 모든 데이터는 Renderable Mesh에서 관리한다.
	// 그런데 Mesh와 Material은 Resource임을 잊지 말아야 한다.
	// 즉, 포인터로 저장하며, 생성 및 관리는 다른 방식으로 해야 한다.
	// 그렇다면 ResourceManager로 부터 생성한 후 데이터를 넣어 구축하고
	// 이를 포인터로 받아 MeshRenderer가 사용하는 형태일 것이다.
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
