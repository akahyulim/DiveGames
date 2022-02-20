#pragma once
#include "Base/Object.h"
#include "Component/Transform.h"

namespace Dive
{
	class Scene;

	class GameObject : public Object
	{
	public:
		GameObject(Scene* pScene, const std::string& name = std::string());
		GameObject(Scene* pScene, unsigned long long id, const std::string& name = std::string());
		~GameObject() = default;

		template<class T>
		T* AddComponent()
		{
			// T�� Component�� ����� ������ Ȯ���� �ʿ��ϴ�.

			if (HasComponent<T>())
				return GetComponent<T>();

			return new T(this);
		}
		
		template<class T>
		void RemoveComponent()
		{

		}

		template<class T>
		bool HasComponent()
		{
			return false;
		}

		template<class T>
		T* GetComponent()
		{
			return nullptr;
		}

	private:
	private:
		Scene* m_pScene			= nullptr;
	};
}