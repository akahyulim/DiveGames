#include "divepch.h"
#include "Serializer.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Component/SpriteRenderable.h"
#include "Component/MeshRenderable.h"

#include <yaml-cpp/yaml.h>

template<>
struct YAML::convert<DirectX::XMFLOAT2>
{
	static YAML::Node encode(const DirectX::XMFLOAT2& rhs)
	{
		YAML::Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.SetStyle(YAML::EmitterStyle::Flow);

		return node;
	}

	static bool decode(const YAML::Node& node, DirectX::XMFLOAT2& rhs)
	{
		if (!node.IsSequence() || node.size() != 2)
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();

		return true;
	}
};

template<>
struct YAML::convert<DirectX::XMFLOAT3>
{
	static YAML::Node encode(const DirectX::XMFLOAT3& rhs)
	{
		YAML::Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.SetStyle(YAML::EmitterStyle::Flow);

		return node;
	}

	static bool decode(const YAML::Node& node, DirectX::XMFLOAT3& rhs)
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
struct YAML::convert<DirectX::XMFLOAT4>
{
	static YAML::Node encode(const DirectX::XMFLOAT4& rhs)
	{
		YAML::Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		node.SetStyle(YAML::EmitterStyle::Flow);

		return node;
	}

	static bool decode(const YAML::Node& node, DirectX::XMFLOAT4& rhs)
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

	Serializer::Serializer(Scene* pScene)
		: m_pScene(pScene)
	{
		DV_ASSERT(pScene != nullptr);
	}

	// 유니티의 yaml 문서 형태처럼 GameObject, Component를 각각 구분하는 편이 나아보인다.
	// 다만 이 경우 Compnent의 직접 생성이 가능해야 하며 개별 Object의 InstanceID를 참조하여 GameObject를 구성하며
	// 이를 위해 Scene에서 GameObject와 Component를 따로 관리해야 한다. 즉, 복잡해진다...
	// 그리고 ID의 제한 범위도 염두해야 한다.
	// 하지만 유니티에선 Component를 직접 생성할 수 없다고 명시해 놓았다.
	// 예제 코드에서 변수 생성은 c#의 특징(일종의 참조 변수)인 듯 하다.
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

		// game objects
		for (auto pGameObject : m_pScene->GetGameObjects())
		{
			out << YAML::BeginMap;
			out << YAML::Key << "GameObject";
			out << YAML::BeginMap;
			out << YAML::Key << "m_InstanceID" << YAML::Value << pGameObject->GetInstanceID();
			out << YAML::Key << "m_Name" << YAML::Value << pGameObject->GetName();
			out << YAML::Key << "m_bActive" << YAML::Value << (int)pGameObject->IsActive();

			if (!pGameObject->GetComponents().empty())
			{
				out << YAML::Key << "m_Component" << YAML::Value << YAML::BeginSeq;

				for (int i = 0; i != (int)eComponentType::Count; i++)
				{
					auto pComponent = pGameObject->GetComponent(static_cast<eComponentType>(i));
					if (pComponent)
					{
						out << YAML::BeginMap;
						out << YAML::Key << std::to_string((int)pComponent->GetType()) << YAML::Value << pComponent->GetInstanceID();
						out << YAML::EndMap;
					}
				}

				out << YAML::EndSeq;
			}

			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		// components
		if(!m_pScene->GetComponents(eComponentType::Transform).empty())
		{
			auto pTransforms = m_pScene->GetComponents(eComponentType::Transform);

			for(auto it = pTransforms.begin(); it != pTransforms.end(); it++)
			{
				auto pTransform = dynamic_cast<Transform*>(*it);
				
				out << YAML::BeginMap;
				out << YAML::Key << "Transform";
				out << YAML::BeginMap;
				out << YAML::Key << "m_InstanceID" << YAML::Value << pTransform->GetInstanceID();
				out << YAML::Key << "m_GameObject" << YAML::Value << pTransform->GetGameObject()->GetInstanceID();
				out << YAML::Key << "m_LocalRotation" << YAML::Value << pTransform->GetLocalRotation();
				out << YAML::Key << "m_LocalPosition" << YAML::Value << pTransform->GetLocalPosition();
				out << YAML::Key << "m_LocalScale" << YAML::Value << pTransform->GetLocalScale();
				out << YAML::Key << "m_Parent" << YAML::Value << (unsigned long long)(pTransform->HasParent() ? pTransform->GetParent()->GetInstanceID() : 0);
				out << YAML::EndMap;
				out << YAML::EndMap;
			}
		}

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
				auto scene	= node["Scene"];
				auto name	= scene["m_Name"].as<std::string>();

				m_pScene->SetName(name);
			}

			if (node["GameObject"])
			{
				// create game object
				auto gameObject = node["GameObject"];
				auto instanceID = gameObject["m_InstanceID"].as<unsigned long long>();
				auto name		= gameObject["m_Name"].as<std::string>();
				auto bActive	= gameObject["m_bActive"].as<int>();

				auto pGameObject = m_pScene->CreateGameObject(name, instanceID);
				pGameObject->SetActive(bActive);

				// create components
				auto components = gameObject["m_Component"];
				for (auto component : components)
				{
					int i = 0;
					// 다른 Component는 Buffer 등이 생성되지 않아 오류가 발생한다.
					//for (int i = 0; i < (int)eComponentType::Count; i++)
					{
						if (component[std::to_string(i)])
						{
							pGameObject->AddComponent(static_cast<eComponentType>(i), component[std::to_string(i)].as<unsigned long long>());
						}
					}
				}
			}

			if (node["Transform"])
			{
				auto transformNode = node["Transform"];

				auto instanceID = transformNode["m_InstanceID"].as<unsigned long long>();
				auto gameObjectID = transformNode["m_GameObject"].as<unsigned long long>();
				auto pOwner = m_pScene->GetGameObject(gameObjectID);
				DV_ASSERT(pOwner->GetComponent<Transform>()->GetInstanceID() == instanceID);
				auto pTransform = pOwner->GetComponent<Transform>();
				DV_ASSERT(pTransform->GetInstanceID() == instanceID);

				pTransform->SetLocalRotation(transformNode["m_LocalRotation"].as<DirectX::XMFLOAT4>());
				pTransform->SetLocalPosition(transformNode["m_LocalPosition"].as<DirectX::XMFLOAT3>());
				pTransform->SetLocalScale(transformNode["m_LocalScale"].as<DirectX::XMFLOAT3>());
				auto parentID = transformNode["m_Parent"].as<unsigned long long>();
				if (parentID != 0)
				{
					auto pParent = dynamic_cast<Transform*>(m_pScene->GetComponent(eComponentType::Transform, parentID));
					pTransform->SetParent(pParent);
				}
			}
		}
		
		return true;
	}
}