#pragma once
#include "Core/Object.h"

namespace Dive
{
	// 클래스 전방선언
	class GameObject;
	class Transform;

	// 게임오브젝트에 첨부되는 모든 컴포넌트의 베이스 클래스입니다.
	class Component
	{
	public:
		// 기본 생성자입니다. 반드시 컴포넌트가 첨부될 게임오브젝트를 전달해야 합니다.
		Component(GameObject* pGameObject);
		virtual ~Component() = default;

		virtual void Update() {}
		
		// 컴포넌트의 식별 ID를 리턴합니다.
		uint64_t GetID() const { return m_ID; }
		// 컴포넌트의 식별 ID를 설정합니다.
		void SetID(uint64_t id) { m_ID = id; }
		
		// 컴포넌트가 첨부된 게임오브젝트의 이름을 리턴합니다.
		std::string GetName() const;
		
		// 컴포넌트가 첨부된 게임오브젝트의 포인터를 리턴합니다.
		GameObject* GetGameObject() const { return m_pGameObject; }
		
		// 게임오브젝트에 첨부된 트랜스폼 컴포넌트 포인터를 리턴합니다.
		Transform* GetTransform() const { return m_pTransform; }

	protected:
		// 컴포넌트가 첨부되는 게임오브젝트입니다. 
		GameObject* m_pGameObject;
		// 컴포넌트 식별을 위한 ID입니다.
		uint64_t m_ID;
		// 게임오브젝트에 첨부된 트랜스폼 컴포넌트의 포인터입니다.
		Transform* m_pTransform;
	};
}