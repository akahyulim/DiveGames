#include "divepch.h"
#include "Serializer.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component/Transform.h"
#include "Component/SpriteRenderable.h"
#include "Component/MeshRenderable.h"

#include <yaml-cpp/yaml.h>

namespace Dive
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static void SerializeGameObject(YAML::Emitter& out, GameObject* pGameObject)
	{
		out << YAML::Key << "GameObject" << YAML::BeginMap;
		out << YAML::Key << "m_InstanceID" << YAML::Value << pGameObject->GetInstanceID();
		out << YAML::Key << "m_Name" << YAML::Value << pGameObject->GetName();
		out << YAML::Key << "m_bActive" << YAML::Value << (int)pGameObject->IsActive();
		out << YAML::EndMap;

		// components
		{
			// Transform
			if (pGameObject->HasComponent<Transform>())
			{
				auto pTransform = pGameObject->GetComponent<Transform>();

				out << YAML::Key << "Transform" << YAML::BeginMap;
				out << YAML::Key << "m_LocalRotation" << YAML::Value << pTransform->GetLocalRotation();
				out << YAML::Key << "m_LocalPosition" << YAML::Value << pTransform->GetLocalPosition();
				out << YAML::Key << "m_LocalScale" << YAML::Value << pTransform->GetLocalScale();
				out << YAML::Key << "m_Parent" << YAML::Value << (unsigned long long)(pTransform->HasParent() ? pTransform->GetParent()->GetInstanceID() : 0);
				out << YAML::EndMap;
			}

			// SpriteRenderable
			if (pGameObject->HasComponent<SpriteRenderable>())
			{
				out << YAML::Key << "SpriteRenderable" << YAML::BeginMap;
				out << YAML::EndMap;
			}

			// MeshRenderable
			if (pGameObject->HasComponent<MeshRenderable>())
			{
				out << YAML::Key << "MeshRenderable" << YAML::BeginMap;
				out << YAML::EndMap;
			}
		}
	}

	Serializer::Serializer(Scene* pScene)
		: m_pScene(pScene)
	{
		DV_ASSERT(pScene != nullptr);
	}

	// 디렉토리만 받는 게 낫다.
	void Serializer::Serialize(const std::string& dir)
	{
		if (!m_pScene)
		{
			DV_CORE_WARN("Scene이 존재하지 않아 Serialize에 실패하였습니다.");
			return;
		}

		// 파일 경로는 dir + scene name + extension
		std::string filepath;
		filepath = dir + '/' + m_pScene->GetName() + ".scene";

		YAML::Emitter out;
		out << YAML::BeginMap;

		// scene
		out << YAML::Key << "Scene" << YAML::BeginMap;
		out << YAML::Key << "m_Name" << YAML::Value << m_pScene->GetName();
		out << YAML::EndMap;

		// game object
		//out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;
		for (auto pGameObject : m_pScene->GetGameObjects())
		{
			//out << YAML::BeginMap;
			SerializeGameObject(out, pGameObject);
			//out << YAML::EndMap;
		}
		//out << YAML::EndSeq;

		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}
	
	// Map과 Seq의 정확한 의미를 알아야 한다.
	// 그리고 결국 사람이 읽고 해석한 것과 같은 처리를 할 것임을 인지하자.
	bool Serializer::Deserialize(const std::string& filepath)
	{
		if (!m_pScene)
		{
			DV_CORE_WARN("Scene이 존재하지 않아 Deserialize에 실패하였습니다.");
			return false;
		}

		if (!std::filesystem::exists(filepath))
		{
			DV_CORE_WARN("{:}가 존재하지 않습니다.", filepath);
			return false;
		}

		auto node = YAML::LoadFile(filepath);

		// scene
		auto scene = node["Scene"];
		auto name = scene["m_Name"].as<std::string>();
		m_pScene->SetName(name);

		auto gameObjects = node["GameObject"];
		DV_CORE_INFO("num gameObjects: {:d}", gameObjects.size());
		if (gameObjects)
		{
			for (auto gameObject : gameObjects)
			{
				//auto id = gameObject["m_InstanceID"].as<unsigned long long>();
				//auto name = gameObject["m_Name"].as<std::string>();
				//auto active = gameObject["m_bActive"].as<bool>();

				// 여기에선 안된다.
				// 이유는 GameObject에 포함된 것이 아니기에
				// 특정할 수 없기 때문인 듯 하다.
				//auto transform = gameObject["Transform"];
				//if (transform)
				{
					// xmfloat를 사용하려면 함수를 추가해야 한다.
				//	auto parent = transform["m_Parent"].as<unsigned long long>();
				//	DV_CORE_INFO("ParentID: {:d}", parent);
				}
			}
		}

		// 이것두 안된다. 위와 같은 이유인듯...?
		auto transforms = node["Transform"];
		if (transforms)
		{
			for (auto transform : transforms)
			{
				auto parent = transform["m_Parent"].as<unsigned long long>();
				DV_CORE_INFO("ParentID: {:d}", parent);
			}
		}

		/*
		// game object
		// 상위 노드로 묶어야 한다...
		// 아니면 동일한 이름의 노드를 구분할 수 없다...
		auto gameObjects = node["GameObjects"];
		if (gameObjects)
		{
			DV_CORE_INFO("num gameObjects: {:d}", gameObjects.size());

			for (auto gameObject : gameObjects)
			{

			//	auto id = gameObject["m_InstanceID"].as<unsigned long long>();
			//	auto name = gameObject["m_Name"].as<std::string>();
			//	auto active = gameObject["m_bActive"].as<bool>();

			//	auto pCreatedGameObject = m_pScene->CreateGameObject(id, name);
			//	pCreatedGameObject->SetActive(active);


				//auto name = gameObject["m_Name"].as<std::string>();
				//DV_CORE_INFO("name: {:s}", name);

				// 이건 된다...
				auto transform = gameObject["Transform"];
				if (transform)
				{
					// xmfloat를 사용하려면 함수를 추가해야 한다.
					auto parent = transform["m_Parent"].as<unsigned long long>();
					DV_CORE_INFO("ParentID: {:d}", parent);
				}
			}
		}
		*/
		return true;
	}
}