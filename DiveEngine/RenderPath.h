#pragma once
#include "GraphicsDevice.h"

namespace Dive
{
	class RenderPath
	{
	public:
		RenderPath() = default;
		virtual ~RenderPath() {}

		// LoadingRenderPath���� ȣ��ȴ�.
		virtual void Load() {}

		// renderpath�� swap�� �� ȣ��?
		virtual void Start() {}
		virtual void Stop() {}

		virtual void PreUpdate() {}
		virtual void FixedUpdate() {}
		virtual void Update(float deltaTime) {}
		virtual void PostUpdate() {}
		virtual void Render() const {}

		// ���� ���� �� �𸣰ڴ�.
		virtual void Compose() const {}

		// get & set mask

	private:
		// mask
	};
}