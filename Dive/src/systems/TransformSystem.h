#pragma once
#include "SystemBase.h"

namespace Dive
{
	class DvWorld;

	class TransformSystem : public SystemBase
	{
	public:
		TransformSystem(DvWorld* world);
		~TransformSystem() = default;

		void OnUpdate() override;		
	};
}
