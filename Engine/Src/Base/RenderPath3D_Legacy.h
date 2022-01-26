#pragma once
#include "RenderPath.h"
#include "Renderer/Graphics/dvTexture2D.h"
#include "Renderer/Graphics/dvRenderTexture.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"

/*
*	이름 그대로 RenderPath 구현
*	Core에는 어울리지 않는다. 사용자에 의해 수정 가능한 Level임을 알리고 싶다.
*/
namespace Dive
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
		RenderPath3D_Legacy();
		~RenderPath3D_Legacy();

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
		dvRenderTexture* m_pCompose = nullptr;

		dvTexture2D* m_pImage = nullptr;
	};
}