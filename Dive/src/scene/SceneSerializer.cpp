#include "stdafx.h"
#include "SceneSerializer.h"
#include "Scene.h"
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

	static void SerializeGameObject(YAML::Emitter& out, GameObject* pGameObject)
	{
		out << YAML::BeginMap;

		out << YAML::Key << "ID" << YAML::Value << pGameObject->GetID();
		out << YAML::Key << "Name" << YAML::Value << pGameObject->GetName();
		out << YAML::Key << "Tag" << YAML::Value << pGameObject->GetTag();

		out << YAML::Key << "Transform";
		out << YAML::BeginMap;

		auto pTransform = pGameObject->GetComponent<Transform>();
		out << YAML::Key << "Position" << YAML::Value << pTransform->GetPosition();
		out << YAML::Key << "Rotation" << YAML::Value << pTransform->GetRotationQuaternion();
		out << YAML::Key << "Scale" << YAML::Value << pTransform->GetScale();
		out << YAML::Key << "ParentID" << YAML::Value << pTransform->GetParentID();

		out << YAML::EndMap;

		if (pGameObject->HasComponent<Camera>())
		{
			out << YAML::Key << "Camera";
			out << YAML::BeginMap;

			// 데이터 저장

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	SceneSerializer::SceneSerializer(Scene* pScene)
		: m_pScene(pScene)
	{
	}

	void SceneSerializer::Serialize(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_pScene->m_Name;
		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;
		for (auto pGameObject : m_pScene->GetAllGameObjects())
		{
			SerializeGameObject(out, pGameObject);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();

		m_pScene->m_bDirty = false;
	}
	
	bool SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e)
		{
			DV_LOG(SceneSerializer, err, "월드 파일({:s})을 역직렬화에 실패하였습니다.", path.string());
			return false;
		}

		m_pScene->m_FilePath = path;

		if (!data["Scene"])
			return false;

		m_pScene->m_Name = data["Scene"].as<std::string>();

		auto gameObjects = data["GameObjects"];
		if (gameObjects)
		{
			for (auto gameObject : gameObjects)
			{
				auto pDeserializeObject = m_pScene->CreateGameObject();
				pDeserializeObject->SetID(gameObject["ID"].as<uint64_t>());
				pDeserializeObject->SetName(gameObject["Name"].as<std::string>());
				pDeserializeObject->SetTag(gameObject["Tag"].as<std::string>());

				auto transform = gameObject["Transform"];
				auto pTransform = pDeserializeObject->GetComponent<Transform>();
				pTransform->SetPosition(transform["Position"].as<DirectX::XMFLOAT3>());
				pTransform->SetRotation(transform["Rotation"].as<DirectX::XMFLOAT4>());
				pTransform->SetScale(transform["Scale"].as<DirectX::XMFLOAT3>());
				pTransform->SetParentID(transform["ParentID"].as<uint64_t>());

				auto camera = gameObject["Camera"];
				if (camera)
				{
					auto pCamera = pDeserializeObject->GetComponent<Camera>();
				}
			}

			// 계층구조 구성
			for (auto pGameObject : m_pScene->GetAllGameObjects())
			{
				auto pTransform = pGameObject->GetComponent<Transform>();
				const auto parentID = pTransform->GetParentID();
				if (parentID != 0)
				{
					pTransform->SetParent(m_pScene->GetGameObjectByID(parentID)->GetComponent<Transform>());
				}
			}
		}

		return true;
	}
}
