#pragma once
#include "Core/Object.h"

namespace Dive
{
	class DvGameObject;
	class DvTransform;

	class DvComponent : public Object
	{
	public:
		DvComponent(DvGameObject* pGameObject);
		virtual ~DvComponent() = default;

		virtual void Update() {}

		std::string GetName() const override;
		void SetName(const std::string&) override {}

		DvGameObject* GetGameObject() const;
		DvTransform* GetTransform();

	protected:
		DvGameObject* m_pGameObject;
		DvTransform* m_pTransform;
	};
}