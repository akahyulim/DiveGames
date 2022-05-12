#include "divepch.h"
#include "Scene.h"
#include "Base/Base.h"
#include "Component/Transform.h"
#include "Component/SpriteRenderable.h"
#include "Component/MeshRenderable.h"
#include "Renderer/LegacyMaterial.h"
#include "Renderer/SpriteMaterial.h"
#include "Renderer/Model.h"
#include "Renderer/Graphics/Texture.h"
#include "Resource/ResourceManager.h"
#include "Events/EngineEvents.h"
#include "Helper/YamlHelper.h"

namespace Dive
{
	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
		if (m_Name.empty())
			m_Name = "Untitled";

		SUBSCRIBE_EVENT(eEventType::ModifyGameObject, EVENT_HANDLER(OnModifyComponents));
	}

	Scene::~Scene()
	{
		Clear();
	}

	void Scene::Update(float elapsedTime)
	{
		if (m_bDirty)
		{
			auto it = m_GameObjects.begin();
			for (it; it != m_GameObjects.end();)
			{
				if ((*it)->IsRemoveTarget())
				{
					// 아직까진 별 문제가 없으나
					// 이렇게 하나씩 제거하면 안될 것 같다.
					DV_DELETE(*it);
					it = m_GameObjects.erase(it);
				}
				else
					it++;
			}

			FIRE_EVENT(SceneResolveEvent(&m_GameObjects));

			m_bDirty = false;
		}

		// 현재 GameObject와 Component에는 Update가 없다.
		// 유니티처럼 한다면 아래의 update script에서 처리하는 모양이 나올 듯?

		// update script

		// set camera

		// begin / end scene
	}

	void Scene::Clear()
	{
		if (!m_GameObjects.empty())
		{
			for (auto pGameObject : m_GameObjects)
			{
				DV_DELETE(pGameObject);
			}
			m_GameObjects.clear();
			m_GameObjects.shrink_to_fit();
		}
	}

	void Scene::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;

		// scene
		out << YAML::BeginMap;
		out << YAML::Key << "Scene";
		out << YAML::BeginMap;
		out << YAML::Key << "m_Name" << YAML::Value << GetName();
		out << YAML::EndMap;
		out << YAML::EndMap;

		// game objects
		for (auto pGameObject : GetGameObjects())
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
		if (!GetComponents(eComponentType::Transform).empty())
		{
			auto pTransforms = GetComponents(eComponentType::Transform);

			for (auto it = pTransforms.begin(); it != pTransforms.end(); it++)
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
		if (!GetComponents(eComponentType::SpriteRenderable).empty())
		{
			auto pSpriteRenderables = GetComponents(eComponentType::SpriteRenderable);

			for (auto it = pSpriteRenderables.begin(); it != pSpriteRenderables.end(); it++)
			{
				auto pSpriteRenderable = dynamic_cast<SpriteRenderable*>(*it);
				auto pMaterial = pSpriteRenderable->GetMaterial();	// 이건 virtual이 아니네... 바꿔야 하나...?

				out << YAML::BeginMap;
				out << YAML::Key << "SpriteRenderable";
				out << YAML::BeginMap;
				out << YAML::Key << "m_InstanceID" << YAML::Value << pSpriteRenderable->GetInstanceID();
				out << YAML::Key << "m_GameObject" << YAML::Value << pSpriteRenderable->GetGameObject()->GetInstanceID();
				out << YAML::Key << "m_bEnabled" << YAML::Value << (int)pSpriteRenderable->IsEnabled();
				// 이하 Material 부분은 Material에서 저장해야 한다.
				out << YAML::Key << "m_Sprite" << YAML::Value << pMaterial->GetSprite()->GetName();
				out << YAML::Key << "m_Color" << YAML::Value << pMaterial->GetColor();
				out << YAML::Key << "m_FlipX" << YAML::Value << (int)pMaterial->IsFlipX();
				out << YAML::Key << "m_FlipY" << YAML::Value << (int)pMaterial->IsFlipY();
				out << YAML::EndMap;
				out << YAML::EndMap;
			}
		}

		// mesh renderable component
		if (!GetComponents(eComponentType::MeshRenderable).empty())
		{
			auto pMeshRenderables = GetComponents(eComponentType::MeshRenderable);

			for (auto it = pMeshRenderables.begin(); it != pMeshRenderables.end(); it++)
			{
				auto pMeshRenderable = dynamic_cast<MeshRenderable*>(*it);
				auto pMaterial = dynamic_cast<LegacyMaterial*>(pMeshRenderable->GetMaterial());	// 현재 Renderalbe의 virtual 함수가 아니다.

				out << YAML::BeginMap;
				out << YAML::Key << "MeshRenderable";
				out << YAML::BeginMap;
				out << YAML::Key << "m_InstanceID" << YAML::Value << pMeshRenderable->GetInstanceID();
				out << YAML::Key << "m_GameObject" << YAML::Value << pMeshRenderable->GetGameObject()->GetInstanceID();
				out << YAML::Key << "m_bEnabled" << YAML::Value << (int)pMeshRenderable->IsEnabled();
				out << YAML::Key << "m_GeometryName" << YAML::Value << pMeshRenderable->GetGeometryName();
				out << YAML::Key << "m_VertexOffset" << YAML::Value << pMeshRenderable->VertexOffset();
				out << YAML::Key << "m_VertexCount" << YAML::Value << pMeshRenderable->VertexCount();
				out << YAML::Key << "m_IndexOffset" << YAML::Value << pMeshRenderable->IndexOffset();
				out << YAML::Key << "m_IndexCount" << YAML::Value << pMeshRenderable->IndexCount();
				out << YAML::Key << "m_Model" << YAML::Value << pMeshRenderable->GetModel()->GetName();	// 이름은 중복될 수 있다.
				out << YAML::Key << "m_AlbedoColor" << YAML::Value << pMaterial->GetAlbedoColor();
				out << YAML::EndMap;
				out << YAML::EndMap;
			}
		}

		// 결국 material과 같은 resource는 분리하여 저장해야 한다.

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool Scene::Deserialize(const std::string& filepath)
	{
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
				auto name = scene["m_Name"].as<std::string>();

				SetName(name);
			}

			if (node["GameObject"])
			{
				// create game object
				auto gameObject = node["GameObject"];
				auto instanceID = gameObject["m_InstanceID"].as<unsigned long long>();
				auto name = gameObject["m_Name"].as<std::string>();
				auto bActive = gameObject["m_bActive"].as<int>();

				auto pGameObject = CreateGameObject(name, instanceID);
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

				auto instanceID = transformNode["m_InstanceID"].as<unsigned long long>();
				auto gameObjectID = transformNode["m_GameObject"].as<unsigned long long>();
				auto pOwner = GetGameObject(gameObjectID);
				DV_ASSERT(pOwner->GetComponent<Transform>()->GetInstanceID() == instanceID);
				auto pTransform = pOwner->GetComponent<Transform>();
				DV_ASSERT(pTransform->GetInstanceID() == instanceID);

				pTransform->SetLocalRotation(transformNode["m_LocalRotation"].as<DirectX::XMFLOAT4>());
				pTransform->SetLocalPosition(transformNode["m_LocalPosition"].as<DirectX::XMFLOAT3>());
				pTransform->SetLocalScale(transformNode["m_LocalScale"].as<DirectX::XMFLOAT3>());
				auto parentID = transformNode["m_Parent"].as<unsigned long long>();
				if (parentID != 0)
				{
					auto pParent = dynamic_cast<Transform*>(GetComponent(eComponentType::Transform, parentID));
					pTransform->SetParent(pParent);
				}
			}

			// 유니티는 GameObject가 구체 id : file id 순으로 저장하고
			// Renderer는 그냥 Renderer로 저장된다.
			// 구체 Renderer의 데이터는 Material에서 분할되는 것 같다.
			// 여러모로 봤을 때 Scene에서 1차 가공을 한 후 yaml에서 파일화 하는 듯 보인다.
			// 그게 아니라면 GameObject가 Component의 file id를 알 수 없다.
			if (node["SpriteRenderable"])
			{
				auto meshRenderableNode = node["SpriteRenderable"];

				auto instanceID = meshRenderableNode["m_InstanceID"].as<unsigned long long>();
				auto gameObjectID = meshRenderableNode["m_GameObject"].as<unsigned long long>();
				auto pOwner = GetGameObject(gameObjectID);
				DV_ASSERT(pOwner->GetComponent<SpriteRenderable>()->GetInstanceID() == instanceID);
				auto pSpriteRenderable = pOwner->GetComponent<SpriteRenderable>();
				DV_ASSERT(pSpriteRenderable->GetInstanceID() == instanceID);

				auto bEnabled = meshRenderableNode["m_bEnabled"].as<int>();

				auto pMaterial = pSpriteRenderable->GetMaterial();

				// sprite는 이름이나 ID로 저장하여야 한다.
				// 이 곳에서는 이를 ResourceManager로 전달하여 포인터를 얻어온 다음
				// SetSprite()로 넣는게 맞다.
				// 그런데 찾아보니 유니티의 경우 Material과 Mesh가 파일로 관리되는 듯 하다.
				// yaml에는 해당 데이터를 file id, guid, type으로 저장했다.
				// 관련문서에 의하면 file id는 yaml에서의 file id인 듯 하나 material과 mesh는 예제에는 없다.
				// file id가 결국 yaml 상에서 인덱스를 의미한다 해도, 이를 직접 구현하기는 쉽지 않다...
				// 생각해보니 Material이 resource라면 하나만 존재하고 다른 곳에서 참조하는 형태가 맞긴 하다.
				auto sprite = meshRenderableNode["m_Sprite"].as<std::string>();
				auto color = meshRenderableNode["m_Color"].as<DirectX::XMFLOAT4>();
				auto bFlipX = meshRenderableNode["m_FlipX"].as<int>();
				auto bFlipY = meshRenderableNode["m_FlipY"].as<int>();

				pSpriteRenderable->SetEnable(bEnabled);
				// 이렇게 하면 버퍼 생성을 건너띈다.
				//pMaterial->SetSprite(ResourceManager::GetInstance().GetResource<Texture2D>(sprite));
				pSpriteRenderable->SetSprite(ResourceManager::GetInstance().GetResource<Texture2D>(sprite));
				pMaterial->SetColor(color);
				pMaterial->SetFlipX(bFlipX);
				pMaterial->SetFlipY(bFlipY);

			}

			if (node["MeshRenderable"])
			{
				auto meshRenderableNode = node["MeshRenderable"];

				auto instanceID = meshRenderableNode["m_InstanceID"].as<unsigned long long>();
				auto gameObjectID = meshRenderableNode["m_GameObject"].as<unsigned long long>();
				auto pOwner = GetGameObject(gameObjectID);
				DV_ASSERT(pOwner->GetComponent<MeshRenderable>()->GetInstanceID() == instanceID);
				auto pMeshRenderable = pOwner->GetComponent<MeshRenderable>();
				DV_ASSERT(pMeshRenderable->GetInstanceID() == instanceID);

				auto bEnabled = meshRenderableNode["m_bEnabled"].as<int>();
				auto geometryName = meshRenderableNode["m_GeometryName"].as<std::string>();
				auto vertexOffset = meshRenderableNode["m_VertexOffset"].as<unsigned int>();
				auto vertexCount = meshRenderableNode["m_VertexCount"].as<unsigned int>();
				auto indexOffset = meshRenderableNode["m_IndexOffset"].as<unsigned int>();
				auto indexCount = meshRenderableNode["m_IndexCount"].as<unsigned int>();
				auto model = meshRenderableNode["m_Model"].as<std::string>();
				auto albedoColor = meshRenderableNode["m_AlbedoColor"].as<DirectX::XMFLOAT4>();

				// 일단 직접 생성
				// 그런데 Material 역시 Resource다...?
				pMeshRenderable->SetMaterail(new LegacyMaterial);
				auto pMaterial = pMeshRenderable->GetMaterial();
				auto pModel = ResourceManager::GetInstance().GetResource<Model>(model);

				pMeshRenderable->SetEnable(bEnabled);
				if (pModel)
					pMeshRenderable->SetGeometry(geometryName, vertexOffset, vertexCount, indexOffset, indexCount, pModel);
				pMaterial->SetAlbedoColor(albedoColor);
			}
		}

		return true;
	}

	GameObject* Scene::CreateGameObject(const std::string& name, unsigned long long id)
	{
		// 유니티에 따르자면 Transform이 기본 추가인데...
		// Deserialize 과정에서 ID를 저장하기 위해 직접 추가하도록 만들어 놓았다.
		return m_GameObjects.emplace_back(new GameObject(this, name, id));
	}

	// 자식까지 모두 제거
	void Scene::RemoveGameObject(unsigned long long id)
	{
		if (m_GameObjects.empty())
			return;

		if (!m_bDirty)
			m_bDirty = true;

		for (auto pGameObject : m_GameObjects)
		{
			if (pGameObject->GetInstanceID() == id)
			{
				auto pTransform = pGameObject->GetComponent<Transform>();
				if (pTransform)
				{
					if (pTransform->HasChildren())
					{
						for (auto pChild : pTransform->GetChildren())
						{
							RemoveGameObject(pChild->GetGameObject()->GetInstanceID());
						}
					}

					if (pTransform->HasParent())
					{
						pTransform->SetParent(nullptr);
					}
				}

				pGameObject->MarkRemoveTarget();
			}
		}
	}

	void Scene::RemoveGameObject(GameObject* pTarget)
	{
		if (!pTarget)
			return;

		RemoveGameObject(pTarget->GetInstanceID());
	}

	GameObject* Scene::GetGameObject(unsigned long long id)
	{
		for (auto pGameObject : m_GameObjects)
		{
			if (pGameObject->GetInstanceID() == id)
				return pGameObject;
		}

		return nullptr;
	}

	std::vector<GameObject*> Scene::GetRoots()
	{
		std::vector<GameObject*> roots;

		for (auto& pGameObject : m_GameObjects)
		{
			auto pTransform = pGameObject->GetComponent<Transform>();
			if( pTransform )
			{
				if (!pTransform->HasParent())
				{
					roots.emplace_back(pGameObject);
				}
			}
			else
			{
				roots.emplace_back(pGameObject);
			}
		}

		return roots;
	}

	std::vector<Component*> Scene::GetComponents(eComponentType type)
	{
		std::vector<Component*> components;
		if (!m_Components[type].empty())
			components = m_Components[type];

		return components;
	}

	Component* Scene::GetComponent(eComponentType type, unsigned long long id)
	{
		auto components = GetComponents(type);
		if (!components.empty())
		{
			for (auto pComponent : components)
			{
				if (pComponent->GetInstanceID() == id)
					return pComponent;
			}
		}

		return nullptr;
	}

	// 참조중인 Component container update
	void Scene::OnModifyComponents(const Event& e)
	{
		m_bDirty = true;

		auto pComponent = dynamic_cast<const ModifyGameObjectEvent&>(e).GetComponent();
		auto type = pComponent->GetType();
		if (!m_Components[type].empty())
		{
			auto it = m_Components[type].begin();
			for (it; it != m_Components[type].end(); it++)
			{
				if ((*it)->GetInstanceID() == pComponent->GetInstanceID())
				{
					m_Components[type].erase(it);
					return;
				}
			}
		}

		m_Components[type].push_back(const_cast<Component*>(pComponent));
	}

}