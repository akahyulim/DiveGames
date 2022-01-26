#pragma once
#include "RenderPath.h"
#include "Renderer/Graphics/dvTexture2D.h"
#include "Renderer/Graphics/dvRenderTexture.h"
#include "Scene/SceneManager.h"
#include "Scene/Scene.h"

/*
*	�̸� �״�� RenderPath ����
*	Core���� ��︮�� �ʴ´�. ����ڿ� ���� ���� ������ Level���� �˸��� �ʹ�.
*/
namespace Dive
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

		// visibility�� ������.

		// gpu resource�� ������
		// �ᱹ Base�� Engine�� ���Ѵ�.
		dvRenderTexture* m_pGBuffer[eGBuffer::Count];
		dvRenderTexture* m_pDepthStencilBuffer = nullptr;
		dvRenderTexture* m_pCompose = nullptr;

		dvTexture2D* m_pImage = nullptr;
	};
}