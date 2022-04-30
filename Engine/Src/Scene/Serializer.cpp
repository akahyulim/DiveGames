#include "divepch.h"
#include "Serializer.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Component/SpriteRenderable.h"
#include "Component/MeshRenderable.h"
#include "Renderer/Material.h"
#include "Renderer/SpriteMaterial.h"
#include "Renderer/LegacyMaterial.h"

#include <yaml-cpp/yaml.h>

// �ᱹ ��� ���� Scene���� ���� ��´�.
// ���� �� �κе� Scene�� �ѱ�� �� ��︰��.
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

	void Serializer::Serialize(const std::string& dir)
	{
		if (!m_pScene)
		{
			DV_CORE_WARN("Scene�� �������� �ʾ� Serialize�� �����Ͽ����ϴ�.");
			return;
		}

		// ���� ��δ� dir + scene name + extension
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

		// transform component
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

		// sprite renderable component
		if (!m_pScene->GetComponents(eComponentType::SpriteRenderable).empty())
		{
			auto pSpriteRenderables = m_pScene->GetComponents(eComponentType::SpriteRenderable);

			for (auto it = pSpriteRenderables.begin(); it != pSpriteRenderables.end(); it++)
			{
				auto pSpriteRenderable = dynamic_cast<SpriteRenderable*>(*it);
				auto pMaterial = pSpriteRenderable->GetMaterial();	// �̰� virtual�� �ƴϳ�... �ٲ�� �ϳ�...?

				out << YAML::BeginMap;
				out << YAML::Key << "SpriteRenderable";
				out << YAML::BeginMap;
				out << YAML::Key << "m_InstanceID" << YAML::Value << pSpriteRenderable->GetInstanceID();
				out << YAML::Key << "m_GameObject" << YAML::Value << pSpriteRenderable->GetGameObject()->GetInstanceID();
				out << YAML::Key << "m_bEnabled" << YAML::Value << (int)pSpriteRenderable->IsEnabled();
				// ���� Material �κ��� Material���� �����ؾ� �Ѵ�.
				out << YAML::Key << "m_Sprite" << YAML::Value << "None";
				out << YAML::Key << "m_Color" << YAML::Value << pMaterial->GetColor();
				out << YAML::Key << "m_FlipX" << YAML::Value << (int)pMaterial->IsFlipX();
				out << YAML::Key << "m_FlipY" << YAML::Value << (int)pMaterial->IsFlipY();
				out << YAML::EndMap;
				out << YAML::EndMap;
			}
		}

		// mesh renderable component
		if (!m_pScene->GetComponents(eComponentType::MeshRenderable).empty())
		{
			auto pMeshRenderables = m_pScene->GetComponents(eComponentType::MeshRenderable);

			for (auto it = pMeshRenderables.begin(); it != pMeshRenderables.end(); it++)
			{
				auto pMeshRenderable = dynamic_cast<MeshRenderable*>(*it);
				auto pMaterial = dynamic_cast<LegacyMaterial*>(pMeshRenderable->GetMaterial());	// ���� Renderalbe�� virtual �Լ��� �ƴϴ�.

				out << YAML::BeginMap;
				out << YAML::Key << "MeshRenderable";
				out << YAML::BeginMap;
				out << YAML::Key << "m_InstanceID" << YAML::Value << pMeshRenderable->GetInstanceID();
				out << YAML::Key << "m_GameObject" << YAML::Value << pMeshRenderable->GetGameObject()->GetInstanceID();
				out << YAML::Key << "m_bEnabled" << YAML::Value << (int)pMeshRenderable->IsEnabled();
				out << YAML::Key << "m_AlbedoColor" << YAML::Value << pMaterial->GetAlbedoColor();
				out << YAML::EndMap;
				out << YAML::EndMap;
			}
		}

		// �ᱹ material�� ���� resource�� �и��Ͽ� �����ؾ� �Ѵ�.

		std::ofstream fout(filepath);
		fout << out.c_str();
	}
	
	bool Serializer::Deserialize(const std::string& filepath)
	{
		if (!m_pScene)
		{
			DV_CORE_WARN("Scene�� �������� �ʾ� Deserialize�� �����Ͽ����ϴ�.");
			return false;
		}

		auto nodes = YAML::LoadAllFromFile(filepath);
		if (nodes.empty())
		{
			DV_CORE_WARN("{:}�� �������� �ʽ��ϴ�.", filepath);
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
					for (int i = 0; i < (int)eComponentType::Count; i++)
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

				auto instanceID		= transformNode["m_InstanceID"].as<unsigned long long>();
				auto gameObjectID	= transformNode["m_GameObject"].as<unsigned long long>();
				auto pOwner			= m_pScene->GetGameObject(gameObjectID);
				DV_ASSERT(pOwner->GetComponent<Transform>()->GetInstanceID() == instanceID);
				auto pTransform		= pOwner->GetComponent<Transform>();
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

			// ����Ƽ�� GameObject�� ��ü id : file id ������ �����ϰ�
			// Renderer�� �׳� Renderer�� ����ȴ�.
			// ��ü Renderer�� �����ʹ� Material���� ���ҵǴ� �� ����.
			// ������� ���� �� Scene���� 1�� ������ �� �� yaml���� ����ȭ �ϴ� �� ���δ�.
			// �װ� �ƴ϶�� GameObject�� Component�� file id�� �� �� ����.
			if (node["SpriteRenderable"])
			{
				auto meshRenderableNode = node["SpriteRenderable"];

				auto instanceID		= meshRenderableNode["m_InstanceID"].as<unsigned long long>();
				auto gameObjectID	= meshRenderableNode["m_GameObject"].as<unsigned long long>();
				auto pOwner			= m_pScene->GetGameObject(gameObjectID);
				DV_ASSERT(pOwner->GetComponent<SpriteRenderable>()->GetInstanceID() == instanceID);
				auto pSpriteRenderable = pOwner->GetComponent<SpriteRenderable>();
				DV_ASSERT(pSpriteRenderable->GetInstanceID() == instanceID);

				auto bEnabled = meshRenderableNode["m_bEnabled"].as<int>();

				auto pMaterial = pSpriteRenderable->GetMaterial();
				
				// sprite�� �̸��̳� ID�� �����Ͽ��� �Ѵ�.
				// �� �������� �̸� ResourceManager�� �����Ͽ� �����͸� ���� ����
				// SetSprite()�� �ִ°� �´�.
				// �׷��� ã�ƺ��� ����Ƽ�� ��� Material�� Mesh�� ���Ϸ� �����Ǵ� �� �ϴ�.
				// yaml���� �ش� �����͸� file id, guid, type���� �����ߴ�.
				// ���ù����� ���ϸ� file id�� yaml������ file id�� �� �ϳ� material�� mesh�� �������� ����.
				// file id�� �ᱹ yaml �󿡼� �ε����� �ǹ��Ѵ� �ص�, �̸� ���� �����ϱ�� ���� �ʴ�...
				// �����غ��� Material�� resource��� �ϳ��� �����ϰ� �ٸ� ������ �����ϴ� ���°� �±� �ϴ�.
				auto sprite = meshRenderableNode["m_Sprite"].as<std::string>();
				auto color = meshRenderableNode["m_Color"].as<DirectX::XMFLOAT4>();
				auto bFlipX = meshRenderableNode["m_FlipX"].as<int>();
				auto bFlipY = meshRenderableNode["m_FlipY"].as<int>();

				pSpriteRenderable->SetEnable(bEnabled);
				pMaterial->SetColor(color);
				pMaterial->SetFlipX(bFlipX);
				pMaterial->SetFlipY(bFlipY);
			}

			if (node["MeshRenderable"])
			{
				auto meshRenderableNode = node["MeshRenderable"];
				
				auto instanceID		= meshRenderableNode["m_InstanceID"].as<unsigned long long>();
				auto gameObjectID	= meshRenderableNode["m_GameObject"].as<unsigned long long>();
				auto pOwner			= m_pScene->GetGameObject(gameObjectID);
				DV_ASSERT(pOwner->GetComponent<MeshRenderable>()->GetInstanceID() == instanceID);
				auto pMeshRenderable = pOwner->GetComponent<MeshRenderable>();
				DV_ASSERT(pMeshRenderable->GetInstanceID() == instanceID);

				auto bEnabled		= meshRenderableNode["m_bEnabled"].as<int>();
				auto albedoColor	= meshRenderableNode["m_AlbedoColor"].as<DirectX::XMFLOAT4>();


				pMeshRenderable->SetEnable(bEnabled);
			}
		}
		
		return true;
	}
}