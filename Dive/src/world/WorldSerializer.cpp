#include "stdafx.h"
#include "WorldSerializer.h"
#include "World.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "core/CoreDefs.h"
//#include "core/UINT64.h"

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
	static YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT3& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	static YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT4& quat)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << quat.x << quat.y << quat.z << quat.w << YAML::EndSeq;
		return out;
	}

	static YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMFLOAT4X4& mat)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq
			<< mat._11 << mat._12 << mat._13 << mat._14
			<< mat._21 << mat._22 << mat._23 << mat._24
			<< mat._31 << mat._32 << mat._33 << mat._34
			<< mat._41 << mat._42 << mat._43 << mat._44 << YAML::EndSeq;
		return out;
	}
	/*
	static void SerializeGameObject(YAML::Emitter& out, std::shared_ptr<GameObject> gameObject)
	{
		out << YAML::BeginMap;

		out << YAML::Key << "UINT64" << YAML::Value << gameObject->GetInstanceID();
		out << YAML::Key << "Name" << YAML::Value << gameObject->GetName();
		out << YAML::Key << "Tag" << YAML::Value << gameObject->GetTag();

		// Transform 저장
		auto tc = gameObject->GetTransform();
		out << YAML::Key << "Transform";
		out << YAML::BeginMap;
		out << YAML::Key << "Position" << YAML::Value << tc->GetLocalPosition();
		out << YAML::Key << "Rotation" << YAML::Value << tc->GetLocalRotationQuaternion();
		out << YAML::Key << "Scale" << YAML::Value << tc->GetLocalScale();
		out << YAML::EndMap;
		
		// 재귀적으로 자식 저장
		const auto& children = tc->GetChildren();
		if (!children.empty())
		{
			out << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
			for (auto child : children)
			{
				auto childGO = child->GetGameObject()->GetSharedPtr();
				SerializeGameObject(out, childGO);
			}
			out << YAML::EndSeq;
		}

		out << YAML::EndMap;
	}

	static void DeserializeGameObject(const YAML::Node& node, World* world, std::shared_ptr<Transform> parent)
	{
		if (!node["UINT64"] || !node["Name"])
			return;

		auto instanceID = node["UINT64"].as<UINT64>();
		auto name = node["Name"].as<std::string>();
		auto tag = node["Tag"] ? node["Tag"].as<std::string>() : "Untagged";

		auto gameObject = world->CreateGameObject(name);
		gameObject->SetTag(tag);

		auto tc = gameObject->GetTransform();
		if (parent)
			tc->SetParent(parent);
		
		// Transform 값 복원
		if (const auto& transformNode = node["Transform"])
		{
			if (transformNode["Position"])
				tc->SetLocalPosition(transformNode["Position"].as<DirectX::XMFLOAT3>());

			if (transformNode["Rotation"])
				tc->SetLocalRotationQuaternion(transformNode["Rotation"].as<DirectX::XMFLOAT4>());

			if (transformNode["Scale"])
				tc->SetLocalScale(transformNode["Scale"].as<DirectX::XMFLOAT3>());
		}

		// 자식 노드 재귀 호출
		if (node["Children"])
		{
			const auto& children = node["Children"];
			for (size_t i = 0; i < children.size(); ++i)
			{
				DeserializeGameObject(children[i], world, tc);
				tc->GetChildren()[i]->GetGameObject()->GetTransform()->SetSiblingIndex(static_cast<int>(i));
			}
		}
	}
	*/
	WorldSerializer::WorldSerializer(World* world)
		: m_World(world)
	{
	}
	/*
	void WorldSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "World" << YAML::Value << m_World->GetName();

		out << YAML::Key << "RootGameObjects" << YAML::Value << YAML::BeginSeq;
		for (auto& root : m_World->GetRootGameObjects())
			SerializeGameObject(out, root);
		out << YAML::EndSeq;
		
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool WorldSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try { data = YAML::LoadFile(filepath.string()); }
		catch (const YAML::ParserException& e)
		{
			DV_LOG(WorldSerializer, warn, "YAML 파싱 실패: {}", e.what());
			return false;
		}

		if (data["World"])
			m_World->SetName(data["World"].as<std::string>());

		if (data["RootGameObjects"])
		{
			for (const auto& root : data["RootGameObjects"])
				DeserializeGameObject(root, m_World, nullptr);
		}

		return true;
	}
	*/
	// =====================================================================================================================================

	std::unordered_map<const void*, uint64_t> objectToFileID;
	uint64_t currentID = 1;

	uint64_t GetFileID(const Object* obj)
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

		auto tf = root->GetTransform();
		for (auto& child : tf->GetChildren())
			CollectHierarchy(child->GetGameObject(), all);
	}

	void SerializeGameObjectsFlat(YAML::Emitter& out, const std::vector<GameObject*>& objects)
	{
		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;

		for (auto go : objects)
		{
			uint64_t goID = GetFileID(go);
			uint64_t tfID = GetFileID(go->GetTransform());

			out << YAML::BeginMap;

			out << YAML::Key << "fileID" << YAML::Value << goID;
			out << YAML::Key << "Name" << YAML::Value << go->GetName();
			out << YAML::Key << "Tag" << YAML::Value << go->GetTag();

			auto tf = go->GetTransform();
			out << YAML::Key << "Transform" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "fileID" << YAML::Value << tfID;
			out << YAML::Key << "Position" << YAML::Value << tf->GetLocalPosition();
			out << YAML::Key << "Rotation" << YAML::Value << tf->GetLocalRotationQuaternion();
			out << YAML::Key << "Scale" << YAML::Value << tf->GetLocalScale();

			if (auto parent = tf->GetParent())
				out << YAML::Key << "Parent" << YAML::Value << GetFileID(parent);

			out << YAML::EndMap; // Transform
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
	}

	std::unordered_map<uint64_t, Object*> fileIDToObject;

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
			auto goID = node["fileID"].as<uint64_t>();
			auto name = node["Name"].as<std::string>();
			auto tag = node["Tag"] ? node["Tag"].as<std::string>() : "Untagged";

			auto go = m_World->CreateGameObject(name);
			go->SetTag(tag);
			fileIDToObject[goID] = go;

			const auto& tfNode = node["Transform"];
			auto tfID = tfNode["fileID"].as<uint64_t>();

			auto tf = go->GetTransform(); // GameObject가 자동으로 Transform 생성한다고 가정
			tf->SetLocalPosition(tfNode["Position"].as<DirectX::XMFLOAT3>());
			tf->SetLocalRotationQuaternion(tfNode["Rotation"].as<DirectX::XMFLOAT4>());
			tf->SetLocalScale(tfNode["Scale"].as<DirectX::XMFLOAT3>());
			fileIDToObject[tfID] = tf;
		}

		// 2차 패스: Transform 부모 연결
		for (const auto& node : nodes)
		{
			const auto& tfNode = node["Transform"];
			if (!tfNode["fileID"]) continue;

			auto tfID = tfNode["fileID"].as<uint64_t>();
			auto tf = static_cast<Transform*>(fileIDToObject[tfID]);

			if (tfNode["Parent"])
			{
				uint64_t parentID = tfNode["Parent"].as<uint64_t>();
				auto parent = static_cast<Transform*>(fileIDToObject[parentID]);
				if (parent)
					tf->SetParent(parent);
			}
		}

		return true;
	}
}