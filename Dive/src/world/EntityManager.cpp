#include "stdafx.h"
#include "EntityManager.h"
#include "core/CoreDefs.h"
#include "core/EventDispatcher.h"
#include "rendering/MeshFactory.h"

namespace Dive
{
	void EntityManager::Clear()
	{
		for (auto& [instanceID, entity] : m_Entities)
		{
			m_Registry.destroy(entity);
		}
		m_Entities.clear();
	}

	entt::entity EntityManager::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(InstanceID(), name);
	}

	entt::entity EntityManager::CreateEntityWithUUID(InstanceID instanceID, const std::string& name)
	{
		entt::entity entity = m_Registry.create();
		AddComponent<IDComponent>(entity).ID = instanceID;
		AddComponent<NameComponent>(entity).Name = name;
		m_Entities[instanceID] = entity;

		return entity;
	}

	void EntityManager::DeleteEntity(entt::entity entity)
	{
		DV_ASSERT(EntityManager, Exists(entity));

		auto id = GetComponent<IDComponent>(entity).ID;

		DV_LOG(EntityManager, debug, "Delete {0}: entity = {1}, id = {2}", GetComponent<NameComponent>(entity).Name, (uint32_t)entity, (UINT64)id);

		m_Entities.erase(id);
		m_Registry.destroy(entity);
	}
	
	bool EntityManager::Exists(entt::entity entity) 
	{
		return m_Registry.valid(entity);
	}

	entt::entity EntityManager::GetEntity(InstanceID instanceID) 
	{
		DV_ASSERT(EntityManager, m_Entities.find(instanceID) != m_Entities.end());

		return m_Entities.at(instanceID);
	}

	std::vector<entt::entity> EntityManager::GetEntities(const std::string& name)
	{
		std::vector<entt::entity> entities;

		auto view = m_Registry.view<NameComponent>();
		for (auto [entity, nameCom] : view.each())
		{
			if (nameCom.Name == name)
				entities.emplace_back(entity);
		}

		return entities;
	}

	std::vector<entt::entity> EntityManager::GetAllEntities()
	{
		std::vector<entt::entity> entities;
		entities.reserve(m_Entities.size());

		for (auto& [instanceID, entity] : m_Entities)
		{
			entities.emplace_back(entity);
		}

		return entities;
	}

	// 정렬되지 않은 상태로 벡터에 저장된다.
	std::vector<entt::entity> EntityManager::GetRootEntities()
	{
		std::vector<entt::entity> roots;

		for (auto& [instanceID, entity] : m_Entities)
		{
			if (!HasComponent<ParentComponent>(entity))
			{
				roots.emplace_back(entity);
			}
		}

		return roots;
	}

	InstanceID EntityManager::GetInstanceID(entt::entity entity)
	{
		DV_ASSERT(EntityManager, Exists(entity));

		if(!HasComponent<IDComponent>(entity))
			return 0;

		return GetComponent<IDComponent>(entity).ID;
	}
	
	std::string EntityManager::GetName(entt::entity entity)
	{
		DV_ASSERT(EntityManager, Exists(entity));

		if(!HasComponent<NameComponent>(entity))
			return {};

		return GetComponent<NameComponent>(entity).Name;
	}

	entt::entity EntityManager::CreatePrimitive(ePrimitiveType type)
	{
		entt::entity entity = CreateEntityWithComponents<ActiveComponent, LocalTransform, RenderMesh>();
		switch (type)
		{
		case ePrimitiveType::Triangle:
			GetComponent<NameComponent>(entity).Name = "Triangle";
			GetComponent<RenderMesh>(entity).mesh = MeshFactory::CreateTriangle();
			// material도 standard로 추가
			break;
		case ePrimitiveType::Quad:
			GetComponent<NameComponent>(entity).Name = "Quad";
			GetComponent<RenderMesh>(entity).mesh = MeshFactory::CreateQuad();
			// material도 standard로 추가
			break;
		case ePrimitiveType::Plane:
			GetComponent<NameComponent>(entity).Name = "Plane";
			GetComponent<RenderMesh>(entity).mesh = MeshFactory::CreatePlane();
			// material도 standard로 추가
			break;
		case ePrimitiveType::Cube:
			GetComponent<NameComponent>(entity).Name = "Cube";
			GetComponent<RenderMesh>(entity).mesh = MeshFactory::CreateCube();
			// material도 standard로 추가
			break;
		case ePrimitiveType::Sphere:
			GetComponent<NameComponent>(entity).Name = "Sphere";
			GetComponent<RenderMesh>(entity).mesh = MeshFactory::CreateSphere();
			// material도 standard로 추가
			break;
		case ePrimitiveType::Capsule:
			GetComponent<NameComponent>(entity).Name = "Capsule";
			GetComponent<RenderMesh>(entity).mesh = MeshFactory::CreateCapsule();
			// material도 standard로 추가
			break;

		default:
			DV_LOG(EntityManager, err, "지원하지 않는 primitive type: {0}", static_cast<int>(type));
			return entt::null;
		}

		return entity;
	}
}