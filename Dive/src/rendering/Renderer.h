#pragma once
#include "GBuffer.h"

namespace Dive
{
	class Graphics;
	//class GBuffer;

	class Renderer
	{
	public:
		Renderer();
		~Renderer() = default;

		bool Initialize(uint32_t width, uint32_t height, std::shared_ptr<Graphics> pGraphics);
		void Shutdown();

		void Tick();

		void ResizeTargets(uint32_t width, uint32_t height);

		uint32_t GetTargetWidth() const { return m_Width; }
		uint32_t GetTargetHeight() const { return m_Height; }

		uint64_t GetFrameCount() const { return m_FrameCount; }

		// 상수 참조 리턴이 나을 것 같다. 다만 이 경우 const 메서드를 구분해야 한다. 
		GBuffer& GetGBuffer() { return *m_pGBuffer.get(); }

	private:
	private:
		std::shared_ptr<Graphics> m_pGraphics;
		std::unique_ptr<GBuffer> m_pGBuffer;

		uint32_t m_Width;
		uint32_t m_Height;

		uint64_t m_FrameCount;
	};
}