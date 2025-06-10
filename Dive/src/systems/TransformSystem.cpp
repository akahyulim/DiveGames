#include "stdafx.h"
#include "TransformSystem.h"
#include "world/EntityManager.h"
#include "world/World.h"
#include "world/Components.h"

namespace Dive
{
	void ParentSystem::OnUpdate()
	{
		if (!m_World)
			return;

		auto& entityManager = m_World->GetEntityManager();
	
		// 부모의 갱신 처리
		{
			auto view = entityManager.GetEntityView<ActiveComponent, ParentComponent>();

			// 자식의 부모가 되는 경우 기존 부모의 Parent 제거
			for (auto [entity, activeCom, parentCom] : view.each())
			{
				if (!activeCom.IsActive)
					continue;

				auto& parent = parentCom.Parent;
				//if (parent == entt::null || !entityManager.Exists(parent))
				//	continue;

				entt::entity current = parent;
				while (entityManager.HasComponent<ParentComponent>(current))
				{
					current = entityManager.GetComponent<ParentComponent>(current).Parent;
					if (current == entity)
					{
						entityManager.RemoveComponent<ParentComponent>(entity);
						break;
					}
				}
			}
			
			for (auto [entity, activeCom, parentCom] : view.each())
			{
				if (!activeCom.IsActive)
					continue;

				auto& parent = parentCom.Parent;

				// 부모가 제거되었다면 자식도 제거
				if (!entityManager.Exists(parent))
				{
					entityManager.DeleteEntity(entity);
				}
				// 부모의 children에 자신이 포함되지 않았다면 추가
				else
				{
					auto& children = entityManager.HasComponent<Children>(parent) ?
						entityManager.GetComponent<Children>(parent).entities : entityManager.AddComponent<Children>(parent).entities;

					if (children.end() == std::find(children.begin(), children.end(), entity))
					{
						children.emplace_back(entity);
					}
				}
			}
		}

		// 자식의 갱신 처리
		{
			auto view = entityManager.GetEntityView<ActiveComponent, Children>();
			for (auto [entity, activeCom, childrenCom] : view.each())
			{
				if (!activeCom.IsActive)
					continue;

				auto& children = childrenCom.entities;
				entt::entity currentEntity = entity;
				children.erase(std::remove_if(children.begin(), children.end(), [&](entt::entity child) {
					return !entityManager.Exists(child) ||
						!entityManager.HasComponent<ParentComponent>(child) ||
						entityManager.GetComponent<ParentComponent>(child).Parent != currentEntity;}), 
					children.end());

				if (children.empty())
				{
					entityManager.RemoveComponent<Children>(entity);
				}
			}
		}
	}

	static void CalcuLocalToWorld(entt::entity entity, EntityManager& entityManager)
	{
		auto& localTransform = entityManager.GetComponent<LocalTransform>(entity);

		auto worldMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&localTransform.Scale))
			* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&localTransform.Rotation))
			* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&localTransform.Position));

		if (entityManager.HasComponent<ParentComponent>(entity))
		{
			auto parent = entityManager.GetComponent<ParentComponent>(entity).Parent;
			worldMatrix = DirectX::XMMatrixMultiply(
				DirectX::XMLoadFloat4x4(&entityManager.GetComponent<LocalToWorld>(parent).Value),
				worldMatrix
			);
		}

		auto& localToWorld = entityManager.HasComponent<LocalToWorld>(entity) ?
			entityManager.GetComponent<LocalToWorld>(entity) : entityManager.AddComponent<LocalToWorld>(entity);

		DirectX::XMStoreFloat4x4(&localToWorld.Value, worldMatrix);

		if (entityManager.HasComponent<Children>(entity))
		{
			auto& children = entityManager.GetComponent<Children>(entity).entities;
			for (auto child : children)
			{
				CalcuLocalToWorld(child, entityManager);
			}
		}
	}

	void LocalToWorldSystem::OnUpdate()
	{
		if (!m_World)
			return;

		auto& entityManager = m_World->GetEntityManager();
		for (auto rootEntity : entityManager.GetRootEntities())
		{
			if(entityManager.GetComponent<ActiveComponent>(rootEntity).IsActive)
				CalcuLocalToWorld(rootEntity, entityManager);
		}
	}
}