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

		// 오버라이드 함수
		void Update(float deltaTime) override;

		// 변환 함수

		// 계층구조 함수
		// 시조를 리턴한다.
		Transform* GetRoot();
		// 부모 존재 여부를 리턴한다.
		bool HasParent() const { return (m_pParent != nullptr); }
		// 부모 포인터를 리턴한다.
		Transform* GetParent() const { return m_pParent; }
		void SetParent(Transform* pParent);
		// 부모가 존재한다면 독립한다.
		void BecomeOrphan();
		// 자식의 수를 리턴한다.
		unsigned int GetChildCount() const { return static_cast<unsigned int>(m_children.size()); }
		// 이름으로 자식을 찾아 리턴한다.
		Transform* Find(const std::string& name) const;
		// 인덱스로 자식을 찾아 리턴한다.
		Transform* GetChild(unsigned int index) const;
		// 모든 자식을 리턴한다.
		std::vector<Transform*> GetChildren() { return m_children; }
		// 자신이 pParent의 자식인지 확인한다.
		bool IsChildOf(const Transform* pParent) const;
		// 모든 자식을 떼어낸다. 자식들을 제거하지 않고 부모를 없애고 싶을 때 사용한다.
		void DetachChildren();
		// 자식들로부터 부모 자식 관계를 재구성한다. 재귀적으로 호출한다.
		void AcquireChidren();

	private:

	private:
		// transform

		// heirarchy
		Transform* m_pParent = nullptr;
		std::vector<Transform*> m_children;
	};
}