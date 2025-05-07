#include "stdafx.h"
#include "WorldSerializer.h"
#include "World.h"
#include "Entity.h"
#include "Components.h"
#include "transforms/Transforms.h"
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
	YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT3& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT4& quat)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << quat.x << quat.y << quat.z << quat.w << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT4X4& mat)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq
			<< mat._11 << mat._12 << mat._13 << mat._14
			<< mat._21 << mat._22 << mat._23 << mat._24
			<< mat._31 << mat._32 << mat._33 << mat._34
			<< mat._41 << mat._42 << mat._43 << mat._44 << YAML::EndSeq;
		return out;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<NameComponent>())
		{
			out << YAML::Key << "NameComponent";
			out << YAML::BeginMap;

			auto& name = entity.GetComponent<NameComponent>().Name;
			out << YAML::Key << "Name" << YAML::Value << name;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<ActiveComponent>())
		{
			out << YAML::Key << "ActiveComponent";
			out << YAML::BeginMap;

			auto& active = entity.GetComponent<ActiveComponent>().IsActive;
			out << YAML::Key << "IsActive" << YAML::Value << active;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << tc.Position;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
			out << YAML::EndMap;
		}

		if (Transforms::HasParent(entity))
		{
			out << YAML::Key << "Hierarchy";
			out << YAML::BeginMap;

			auto parentID = Transforms::GetParent(entity).GetUUID();
			out << YAML::Key << "ParentID" << YAML::Value << parentID;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;

		// 역직렬화 과정에서 생성 순서가 중요해 이렇게 해놓았다.
		// 차라리 역직렬화 과정에서 생성과 계층구조 설정 부분을 분리하는 게 낫지 않을까?
		if (Transforms::HasChildren(entity))
		{
			for (auto& child : Transforms::GetChildren(entity))
			{
				SerializeEntity(out, child);
			}
		}
	}

	WorldSerializer::WorldSerializer(const std::shared_ptr<World> world)
		: m_World(world)
	{
	}

	void WorldSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "World" << YAML::Value << m_World->m_Name;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		
		for(auto& entity : Transforms::GetRootNodes(m_World->GetAllEntities()))
		{
			SerializeEntity(out, entity);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}
	
	bool WorldSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException& e)
		{
			DV_LOG(WorldSerializer, warn, "YAML 파일 파싱 실패: '{0}'\n {1}", filepath.string().c_str(), e.what());
			return false;
		}

		if (!data["World"])
			return false;

		std::string worldName = data["World"].as<std::string>();
		m_World->m_Name = worldName;
		DV_LOG(WorldSerializer, trace, "Deserializing World '{0}'", worldName);

		auto gameObjects = data["Entities"];
		if (gameObjects)
		{
			for (auto entity : gameObjects)
			{
				UINT64 uuid = entity["Entity"].as<UINT64>();

				std::string name;
				auto nameComponent = entity["NameComponent"];
				if (nameComponent)
					name = nameComponent["Name"].as<std::string>();

				DV_LOG(WorldSerializer, trace, "Deserializerd Entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedGameObject = m_World->CreateEntityWithUUID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& tc = deserializedGameObject.GetComponent<TransformComponent>();
					tc.Position = transformComponent["Position"].as<DirectX::XMFLOAT3>();
					tc.Rotation = transformComponent["Rotation"].as<DirectX::XMFLOAT4>();
					tc.Position = transformComponent["Scale"].as<DirectX::XMFLOAT3>();
				}

				auto hierarchy = entity["Hierarchy"];
				if (hierarchy)
				{
					UINT64 parentID = hierarchy["ParentID"].as<UINT64>();
					Transforms::SetParent(deserializedGameObject, m_World->GetEntityByUUID(parentID));
				}
			}
		}

		return true;
	}
}