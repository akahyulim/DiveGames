#pragma once
#include "RendererDefs.h"		// eRenderPath��. ���� Renderer�� �����Ű��.
#include "DvRenderer.h"

namespace Dive
{
	class DvScene;
	class DvCamera;
	class Renderable;
	class DvLight;
	class DvMesh;
	class DvMaterial;

	// urhoó�� �Ϸ��� �̰� batch�� ���� �Լ��� ���� Ŭ�������� �Ѵ�.
	struct RenderableBatch
	{
		// camera distance
		DirectX::XMMATRIX worldTransform;
		DvMesh* pMesh;
		// mesh type
		DvMaterial* pMaterial;
	};

	struct LightBatch
	{

	};

	// sparky�� layer�� layer2d, layer3d�� ��üȭ�ߴ�.
	// �׸��� ������ ��ü�� renderpath�� render2d, render3d(forwardrenderer, deferredrenderer)�� ����Ѵ�.
	class ViewScreen
	{
	public:
		ViewScreen(DvScene* pScene, DvCamera* pCamera, eRenderPath renderPath = eRenderPath::Forward);

		void Update();
		void Render();

		DvScene* GetScene() { return m_pScene; }

	private:
		void forwardRender();
		void deferredRender();

	private:
		DvScene* m_pScene;
		DvCamera* m_pCamera;
		eRenderPath m_RenderPath;

		std::vector<Renderable*> m_Renderables;
		std::vector<DvLight*> m_Lights;

		std::vector<RenderableBatch> m_RenderableBatches;
		std::vector<LightBatch> m_LightBatches;

		FrameBuffer m_cpuFrameBuffer;
		MaterialBuffer m_cpuMaterialBuffer;
		CameraBuffer m_cpuCameraBuffer;
		LightBuffer m_cpuLightBuffer;
	};
}