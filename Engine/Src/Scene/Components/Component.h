#pragma once
#include "Core/Object.h"

namespace Dive
{
	class ConstantBuffer;
	class GameObject;

	class Component : public Object
	{
		DV_CLASS(Component, Object)

	public:
		Component(GameObject* pGameObject);
		~Component() override = default;

		virtual void Update() {}

		virtual ConstantBuffer* GetConstantBufferVS() { return nullptr; }
		virtual ConstantBuffer* GetConstantBufferDS() { return nullptr; }
		virtual ConstantBuffer* GetConstantBufferPS() { return nullptr; }

		std::string GetName() const;

		GameObject* GetGameObject() const;
		void SetGameObject(GameObject* pGameObject) { m_pGameObject = pGameObject; }
		
	protected:
		GameObject* m_pGameObject;
	};
}