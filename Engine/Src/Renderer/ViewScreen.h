#pragma once
#include "RendererDefs.h"		// eRenderPath용. 추후 Renderer에 흡수시키자.
#include "DvRenderer.h"

namespace Dive
{
	class DvScene;
	class DvCamera;
	class Renderable;
	class DvLight;
	class DvMesh;
	class DvMaterial;

	// urho처럼 하려면 이게 batch와 같이 함수를 지닌 클래스여야 한다.
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

	// sparky는 layer를 layer2d, layer3d로 구체화했다.
	// 그리고 각각의 객체는 renderpath를 render2d, render3d(forwardrenderer, deferredrenderer)를 사용한다.
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