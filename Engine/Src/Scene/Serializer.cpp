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
		out << YAML::BeginMap;
		out << YAML::Key << "GameObject";
		out << YAML::BeginMap;
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

				//out << YAML::BeginMap;
				out << YAML::Key << "Transform";
				out << YAML::BeginMap;
				out << YAML::Key << "m_LocalRotation" << YAML::Value << pTransform->GetLocalRotation();
				out << YAML::Key << "m_LocalPosition" << YAML::Value << pTransform->GetLocalPosition();
				out << YAML::Key << "m_LocalScale" << YAML::Value << pTransform->GetLocalScale();
				out << YAML::Key << "m_Parent" << YAML::Value << (unsigned long long)(pTransform->HasParent() ? pTransform->GetParent()->GetInstanceID() : 0);
				out << YAML::EndMap;
				//out << YAML::EndMap;
			}

			// SpriteRenderable
			if (pGameObject->HasComponent<SpriteRenderable>())
			{
				//out << YAML::BeginMap;
				out << YAML::Key << "SpriteRenderable";
				out << YAML::BeginMap;
				out << YAML::EndMap;
				//out << YAML::EndMap;
			}

			// MeshRenderable
			if (pGameObject->HasComponent<MeshRenderable>())
			{
				//out << YAML::BeginMap;
				out << YAML::Key << "MeshRenderable";
				out << YAML::BeginMap;
				out << YAML::EndMap;
				//out << YAML::EndMap;
			}
		}

		YAML::EndMap;
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

		// scene
		out << YAML::BeginMap;
		out << YAML::Key << "Scene";
		out << YAML::BeginMap;
		out << YAML::Key << "m_Name" << YAML::Value << m_pScene->GetName();
		out << YAML::EndMap;
		out << YAML::EndMap;

		// game object
		for (auto pGameObject : m_pScene->GetGameObjects())
		{
			SerializeGameObject(out, pGameObject);
		}

		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}
	
	bool Serializer::Deserialize(const std::string& filepath)
	{
		if (!m_pScene)
		{
			DV_CORE_WARN("Scene이 존재하지 않아 Deserialize에 실패하였습니다.");
			return false;
		}

		auto nodes = YAML::LoadAllFromFile(filepath);
		if (nodes.empty())
		{
			DV_CORE_WARN("{:}가 존재하지 않습니다.", filepath);
			return false;
		}

		for (auto node : nodes)
		{
			if (node["Scene"])
			{
				auto scene = node["Scene"];

				DV_CORE_INFO("Sene name: {:s}", scene["m_Name"].as<std::string>());
			}

			if (node["GameObject"])
			{
				auto gameObject = node["GameObject"];

				DV_CORE_INFO("GameObject Info: {0:d}, {1:s}, {2:d}",
					gameObject["m_InstanceID"].as<unsigned long long>(),
					gameObject["m_Name"].as<std::string>(),
					gameObject["m_bActive"].as<int>());
			}
		}
		
		return true;
	}
}