#pragma once
#include "GraphicsDevice.h"

namespace Dive
{
	class RenderPath
	{
	public:
		RenderPath() = default;
		virtual ~RenderPath() {}

		// LoadingRenderPath에서 호출된다.
		virtual void Load() {}

		// renderpath가 swap될 때 호출?
		virtual void Start() {}
		virtual void Stop() {}

		virtual void PreUpdate() {}
		virtual void FixedUpdate() {}
		virtual void Update(float deltaTime) {}
		virtual void PostUpdate() {}
		virtual void Render() const {}

		// 아직 뭔지 잘 모르겠다.
		virtual void Compose() const {}

		// get & set mask

	private:
		// mask
	};
}