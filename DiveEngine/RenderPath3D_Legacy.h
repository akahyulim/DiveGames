#pragma once
#include "RenderPath.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Src/Renderer/Graphics/dvRenderTexture.h"

// Basic class for 3d rendering paths.
// The post proecess chain is also implemented here.
namespace dive
{
	// �ᱹ�� �����͸� ������ �ִ� ���� ���� �� ���� �ϴ�...
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

		// visibility�� ������.

		// gpu resource�� ������
		// �ᱹ Base�� Engine�� ���Ѵ�.
		dvRenderTexture* m_pGBuffer[eGBuffer::Count];
		dvRenderTexture* m_pDepthStencilBuffer = nullptr;
	};
}