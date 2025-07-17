#include "stdafx.h"
#include "WorldSerializer.h"
#include "World.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "rendering/StaticMesh.h"
#include "rendering/Material.h"
#include "rendering/MeshFactory.h"
#include "core/CoreDefs.h"
#include "resource/YamlHelper.h"

namespace Dive
{
	WorldSerializer::WorldSerializer(World* world)
		: m_World(world)
	{
	}
	
	std::unordered_map<const void*, UINT64> objectToFileID;
	UINT64 currentID = 1;

	UINT64 GetFileID(const Object* obj)
	{
		const void* key = static_cast<const void*>(obj);
		auto [it, inserted] = objectToFileID.emplace(key, currentID);
		if (inserted) currentID++;
		return it->second;
	}

	// 루트부터 재귀적으로 all에 저장
	// 순서를 유지하기 위해서이다.
	void CollectHierarchy(GameObject* root, std::vector<GameObject*>& all)
	{
		all.push_back(root);

		auto transform = root->GetTransform();
		for (auto& child : transform->GetChildren())
			CollectHierarchy(child->GetGameObject(), all);
	}

	static void SerializeGameObjectsFlat(YAML::Emitter& out, const std::vector<GameObject*>& objects)
	{
		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;

		for (auto go : objects)
		{
			UINT64 goID = GetFileID(go);

			out << YAML::BeginMap;

			out << YAML::Key << "fileID" << YAML::Value << goID;
			out << YAML::Key << "Name" << YAML::Value << go->GetName();
			out << YAML::Key << "Tag" << YAML::Value << go->GetTag();


			UINT64 transformID = GetFileID(go->GetTransform());
			auto transform = go->GetTransform();
			out << YAML::Key << "Transform" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "fileID" << YAML::Value << transformID;
			out << YAML::Key << "Position" << YAML::Value << transform->GetLocalPosition();
			out << YAML::Key << "Rotation" << YAML::Value << transform->GetLocalRotationQuaternion();
			out << YAML::Key << "Scale" << YAML::Value << transform->GetLocalScale();

			if (auto parent = transform->GetParent())
				out << YAML::Key << "Parent" << YAML::Value << GetFileID(parent);

			out << YAML::EndMap; // Transform

			if (go->HasComponent<Camera>())
			{
				UINT64 cameraID = GetFileID(go->GetComponent<Camera>());
				auto camera = go->GetComponent<Camera>();
				out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;
				out << YAML::Key << "fileID" << YAML::Value << cameraID;
				out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(camera->GetProjectionType());
				out << YAML::Key << "FieldOfView" << YAML::Value << camera->GetFieldOfView();
				out << YAML::Key << "NearClipPlane" << YAML::Value << camera->GetNearClipPlane();
				out << YAML::Key << "FarClipPlane" << YAML::Value << camera->GetFarClipPlane();
				out << YAML::Key << "ViewportLeft" << YAML::Value << camera->GetViewportLeft();
				out << YAML::Key << "ViewportTop" << YAML::Value << camera->GetViewportTop();
				out << YAML::Key << "ViewportRight" << YAML::Value << camera->GetViewportRight();
				out << YAML::Key << "ViewportBottom" << YAML::Value << camera->GetViewportBottom();
				out << YAML::Key << "BackgroundColor" << YAML::Value << camera->GetBackgroundColor();
				out << YAML::EndMap; // Camera
			}

			if (go->HasComponent<MeshRenderer>())
			{
				UINT64 staticMeshID = GetFileID(go->GetComponent<MeshRenderer>());
				auto meshRenderer = go->GetComponent<MeshRenderer>();
				auto staticMesh = meshRenderer->GetStaticMesh();
				auto material = meshRenderer->GetMaterial();
				out << YAML::Key << "MeshRenderer" << YAML::Value << YAML::BeginMap;
				out << YAML::Key << "fileID" << YAML::Value << staticMeshID;
				out << YAML::Key << "SourceType" << YAML::Value << static_cast<int>(staticMesh->GetSourceType());
				out << YAML::Key << "StaticMesh" << YAML::Value << staticMesh->GetName();
				out << YAML::Key << "Material" << YAML::Value << material->GetName();
				material->SaveToFile("Materials/" + material->GetName() + ".mat");
				out << YAML::EndMap; // MeshRenderer
			}

			out << YAML::EndMap; // GameObject
		}

		out << YAML::EndSeq;
	}

	void WorldSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "World" << YAML::Value << m_World->GetName();

		std::vector<GameObject*> flatList;
		for (auto& root : m_World->GetRootGameObjects())
			CollectHierarchy(root, flatList);

		SerializeGameObjectsFlat(out, flatList);

		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();

		DV_LOG(WorldSerializer, info, "월드 직렬화 완료 - {}", m_World->GetName());
	}

	std::unordered_map<UINT64, Object*> fileIDToObject;

	bool WorldSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try {
			data = YAML::LoadFile(filepath.string());
		}
		catch (const YAML::ParserException& e) {
			DV_LOG(WorldSerializer, warn, "YAML 파싱 실패: {}", e.what());
			return false;
		}

		if (data["World"])
			m_World->SetName(data["World"].as<std::string>());

		const auto& nodes = data["GameObjects"];
		if (!nodes || !nodes.IsSequence())
			return false;

		// 1차 패스: GameObject/Transform 생성 + fileID 등록
		for (const auto& node : nodes)
		{
			auto goID = node["fileID"].as<UINT64>();
			auto name = node["Name"].as<std::string>();
			auto tag = node["Tag"] ? node["Tag"].as<std::string>() : "Untagged";

			auto go = m_World->CreateGameObject(name);
			go->SetTag(tag);
			fileIDToObject[goID] = go;

			const auto& transformNode = node["Transform"];
			auto transformID = transformNode["fileID"].as<UINT64>();

			auto transform = go->GetTransform(); // GameObject가 자동으로 Transform 생성한다고 가정
			transform->SetLocalPosition(transformNode["Position"].as<DirectX::XMFLOAT3>());
			transform->SetLocalRotationQuaternion(transformNode["Rotation"].as<DirectX::XMFLOAT4>());
			transform->SetLocalScale(transformNode["Scale"].as<DirectX::XMFLOAT3>());
			fileIDToObject[transformID] = transform;

			if (const auto& cameraNode = node["Camera"])
			{
				auto cameraID = cameraNode["fileID"].as<UINT64>();
				auto camera = go->AddComponent<Camera>();
				camera->SetProjectionType(static_cast<eProjectionType>(cameraNode["ProjectionType"].as<int>()));
				camera->SetFieldOfView(cameraNode["FieldOfView"].as<float>());
				camera->SetNearClipPlane(cameraNode["NearClipPlane"].as<float>());
				camera->SetFarClipPlane(cameraNode["FarClipPlane"].as<float>());
				camera->SetViewportLeft(cameraNode["ViewportLeft"].as<float>());
				camera->SetViewportTop(cameraNode["ViewportTop"].as<float>());
				camera->SetViewportRight(cameraNode["ViewportRight"].as<float>());
				camera->SetViewportBottom(cameraNode["ViewportBottom"].as<float>());
				camera->SetBackgroundColor(cameraNode["BackgroundColor"].as<DirectX::XMFLOAT4>());
				fileIDToObject[cameraID] = camera;
			}

			if (const auto& meshRendererNode = node["MeshRenderer"])
			{
				auto staticMeshID = meshRendererNode["fileID"].as<UINT64>();
				auto meshRenderer = go->AddComponent<MeshRenderer>();
				std::string staticMeshName = meshRendererNode["StaticMesh"].as<std::string>();
				
				std::shared_ptr<StaticMesh> staticMesh = nullptr;
				switch (static_cast<eSourceType>(meshRendererNode["SourceType"].as<int>()))
				{
				case eSourceType::File:
					break;
				case eSourceType::Factory:
					staticMesh = MeshFactory::Create(staticMeshName);
					break;
				default:
					break;
				}
				meshRenderer->SetStaticMesh(staticMesh);
				
				// 이것두 default는 리소스 매니져에서, file은 직접?
				std::shared_ptr<Material> material = std::make_shared<Material>();
				material->LoadFromFile("Materials/" + meshRendererNode["Material"].as<std::string>() + ".mat");
				meshRenderer->SetMaterial(material);
				
				fileIDToObject[staticMeshID] = meshRenderer;
			}
		}

		// 2차 패스: Transform 부모 연결
		for (const auto& node : nodes)
		{
			const auto& transformNode = node["Transform"];
			if (!transformNode["fileID"]) continue;

			auto transformID = transformNode["fileID"].as<UINT64>();
			auto transform = static_cast<Transform*>(fileIDToObject[transformID]);

			if (transformNode["Parent"])
			{
				UINT64 parentID = transformNode["Parent"].as<UINT64>();
				auto parent = static_cast<Transform*>(fileIDToObject[parentID]);
				if (parent)
					transform->SetParent(parent);
			}
		}

		DV_LOG(WorldSerializer, info, "월드 역직렬화 완료 - {}", m_World->GetName());

		return true;
	}
}
