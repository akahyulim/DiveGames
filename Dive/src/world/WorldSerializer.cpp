#include "stdafx.h"
#include "WorldSerializer.h"
#include "World.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "core/CoreDefs.h"

#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<DirectX::XMFLOAT3>
	{
		static Node encode(const DirectX::XMFLOAT3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, DirectX::XMFLOAT3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();

			return true;
		}
	};

	template<>
	struct convert<DirectX::XMFLOAT4>
	{
		static Node encode(const DirectX::XMFLOAT4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, DirectX::XMFLOAT4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();

			return true;
		}
	};

	template<>
	struct convert<DirectX::XMFLOAT4X4>
	{
		static Node encode(const DirectX::XMFLOAT4X4& rhs)
		{
			Node node;
			node.push_back(rhs._11);	node.push_back(rhs._12);	node.push_back(rhs._13);	node.push_back(rhs._14);
			node.push_back(rhs._21);	node.push_back(rhs._22);	node.push_back(rhs._23);	node.push_back(rhs._24);
			node.push_back(rhs._31);	node.push_back(rhs._32);	node.push_back(rhs._33);	node.push_back(rhs._34);
			node.push_back(rhs._41);	node.push_back(rhs._42);	node.push_back(rhs._43);	node.push_back(rhs._44);
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, DirectX::XMFLOAT4X4& rhs)
		{
			if (!node.IsSequence() || node.size() != 15)
				return false;

			rhs._11 = node[0].as<float>();	rhs._12 = node[1].as<float>();	rhs._13 = node[2].as<float>();	rhs._14 = node[3].as<float>();
			rhs._21 = node[4].as<float>();	rhs._22 = node[5].as<float>();	rhs._23 = node[6].as<float>();	rhs._24 = node[7].as<float>();
			rhs._31 = node[8].as<float>();	rhs._32 = node[9].as<float>();	rhs._33 = node[10].as<float>();	rhs._34 = node[11].as<float>();
			rhs._41 = node[12].as<float>();	rhs._42 = node[13].as<float>();	rhs._43 = node[14].as<float>();	rhs._44 = node[15].as<float>();

			return true;
		}
	};
}

namespace Dive
{
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

	YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT4X4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq 
			<< v._11 << v._12 << v._13 << v._14
			<< v._21 << v._22 << v._23 << v._24 
			<< v._31 << v._32 << v._33 << v._34 
			<< v._41 << v._42 << v._43 << v._44 << YAML::EndSeq;
		return out;
	}

	static void SerializeGameObject(YAML::Emitter& out, GameObject* gameObject)
	{
		out << YAML::BeginMap;

		out << YAML::Key << "ID" << YAML::Value << gameObject->GetID();
		out << YAML::Key << "Name" << YAML::Value << gameObject->GetName();
		out << YAML::Key << "Tag" << YAML::Value << gameObject->GetTag();

		out << YAML::Key << "Transform";
		out << YAML::BeginMap;

		auto transform = gameObject->GetComponent<Transform>();
		out << YAML::Key << "Position" << YAML::Value << transform->GetPosition();
		out << YAML::Key << "Rotation" << YAML::Value << transform->GetRotationQuaternion();
		out << YAML::Key << "Scale" << YAML::Value << transform->GetScale();
		out << YAML::Key << "ParentID" << YAML::Value << transform->GetParentID();

		out << YAML::EndMap;

		if (gameObject->HasComponent<Camera>())
		{
			out << YAML::Key << "Camera";
			out << YAML::BeginMap;

			// 데이터 저장

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	WorldSerializer::WorldSerializer(World* world)
		: m_World(world)
	{
	}

	void WorldSerializer::Serialize(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "World" << YAML::Value << m_World->m_Name;
		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;
		for (auto gameObject : m_World->GetAllGameObjects())
		{
			SerializeGameObject(out, gameObject);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();

		m_World->m_IsDirty = false;
	}
	
	bool WorldSerializer::Deserialize(const std::filesystem::path& path)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::Exception& e)
		{
			DV_LOG(WorldSerializer, err, "월드 파일 역직렬화 실패: {}", e.what());
			return false;
		}

		m_World->m_FilePath = path;

		if (!data["World"])
			return false;

		m_World->m_Name = data["World"].as<std::string>();

		auto gameObjectsNode = data["GameObjects"];
		if (gameObjectsNode)
		{
			for (auto gameObjectNode : gameObjectsNode)
			{
				auto id = gameObjectNode["ID"].as<UINT64>();
				auto name = gameObjectNode["Name"].as<std::string>();
				auto gameObject = m_World->CreateGameObject(id, name);
				gameObject->SetTag(gameObjectNode["Tag"].as<std::string>());

				auto transformNode = gameObjectNode["Transform"];
				if (transformNode)
				{
					auto transform = gameObject->AddComponent<Transform>();
					transform->SetPosition(transformNode["Position"].as<DirectX::XMFLOAT3>());
					transform->SetRotation(transformNode["Rotation"].as<DirectX::XMFLOAT4>());
					transform->SetScale(transformNode["Scale"].as<DirectX::XMFLOAT3>());
					transform->SetParentID(transformNode["ParentID"].as<UINT64>());
				}

				auto cameraNode = gameObjectNode["Camera"];
				if (cameraNode)
				{
					auto camera = gameObject->AddComponent<Camera>();
				}
			}

			// 계층구조 구성
			for (auto gameObject : m_World->GetAllGameObjects())
			{
				auto transform = gameObject->GetComponent<Transform>();
				const auto parentID = transform->GetParentID();
				if (parentID != 0)
				{
					transform->SetParent(m_World->GetGameObjectByID(parentID)->GetComponent<Transform>());
				}
			}
		}

		return true;
	}
}
