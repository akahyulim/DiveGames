#pragma once
#include "Core/Object.h"

namespace Dive
{
	// Ŭ���� ���漱��
	class GameObject;
	class Transform;

	// ���ӿ�����Ʈ�� ÷�εǴ� ��� ������Ʈ�� ���̽� Ŭ�����Դϴ�.
	class Component
	{
	public:
		// �⺻ �������Դϴ�. �ݵ�� ������Ʈ�� ÷�ε� ���ӿ�����Ʈ�� �����ؾ� �մϴ�.
		Component(GameObject* pGameObject);
		virtual ~Component() = default;

		virtual void Update() {}
		
		// ������Ʈ�� �ĺ� ID�� �����մϴ�.
		uint64_t GetID() const { return m_ID; }
		// ������Ʈ�� �ĺ� ID�� �����մϴ�.
		void SetID(uint64_t id) { m_ID = id; }
		
		// ������Ʈ�� ÷�ε� ���ӿ�����Ʈ�� �̸��� �����մϴ�.
		std::string GetName() const;
		
		// ������Ʈ�� ÷�ε� ���ӿ�����Ʈ�� �����͸� �����մϴ�.
		GameObject* GetGameObject() const { return m_pGameObject; }
		
		// ���ӿ�����Ʈ�� ÷�ε� Ʈ������ ������Ʈ �����͸� �����մϴ�.
		Transform* GetTransform() const { return m_pTransform; }

	protected:
		// ������Ʈ�� ÷�εǴ� ���ӿ�����Ʈ�Դϴ�. 
		GameObject* m_pGameObject;
		// ������Ʈ �ĺ��� ���� ID�Դϴ�.
		uint64_t m_ID;
		// ���ӿ�����Ʈ�� ÷�ε� Ʈ������ ������Ʈ�� �������Դϴ�.
		Transform* m_pTransform;
	};
}