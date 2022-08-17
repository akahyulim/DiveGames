#pragma once
#include "Component.h"

namespace Dive
{
	class Context;

	class Transform : public Component
	{
		DIVE_OBJECT(Transform, Component)

	public:
		Transform(Context* pContext);
		~Transform();

		// �θ� Ʈ������ ���翩�� ����.
		bool HasParent() const { return m_pParent; }
		// �θ� Ʈ������ ������ ����.
		Transform* GetParent() const { return m_pParent; }
		// �θ� Ʈ������ ����.
		void SetParent(Transform* pParent);
		// ��Ʈ Ʈ������ ����.
		Transform* GetRoot();
		// �ڽ� ���翩�� ����.
		bool HasChild() const { return !m_Children.empty(); }
 		// �ڽ� �� ����.
		unsigned int GetChildCount() const { return static_cast<unsigned int>(m_Children.size()); }
		// ���޵� �ε����� �´� �ڽ� ������ ����.
		Transform* GetChild(unsigned int index) const;
		// ���� Ʈ�������� ���޹��� Ʈ�������� �ڽ� ���θ� ����.
		bool IsChildOf(Transform* pParent) const;
		// ��� �ڽ��� ����.
		void DetachChildren();
		// �ڽ� ���� ����.
		std::vector<Transform*> GetChildren() const;

	private:
	private:
		// �θ� Ʈ������ ������.
		Transform* m_pParent;
		// �ڽ� Ʈ������.
		std::vector<Transform*> m_Children;
	};
}
