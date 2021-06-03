#pragma once
#include "Component.h"
#include "GraphicsInclude.h"
#include <string>
#include <vector>

namespace Dive
{
	class Transform : public Component
	{
	public:
		Transform();
		~Transform();

		// �������̵� �Լ�
		void Update(float deltaTime) override;

		// ��ȯ �Լ�

		// �������� �Լ�
		// ������ �����Ѵ�.
		Transform* GetRoot();
		// �θ� ���� ���θ� �����Ѵ�.
		bool HasParent() const { return (m_pParent != nullptr); }
		// �θ� �����͸� �����Ѵ�.
		Transform* GetParent() const { return m_pParent; }
		void SetParent(Transform* pParent);
		// �θ� �����Ѵٸ� �����Ѵ�.
		void BecomeOrphan();
		// �ڽ��� ���� �����Ѵ�.
		unsigned int GetChildCount() const { return static_cast<unsigned int>(m_children.size()); }
		// �̸����� �ڽ��� ã�� �����Ѵ�.
		Transform* Find(const std::string& name) const;
		// �ε����� �ڽ��� ã�� �����Ѵ�.
		Transform* GetChild(unsigned int index) const;
		// ��� �ڽ��� �����Ѵ�.
		std::vector<Transform*> GetChildren() { return m_children; }
		// �ڽ��� pParent�� �ڽ����� Ȯ���Ѵ�.
		bool IsChildOf(const Transform* pParent) const;
		// ��� �ڽ��� �����. �ڽĵ��� �������� �ʰ� �θ� ���ְ� ���� �� ����Ѵ�.
		void DetachChildren();
		// �ڽĵ�κ��� �θ� �ڽ� ���踦 �籸���Ѵ�. ��������� ȣ���Ѵ�.
		void AcquireChidren();

	private:

	private:
		// transform

		// heirarchy
		Transform* m_pParent = nullptr;
		std::vector<Transform*> m_children;
	};
}