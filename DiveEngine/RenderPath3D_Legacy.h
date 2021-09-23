#pragma once
#include "RenderPath.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Src/Renderer/Graphics/dvRenderTexture.h"

// Basic class for 3d rendering paths.
// The post proecess chain is also implemented here.
namespace dive
{
	// 결국엔 데이터를 가르쳐 주는 편이 나을 것 같긴 하다...
	enum eGBuffer
	{
		RT0,
		RT1,
		RT2,
		Count,
	};

	class RenderPath3D_Legacy : public RenderPath
	{
	public:

		void ResizeBuffers();

		//= OVERRIDE ================================
		void PreUpdate() override;
		void Update(float deltaTime) override;
		void Render() const override;
		void Compose() const override;
		//============================================

	private:
	private:

		// visibility를 가진다.

		// gpu resource를 가진다
		// 결국 Base는 Engine에 속한다.
		dvRenderTexture* m_pGBuffer[eGBuffer::Count];
		dvRenderTexture* m_pDepthStencilBuffer = nullptr;
	};
}