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

		// 부모 트랜스폼 존재여부 리턴.
		bool HasParent() const { return m_pParent; }
		// 부모 트랜스폼 포인터 리턴.
		Transform* GetParent() const { return m_pParent; }
		// 부모 트랜스폼 설정.
		void SetParent(Transform* pParent);
		// 루트 트랜스폼 리턴.
		Transform* GetRoot();
		// 자식 존재여부 리턴.
		bool HasChild() const { return !m_Children.empty(); }
 		// 자식 수 리턴.
		unsigned int GetChildCount() const { return static_cast<unsigned int>(m_Children.size()); }
		// 전달된 인덱스에 맞는 자식 포인터 리턴.
		Transform* GetChild(unsigned int index) const;
		// 현재 트랜스폼이 전달받은 트랜스폼의 자식 여부를 리턴.
		bool IsChildOf(Transform* pParent) const;
		// 모든 자식을 독립.
		void DetachChildren();
		// 자식 벡터 리턴.
		std::vector<Transform*> GetChildren() const;

	private:
	private:
		// 부모 트랜스폼 포인터.
		Transform* m_pParent;
		// 자식 트랜스폼.
		std::vector<Transform*> m_Children;
	};
}
